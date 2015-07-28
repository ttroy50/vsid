#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

#include "Flow.h"
#include "Logger.h"
#include "Hasher.h"
#include "IPv4.h"
#include "TcpIpv4.h"

#include "AttributeMeter.h"
#include "AttributeMeterFactory.h"
#include "ProtocolModelDb.h"

#include "CommonConfig.h"

using namespace std;
using namespace VsidCommon;
using namespace Vsid;

Flow::Flow(IPv4Packet* packet, ProtocolModelDb* database) :
	_hash(0),
	_pktCount(0),
	_overallPktCount(0),
	_flowState(State::NEW),
	_isFirstPacket(false),
	_flowClassified(false),
	_protocolModelDb(database),
	_classifiedDivergence(0),
	_bestMatchDivergence(100),
	_threadQueueId(-1),
	_pktsInQueue(0)
{
	_firstPacketTuple.src_ip = packet->srcIp();
	_firstPacketTuple.src_port = packet->srcPort();
	_firstPacketTuple.dst_ip = packet->dstIp();
	_firstPacketTuple.dst_port = packet->dstPort();
	_firstPacketTuple.transport = packet->protocol();
	_startTimestamp = packet->timestamp();
	_lastPacketTimestamp = packet->timestamp();
	_firstOrigToDestDataSize = 0;
	_lastPacketDirection = Direction::UNKNOWN;
	_currentPacketDirection = Direction::UNKNOWN;

	_attributeMeters = Vsid::AttributeMeterFactory::instance()->getAllMeters();
	for(auto it = _attributeMeters.begin(); it != _attributeMeters.end(); ++it)
	{
		_attributeMetersMap[(*it)->name()] = (*it);
	}

	Ipv4FlowHasher hasher;
	_hash = hasher(&_firstPacketTuple);
}

void Flow::addPacket(IPv4Packet* packet)
{
	// make sure the packet gets deleted.
	// If we later want to keep the packet we will release it from the ptr
	std::unique_ptr<IPv4Packet> packetPtr(packet);

	_overallPktCount++;
	State stateUponArrival = _flowState;
	Direction tmpPktDirection = packetDirection(packet);

	_isFirstPacket = false;

	// TODO cleanup this to make it easier to understand and only do one cast
	if ( stateUponArrival == Flow::State::NEW )
	{
		_flowState = Flow::State::ESTABLISHING;

		if( packet->protocol() == IPPROTO_UDP )
		{
			_firstOrigToDestDataSize = (PACKET_MAX_BUFFER_SIZE < packet->dataSize()) ? PACKET_MAX_BUFFER_SIZE : packet->dataSize();
			memcpy(_firstOrigToDestData, packet->data(), _firstOrigToDestDataSize);
			_isFirstPacket = true;
			SLOG_INFO(<< "Got first packet " << *this);
		}
		else // TCP
		{
			TcpIPv4* tcpPacket = static_cast<TcpIPv4*>(packet);
			uint8_t flags = tcpPacket->flags();

			if( (flags & TH_FIN) || flags & TH_RST )
			{
				_flowState = Flow::State::FINISHED;
				SLOG_INFO(<< "RST or FIN in NEW state returning")
				return;
			}
			else if( (flags & TH_SYN) == false)
			{
				SLOG_INFO(<< "not a syn : datasize " << packet->dataSize() );
				
				if( (flags & TH_ACK) && packet->dataSize() == 0)
				{	
					// ACK and empty packet on NEW. Consider it finished
					_flowState = Flow::State::FINISHED;
				}
				else
				{
					// new flow where we didn't see SYN?
					_firstOrigToDestDataSize = (PACKET_MAX_BUFFER_SIZE < packet->dataSize()) ? PACKET_MAX_BUFFER_SIZE : packet->dataSize();
					memcpy(_firstOrigToDestData, packet->data(), _firstOrigToDestDataSize);
					_isFirstPacket = true;
					SLOG_INFO(<< "Got first packet " << *this);
				}
			}
		}
	}
	else if( stateUponArrival == Flow::State::ESTABLISHING)
	{
		// this is rather naive for TCP but will leave for now
		if( packet->protocol() == IPPROTO_UDP )
		{
			if( tmpPktDirection == Direction::DEST_TO_ORIG )
			{
				_flowState = Flow::State::ESTABLISHED;
			}
		}
		else
		{
			TcpIPv4* tcpPacket = static_cast<TcpIPv4*>(packet);
			uint8_t flags = tcpPacket->flags();

			if( (flags & TH_FIN) || flags & TH_RST )
			{
				_flowState = Flow::State::FINISHED;
				SLOG_INFO(<< "RST or FIN in ESTABLISHING state returning")
			}
			else if ( flags & TH_SYN ) // TODO maybe make this a syn ack check
			{
				_flowState = Flow::State::ESTABLISHING;
			}
			else 
			{
				_flowState = Flow::State::ESTABLISHED;
			}
		}
	}
	else if( stateUponArrival == Flow::State::ESTABLISHED )
	{
		if( packet->protocol() == IPPROTO_TCP )
		{
			TcpIPv4* tcpPacket = static_cast<TcpIPv4*>(packet);
			uint8_t flags = tcpPacket->flags();

			if( (flags & TH_FIN) )
			{
				_flowState = Flow::State::FINISHING;
			}
			else if( flags & TH_RST )
			{
				_flowState = Flow::State::FINISHING;
			}
			else
			{
				if( _firstOrigToDestDataSize == 0 && tmpPktDirection == Direction::ORIG_TO_DEST)
				{
					// First data packet after SYN / ACK
					_firstOrigToDestDataSize = (PACKET_MAX_BUFFER_SIZE < packet->dataSize()) ? PACKET_MAX_BUFFER_SIZE : packet->dataSize();
					memcpy(_firstOrigToDestData, packet->data(), _firstOrigToDestDataSize);
					_isFirstPacket = true;
					SLOG_INFO(<< "Got first packet " << *this);
				}
			}
		}
	}
	else if( stateUponArrival == Flow::State::FINISHING )
	{
		if( packet->protocol() == IPPROTO_TCP )
		{
			TcpIPv4* tcpPacket = static_cast<TcpIPv4*>(packet);
			uint8_t flags = tcpPacket->flags();

			if( (flags & TH_ACK) || (flags & TH_FIN) || flags & TH_RST )
			{
				_flowState = Flow::State::FINISHED;
				SLOG_INFO(<< " Flow entering Finished state");
				return;
			}
		}
	}
	else if ( stateUponArrival == Flow::State::FINISHED )
	{
		SLOG_INFO(<< " Flow already Finished");
		// Flow already fisinhed ??
		return;
	}




	// Check if we consider it a "Data Packet"
	if( packet->protocol() == IPPROTO_TCP )
	{
		TcpIPv4* tcpPacket = static_cast<TcpIPv4*>(packet);

		// No need to analyse a SYN packet
		if ( (tcpPacket->flags() & TH_SYN) && tcpPacket->dataSize() <= 0 )
		{
			SLOG_INFO(<< "TCP SYN with no data")
			//_lastPacketTimestamp = packet->timestamp();
			return;
		}

		// Don't bother to analyse an empty TCP ACK packets
		if ( (tcpPacket->flags() & TH_ACK) && tcpPacket->dataSize() <= 0 )
		{
			//_lastPacketTimestamp = packet->timestamp();
			SLOG_INFO(<< "TCP Ack with no data")
			return;
		}
	} 
	
	// Update these because we use these only for "Data" packets on a flow
	_pktCount++;
	_lastPacketDirection = _currentPacketDirection;
	_currentPacketDirection = tmpPktDirection;

	if( _flowClassified )
	{
		_lastPacketTimestamp = packet->timestamp();
		SLOG_INFO(<< " Flow already classified");
		// Already classified. Don't waste time updating calculations
		return;
	}

	if( _pktCount > _protocolModelDb->cutoffLimit() )
	{
		_lastPacketTimestamp = packet->timestamp();
		SLOG_INFO(<< " Flow above cutoffLimit " << _protocolModelDb->cutoffLimit() 
						<< " : " << _pktCount );
		// Don't look at packet because it is over the limit
		return;
	}

	SLOG_INFO( << "Calculating packet ");
	for(auto it = _attributeMeters.begin(); it != _attributeMeters.end(); ++it)
	{
		try
		{
			SLOG_INFO( << "Calculating " << (*it)->name());
			(*it)->calculateMeasurement(this, packet);
		}
		catch(std::exception& ex)
		{
			SLOG_ERROR( << "Exception caught calculation [" << (*it)->name() << "] : ["
							<< ex.what() << "]");
		}
	}

	SLOG_INFO( << "Meters updated")
	_lastPacketTimestamp = packet->timestamp();

	if( !CommonConfig::instance()->learningMode() && !_flowClassified )
	{
		SLOG_INFO(<< "Calculating K-L Divergence")
		// TODO Calculate K-L Divergence
		for(size_t i = 0; i < _protocolModelDb->size(); i++)
		{
			double klDivergence = 0.0;

			// get protocol model in order or dst port of flow
			std::shared_ptr<ProtocolModel> pm = _protocolModelDb->at(i, 0);// _firstPacketTuple.dst_port);

			if( !pm )
			{
				SLOG_ERROR(<< "Unable to get protocol model");
				continue;
			}

			if( !pm->enabled() )
				continue;
			
			SLOG_INFO(<< "Checking model " << pm->name());
			for(size_t attr = 0; attr < pm->size(); attr++)
			{
				std::shared_ptr<AttributeMeter> pm_am = pm->at(attr);
				if( !pm_am->enabled() )
					continue;

				std::shared_ptr<AttributeMeter> am = _attributeMetersMap[pm_am->name()];

				double sum = 0.0;

				for(size_t fp = 0; fp < pm_am->size(); ++fp)
				{
					if(am->at(fp) > 0 && pm_am->at(fp) > 0)
					{
						sum += am->at(fp) * log2(am->at(fp) / pm_am->at(fp));
						//SLOG_INFO(<<" K-L sum is " << sum);
					}
				}
				klDivergence += sum;
			}
			
			// We will only do a full classification if we have more than the defining Limit worth
			// of packets
			if( klDivergence > 0 
					&&_pktCount > _protocolModelDb->definingLimit() 
					&& klDivergence < CommonConfig::instance()->divergenceThreshold() )
			{
				_flowClassified = true;
				_classifiedProtocol = pm->name();
				_classifiedDivergence = klDivergence;
				SLOG_DEBUG(<< "Flow classified as [" << _classifiedProtocol 
							<< "] with divergence [" << _classifiedDivergence 
							<< "] : " << *this);
				break;
			}
			
			if( _pktCount == _protocolModelDb->definingLimit()  )
			{
				if( klDivergence < CommonConfig::instance()->divergenceThreshold() )
				{
					_flowClassified = true;
					_classifiedProtocol = pm->name();
					_classifiedDivergence = klDivergence;
					SLOG_DEBUG(<< "Flow classified as [" << _classifiedProtocol 
								<< "] with divergence [" << _classifiedDivergence 
								<< "] : " << *this);
					break;
				}
				else if( _bestMatchDivergence < CommonConfig::instance()->divergenceThreshold() )
				{
					_flowClassified = true;
					_classifiedProtocol = _bestMatchProtocol;
					_classifiedDivergence = _bestMatchDivergence;
					SLOG_DEBUG(<< "Flow classified as [" << _classifiedProtocol 
								<< "] with divergence [" << _classifiedDivergence 
								<< "] : " << *this);
					break;
				}
			}
			

			if( klDivergence > 0 
				//&&_pktCount >= _protocolModelDb->definingLimit()
				&& klDivergence < _bestMatchDivergence )
			{
				_bestMatchDivergence = klDivergence;
				_bestMatchProtocol = pm->name();
				SLOG_INFO(<< "Updating best match as [" << _bestMatchDivergence 
								<< "] with divergence [" << _bestMatchProtocol 
								<< "] : " << *this);
			}

			SLOG_DEBUG(<< "K-L Divergence for [" << pm->name() <<"] is [" << klDivergence <<"] on flow [" << *this << "]");
			
		}
	}

}

/*
uint32_t Flow::flowHash()
{
		return _hash;
}*/
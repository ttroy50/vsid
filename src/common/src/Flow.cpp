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

#include "CommonConfig.h"

using namespace std;
using namespace VsidCommon;
using namespace Vsid;

Flow::Flow(IPv4Packet* packet) :
	_hash(0),
	_pktCount(0),
	_flowState(State::NEW),
	_isFirstPacket(false),
	_flowClassified(false)
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
}


Flow::Flow(IPv4Tuple tuple) :
	_firstPacketTuple(tuple),
	_hash(0),
	_pktCount(0),
	_firstOrigToDestDataSize(0),
	_flowState(State::NEW),
	_isFirstPacket(false),
	_lastPacketDirection(Direction::UNKNOWN),
	_currentPacketDirection(Direction::UNKNOWN),
	_flowClassified(false)
{
	gettimeofday(&_startTimestamp, NULL);
	_lastPacketTimestamp = _startTimestamp;

	// Do we need a fingerprint for this one??
}

Flow::Flow(uint32_t hash) :
	_hash(hash),
	_pktCount(0),
	_firstOrigToDestDataSize(0),
	_flowState(State::NEW),
	_isFirstPacket(false),
	_lastPacketDirection(Direction::UNKNOWN),
	_currentPacketDirection(Direction::ORIG_TO_DEST),
	_flowClassified(false)
{
	gettimeofday(&_startTimestamp, NULL);
	_lastPacketTimestamp = _startTimestamp;

	// No need for fingerprint here because is only for searching
}

void Flow::addPacket(IPv4Packet* packet)
{
	SLOG_INFO(<< "Packet added to flow");
	_pktCount++;
	State stateUponArrival = _flowState;
	
	_isFirstPacket = false;

	_lastPacketDirection = _currentPacketDirection;
	_currentPacketDirection = packetDirection(packet);

	if ( stateUponArrival == Flow::State::NEW )
	{
		_flowState = Flow::State::ESTABLISHING;

		if( packet->protocol() == IPPROTO_UDP )
		{
			_firstOrigToDestDataSize = (PACKET_MAX_BUFFER_SIZE < packet->dataSize()) ? PACKET_MAX_BUFFER_SIZE : packet->dataSize();
			memcpy(_firstOrigToDestData, packet->data(), _firstOrigToDestDataSize);
			_isFirstPacket = true;
		}
		else // TCP
		{
			TcpIPv4* tcpPacket = static_cast<TcpIPv4*>(packet);
			uint8_t flags = tcpPacket->flags();

			if( (flags & TH_FIN) || flags & TH_RST )
			{
				_flowState = Flow::State::FINISHED;
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
				}
			}
		}
	}
	else if( stateUponArrival == Flow::State::ESTABLISHING)
	{
		// this is rather naive for TCP but will leave for now
		if( packet->protocol() == IPPROTO_UDP )
		{
			if( packetDirection(packet) == Direction::DEST_TO_ORIG )
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
				if( _firstOrigToDestDataSize == 0 && _currentPacketDirection == Direction::ORIG_TO_DEST)
				{
					// First data packet after SYN / ACK
					_firstOrigToDestDataSize = (PACKET_MAX_BUFFER_SIZE < packet->dataSize()) ? PACKET_MAX_BUFFER_SIZE : packet->dataSize();
					memcpy(_firstOrigToDestData, packet->data(), _firstOrigToDestDataSize);
					_isFirstPacket = true;
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
				return;
			}
		}
	}
	else if ( stateUponArrival == Flow::State::FINISHED )
	{
		// Flow already fisinhed ??
		return;
	}

	if( _flowClassified )
	{
		// Already classified. Don't waste time updating calculations
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

	_lastPacketTimestamp = packet->timestamp();

	if( !CommonConfig::instance()->learningMode() && !_flowClassified )
	{
		// TODO Calculate K-L Divergence
	}

}

uint32_t Flow::flowHash()
{
	if(_hash > 0)
		return _hash;

	Ipv4FlowHasher hasher;
	return hasher(&_firstPacketTuple);
}
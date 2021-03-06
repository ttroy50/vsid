#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

#include "yaml-cpp/yaml.h"

#include "TrainingInput.h"
#include "Logger.h"
#include "Hasher.h"
#include "ProtocolModelDb.h"

using namespace VsidTraining;
using namespace VsidCommon;
using namespace std;
using namespace Vsid;

uint32_t TrainingFlow::flowHash()
{
    Ipv4FlowHasher hasher;
    return hasher(&tuple);
}

bool TrainingInput::read(const std::string& fileName, ProtocolModelDb* protocolModelDb)
{

	// parse the atom/yaml...
    YAML::Node config;
    try 
    {
        config = YAML::LoadFile(fileName);
    } 
    catch (YAML::Exception& e)
    {
        LOG_ERROR(("Exception loading file [%v] into YAML [%v]", fileName, e.what()));
        return false;
    }

    for (YAML::const_iterator it=config.begin(); it!=config.end(); ++it) 
    {
    	TrainingFile pcapFile;
    	pcapFile.filename = (*it)["file"].as<std::string>();

    	for (YAML::const_iterator fls = (*it)["flows"].begin(); 
    	 		fls != (*it)["flows"].end(); ++fls)
    	{
    	 	TrainingFlow flow;

            // src
    	 	string src_ip = (*fls)["src_ip"].as<string>();
            struct sockaddr_in src_sa;
            if(inet_pton(AF_INET, src_ip.c_str(), &(src_sa.sin_addr)) != 1)
            {
                SLOG_ERROR( << "Error converting src address [" << src_ip << "] : " << strerror(errno));
                continue;
            } 
            flow.tuple.src_ip = src_sa.sin_addr.s_addr;
    	 	flow.tuple.src_port = (*fls)["src_port"].as<uint16_t>();

            // dst
    	 	string dst_ip = (*fls)["dst_ip"].as<string>();
            struct sockaddr_in dst_sa;
            if(inet_pton(AF_INET, dst_ip.c_str(), &(dst_sa.sin_addr)) != 1)
            {
                SLOG_ERROR( << "Error converting dst address [" << dst_ip << "] : " << strerror(errno));
                continue;
            }
            flow.tuple.dst_ip = dst_sa.sin_addr.s_addr;
    	 	flow.tuple.dst_port = (*fls)["dst_port"].as<uint16_t>();

            // transport
    	 	string transport = (*fls)["transport"].as<string>();
            flow.tuple.transport = strToTransport(transport);
            if( flow.tuple.transport == 0 )
            {
                SLOG_ERROR( << "Error converting transport [" << transport << "]");
                continue;
            }

            // protocol
    	 	string protocol = (*fls)["protocol"].as<string>();
            flow.protocol = protocol; //strToProtocol(protocol);
            if( !protocolModelDb->find(protocol) )
            {
                SLOG_ERROR( << "Cannot find protocol in protocol DB [" << protocol << "]");
                continue;
            }

            pcapFile.flows.push_back(flow);
    	}

        SLOG_INFO( << "Adding training file : " << pcapFile.filename );
        _trainingFiles.push_back(pcapFile);

    }

    SLOG_INFO( << "Added [" << _trainingFiles.size() << "] training files" );
    return true;

}


uint8_t TrainingInput::strToTransport(const std::string& str)
{
    if(str == "TCP")
        return IPPROTO_TCP;
    else if(str == "UDP")
        return IPPROTO_UDP;
    else
        return 0;
}

Protocol TrainingInput::strToProtocol(const std::string& str)
{
    if(str == "HTTP")
        return HTTP;
    else if(str == "HTTPS")
        return HTTPS;
    else if(str == "HTTPS")
        return SIP;
    else if( str == "HTTP-Progressive")
        return HTTP_PROGRESSIVE;
    else if (str == "DNS")
        return DNS;
    else if (str == "RTMP")
        return RTMP;
    else
        return UNKNOWN;
}

std::string TrainingInput::protocolToStr(Protocol protocol)
{
    switch(protocol)
    {
        case HTTP:
            return "HTTP";
        case HTTPS:
            return "HTTPS";
        case SIP:
            return "SIP";
        case HTTP_PROGRESSIVE:
            return "HTTP-Progressive";
        case RTMP:
            return "RTMP";
        case DNS:
            return "DNS";
    }

    return "";
}

#include <regex>

#include "RtmpRegexMatchMeter.h"
#include "AttributeMeter.h"
#include "Logger.h"
#include "AttributeMeterFactory.h"

using namespace std;
using namespace Vsid;
using namespace VsidCommon;

std::unique_ptr<AttributeMeter> create_rtmp_regex_match_meter()
{
    std::unique_ptr<AttributeMeter> tmp(new RtmpRegexMatchMeter());
    return tmp;
}

Vsid::Registrar RtmpRegexMatchMeter::registrar("RtmpRegexMatchMeter", &create_rtmp_regex_match_meter);

RtmpRegexMatchMeter::RtmpRegexMatchMeter() :
    AttributeMeter(1),
    _overall_byte_size(0)
{
    _regex = std::regex("\x03.+\x14.+\x02.+\x07.*(connect){0,1}.+(app){0,1}.+");
}

void RtmpRegexMatchMeter::calculateMeasurement(Flow* flow, 
                                                    IPv4Packet* currentPacket )
{   
    if(currentPacket->dataSize() <= 0)
        return;

    if(_fingerprint[0] != 0)
        return;

    // if we match this we mark it down and never try again
    if (std::regex_match (currentPacket->data(), currentPacket->data()+currentPacket->dataSize(), 
                            _regex))
    {
        _fingerprint[0] = 1;
    }
}
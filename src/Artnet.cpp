//
// Artnet.cpp
//
#include "Particle.h"
#include "Artnet.h"

//==================================================================================================
// Data
//==================================================================================================

uint8_t ArtnetPacketHeaderData::rgbIdExpected[ArtnetPacketHeaderData::cbId] = { 'A', 'r', 't', '-', 'N', 'e', 't', '\0' };

//==================================================================================================
// Functions
//==================================================================================================

bool ArtnetPacketHeader::validate()
{
    bool result = false;
    if (cbData >= sizeof(ArtnetPacketHeaderData))
    {
        result = memcmp(&pData->rgbIdExpected[0], &pData->_rgbId[0], pData->cbId)==0;
        if (result)
        {
            switch (artnetOpMode())
            {
                case ArtnetOpModePoll:
                    {
                        ArtPollPacket pollPacket(pData, cbData);
                        result = pollPacket.validate();
                    }
                    break;
                case ArtnetOpModeDmx:
                    {
                        ArtDmxPacket dmxPacket(pData, cbData);
                        result = dmxPacket.validate();
                    }
                    break;
            }
        }
    }
    return result;
}

void ArtPollPacket::pollReply(const IPAddress& remoteIP, ArtnetDevice& device)
{
    ArtPollReplyPacket reply;
    reply.initialize();
    reply.setShortName(device.shortName());
    reply.setLongName(device.longName());
    reply.setNodeReport(device.reportCode(), device.pollReplyCount());
    //
    Log.info("sending ArtPollReply to %s", remoteIP.toString().c_str());
    device.sendPacket(reinterpret_cast<byte*>(&reply), reply.cb(), remoteIP);
    //
    device.incrementPollReplyCount();
}

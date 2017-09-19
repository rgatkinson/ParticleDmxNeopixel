//
// ArtDmxPacket.h
//
#ifndef _ARTDMXPACKET_H_
#define _ARTDMXPACKET_H_

#include "Color.h"

//==================================================================================================
// ArtDmxPacket
//==================================================================================================

#pragma pack(push, 1)
struct PACKED ArtDmxPacketData : ArtnetPacketHeaderData
{
    int8_t  _protVerHi;
    int8_t  _protVerLo;
    int8_t  _sequence;
    int8_t  _physical;
    int8_t  _subuni;
    int8_t  _net;
    int8_t  _lengthHi;
    int8_t  _lengthLo;
    uint8_t _data[0];

    static int cb(int channelCount)
    {
        return offset_of(ArtDmxPacketData, _data) + channelCount * sizeof(byte);
    }
};
#pragma pack(pop)

//==================================================================================================
// DmxPacketConsumer
//==================================================================================================

struct DmxPacketConsumer
{
    virtual void onDmxPacket(ArtDmxPacket& packet) = 0;
};

//==================================================================================================
// ArtDmxPacket
//==================================================================================================

struct ArtDmxPacket : ArtnetPacketHeader
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    ArtDmxPacketData* pData;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    ArtDmxPacket(int channelCount) : ArtnetPacketHeader(ArtDmxPacketData::cb(channelCount), ArtnetOpModePoll)
    {
        pData = reinterpret_cast<ArtDmxPacketData*>(ArtnetPacketHeader::pData);
    }

    ArtDmxPacket(void* pb, int cb) : ArtnetPacketHeader(pb, cb)
    {
        pData = reinterpret_cast<ArtDmxPacketData*>(ArtnetPacketHeader::pData);
    }

    void initialize()
    {
        ArtnetPacketHeader::initialize(ArtnetOpModePoll);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    int protocolVersion()
    {
        return TypeConversion::bytesToInt(&pData->_protVerHi, 2, BigEndian);
    }
    void setProtocolVersion(int protocolVersion)
    {
        TypeConversion::intToBytes(protocolVersion, &pData->_protVerHi, 2, BigEndian);
    }

    int channelCount()
    {
        return length();
    }
    int length()
    {
        return TypeConversion::bytesToInt(&pData->_lengthHi, 2, BigEndian);
    }
    void setLength(int length)
    {
        TypeConversion::intToBytes(length, &pData->_lengthHi, 2, BigEndian);
    }

    bool validate()
    {
        bool result = ArtDmxPacketData::cb(channelCount()) <= cbData;
    }

    int operator[](DMX_ADDRESS dmxAddress)
    {
        int channelNumber = dmxAddress - 1;    // make zero-based
        if (0 <= channelNumber && channelNumber < channelCount())
        {
            return pData->_data[channelNumber];
        }
        return -1;  // anything negative
    }
};

#endif

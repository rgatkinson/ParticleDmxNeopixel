//
// ArtDmxPacket.h
//
#ifndef _ARTDMXPACKET_H_
#define _ARTDMXPACKET_H_

#include "Util/Color.h"
#include "DmxPacketConsumer.h"

//==================================================================================================
// ArtDmxPacket
//==================================================================================================

// Port-Address: one of the 32,768 possible addresses to which a DMX frame can be directed. The
// Port-Address is a 15 bit number composed of Net+Sub-Net+Universe

struct PACKED ArtDmxPacketData : ArtnetPacketHeaderData
{
    int8_t  _protVerHi;     // 10, 1    High byte of the Art-Net protocol revision number
    int8_t  _protVerLo;     // 11, 1    Low byte of the Art-Net protocol revision number
    int8_t  _sequence;      // 12, 1    The sequence number is used to ensure that ArtDmx packets are used in the correct order
    int8_t  _physical;      // 13, 1    The physical input port from which DMX512 data was input. This field is for information only. Use Universe for data routing
    int8_t  _subuni;        // 14, 1    The low byte of the 15 bit Port-Address to which this packet is destined
    int8_t  _net;           // 15, 1    The top 7 bits of the 15 bit Port-Address to which this packet is destined
    int8_t  _lengthHi;      // 16, 1    The length of the DMX512 data array. This value should be an even number in the range 2 – 512
    int8_t  _lengthLo;      // 17, 1    Low Byte of above
    byte    _data[0];       // 18, ...

    static int cb(int channelCount)
    {
        return offsetof(ArtDmxPacketData, _data) + channelCount * sizeof(byte);
    }
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

    ArtDmxPacket(int channelCount) : ArtnetPacketHeader(ArtDmxPacketData::cb(channelCount), ArtnetOpMode::Poll)
    {
        pData = reinterpret_cast<ArtDmxPacketData*>(ArtnetPacketHeader::pData);
    }

    ArtDmxPacket(void* pb, int cb) : ArtnetPacketHeader(pb, cb)
    {
        pData = reinterpret_cast<ArtDmxPacketData*>(ArtnetPacketHeader::pData);
    }

    void initialize()
    {
        ArtnetPacketHeader::initialize(ArtnetOpMode::Poll);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    int universe() // colloquially, we don't here care about any internal structure of subnets etc. Not real subnets anyway, btw...
    {
        return portAddress();
    }
    int portAddress()
    {
        return TypeConversion::bytesToInt(&pData->_subuni, 2, LittleEndian) & 0x7FFF;
    }

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
        return ArtDmxPacketData::cb(channelCount()) <= cbData;
    }

    byte* dmxValuesPointer(DMX_ADDRESS dmxAddress, int count)
    {
        int channelNumber = dmxAddress - 1;
        if (0 <= channelNumber && channelNumber + count <= channelCount())
        {
            return &pData->_data[channelNumber];
        }
        else
        {
            return nullptr;
        }
    }

    byte dmxValue(DMX_ADDRESS dmxAddress)
    {
        int channelNumber = dmxAddress - 1;    // make zero-based
        if (0 <= channelNumber && channelNumber < channelCount())
        {
            return pData->_data[channelNumber];
        }
        return 0;   // useful default
    }
};

#endif

//
// ArtnetDevice.h
//
#ifndef __ARTNET_DEVICE_H__
#define __ARTNET_DEVICE_H__

#include "ArtnetConstants.h"
#include "System/PersistentSettings.h"
#include "System/CloudVariable.h"

struct ArtnetDevice
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:
    static const int cbUdpBuffer = 1024;
    DmxPacketConsumer*  _pOwner;
    bool                _initialized;
    uint8_t*            _rgbUdpBuffer;
    UDP                 _udp;
    ArtnetReportCode    _reportCode = ArtnetReportCode::PowerOk;
    int                 _pollReplyCount  = 0;

    PersistentIntSetting                            _dmxAddress;
    PersistentStringSetting<CCH_ARTNET_SHORT_NAME>  _name;
    PersistentStringSetting<CCH_ARTNET_LONG_NAME>   _description;

    CloudVariable<int>                              _dmxAddressCloudVariable;
    CloudVariable<LPCSTR>                           _nameCloudVariable;
    CloudVariable<LPCSTR>                           _descriptionCloudVariable;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:
    ArtnetDevice(DmxPacketConsumer* pOwner, DMX_ADDRESS dmxAddress, LPCSTR name="<name>", LPCSTR description="<description>")
        : _dmxAddress(dmxAddress),
          _name(name),
          _description(description),
          _dmxAddressCloudVariable("dmxAddress", &_dmxAddress),
          _nameCloudVariable("name", &_name),
          _descriptionCloudVariable("description", &_description)
    {
        _pOwner = pOwner;
        _initialized = false;
        _rgbUdpBuffer = reinterpret_cast<uint8_t*>(mallocNoFail(cbUdpBuffer));
    }

    ~ArtnetDevice()
    {
        free(_rgbUdpBuffer);
    }

protected:
    void initialize()
    {
        _udp.begin(ARTNET_PORT);
        _initialized = true;
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:
    DMX_ADDRESS dmxAddress()
    {
        return _dmxAddress.value();
    }

    void setShortName(LPCSTR sz)
    {
        _name.setValue(sz);
    }
    LPCSTR shortName()
    {
        return _name.value();
    }

    void setLongName(LPCSTR sz)
    {
        _description.setValue(sz);
    }
    LPCSTR longName()
    {
        return _description.value();
    }

    ArtnetReportCode reportCode()
    {
        return _reportCode;
    }

    int pollReplyCount()
    {
        return _pollReplyCount;
    }
    void incrementPollReplyCount()
    {
        _pollReplyCount++;
    }

    //----------------------------------------------------------------------------------------------
    // Main loop
    //----------------------------------------------------------------------------------------------
public:
    void begin()
    {
        _dmxAddressCloudVariable.begin();
        _nameCloudVariable.begin();
        _descriptionCloudVariable.begin();
    }

    void loop()
    {
        if (!_initialized && WiFi.ready())
        {
            initialize();
        }
        if (_initialized)
        {
            receivePacket();
        }
    }

    void report()
    {
        if (_initialized)
        {
            Log.info("local address = %s", WiFi.localIP().toString().c_str());
            Log.info("subnet mask = %s", WiFi.subnetMask().toString().c_str());
            Log.info("broadcast address = %s", broadcastAddress().toString().c_str());
        }
        else
        {
            Log.info("Artnet not yet initialized");
        }

        Log.info("sizeof(ArtPollReplyPacketData)==%d", sizeof(ArtPollReplyPacketData));
    }

    void sendPacket(const byte* pv, int cb, const IPAddress& remoteIP)
    {
        _udp.sendPacket(pv, cb, remoteIP, ARTNET_PORT);
    }

protected:

    void receivePacket()
    {
        byte* pbPacket = &_rgbUdpBuffer[0];
        int cbPacket = _udp.receivePacket(pbPacket, cbUdpBuffer);
        if (cbPacket > 0)
        {
            Log.info("packet received: cb=%d addr=%s", cbPacket, _udp.remoteIP().toString().c_str());
            ArtnetPacketHeader header(pbPacket, cbPacket);
            if (header.validate())
            {
                ArtnetOpMode artnetOpMode = header.artnetOpMode();
                // Log.info("opMode=0x%04x", artnetOpMode);
                switch (artnetOpMode)
                {
                    case ArtnetOpMode::Poll:
                        {
                            ArtPollPacket pollPacket(pbPacket, cbPacket);
                            pollPacket.pollReply(_udp.remoteIP(), *this);
                        }
                        break;
                    case ArtnetOpMode::Dmx:
                        {
                            ArtDmxPacket dmxPacket(pbPacket, cbPacket);
                            if (_pOwner != NULL)
                            {
                                _pOwner->onDmxPacket(dmxPacket);
                            }
                        }
                        break;
                }
            }
            else
            {
                Log.error("invalid packet header");
            }
        }
    }

    /* returns the broadcast address */
    IPAddress broadcastAddress()
    {
        int cbitMask = countBits(WiFi.subnetMask());
        int cbitBroadcast = 32 - cbitMask;
        IPAddress result = WiFi.localIP();
        int ib = 3;
        while (cbitBroadcast > 0)
        {
            int cbit = min(8, cbitBroadcast);
            result[ib] = result[ib] | ones(cbit);
            cbitBroadcast -= cbit;
            ib--;
        }
        return result;
    }

    uint8_t ones(int cbit)
    {
        return byte(((int)1 << cbit) -1);
    }

    int countBits(IPAddress addr)
    {
        int result = 0;
        result += countBits(addr[0]);
        result += countBits(addr[1]);
        result += countBits(addr[2]);
        result += countBits(addr[3]);
        return result;
    }

    int countBits(uint8_t b)
    {
        int result = 0;
        if (b & 0x80) result++;
        if (b & 0x40) result++;
        if (b & 0x20) result++;
        if (b & 0x10) result++;
        if (b & 0x08) result++;
        if (b & 0x04) result++;
        if (b & 0x02) result++;
        if (b & 0x01) result++;
        return result;
    }
};

#endif

//
// Artnet.h:
//
#ifndef _ARTNET_H_
#define _ARTNET_H_

#include "Util/Misc.h"
#include "Util/TypeConversion.h"

struct ArtnetDevice;
struct ArtnetPacketHeader;
struct ArtPollPacket;
struct ArtPollReplyPacket;
struct ArtDmxPacket;

#define PACKED // __attribute__((packed)) __attribute__((aligned(1)))

//==================================================================================================
// Prolog
//==================================================================================================

#include "ArtnetConstants.h"
#include "ArtnetPacketHeader.h"
#include "ArtnetPollPacket.h"
#include "ArtnetPollReplyPacket.h"
#include "ArtDmxPacket.h"

//==================================================================================================
// Artnet
//==================================================================================================

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
    ReportCode          _reportCode = RcPowerOk;
    int                 _pollReplyCount  = 0;
    LPSTR               _shortName = NULL;
    LPSTR               _longName = NULL;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:
    ArtnetDevice(DmxPacketConsumer* pOwner)
    {
        _pOwner = pOwner;
        _initialized = false;
        _shortName = strdup("<short name>");
        _longName = strdup("<long name>");
        _rgbUdpBuffer = reinterpret_cast<uint8_t*>(mallocNoFail(cbUdpBuffer));
    }

    ~ArtnetDevice()
    {
        strfree(_shortName);
        strfree(_longName);
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
    void setShortName(LPCSTR sz)
    {
        setString(_shortName, sz);
    }
    LPCSTR shortName()
    {
        return _shortName;
    }

    void setLongName(LPCSTR sz)
    {
        setString(_longName, sz);
    }
    LPCSTR longName()
    {
        return _longName;
    }

    ReportCode reportCode()
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
        // nothing to do
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
            Log.info("not initialized");
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
                    case ArtnetOpModePoll:
                        {
                            ArtPollPacket pollPacket(pbPacket, cbPacket);
                            pollPacket.pollReply(_udp.remoteIP(), *this);
                        }
                        break;
                    case ArtnetOpModeDmx:
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

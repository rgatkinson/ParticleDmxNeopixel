//
// ArtnetDevice.h
//
#ifndef __ARTNET_DEVICE_H__
#define __ARTNET_DEVICE_H__

#include "Artnet/ArtnetConstants.h"
#include "Artnet/DmxPacketConsumer.h"
#include "Artnet/ArtnetPollPacket.h"
#include "Artnet/ArtnetPollReplyPacket.h"
#include "Artnet/ArtnetDmxPacket.h"
#include "System/NetworkStatusMonitor.h"
#include "System/PersistentSettings.h"
#include "System/PersistentValueSetting.h"
#include "System/PersistentStringSetting.h"
#include "System/CloudVariable.h"

struct ArtnetDevice : SystemEventNotifications
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:
    static const int _cbUdpBuffer = 1024;
    DmxPacketConsumer*  _pOwner;
    bool                _udpBegun;
    uint8_t*            _rgbUdpBuffer;
    UDP                 _udp;
    ArtnetReportCode    _reportCode = ArtnetReportCode::PowerOk;
    int                 _pollReplyCount  = 0;

    int                                             _dmxCount;
    CloudVariable<int>                              _dmxUniverseCloudVar;
    CloudVariable<int>                              _dmxAddressCloudVar;
    ComputedCloudVariable<int>                      _dmxLastCloudVar;
    CloudVariable<LPCSTR>                           _nameCloudVar;
    CloudVariable<LPCSTR>                           _descriptionCloudVar;

    static PersistentIntSetting                            _dmxUniverse;
    static PersistentIntSetting                            _dmxAddress;
    static PersistentStringSetting<CCH_ARTNET_SHORT_NAME>  _name;
    static PersistentStringSetting<CCH_ARTNET_LONG_NAME>   _description;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:
    ArtnetDevice(DmxPacketConsumer* pOwner, DMX_ADDRESS dmxAddress, int dmxCount, LPCSTR name="<name>", LPCSTR description="<description>")
    {
        _dmxCount = dmxCount;
        _dmxUniverse.setDefault(0);
        _dmxAddress.setDefault(dmxAddress);
        _name.setDefault(name);
        _description.setDefault(description);

        _pOwner = pOwner;
        _udpBegun = false;
        _rgbUdpBuffer = reinterpret_cast<uint8_t*>(mallocNoFail(_cbUdpBuffer));
        SystemEventRegistrar::theInstance->registerSystemEvents(this);
    }

    ~ArtnetDevice()
    {
        SystemEventRegistrar::theInstance->unregisterSystemEvents(this);
        free(_rgbUdpBuffer);
    }

protected:
    void beginUdp()
    {
        _udp.begin(ARTNET_PORT);
        _udpBegun = true;
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:
    int dmxUniverse()
    {
        return _dmxUniverse.value();
    }
    DMX_ADDRESS dmxAddress()
    {
        return _dmxAddress.value();
    }
    DMX_ADDRESS dmxFirst()
    {
        return dmxAddress();
    }

    DMX_ADDRESS dmxLast()
    {
        return dmxMax() -1;
    }
    DMX_ADDRESS dmxMax()
    {
        return dmxFirst() + dmxCount();
    }

    int dmxCount()
    {
        return _dmxCount;
    }
    void setDmxCount(int dmxCount)
    {
        _dmxCount = dmxCount;
    }

    void setName(LPCSTR sz)
    {
        _name.setValue(sz);
    }
    LPCSTR name()
    {
        return _name.value();
    }

    void setDescription(LPCSTR sz)
    {
        _description.setValue(sz);
    }
    LPCSTR description()
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
    // System Events
    //----------------------------------------------------------------------------------------------

    void onNetworkStatus(NetworkStatus netStatus) override
    {
        switch (netStatus)
        {
            case NetworkStatus::Disconnecting:
            case NetworkStatus::Disconnected:
                _udpBegun = false;
                _udp.stop();
                break;
        }
    }

    void onCloudStatus(int cloudStatus) override
    {
    }

    void onTimeChanged(int timeStatus) override
    {
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:
    void beginVariables()
    {
        _dmxUniverseCloudVar.initialize("dmxUniverse", &_dmxUniverse);
        _dmxAddressCloudVar.initialize("dmxAddrFirst", &_dmxAddress);
        _dmxLastCloudVar.initialize("dmxAddrLast", [this]() { return dmxLast(); });
        _nameCloudVar.initialize("name", &_name);
        _descriptionCloudVar.initialize("description", &_description);

        _dmxUniverseCloudVar.begin();
        _dmxAddressCloudVar.begin();
        _dmxLastCloudVar.begin();
        _nameCloudVar.begin();
        _descriptionCloudVar.begin();
    }

    virtual void begin()
    {
        beginVariables();
    }

    virtual void loop()
    {
        _dmxUniverseCloudVar.loop();
        _dmxAddressCloudVar.loop();
        _dmxLastCloudVar.loop();
        _nameCloudVar.loop();
        _descriptionCloudVar.loop();

        if (!_udpBegun && NetworkStatusMonitor::theInstance->status()==NetworkStatus::Connected)
        {
            beginUdp();
        }
        if (_udpBegun)
        {
            receivePacket();
        }
    }

    virtual void report()
    {
        _dmxUniverseCloudVar.report();
        _dmxAddressCloudVar.report();
        _dmxLastCloudVar.report();
        _nameCloudVar.report();
        _descriptionCloudVar.report();

        if (_udpBegun)
        {
            Log.info("local address = %s", WiFi.localIP().toString().c_str());
            Log.info("subnet mask = %s", WiFi.subnetMask().toString().c_str());
            Log.info("broadcast address = %s", broadcastAddress().toString().c_str());
        }
        else
        {
            Log.info("Artnet not yet beginUdpd");
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
        int cbPacket = _udp.receivePacket(pbPacket, _cbUdpBuffer);
        if (cbPacket > 0)
        {
            // INFO("packet received: cb=%d addr=%s port=%d", cbPacket, _udp.remoteIP().toString().c_str(), _udp.remotePort());
            ArtnetPacketHeader header(pbPacket, cbPacket);
            if (header.validate())
            {
                resetSleepTimer("receiveArtnet");
                ArtnetOpMode artnetOpMode = header.artnetOpMode();
                if (artnetOpMode != ArtnetOpMode::Dmx)
                {
                    INFO("opMode=0x%04x", (int)artnetOpMode);
                }
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
                            ArtnetDmxPacket dmxPacket(pbPacket, cbPacket);
                            if (_pOwner != NULL)
                            {
                                if (dmxPacket.universe() == dmxUniverse())
                                {
                                    _pOwner->onDmxPacket(dmxPacket);
                                }
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

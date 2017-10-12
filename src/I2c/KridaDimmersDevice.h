//
// KridaDimmersDevice.h
//
// An Artnet device that controls zero or more Krida I2c dimmers

#ifndef __KRIDA_DIMMERS_DEVICE_H__
#define __KRIDA_DIMMERS_DEVICE_H__

#include <vector>
#include "Artnet/Artnet.h"
#include "Artnet/DmxPacketConsumer.h"
#include "DmxKridaParameters.h"
#include "KridaDimmer.h"

struct KridaDimmersDevice : DmxPacketConsumer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:
    ArtnetDevice                                _artnet;        // before _dimmerCount to keep EEPROM sane
    PersistentIntSetting                        _dimmerCount;   // the *persistent* number of dimmers that are attached
    CloudVariable<decltype(_dimmerCount), int>  _dimmerCountCloudVar;

    std::vector<KridaDimmer*>                   _dimmers;       // size should be tracked to _dimmerCount

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    KridaDimmersDevice(LPCSTR shortName, int dimmerCount)
        : _artnet(this, DMX_ADDRESS_DEFAULT, dmxCount(dimmerCount), shortName),
          _dimmerCount(dimmerCount),
          _dimmerCountCloudVar("dimmerCount", &_dimmerCount)
    {
        Wire.setSpeed(CLOCK_SPEED_100KHZ);
        Wire.begin();
        createOrDestroyDimmers();
    }

    virtual ~KridaDimmersDevice()
    {
        clear();
        Wire.end();
    }

    void clear()
    {
        createOrDestroyDimmers(0);
    }

protected:

    void createOrDestroyDimmers()
    {
        createOrDestroyDimmers(_dimmerCount.value());
    }

    void createOrDestroyDimmers(int count)
    {
        while (_dimmers.size() < count)
        {
            int dipSwitch = _dimmers.size();  // dipSwitches must be consecutive starting from zero
            _dimmers.push_back(new KridaDimmer(dipSwitch));
        }
        while (_dimmers.size() > count)
        {
            auto it = _dimmers.end();
            releaseRef((*it));
            _dimmers.erase(it);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:

    int dimmerCount()
    {
        return _dimmerCount.value();
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    virtual void begin()
    {
        _artnet.begin();
        for (auto it = _dimmers.begin(); it != _dimmers.end(); it++)
        {
            (*it)->begin();
        }
    }

    virtual void loop()
    {
        _artnet.loop();
        for (auto it = _dimmers.begin(); it != _dimmers.end(); it++)
        {
            (*it)->loop();
        }
    }

    virtual void report()
    {
        _artnet.report();
        for (auto it = _dimmers.begin(); it != _dimmers.end(); it++)
        {
            (*it)->report();
        }
    }

    //----------------------------------------------------------------------------------------------
    // Packets
    //----------------------------------------------------------------------------------------------

    static int dmxCount(int dimmersCount)
    {
        return dimmersCount * sizeof(DmxKridaDimmer);
    }

    int dmxCount()
    {
        return dmxCount(dimmerCount());
    }

    void onDmxPacket(ArtDmxPacket& packet) override
    {
        byte* dmxValuesPointer = packet.dmxValuesPointer(_artnet.dmxAddress(), dmxCount());
        if (dmxValuesPointer)
        {
        }
    }

};

#endif

//
// KridaDimmersDevice.h
//
// An Artnet device that controls zero or more Krida I2c dimmers

#ifndef __KRIDA_DIMMERS_DEVICE_H__
#define __KRIDA_DIMMERS_DEVICE_H__

#include <vector>
#include "Artnet/Artnet.h"
#include "Artnet/DmxPacketConsumer.h"
#include "DmxParams/DmxKridaParameters.h"
#include "KridaDimmer.h"

struct KridaDimmersDevice : DmxPacketConsumer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:
    ArtnetDevice                _artnet;        // before _dimmerCount to keep EEPROM sane
    PersistentIntSetting        _dimmerCount;   // the *persistent* number of dimmers that are attached
    CloudVariable<int>          _dimmerCountCloudVar;

    rga::vector<KridaDimmer*>   _dimmers;       // size should be tracked to _dimmerCount

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

        _dimmerCount.setChangeNotification([this](int oldCount)
        {
            INFO("Krida: dimmer count changed: old=%d new=%d", oldCount, _dimmerCount.value());
            createOrDestroyDimmers();
            _artnet.setDmxCount(dmxCount());
        });
    }

    virtual ~KridaDimmersDevice()
    {
        clear();
        _dimmerCount.setChangeNotification(nullptr);
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
        while (_dimmers.count() < count)
        {
            // dipSwitches of attached dimmers must be consecutive starting from zero
            // though they may be in any *physical* order
            int dipSwitch = _dimmers.size();
            _dimmers.push_back(new KridaDimmer(dipSwitch));
        }
        while (_dimmers.count() > count)
        {
            auto it = _dimmers.last();
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
        _dimmerCountCloudVar.begin();
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
        int dimmerCount = this->dimmerCount();
        byte* dmxValuesPointer = packet.dmxValuesPointer(_artnet.dmxAddress(), dmxCount(dimmerCount));
        if (dmxValuesPointer)
        {
            DmxKridaParameters dmxParameters(dmxValuesPointer, dimmerCount);
            for (int i = 0; i < dimmerCount; i++)
            {
                _dimmers[i]->processDmxKridaDimmer(dmxParameters.dimmer(i));
            }
        }
    }

};

#endif

//
// KridaDimmersDevice.h
//
// An Artnet device that controls zero or more Krida I2c dimmers

#ifndef __KRIDA_DIMMERS_DEVICE_H__
#define __KRIDA_DIMMERS_DEVICE_H__

#include <vector>
#include "System/Looper.h"
#include "Artnet/Artnet.h"
#include "Artnet/DmxPacketConsumer.h"
#include "DmxParams/DmxKridaParameters.h"
#include "I2c/KridaDimmer.h"
#include "I2c/KridaGlobals.h"

struct KridaDimmersDevice : DmxPacketConsumer, Looper, ReferenceCounted
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:
    ArtnetDevice                _artnet;        // before _dimmerCount to keep EEPROM sane
    CloudVariable<int>          _dimmerCountCloudVar;

    rga::vector<KridaDimmer*>   _dimmers;       // size should be tracked to _dimmerCount

    static PersistentIntSetting _dimmerCount;   // the *persistent* number of dimmers that are attached

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    KridaDimmersDevice(int dimmerCount=1, LPCSTR shortName="Dimmers")
        : _artnet(this, DMX_ADDRESS_DEFAULT, dmxCount(dimmerCount), shortName),
          _dimmerCountCloudVar("dimmerCount", &_dimmerCount)
    {
        _dimmerCount.setDefault(dimmerCount);

        Wire.setSpeed(CLOCK_SPEED_100KHZ);
        Wire.begin();
        createOrDestroyDimmers();

        _dimmerCount.registerNotifyChanged([this](int oldCount)
        {
            INFO("Krida: dimmer count changed: old=%d new=%d", oldCount, _dimmerCount.value());
            createOrDestroyDimmers();
            _artnet.setDmxCount(dmxCount());
        });
    }

    ~KridaDimmersDevice() override
    {
        clear();
        _dimmerCount.registerNotifyChanged(nullptr);
        Wire.end();
    }

    DELEGATE_REF_COUNTING

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

    void begin() override
    {
        _artnet.begin();
        _dimmerCountCloudVar.begin();
        KridaGlobals::theInstance->begin();
        for (auto it = _dimmers.begin(); it != _dimmers.end(); it++)
        {
            (*it)->begin();
        }
    }

    void loop() override
    {
        _artnet.loop();
        _dimmerCountCloudVar.loop();
        KridaGlobals::theInstance->loop();
        for (auto it = _dimmers.begin(); it != _dimmers.end(); it++)
        {
            (*it)->loop();
        }
    }

    void report() override
    {
        _artnet.report();
        _dimmerCountCloudVar.report();
        KridaGlobals::theInstance->report();
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

    void onDmxPacket(ArtnetDmxPacket& packet) override
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

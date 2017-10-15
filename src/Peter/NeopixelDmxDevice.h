//
// NeoPixelDmxDevice.h
//
#ifndef __NEOPIXEL_DMX_DEVICE_H__
#define __NEOPIXEL_DMX_DEVICE_H__

#include "System/Looper.h"
#include "Artnet/Artnet.h"
#include "Pixels/PixelSequence.h"
#include "DmxParams/DmxColorLuminanceParameters.h"
#include "Artnet/DmxLuminanceEffectSelector.h"
#include "Artnet/DmxColorEffectSelector.h"

struct NeoPixelDmxDevice : DmxPacketConsumer, Looper, ReferenceCounted
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    PixelSequence*              _pPixels;
    ArtnetDevice                _artnet;
    DmxLuminanceEffectSelector* _pLuminanceEffectSelector;
    DmxColorEffectSelector*     _pColorEffectSelector;

    ComputedCloudVariable<String> _cloudLuminanceEffect;
    ComputedCloudVariable<String> _cloudColorEffect;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    NeoPixelDmxDevice(PixelSequence* pixels /*takes ownership*/, LPCSTR shortName)
        : _pPixels(pixels),
          _pLuminanceEffectSelector(new DmxLuminanceEffectSelector(_pPixels)),
          _pColorEffectSelector(new DmxColorEffectSelector(_pPixels)),
          _artnet(this, DMX_ADDRESS_DEFAULT, dmxCount(), shortName)
    {
        _cloudLuminanceEffect.initialize("lumEffect", [this]() { return _pLuminanceEffectSelector->effectName(); });
        _cloudColorEffect.initialize("colorEffect", [this]() { return _pColorEffectSelector->effectName(); });
    }

    ~NeoPixelDmxDevice() override
    {
        releaseRef(_pLuminanceEffectSelector);
        releaseRef(_pColorEffectSelector);
        releaseRef(_pPixels);
    }

    DELEGATE_REF_COUNTING

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin() override
    {
        _pPixels->begin();
        _artnet.begin();
        _cloudLuminanceEffect.begin();
        _cloudColorEffect.begin();
    }

    void loop() override
    {
        _pPixels->loop();
        _artnet.loop();
        _cloudLuminanceEffect.loop();
        _cloudColorEffect.loop();
    }

    void report() override
    {
        _pPixels->report();
        _artnet.report();
        _cloudLuminanceEffect.report();
        _cloudColorEffect.report();
        INFO("offsetof(ArtDmxPacketData, _data)=%d", offsetof(ArtDmxPacketData, _data));
    }

    //----------------------------------------------------------------------------------------------
    // Packets
    //----------------------------------------------------------------------------------------------

    static int dmxCount()
    {
        return sizeof(DmxColorLuminanceParametersData);
    }

    void onDmxPacket(ArtnetDmxPacket& packet) override
    {
        byte* dmxValuesPointer = packet.dmxValuesPointer(_artnet.dmxAddress(), dmxCount());
        if (dmxValuesPointer)
        {
            DmxColorLuminanceParameters parameterBlock = DmxColorLuminanceParameters(dmxValuesPointer);

            _pColorEffectSelector->processDmxColorLuminance(parameterBlock);
            _pLuminanceEffectSelector->processDmxColorLuminance(parameterBlock);

            Colorizer* pColorizer = _pPixels->colorizer();
            if (pColorizer)
            {
                pColorizer->processDmxColorLuminance(parameterBlock);
            }

            Lumenizer* pLumenizer = _pPixels->lumenizer();
            if (pLumenizer)
            {
                pLumenizer->processDmxColorLuminance(parameterBlock);
            }
        }
    }

};

#endif

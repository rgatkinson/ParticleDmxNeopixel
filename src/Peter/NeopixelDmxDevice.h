//
// NeoPixelDmxDevice.h
//
#ifndef __NEOPIXEL_DMX_DEVICE_H__
#define __NEOPIXEL_DMX_DEVICE_H__

#include "Artnet/Artnet.h"
#include "Pixels/PixelSequence.h"
#include "DmxParams/DmxColorLuminanceParameters.h"
#include "Artnet/DmxLuminanceEffectSelector.h"
#include "Artnet/DmxColorEffectSelector.h"

struct NeoPixelDmxDevice : DmxPacketConsumer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    PixelSequence*              _pPixels;
    ArtnetDevice                _artnet;
    DmxLuminanceEffectSelector<false>* _pLuminanceEffectSelector;
    DmxColorEffectSelector<false>*     _pColorEffectSelector;

    ComputedCloudVariable<String> _cloudLuminanceEffect;
    ComputedCloudVariable<String> _cloudColorEffect;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    NeoPixelDmxDevice(PixelSequence* pixels, LPCSTR shortName)
        : _pPixels(pixels),
          _pLuminanceEffectSelector(new DmxLuminanceEffectSelector<false>(_pPixels)),
          _pColorEffectSelector(new DmxColorEffectSelector<false>(_pPixels)),
          _cloudLuminanceEffect("lumEffect", [this]() { return _pLuminanceEffectSelector->effectName(); }),
          _cloudColorEffect("colorEffect", [this]() { return _pColorEffectSelector->effectName(); }),
          _artnet(this, DMX_ADDRESS_DEFAULT, dmxCount(), shortName)
    {
    }

    virtual ~NeoPixelDmxDevice()
    {
        releaseRef(_pLuminanceEffectSelector);
        releaseRef(_pColorEffectSelector);
        releaseRef(_pPixels);
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    virtual void begin()
    {
        _pPixels->begin();
        _artnet.begin();
        _cloudLuminanceEffect.begin();
        _cloudColorEffect.begin();
    }

    virtual void loop()
    {
        _pPixels->loop();
        _artnet.loop();
    }

    virtual void report()
    {
        _pPixels->report();
        _artnet.report();
        INFO("offsetof(ArtDmxPacketData, _data)=%d", offsetof(ArtDmxPacketData, _data));
    }

    //----------------------------------------------------------------------------------------------
    // Packets
    //----------------------------------------------------------------------------------------------

    static int dmxCount()
    {
        return sizeof(DmxColorLuminanceParametersData);
    }

    void onDmxPacket(ArtDmxPacket& packet) override
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

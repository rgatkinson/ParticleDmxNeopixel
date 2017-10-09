//
// NeoPixelDmxDevice.h
//
#ifndef __NEOPIXEL_DMX_DEVICE_H__
#define __NEOPIXEL_DMX_DEVICE_H__

#include "Artnet/Artnet.h"
#include "Pixels/PixelSequence.h"
#include "Artnet/ColorLuminanceParameterBlock.h"
#include "Artnet/DmxLuminanceEffectSelector.h"
#include "Artnet/DmxColorEffectSelector.h"

struct NeoPixelDmxDevice : DmxPacketConsumer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    ArtnetDevice                _artnet;
    DmxLuminanceEffectSelector* _pLuminanceEffectSelector;
    DmxColorEffectSelector*     _pColorEffectSelector;
    PixelSequence*              _pPixels;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    NeoPixelDmxDevice(PixelSequence* pixels, LPCSTR shortName)
        : _pPixels(pixels),
          _artnet(this, DMX_ADDRESS_DEFAULT, shortName)
    {
        _pLuminanceEffectSelector = new DmxLuminanceEffectSelector(_pPixels);
        _pColorEffectSelector = new DmxColorEffectSelector(_pPixels);
        _artnet.setShortName(shortName);
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

    void onDmxPacket(ArtDmxPacket& packet) override
    {
        byte* dmxValuesPointer = packet.dmxValuesPointer(_artnet.dmxAddress(), sizeof(ColorLuminanceParameterBlockData));
        if (dmxValuesPointer)
        {
            ColorLuminanceParameterBlock parameterBlock = ColorLuminanceParameterBlock(dmxValuesPointer);

            _pColorEffectSelector->processParameterBlock(parameterBlock);
            _pLuminanceEffectSelector->processParameterBlock(parameterBlock);

            Colorizer* pColorizer = _pPixels->colorizer();
            if (pColorizer)
            {
                pColorizer->processParameterBlock(parameterBlock);
            }

            Lumenizer* pLumenizer = _pPixels->lumenizer();
            if (pLumenizer)
            {
                pLumenizer->processParameterBlock(parameterBlock);
            }
        }
    }

};

#endif
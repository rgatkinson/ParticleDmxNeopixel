//
// Amulet.h
//
// Amulet design for Peter & the Starcatcher

#ifndef __AMULET_H_
#define __AMULET_H_

#include "Artnet.h"
#include "PixelRing.h"
#include "Color.h"
#include "BreathingBrightness.h"
#include "ColorizerSequence.h"
#include "DimmerSequence.h"

struct Amulet : DmxPacketConsumer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
public:

    static const int DMX_ADDRESS_COUNT = 4;

protected:

    DMX_ADDRESS     _dmxAddress;
    ArtnetDevice    _artnet;
    PixelRing       _pixels;
    COLOR_INT       _indicatorColor;
    int             _msIdleQuantum          = 1500;
    float           _idleBrightnessLevel    = 0.1f;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Amulet(DMX_ADDRESS dmxAddress, LPCSTR shortName, COLOR_INT indicatorColor)
        : _artnet(this)
    {
        _dmxAddress = dmxAddress;
        _indicatorColor = indicatorColor;

        _artnet.setShortName(shortName);

        _pixels.setColorizer(indicatorColorizer());
        _pixels.setDimmer(indicatorDimmer(2 * _pixels.colorizer()->msLoopingDuration()));
    }

    Colorizer* indicatorColorizer()
    {
        ColorizerSequence* pSequence = new ColorizerSequence();
        pSequence->addColorizer(new RainbowColors(10, _msIdleQuantum));
        pSequence->addColorizer(new ConstantColor(_indicatorColor, _msIdleQuantum * 2));
        pSequence->addColorizer(new ConstantColor(Color::BLACK, _msIdleQuantum * 6));
        pSequence->setLooping(true);
        return pSequence;
    }

    DimmerSequence* indicatorDimmer(int msDuration)
    {
        DimmerSequence* pSequence = new DimmerSequence();
        pSequence->addDimmer(new ConstantBrightness(1.0f, msDuration));
        pSequence->addDimmer(new ConstantBrightness(_idleBrightnessLevel, Deadline::Infinite));
        return pSequence;
    }

    ArtnetDevice device()
    {
        return _artnet;
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin()
    {
        _pixels.begin();
        _artnet.begin();
    }

    void loop()
    {
        _pixels.loop();
        _artnet.loop();
    }

    void report()
    {
        _pixels.report();
        _artnet.report();
    }

    //----------------------------------------------------------------------------------------------
    // Packets
    //----------------------------------------------------------------------------------------------

    override void onDmxPacket(ArtDmxPacket& packet)
    {
        int r = packet[_dmxAddress];
        int g = packet[_dmxAddress+1];
        int b = packet[_dmxAddress+2];
        int i = packet[_dmxAddress+3];
        if (r >= 0 && g >= 0 && b >= 0 && i >= 0)
        {
            COLOR_INT color = Color::rgb(r, g, b);
            // Log.info("dmx: %d,%d,%d,%d", r,g,b,i);
            _pixels.setColorizerIfDifferent(new ConstantColor(color, Deadline::Infinite));
            _pixels.setDimmerBrightness(i);
        }
    }
};

#endif

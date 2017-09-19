//
// Amulet.h
//
#ifndef __AMULET_H_
#define __AMULET_H_

#include "Artnet.h"
#include "PixelRing.h"
#include "Color.h"
#include "BreathingBrightness.h"
#include "ColorizerSequence.h"

struct Amulet : DmxPacketConsumer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
public:

    static const int DMX_ADDRESS_COUNT = 4;

protected:

    DMX_ADDRESS     _dmxAddress;
    ArtnetDevice    _device;
    PixelRing       _pixels;
    COLOR_INT       _indicatorColor;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Amulet(DMX_ADDRESS dmxAddress, LPCSTR shortName, COLOR_INT indicatorColor)
        : _device(this)
    {
        _pixels.initialize();
        _dmxAddress = dmxAddress;
        _indicatorColor = indicatorColor;

        _device.setShortName(shortName);

        _pixels.setColorizer(indicatorSequence());
        _pixels.setDimmer(new ConstantBrightness(0.1));
    }

    ColorizerSequence* indicatorSequence()
    {
        int ms = 1500;
        ColorizerSequence* pSequence = new ColorizerSequence();
        pSequence->addColorizer(new RainbowColors(), ms);
        pSequence->addColorizer(new ConstantColor(_indicatorColor), ms);
        pSequence->addColorizer(new ConstantColor(Color::BLACK), ms * 9);
        pSequence->setLooping(true);
        return pSequence;
    }

    ArtnetDevice device()
    {
        return _device;
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin()
    {
        _pixels.begin();
        _device.begin();
    }

    void loop()
    {
        _pixels.loop();
        _device.loop();
    }

    void report()
    {
        _pixels.report();
        _device.report();
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
            _pixels.setColorizerIfDifferent(new ConstantColor(color));
            _pixels.setDimming(i);
        }
    }
};

#endif

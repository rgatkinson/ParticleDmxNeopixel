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

    enum Demo { DemoNone, DemoFirst, DemoShow=DemoFirst, DemoWhite, DemoMax };

protected:

    DMX_ADDRESS     _dmxAddress;
    ArtnetDevice    _artnet;
    PixelRing       _pixels;
    COLOR_INT       _indicatorColor;
    int             _msIdleQuantum          = 1500;
    float           _idleBrightnessLevel    = 0.1f;
    Demo            _demo                   = DemoShow;


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
        setDemo();
    }

    void setDemo()
    {
        _pixels.setColorizerIfDifferent(demoColorizer());
        _pixels.setDimmerIfDifferent(demoDimmer());
    }

    Colorizer* demoColorizer()
    {
        switch (_demo)
        {
            case DemoWhite:
            {
                return new ConstantColor(Color::WHITE, Deadline::Infinite);
            }
            default:
            {
                ColorizerSequence* pSequence = new ColorizerSequence();
                pSequence->addColorizer(new RainbowColors(10, _msIdleQuantum));
                pSequence->addColorizer(new ConstantColor(_indicatorColor, _msIdleQuantum * 2));
                pSequence->addColorizer(new ConstantColor(Color::BLACK, _msIdleQuantum * 6));
                pSequence->setLooping(true);
                return pSequence;
            }
        }
    }

    DimmerSequence* demoDimmer()
    {
        switch (_demo)
        {
            case DemoWhite:
            {
                DimmerSequence* pSequence = new DimmerSequence();
                pSequence->addDimmer(new ConstantBrightness(1.0f, 5000));
                pSequence->addDimmer(new BreathingBrightness(5000, Deadline::Infinite));

                return pSequence;
            }
            default:
            {
                int msDuration = 2 * _pixels.colorizer()->msLoopingDuration();
                DimmerSequence* pSequence = new DimmerSequence();
                pSequence->addDimmer(new ConstantBrightness(1.0f, msDuration));
                pSequence->addDimmer(new ConstantBrightness(_idleBrightnessLevel, Deadline::Infinite));
                return pSequence;
            }
        }
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
        INFO("offset_of(ArtDmxPacketData, _data)=%d", offset_of(ArtDmxPacketData, _data));
    }

    void cycleDemo()
    {
        _demo = Demo(_demo + 1);
        if (_demo >= DemoMax) _demo = DemoFirst;
        setDemo();
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
        int m = packet[_dmxAddress+4];
        if (r >= 0 && g >= 0 && b >= 0 && i >= 0)
        {
            COLOR_INT color = Color::rgb(r, g, b);
            INFO("addr=%d dmx=%d,%d,%d,%d", _dmxAddress, r,g,b,i);
            switch (m)
            {
                default:
                case 0:
                    _pixels.setDimmerIfDifferent(new ConstantBrightness(1.0f, Deadline::Infinite));
                    break;
            }
            _pixels.setColorizerIfDifferent(new ConstantColor(color, Deadline::Infinite));
            _pixels.setDimmerBrightness(i);
        }
    }
};

#endif

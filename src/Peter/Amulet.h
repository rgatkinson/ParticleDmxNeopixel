//
// Amulet.h
//
// Amulet design for Peter & the Starcatcher

#ifndef __AMULET_H_
#define __AMULET_H_

#include "Artnet/Artnet.h"
#include "Pixels/PixelRing.h"
#include "Util/Color.h"
#include "Dimmers/BreathingLuminance.h"
#include "Dimmers/LumenizerSequence.h"
#include "Dimmers/TwinklingLuminance.h"
#include "Dimmers/SelfTestLuminance.h"
#include "Colorizers/ColorizerSequence.h"
#include "Colorizers/SelfTestColorizer.h"
#include "Artnet/DmxParameterBlock.h"
#include "Artnet/DmxLuminanceEffectSelector.h"
#include "Artnet/DmxColorEffectSelector.h"

struct Amulet : DmxPacketConsumer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
public:

    static const int DMX_ADDRESS_COUNT = sizeof(DmxParameterBlock);

    enum Demo { DemoNone, DemoFirst, DemoShow=DemoFirst, DemoWhite, DemoMax };

protected:

    DMX_ADDRESS                     _dmxAddress;
    ArtnetDevice                    _artnet;
    DmxLuminanceEffectSelector*     _pLuminanceEffectSelector;
    DmxColorEffectSelector*         _pColorEffectSelector;
    PixelRing*                      _pPixels;
    COLOR_INT                       _indicatorColor;
    Demo                            _demo = DemoShow;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Amulet(DMX_ADDRESS dmxAddress, LPCSTR shortName, COLOR_INT indicatorColor)
        : _artnet(this)
    {
        _pPixels = new PixelRing();
        _pLuminanceEffectSelector = new DmxLuminanceEffectSelector(_pPixels);
        _pColorEffectSelector = new DmxColorEffectSelector(_pPixels);
        _dmxAddress = dmxAddress;
        _indicatorColor = indicatorColor;

        _artnet.setShortName(shortName);
        setDemo();
    }

    ~Amulet()
    {
        releaseRef(_pLuminanceEffectSelector);
        releaseRef(_pColorEffectSelector);
        releaseRef(_pPixels);
    }

    void setDemo()
    {
        _pPixels->setColorizer(demoColorizer());
        _pPixels->setLumenizer(demoLumenizer());
    }

    Colorizer* demoColorizer()
    {
        switch (_demo)
        {
            case DemoWhite:
            {
                COLOR_INT color = Color::temperature(2550);
                Log.info("demo white: r=%d g=%d b=%d", Color::red(color), Color::green(color), Color::blue(color));
                return new UniformColor(color, Deadline::Infinite);
            }
            default:
            {
                return new UniformColor(Color::temperature(2550), Deadline::Infinite);
            }
        }
    }

    Lumenizer* demoLumenizer()
    {
        switch (_demo)
        {
            case DemoWhite:
            {
                LumenizerSequence* pSequence = new LumenizerSequence();
                pSequence->addLumenizer(new UniformLuminance(1.0f, 5000));
                pSequence->addLumenizer(new BreathingLuminance(0, 5000, Deadline::Infinite));

                return pSequence;
            }
            default:
            {
                return new TwinklingLuminance(
                    TwinklingLuminance::msTwinklePauseDefault,
                    TwinklingLuminance::msTwinkleIntervalDefault,
                    Deadline::Infinite);
            }
        }
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin()
    {
        _pPixels->begin();
        _artnet.begin();
    }

    void loop()
    {
        _pPixels->loop();
        _artnet.loop();
    }

    void report()
    {
        _pPixels->report();
        _artnet.report();
        INFO("offsetof(ArtDmxPacketData, _data)=%d", offsetof(ArtDmxPacketData, _data));
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

    void onDmxPacket(ArtDmxPacket& packet) override
    {
        DmxParameterBlock parameterBlock = DmxParameterBlock(packet.pDmx(_dmxAddress));

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
};

#endif

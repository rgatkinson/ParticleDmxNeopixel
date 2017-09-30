//
// Stardrop.h
//
#ifndef _STARDROP_H_
#define _STARDROP_H_

#include "Artnet/Artnet.h"
#include "Pixels/StardropPixels.h"
#include "Util/Color.h"
#include "Lumenizers/BreathingLuminance.h"
#include "Lumenizers/LumenizerSequence.h"
#include "Lumenizers/TwinklingLuminance.h"
#include "Lumenizers/AmuletSelfTestLuminance.h"
#include "Lumenizers/MorseCodeLuminance.h"
#include "Colorizers/ColorizerSequence.h"
#include "Colorizers/AmuletSelfTestColorizer.h"
#include "Artnet/ColorLuminanceParameterBlock.h"
#include "Artnet/DmxLuminanceEffectSelector.h"
#include "Artnet/DmxColorEffectSelector.h"

struct Stardrop : DmxPacketConsumer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
public:

    static const int DMX_ADDRESS_COUNT = sizeof(ColorLuminanceParameterBlock);

protected:

    enum class Demo
    {
        None,
        First,
        Second,
        Third,
        Fourth,
        Fifth,
        Sixth,
        Seventh,
        Eigth,
        Max,
        Default=First
    };

    DMX_ADDRESS                     _dmxAddress;
    ArtnetDevice                    _artnet;
    DmxLuminanceEffectSelector*     _pLuminanceEffectSelector;
    DmxColorEffectSelector*         _pColorEffectSelector;
    StardropPixels*                 _pPixels;
    Demo                            _demo = Demo::Default;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Stardrop(DMX_ADDRESS dmxAddress, LPCSTR shortName)
        : _artnet(this)
    {
        _pPixels = new StardropPixels();
        _pLuminanceEffectSelector = new DmxLuminanceEffectSelector(_pPixels);
        _pColorEffectSelector = new DmxColorEffectSelector(_pPixels);
        _dmxAddress = dmxAddress;

        _artnet.setShortName(shortName);
        setDemo();
    }

    ~Stardrop()
    {
        releaseRef(_pLuminanceEffectSelector);
        releaseRef(_pColorEffectSelector);
        releaseRef(_pPixels);
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

    //----------------------------------------------------------------------------------------------
    // Demo
    //----------------------------------------------------------------------------------------------

    void setDemo()
    {
        float kelvin;
        switch (_demo)
        {
            case Demo::First:   kelvin = 2000; break;
            default:
            case Demo::Second:  kelvin = 2550; break;
            case Demo::Third:   kelvin = 2800; break;
            case Demo::Fourth:  kelvin = 3000; break;
            case Demo::Fifth:   kelvin = 3500; break;
            case Demo::Sixth:   kelvin = 4000; break;
            case Demo::Seventh: kelvin = 4500; break;
            case Demo::Eigth:   kelvin = 5000; break;
        }

        Colorizer* pColorizer = new UniformColor(Color::temperature(kelvin), Deadline::Infinite);
        Lumenizer* pLumenizer = new TwinklingLuminance(
            TwinklingLuminance::msTwinklePauseDefault,
            TwinklingLuminance::msTwinkleBreatheDefault,
            Deadline::Infinite);
        pLumenizer->setDimmerLevel(0.75f);
        //
        _pPixels->ownColorizer(pColorizer);
        _pPixels->ownLumenizer(pLumenizer);
    }

    void cycleDemo()
    {
        _demo = ::cycle(_demo, Demo::First, Demo::Max);
        setDemo();
    }

    //----------------------------------------------------------------------------------------------
    // Packets
    //----------------------------------------------------------------------------------------------

    void onDmxPacket(ArtDmxPacket& packet) override
    {
        ColorLuminanceParameterBlock parameterBlock = ColorLuminanceParameterBlock(packet.pDmx(_dmxAddress));

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

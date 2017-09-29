//
// Stardrop.h
//
#ifndef _STARDROP_H_
#define _STARDROP_H_

#include "Artnet/Artnet.h"
#include "Pixels/PixelRing.h"
#include "Util/Color.h"
#include "Lumenizers/BreathingLuminance.h"
#include "Lumenizers/LumenizerSequence.h"
#include "Lumenizers/TwinklingLuminance.h"
#include "Lumenizers/AmuletSelfTestLuminance.h"
#include "Lumenizers/MorseCodeLuminance.h"
#include "Colorizers/ColorizerSequence.h"
#include "Colorizers/AmuletSelfTestColorizer.h"
#include "Artnet/DmxParameterBlock.h"
#include "Artnet/DmxLuminanceEffectSelector.h"
#include "Artnet/DmxColorEffectSelector.h"

struct Stardrop : DmxPacketConsumer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
public:

    static const int DMX_ADDRESS_COUNT = sizeof(DmxParameterBlock);

protected:

    DMX_ADDRESS                     _dmxAddress;
    ArtnetDevice                    _artnet;
    DmxLuminanceEffectSelector*     _pLuminanceEffectSelector;
    DmxColorEffectSelector*         _pColorEffectSelector;
    PixelRing*                      _pPixels;
    DmxEffectSelector::Demo         _demo = DmxEffectSelector::Demo::Default;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Stardrop(DMX_ADDRESS dmxAddress, LPCSTR shortName)
        : _artnet(this)
    {
        _pPixels = new PixelRing();
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

    void setDemo()
    {
        Colorizer* pColorizer = new UniformColor(Color::temperature(2550), Deadline::Infinite);
        Lumenizer* pLumenizer = new TwinklingLuminance(
            TwinklingLuminance::msTwinklePauseDefault,
            TwinklingLuminance::msTwinkleBreatheDefault,
            Deadline::Infinite);
        pLumenizer->setDimmerLevel(0.75f);
        DmxEffectSelector::setDemo(_pPixels, _demo, pColorizer, pLumenizer);
        releaseRef(pColorizer);
        releaseRef(pLumenizer);
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
        _demo = DmxEffectSelector::cycle(_demo);
        if (_demo >= DmxEffectSelector::Demo::Max) _demo = DmxEffectSelector::Demo::First;
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

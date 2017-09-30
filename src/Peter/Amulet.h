//
// Amulet.h
//
#ifndef __AMULET_H_
#define __AMULET_H_

#include "Artnet/Artnet.h"
#include "Pixels/ParticleRing.h"
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

struct Amulet : DmxPacketConsumer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
public:

    static const int DMX_ADDRESS_COUNT = sizeof(ColorLuminanceParameterBlock);

protected:

    DMX_ADDRESS                     _dmxAddress;
    ArtnetDevice                    _artnet;
    DmxLuminanceEffectSelector*     _pLuminanceEffectSelector;
    DmxColorEffectSelector*         _pColorEffectSelector;
    ParticleRing*                      _pPixels;
    COLOR_INT                       _indicatorColor;
    DmxEffectSelector::Demo         _demo = DmxEffectSelector::Demo::Default;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Amulet(DMX_ADDRESS dmxAddress, LPCSTR shortName, COLOR_INT indicatorColor)
        : _artnet(this)
    {
        _pPixels = new ParticleRing();
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
        Colorizer* pColorizer = new AmuletSelfTestColorizer(_indicatorColor);
        Lumenizer* pLumenizer = new AmuletSelfTestLuminance(pColorizer);
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

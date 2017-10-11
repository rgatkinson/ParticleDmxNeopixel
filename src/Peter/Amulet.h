//
// Amulet.h
//
#ifndef __AMULET_H_
#define __AMULET_H_

#include "NeopixelDmxDevice.h"
#include "Pixels/ParticleRing.h"
#include "Util/Color.h"
#include "Lumenizers/AmuletSelfTestLuminance.h"
#include "Colorizers/AmuletSelfTestColorizer.h"
#include "Artnet/DmxEffectSelector.h"

struct Amulet : NeoPixelDmxDevice
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    DmxEffectSelector::Demo _demo = DmxEffectSelector::Demo::Default;
    int                     _buttonToken;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Amulet(LPCSTR shortName="Amulet")
        : NeoPixelDmxDevice(new ParticleRing(), shortName)
    {
        setDemo();
        _buttonToken = SystemEventRegistrar::theInstance->registerButtonFinalClick(
            [this](int clickCount)
            {
                cycleDemo();
            });
    }

    ~Amulet()
    {
        SystemEventRegistrar::theInstance->unregisterButtonClick(_buttonToken);
    }

    void setDemo()
    {
        INFO("amulet demo #%d", (int)_demo);
        Colorizer* pColorizer = new AmuletSelfTestColorizer();
        Lumenizer* pLumenizer = new AmuletSelfTestLuminance(2,3);
        DmxEffectSelector::setDemo(_pPixels, _demo, pColorizer, pLumenizer);
        releaseRef(pColorizer);
        releaseRef(pLumenizer);
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void cycleDemo()
    {
        _demo = DmxEffectSelector::cycle(_demo);
        setDemo();
    }
};

#endif

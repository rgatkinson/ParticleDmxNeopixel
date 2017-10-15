//
// Crate.h
//
#ifndef __CRATE_H__
#define __CRATE_H__

#include "Util/Misc.h"
#include "Util/Color.h"
#include "Peter/NeopixelDmxDevice.h"
#include "Pixels/MiniSkinnyNeoPixelStrip.h"
#include "Lumenizers/AmuletSelfTestLuminance.h"
#include "Colorizers/AmuletSelfTestColorizer.h"
#include "Artnet/DmxDemoManager.h"

struct Crate : NeoPixelDmxDevice
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    DmxDemoManager::Demo _demo = DmxDemoManager::Demo::Default;
    int                  _buttonToken;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Crate(int pixelCount=60, LPCSTR shortName="Crate")
        : NeoPixelDmxDevice(new MiniSkinnyNeoPixelStrip(pixelCount), shortName)
    {
        setDemo();
        _buttonToken = SystemEventRegistrar::theInstance->registerButtonFinalClick(
            [this](int clickCount)
            {
                cycleDemo();
            });
    }

    ~Crate()
    {
        SystemEventRegistrar::theInstance->unregisterButtonClick(_buttonToken);
    }

    void setDemo()
    {
        INFO("crate demo #%d", (int)_demo);
        Colorizer* pColorizer = new AmuletSelfTestColorizer();
        Lumenizer* pLumenizer = new AmuletSelfTestLuminance(2,3);
        DmxDemoManager::setDemo(_pPixels, _demo, pColorizer, pLumenizer);
        releaseRef(pColorizer);
        releaseRef(pLumenizer);
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void cycleDemo()
    {
        _demo = DmxDemoManager::cycle(_demo);
        setDemo();
    }
};

#endif

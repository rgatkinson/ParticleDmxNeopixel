//
// Trunk.h
//
#ifndef __TRUNK_H__
#define __TRUNK_H__

#include "Util/Misc.h"
#include "Util/Color.h"
#include "Peter/NeopixelDmxDevice.h"
#include "Pixels/MiniSkinnyNeoPixelStrip.h"
#include "Lumenizers/BreathingLuminance.h"
#include "Colorizers/UniformColor.h"
#include "Artnet/DmxDemoManager.h"

struct Trunk : NeoPixelDmxDevice
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

    Trunk(int pixelCount=60, LPCSTR shortName="Trunk")
        : NeoPixelDmxDevice(new MiniSkinnyNeoPixelStrip(pixelCount), shortName)
    {
        setDemo();
        _buttonToken = SystemEventRegistrar::theInstance->registerButtonFinalClick(
            [this](int clickCount)
            {
                cycleDemo();
            });
    }

    ~Trunk()
    {
        SystemEventRegistrar::theInstance->unregisterButtonClick(_buttonToken);
    }

    void setDemo()
    {
        INFO("Trunk demo #%d", (int)_demo);
        Colorizer* pColorizer = new UniformColor(Color::rgb(0,128,0));
        Lumenizer* pLumenizer = new BreathingLuminance(6000,4000);
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

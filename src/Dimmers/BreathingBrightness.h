//
// BreathingBrightness.h
//
#ifndef __BREATHING_BRIGHTNESS_H__
#define __BREATHING_BRIGHTNESS_H__

#include "Dimmer.h"
#include "Breather.h"
#include <math.h>

struct BreathingBrightness : Dimmer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    Breather _breather;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    BreathingBrightness(int msInterval, int msDuration) : Dimmer(Flavor::Breathing, msDuration)
    {
        setInterval(msInterval);
        setMinBrightness(20);    // empiricly determined
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
protected:

    bool usesGammaCorrection() override
    {
        return false;
    }

    void setInterval(int msInterval)
    {
        _breather.setInterval(msInterval);
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    void processParameterBlock(DmxParameterBlock& parameterBlock) override
    {
        Dimmer::processParameterBlock(parameterBlock);
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    void begin() override
    {
        Dimmer::begin();
        _breather.begin();
    }

    void loop() override
    {
        Dimmer::loop();
        _breather.loop();
        setCurrentLevel(_breather.currentLevel());
    }

    void report() override
    {
        Dimmer::report();
        INFO("BreathingBrightness: interval=%d", _breather.msInterval());
    }

};

#endif

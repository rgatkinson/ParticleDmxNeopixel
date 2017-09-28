//
// BreathingLumenizer.h
//
#ifndef __BREATHING_LUMENIZER_H__
#define __BREATHING_LUMENIZER_H__

#include "Lumenizer.h"
#include "Breather.h"
#include <math.h>

struct BreathingLuminance : Lumenizer
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

    BreathingLuminance(int msInterval, int msDuration) : Lumenizer(Flavor::Breathing, msDuration)
    {
        setBreatheInterval(msInterval);
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

    void setBreatheInterval(int msInterval)
    {
        _breather.setBreatheInterval(msInterval);
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    void processParameterBlock(DmxParameterBlock& parameterBlock) override
    {
        Lumenizer::processParameterBlock(parameterBlock);
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    void begin() override
    {
        Lumenizer::begin();
        _breather.begin();
    }

    void loop() override
    {
        Lumenizer::loop();
        _breather.loop();
        setCurrentLevel(_breather.currentLevel());
    }

    void report() override
    {
        Lumenizer::report();
        INFO("BreathingLuminance: breathe interval=%d", _breather.breathInterval());
    }

};

#endif

//
// BreathingBrightness.h
//
#ifndef __BREATHING_BRIGHTNESS_H__
#define __BREATHING_BRIGHTNESS_H__

#include "Dimmer.h"
#include <math.h>

struct BreathingBrightness : Dimmer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    int         _msInterval;
    float       _floatIntervalInverse;
    ElapsedTime _intervalTimer;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    BreathingBrightness(int msInterval, int msDuration) : Dimmer(Flavor::Breathing, msDuration)
    {
        _msInterval = msInterval;
        _floatIntervalInverse = 1.0f / (float)_msInterval;
        _intervalTimer = ElapsedTime();
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

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    void begin() override
    {
        Dimmer::begin();
        _intervalTimer.reset();
    }

    void loop() override
    {
        Dimmer::loop();

        int ms = _intervalTimer.milliseconds() % _msInterval;
        float intervalFraction = (float)ms * _floatIntervalInverse;

        // linear up, linear down: the eye perceives it differently
        float level = (intervalFraction <= 0.5f)
            ? (intervalFraction * 2.0f)
            : 1.0f - ( (intervalFraction - 0.5f) * 2.0f );

        setCurrentLevel(level);
    }

    void report() override
    {
        Dimmer::report();
        INFO("BreathingBrightness: interval=%d", _msInterval);
    }

};

#endif

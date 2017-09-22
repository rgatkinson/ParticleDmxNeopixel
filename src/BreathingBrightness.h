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

    int _msInterval;
    float _floatIntervalInverse;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    BreathingBrightness(int msInterval) : Dimmer(msInterval)
    {
        _msInterval = msInterval;
        _floatIntervalInverse = 1.0f / (float)_msInterval;
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    override BRIGHTNESS currentBrightness()
    {
        // We specifically want to NOT be gamma-corrected
        return rawCurrentBrightness();
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    override void begin()
    {
        Dimmer::begin();
    }

    override void loop()
    {
        Dimmer::loop();

        int ms = _duration.milliseconds() % _msInterval;
        float intervalFraction = (float)ms * _floatIntervalInverse;

        // linear up, linear down: the eye perceives it differently
        float level = (intervalFraction <= 0.5f)
            ? (intervalFraction * 2.0f)
            : 1.0f - ( (intervalFraction - 0.5f) * 2.0f );

        setCurrentLevel(level);
    }

    override void report()
    {
        Dimmer::report();
        INFO("BreathingBrightness: interval=%d", _msInterval);
    }

};

#endif

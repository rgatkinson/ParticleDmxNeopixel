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

    int msInterval;
    float floatIntervalInverse;
    ElapsedTime timer;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    BreathingBrightness(int msInterval=4000)
    {
        this->msInterval = msInterval;
        this->floatIntervalInverse = 1.0f / (float)msInterval;
        this->timer.reset();
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
        Dimmer::begin(); // pro forma
        timer.reset();
    }

    override void loop()
    {
        Dimmer::loop(); // pro forma

        int ms = timer.milliseconds() % msInterval;
        float intervalFraction = (float)ms * floatIntervalInverse;

        // linear up, linear down: the eye perceives it differently
        float level = (intervalFraction <= 0.5f)
            ? (intervalFraction * 2.0f)
            : 1.0f - ( (intervalFraction - 0.5f) * 2.0f );

        setCurrentLevel(level);
    }
};

#endif

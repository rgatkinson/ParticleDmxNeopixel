//
// UniformBrightness.h
//
#ifndef _CONSTANT_BRIGHTNESS_H_
#define _CONSTANT_BRIGHTNESS_H_

#include "Dimmer.h"

struct UniformBrightness : Dimmer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    float _level;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    UniformBrightness(float level, int msDuration) : Dimmer(Flavor::Uniform, msDuration)
    {
        _level = level;
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin() override
    {
        Dimmer::begin();
        setCurrentLevel(_level);
    }

    void loop() override
    {
        Dimmer::loop();
    }

    void report() override
    {
        Dimmer::report();
    }

};

#endif
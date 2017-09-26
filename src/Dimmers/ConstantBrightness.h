//
// ConstantBrightness.h
//
#ifndef _CONSTANT_BRIGHTNESS_H_
#define _CONSTANT_BRIGHTNESS_H_

#include "Dimmer.h"

struct ConstantBrightness : Dimmer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    float _level;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    ConstantBrightness(float level, int msDuration) : Dimmer(DimmerFlavorConstant, msDuration)
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

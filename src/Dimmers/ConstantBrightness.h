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

    override void begin()
    {
        Dimmer::begin();
        setCurrentLevel(_level);
    }

    override void loop()
    {
        Dimmer::loop();
    }

    override void report()
    {
        Dimmer::report();
    }

};

#endif

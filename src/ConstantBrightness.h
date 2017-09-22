//
// ConstantBrightness.h
//
#ifndef _CONSTANT_BRIGHTNESS_H_
#define _CONSTANT_BRIGHTNESS_H_

#include "Dimmer.h"

struct ConstantBrightness : Dimmer
{
    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    ConstantBrightness(float level = 1.0f) : Dimmer(Deadline::Infinite)
    {
        setConstantBrightness(level);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    void setConstantBrightness(float f)
    {
        setCurrentLevel(f);
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
    }

    override void report()
    {
        Dimmer::report();
    }

};

#endif

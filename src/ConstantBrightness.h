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

    ConstantBrightness(float level = 1.0f)
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
        Dimmer::begin(); // pro forma
    }

    override void loop()
    {
        Dimmer::loop(); // pro forma
    }

};

#endif

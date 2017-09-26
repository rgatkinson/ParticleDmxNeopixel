//
// Colorizeable.h
//
#ifndef __COLORIZABLE_H__
#define __COLORIZABLE_H__

#include "Util/ReferenceCounted.h"
struct Colorizer;
struct Dimmer;

//==================================================================================================
// Colorizeable
//==================================================================================================

struct Colorizeable : RefCountable
{
    virtual void setPixelColor(uint16_t iPixel, COLOR_INT color) = 0;
    virtual int  numberOfPixels() = 0;
    virtual void setColorizer(Colorizer*) = 0;
    virtual void setDimmer(Dimmer*) = 0;
};

#endif

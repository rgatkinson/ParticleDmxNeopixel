//
// Colorizeable.h
//
#ifndef __COLORIZABLE_H__
#define __COLORIZABLE_H__

#include "Util/ReferenceCounted.h"
struct Colorizer;
struct Lumenizer;
typedef int BRIGHTNESS;

//==================================================================================================
// Colorizeable
//==================================================================================================

struct Colorizeable : RefCountable
{
    virtual void setPixelColor(uint16_t iPixel, COLOR_INT color) = 0;
    virtual int  numberOfPixels() = 0;
    virtual void ownColorizer(Colorizer*) = 0;
    virtual void ownLumenizer(Lumenizer*) = 0;

    virtual Colorizer* colorizer() = 0;
    virtual Lumenizer* lumenizer() = 0;

    virtual void setDimmerLevel(float dimmerLevel) = 0;
};

#endif

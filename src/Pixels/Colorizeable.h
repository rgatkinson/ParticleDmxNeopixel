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
// Lumenizeable
//==================================================================================================

struct Lumenizeable : RefCountable
{
    virtual void ownLumenizer(Lumenizer*) = 0;
    virtual Lumenizer* lumenizer() = 0;
};

//==================================================================================================
// Colorizeable
//==================================================================================================

struct Colorizeable : Lumenizeable
{
    virtual void ownColorizer(Colorizer*) = 0;
    virtual Colorizer* colorizer() = 0;
    virtual int  numberOfPixels() = 0;
    virtual void setPixelColor(uint16_t iPixel, Color color) = 0;
};

#endif

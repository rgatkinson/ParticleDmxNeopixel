//
// PixelRing.h
//
#ifndef _PIXEL_RING_H_
#define _PIXEL_RING_H_

#include "PixelSequence.h"

struct PixelRing : PixelSequence
{
    static const int TYPE = WS2812B;

    PixelRing() : PixelSequence(24, D6, TYPE)
    {
    }
};

#endif

//
// ParticleRing.h
//
#ifndef _PIXEL_RING_H_
#define _PIXEL_RING_H_

#include "PixelSequence.h"

struct ParticleRing : PixelSequence
{
    static const int TYPE = WS2812B;

    ParticleRing() : PixelSequence(24, D6, TYPE)
    {
    }
};

#endif

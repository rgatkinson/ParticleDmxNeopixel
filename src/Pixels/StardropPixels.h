//
// StardropPixels.h
//
#ifndef _STARDROP_PIXELS_H_
#define _STARDROP_PIXELS_H_

struct StardropPixels : PixelSequence
{
    static const int TYPE = WS2812B;

    StardropPixels() : PixelSequence(10, D6, TYPE)
    {
    }
};

#endif

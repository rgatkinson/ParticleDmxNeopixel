//
// MiniSkinnyNeoPixelStrip.h
//
// https://www.adafruit.com/product/2964

#ifndef __MINI_SKINNY_NEOPIXEL_STRIP_H__
#define __MINI_SKINNY_NEOPIXEL_STRIP_H__

struct MiniSkinnyNeoPixelStrip: PixelSequence
{
    static const int TYPE = WS2812;

    MiniSkinnyNeoPixelStrip(int pixelCount) : PixelSequence(pixelCount, D6, TYPE)
    {
    }
};

#endif

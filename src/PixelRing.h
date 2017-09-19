//
// PixelRing.h
//
#ifndef _PIXEL_RING_H_
#define _PIXEL_RING_H_

#include "PixelSequence.h"
#include "ConstantColor.h"
#include "RainbowColors.h"

struct PixelRing : PixelSequence<24, D6, WS2812B>
{
    override void initialize()
    {
        int ms = 500;
        ColorizerSequence* pSequence = new ColorizerSequence();
        pSequence->addColorizer(new RainbowColors(), ms * 3);
        pSequence->addColorizer(new ConstantColor(Color::RED), ms);
        pSequence->addColorizer(new ConstantColor(Color::GREEN), ms);
        pSequence->addColorizer(new ConstantColor(Color::BLUE), ms);
        pSequence->addColorizer(new RainbowColors(), ms * 3);
        pSequence->addColorizer(new ConstantColor(Color::BLACK), ElapsedTime::Infinite);
        setColorizer(pSequence);

        setDimmer(new ConstantBrightness());
    }

};

#endif

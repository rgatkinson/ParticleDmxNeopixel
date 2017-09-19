//
// ConstantColor.h
//
#ifndef __CONSTANT_COLOR_H__
#define __CONSTANT_COLOR_H__

#include "Colorizer.h"

struct ConstantColor : Colorizer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    COLOR_INT color;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    ConstantColor(COLOR_INT color = Color::BLACK) : Colorizer(ColorizerModeConstant)
    {
        this->color = color;
        colorUpdateDeadline = Deadline(Deadline::Infinite);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    override bool sameAs(Colorizer* pThem)
    {
        bool result = Colorizer::sameAs(pThem);
        if (result)
        {
            result = this->color == static_cast<ConstantColor*>(pThem)->color;
        }
        return result;
    }

    //----------------------------------------------------------------------------------------------
    // Loop (no begin() is intentional: use ctor)
    //----------------------------------------------------------------------------------------------
public:

    override void begin()
    {
        colorUpdateDeadline.expire();
    }

    override void loop()
    {
        Colorizer::loop(); // pro forma
        if (colorUpdateDeadline.hasExpired())
        {
            for (uint16_t iPixel=0; iPixel < pixelCount; iPixel++)
            {
                setPixelColor(iPixel, color);
            }
            colorUpdateDeadline.reset();
        }
    }

    override void report()
    {
        Colorizer::report(); // pro forma
        Log.info("ConstantColor: color=0x%04x", color);
    }
};

#endif

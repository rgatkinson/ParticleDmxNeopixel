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

    COLOR_INT   _color;
    Deadline    _colorUpdateDeadline;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    ConstantColor(COLOR_INT color, int msDuration) : Colorizer(ColorizerFlavorConstant, msDuration)
    {
        _color = color;
        _colorUpdateDeadline = Deadline(Deadline::Infinite);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    bool sameAs(Colorizer* pThem) override
    {
        bool result = Colorizer::sameAs(pThem);
        if (result)
        {
            result = _color == static_cast<ConstantColor*>(pThem)->_color;
        }
        return result;
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    void begin() override
    {
        Colorizer::begin();
        _colorUpdateDeadline.expire();
    }

    void loop() override
    {
        Colorizer::loop(); // pro forma
        if (_colorUpdateDeadline.hasExpired())
        {
            for (uint16_t iPixel=0; iPixel < _pixelCount; iPixel++)
            {
                setPixelColor(iPixel, _color);
            }
            _colorUpdateDeadline.reset();
        }
    }

    void report() override 
    {
        Colorizer::report(); // pro forma
        INFO("ConstantColor: color=0x%04x", _color);
    }
};

#endif

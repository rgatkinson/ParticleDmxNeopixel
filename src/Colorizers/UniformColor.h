//
// UniformColor.h
//
#ifndef __CONSTANT_COLOR_H__
#define __CONSTANT_COLOR_H__

#include "Colorizer.h"

struct UniformColor : Colorizer
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

    UniformColor(COLOR_INT color, int msDuration) : Colorizer(Flavor::Uniform, msDuration)
    {
        _color = color;
        _colorUpdateDeadline = Deadline(Deadline::Infinite);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    COLOR_INT color()
    {
        return _color;
    }
    void setColor(COLOR_INT color)
    {
        if (_color != color)
        {
            _color = color;
            begin();
        }
    }

    bool sameAs(Colorizer* pThem) override
    {
        bool result = Colorizer::sameAs(pThem);
        if (result)
        {
            result = _color == static_cast<UniformColor*>(pThem)->_color;
        }
        return result;
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    void processParameterBlock(DmxParameterBlock& parameterBlock) override
    {
        Colorizer::processParameterBlock(parameterBlock);
        COLOR_INT colorDesired = parameterBlock.effectiveColor();
        setColor(colorDesired);
    }

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
        INFO("UniformColor: color=0x%04x", _color);
    }
};

#endif

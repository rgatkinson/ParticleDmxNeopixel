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

    UniformColor(COLOR_INT color) : UniformColor(Duration::Infinite, color)
    {
    }

    UniformColor(Duration duration, COLOR_INT color) : Colorizer(Flavor::Uniform, duration)
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
            INFO("UniformColor: color=0x%08x", color);
            _color = color;
            _colorUpdateDeadline.expire();
        }
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    void processParameterBlock(ColorLuminanceParameterBlock& parameterBlock) override
    {
        Colorizer::processParameterBlock(parameterBlock);
        COLOR_INT colorDesired = parameterBlock.effectiveColor();
        setColor(colorDesired);
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
        INFO("UniformColor: color=0x%04x", _color);
    }
};

#endif

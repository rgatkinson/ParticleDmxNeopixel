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

    Color       _color;
    Deadline    _colorUpdateDeadline;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    UniformColor(Color color) : UniformColor(Duration::Infinite, color)
    {
    }

    UniformColor(Duration duration, Color color) : Colorizer(Flavor::Uniform, duration)
    {
        _color = color;
        _colorUpdateDeadline = Deadline(Deadline::Infinite);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    Color color()
    {
        return _color;
    }
    void setColor(Color color)
    {
        if (_color != color)
        {
            INFO("UniformColor: color=0x%08x", color.value());
            _color = color;
            _colorUpdateDeadline.expire();
        }
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    void processDmxColorLuminance(DmxColorLuminanceParameters& parameterBlock) override
    {
        Colorizer::processDmxColorLuminance(parameterBlock);
        Color colorDesired = parameterBlock.effectiveColor();
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
        INFO("UniformColor: color=0x%04x", _color.value());
    }
};

#endif

//
// RainbowColors.h
//
#ifndef __RAINBOW_COLORS__
#define __RAINBOW_COLORS__

#include <math.h>

struct RainbowColors : Colorizer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    Deadline    _colorUpdateDeadline;
    int         _pixelOffset = 0;
    int         _pixelIncrement = 1;
    float       _wheelFractionVisible = 0.1f;
    int         _msOriginalInterval;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    RainbowColors(int msInterval, int msDuration) : Colorizer(Flavor::Rainbow, msDuration)
    {
        _colorUpdateDeadline = Deadline(msInterval);
        _msOriginalInterval = msInterval;
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    bool sameAs(Colorizer* pThem) override
    {
        bool result = Colorizer::sameAs(pThem);
        if (result)
        {
            result = this->_colorUpdateDeadline.msDuration() == static_cast<RainbowColors*>(pThem)->_colorUpdateDeadline.msDuration();
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
        float speed = parameterBlock.colorSpeedLevel();
        int pixelIncrement = speed > 0 ? 1 : (speed < 0 ? -1 : 0);

        const float msMin = 5;      const float maxPerSecond = 1000 / msMin;
        const float msMax = 100;    const float minPerSecond = 1000 / msMax;

        const float perSecond = scaleRange(fabs(speed), 0, 1, minPerSecond, maxPerSecond);
        const int msInterval = 1000 / perSecond;

        if (pixelIncrement != _pixelIncrement || msInterval != _colorUpdateDeadline.msDuration())
        {
            _pixelIncrement = pixelIncrement;
            _colorUpdateDeadline = Deadline(msInterval);
            _colorUpdateDeadline.expire();
        }
    }

    void begin() override
    {
        // INFO("RainbowColors::begin()");
        Colorizer::begin();
        _pixelOffset = 0;
        _pixelIncrement = 1;
        _colorUpdateDeadline = Deadline(_msOriginalInterval);
        _colorUpdateDeadline.expire();
    }

    void loop() override
    {
        Colorizer::loop();
        if (_colorUpdateDeadline.hasExpired())
            {
                // INFO("RainbowColors: pixelOffset=%d", _pixelOffset);
                for (uint16_t iPixel=0; iPixel < _pixelCount; iPixel++)
                {
                    COLOR_INT color = Color::wheelSin((iPixel+_pixelOffset) / float(_pixelCount) * _wheelFractionVisible);
                    setPixelColor(iPixel, color);
                }
                _colorUpdateDeadline.reset();
                _pixelOffset += _pixelIncrement;
            }
    }

    void report() override
    {
        Colorizer::report();
        INFO("RainbowColors");
    }

};

#endif

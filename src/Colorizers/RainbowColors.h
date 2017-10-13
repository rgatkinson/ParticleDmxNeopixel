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

    Deadline    _timer;
    int         _pixelOffset = 0;
    int         _pixelIncrement = 1;
    float       _wheelFractionVisible = wheelFractionVisibleDefault;
    int         _msOriginalInterval;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    static const int msIntervalDefault = 10;
    static constexpr float wheelFractionVisibleDefault = 0.1f;

    RainbowColors(int msInterval) : RainbowColors(Duration::Infinite, msInterval)
    {
    }

    RainbowColors(Duration duration, int msInterval) : Colorizer(Flavor::Rainbow, duration)
    {
        _timer = Deadline(msInterval);
        _msOriginalInterval = msInterval;
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    // colorSpeedLevel() controls rate and direction of rainbow rotation
    // colorControl() controls fraction of color wheel visible
    void processDmxColorLuminance(DmxColorLuminanceParameters& parameterBlock) override
    {
        Colorizer::processDmxColorLuminance(parameterBlock);
        float speed = parameterBlock.colorSpeedLevel();
        int pixelIncrement = speed > 0 ? 1 : (speed < 0 ? -1 : 0);

        const float msMin = 5;      const float maxPerSecond = 1000 / msMin;
        const float msMax = 100;    const float minPerSecond = 1000 / msMax;

         float perSecond = scaleRange(fabs(speed), 0, 1, minPerSecond, maxPerSecond);
        int msInterval = 1000 / perSecond;

        // Zero is a non-stopped default
        if (parameterBlock.colorSpeed()==0)
        {
            pixelIncrement = -1;
            msInterval = msIntervalDefault;
        }

        // Zero is a pleasant default
        float fraction = parameterBlock.colorControl()==0
            ? wheelFractionVisibleDefault
            : scaleRange(parameterBlock.colorControl(), 0, 255, 0, 1);

        if (pixelIncrement != _pixelIncrement || msInterval != _timer.msDuration() || fraction != _wheelFractionVisible)
        {
            INFO("Rainbow: speed=%f incr=%d ms=%d fraction=%f", speed, pixelIncrement, msInterval, fraction);
            _pixelIncrement = pixelIncrement;
            _wheelFractionVisible = fraction;
            _timer = Deadline(msInterval);
            _timer.expire();
        }
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    void begin() override
    {
        // INFO("RainbowColors::begin()");
        Colorizer::begin();
        _pixelOffset = 0;
        _pixelIncrement = 1;
        _timer = Deadline(_msOriginalInterval);
        _timer.expire();
    }

    void loop() override
    {
        Colorizer::loop();
        if (_timer.hasExpired())
            {
                // INFO("RainbowColors: pixelOffset=%d", _pixelOffset);
                for (uint16_t iPixel=0; iPixel < _pixelCount; iPixel++)
                {
                    Color color = Color::wheelSin((iPixel+_pixelOffset) / float(_pixelCount) * _wheelFractionVisible);
                    setPixelColor(iPixel, color);
                }
                _timer.reset();
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

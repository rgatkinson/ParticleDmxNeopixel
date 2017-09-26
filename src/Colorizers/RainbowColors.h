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

    int         _pixelOffset;
    Deadline    _colorUpdateDeadline;
    float       _wheelFractionVisible = 0.1f;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    RainbowColors(int msInterval, int msDuration) : Colorizer(ColorizerFlavorRainbow, msDuration)
    {
        _pixelOffset = 0;
        _colorUpdateDeadline = Deadline(msInterval);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    override bool sameAs(Colorizer* pThem)
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

    override void begin()
    {
        // INFO("RainbowColors::begin()");
        Colorizer::begin();
        _pixelOffset = 0;
        _colorUpdateDeadline.expire();
    }

    override void loop()
    {
        Colorizer::loop();
        if (_colorUpdateDeadline.hasExpired())
            {
                // INFO("RainbowColors: pixelOffset=%d", _pixelOffset);
                for (uint16_t iPixel=0; iPixel < _pixelCount; iPixel++)
                {
                    COLOR_INT color = wheelF((iPixel+_pixelOffset) / float(_pixelCount) * _wheelFractionVisible);
                    setPixelColor(iPixel, color);
                }
                _colorUpdateDeadline.reset();
                _pixelOffset++;
            }
    }

    override void report()
    {
        Colorizer::report();
        INFO("RainbowColors");
    }

protected:

    // For a given (fractional) angle around the color wheel, returns
    // the color associated therewith. Red is at 0 (and 1); green is
    // at 1/3, and blue is at 2/3;
    COLOR_INT wheelF(float f)
    {
        const float oneThird = 1.f / 3.f;
        const float twoThirds = 2.f / 3.f;

        const float redScale = 1.0f;
        const float greenScale = 0.5f;
        const float blueScale = 0.5f;

        f -= floorf(f); // remove integer part

        if (f <= oneThird)
        {
            return Color::rgb(
                triDown((f + oneThird) / twoThirds, redScale),
                triUp(f / twoThirds, greenScale),
                0);
        }
        else if (f <= twoThirds)
        {
            return Color::rgb(
                0,
                triDown(f / twoThirds, greenScale),
                triUp((f-oneThird) / twoThirds, blueScale));
        }
        else
        {
            return Color::rgb(
                triUp((f - twoThirds) / twoThirds, redScale),
                0,
                triDown((f-oneThird) / twoThirds, blueScale));
        }
    }

    inline float triFUp(float f)
    {
        // return 2 * f;
        float result = sin(f * PiF);
        return result * result;
    }
    inline float triFDown(float f)
    {
        // return 2 * (1 - f);
        return triFUp(f);
    }

    inline byte triUp(float dx, float scale)
    {
        byte result = byte(min(255.0, scale * triFUp(dx) * 256.0f));
        return result;
    }
    inline byte triDown(float dx, float scale)
    {
        byte result = byte(min(255, scale * triFDown(dx) * 256.0f));
        return result;
    }


};

#endif

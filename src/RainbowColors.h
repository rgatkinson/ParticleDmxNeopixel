//
// RainbowColors.h
//
#ifndef __RAINBOW_COLORS__
#define __RAINBOW_COLORS__

struct RainbowColors : Colorizer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    int         _pixelOffset;
    Deadline    _colorUpdateDeadline;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    RainbowColors(int msInterval, int msDuration=Deadline::Infinite) : Colorizer(ColorizerFlavorRainbow, msDuration)
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
        // TRACE("RainbowColors::begin()");
        Colorizer::begin();
        _pixelOffset = 0;
        _colorUpdateDeadline.expire();
    }

    override void loop()
    {
        Colorizer::loop();  // pro forma
        if (_colorUpdateDeadline.hasExpired())
            {
                // TRACE("RainbowColors: pixelOffset=%d", _pixelOffset);
                for (uint16_t iPixel=0; iPixel < _pixelCount; iPixel++)
                {
                    COLOR_INT color = wheel( (iPixel+(_pixelOffset & 255)) & 255);
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

    // Input a value 0 to 255 to get a color value.
    // The colours are a transition r - g - b - back to r.
    COLOR_INT wheel(byte wheelPos)
    {
        if (wheelPos < 85)
        {
            return Color::rgb(wheelPos * 3, 255 - wheelPos * 3, 0);
        }
        else if (wheelPos < 170)
        {
            wheelPos -= 85;
            return Color::rgb(255 - wheelPos * 3, 0, wheelPos * 3);
        }
        else
        {
            wheelPos -= 170;
            return Color::rgb(0, wheelPos * 3, 255 - wheelPos * 3);
        }
    }

};

#endif

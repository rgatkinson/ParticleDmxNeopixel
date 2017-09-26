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
                    COLOR_INT color = Color::wheelSin((iPixel+_pixelOffset) / float(_pixelCount) * _wheelFractionVisible);
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

};

#endif

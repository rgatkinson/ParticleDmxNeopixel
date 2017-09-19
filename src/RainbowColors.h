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

    int pixelOffset;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    RainbowColors(int msInterval=10) : Colorizer(ColorizerModeRainbow)
    {
        pixelOffset = 0;
        colorUpdateDeadline = Deadline(msInterval);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    override bool sameAs(Colorizer* pThem)
    {
        bool result = Colorizer::sameAs(pThem);
        if (result)
        {
            result = this->colorUpdateDeadline.msDuration() == static_cast<RainbowColors*>(pThem)->colorUpdateDeadline.msDuration();
        }
        return result;
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    override void begin()
    {
        pixelOffset = 0;
        colorUpdateDeadline.expire();
    }

    override void loop()
    {
        Colorizer::loop();  // pro forma
        if (colorUpdateDeadline.hasExpired())
            {
                for (uint16_t iPixel=0; iPixel < pixelCount; iPixel++)
                {
                    COLOR_INT color = wheel( (iPixel+(pixelOffset & 255)) & 255);
                    setPixelColor(iPixel, color);
                }
                colorUpdateDeadline.reset();
                pixelOffset++;
            }
    }

    override void report()
    {
        Log.info("RainbowColors");
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

//
// PixelSequence.h
//
#ifndef __PIXEL_SEQUENCE_H_
#define __PIXEL_SEQUENCE_H_

#include "neopixel.h"
#include "Deadline.h"
#include "Color.h"
#include "Dimmer.h"
#include "ConstantBrightness.h"
#include "Colorizer.h"
#include "ConstantColor.h"
#include "RainbowColors.h"
#include "ColorizerSequence.h"

template<int PixelCount, int Pin, int PixelType>
struct PixelSequence : Colorizeable
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:
    static const int    pixelCount = PixelCount;
    static const int    pin = Pin;
    static const int    pixelType = PixelType;

private:
    Adafruit_NeoPixel   _neopixels;
    Deadline            _showDeadline;
    Dimmer*             _pDimmer;
    Colorizer*          _pColorizer;
    COLOR_INT           _pixelValues[pixelCount];

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    PixelSequence()
        : _neopixels(pixelCount, pin, pixelType),
          _showDeadline(10)
    {
        _pDimmer = NULL;
        _pColorizer = NULL;
    }

    virtual void initialize()
    {
        setColorizer(new ConstantColor(Color::BLACK, Deadline::Infinite));
        setDimmer(new ConstantBrightness(1.0f, Deadline::Infinite));
    }

    virtual ~PixelSequence()
    {
        releaseRef(_pDimmer);
        releaseRef(_pColorizer);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    void setDimmer(Dimmer* pDimmer)
    {
        releaseRef(_pDimmer);
        _pDimmer = pDimmer; // takes ownership
    }
    void setColorizer(Colorizer* pColorizer)
    {
        releaseRef(_pColorizer);
        _pColorizer = pColorizer; // takes ownwership
        _pColorizer->setColorizeable(this);
    }
    Colorizer* colorizer()
    {
        return _pColorizer;
    }

    void setColorizerIfDifferent(Colorizer* pColorizer)
    {
        if (_pColorizer->sameAs(pColorizer))
        {
            releaseRef(pColorizer); // already have 'em
        }
        else
        {
            setColorizer(pColorizer);
            pColorizer->begin();
        }
    }


    void setDimmerBrightness(BRIGHTNESS brightness)
    {
        _pDimmer->setDimmerBrightness(brightness);
    }

    override void setPixelColor(uint16_t iPixel, COLOR_INT color)
    {
        _pixelValues[iPixel] = color;
    }

    override int numberOfPixels()
    {
        return pixelCount;
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    virtual void begin()
    {
        _neopixels.begin();
        for (int iPixel = 0; iPixel < pixelCount; iPixel++)
        {
            _pixelValues[iPixel] = Color::BLACK;
        }
        _pDimmer->begin();
        _pColorizer->begin();
        _showDeadline.expire();
    }

    virtual void loop()
    {
        _pColorizer->loop();

        if (_showDeadline.hasExpired())
        {
            // Only call the dimmer when we actually are going to show _neopixels
            _pDimmer->loop();
            byte brightness = _pDimmer->currentBrightness();

            for (int iPixel = 0; iPixel < pixelCount; iPixel++)
            {
                COLOR_INT color = _pixelValues[iPixel];
                _neopixels.setColorScaled(iPixel, Color::red(color), Color::green(color), Color::blue(color), brightness);
            }

            _neopixels.show();
            _showDeadline.reset();
        }
    }

    virtual void report()
    {
        _pColorizer->report();
        _pDimmer->report();
    }


};

#endif

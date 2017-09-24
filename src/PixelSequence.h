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
        setColorizerNoBegin(new ConstantColor(Color::BLACK, Deadline::Infinite));
        setDimmerNoBegin(new ConstantBrightness(1.0f, Deadline::Infinite));
    }

    virtual ~PixelSequence()
    {
        releaseRef(_pDimmer);
        releaseRef(_pColorizer);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
protected:
    void setColorizerNoBegin(Colorizer* pColorizer)
    {
        releaseRef(_pColorizer);
        _pColorizer = pColorizer; // takes ownwership
        if (_pColorizer) _pColorizer->setColorizeable(this);
    }

    void setDimmerNoBegin(Dimmer* pDimmer)
    {
        releaseRef(_pDimmer);
        _pDimmer = pDimmer; // takes ownership
    }

public:

    Dimmer* dimmer()
    {
        return _pDimmer;
    }

    Colorizer* colorizer()
    {
        return _pColorizer;
    }

    void setDimmer(Dimmer* pDimmer)
    {
        setDimmerNoBegin(pDimmer);
        if (_pDimmer) _pDimmer->begin();
    }

    void setColorizer(Colorizer* pColorizer)
    {
        setColorizerNoBegin(pColorizer);
        if (_pColorizer) _pColorizer->begin();
    }

    void setDimmerIfDifferent(Dimmer* pDimmer)
    {
        if (_pDimmer && _pDimmer->sameAs(pDimmer))
        {
            releaseRef(pDimmer);
        }
        else
        {
            setDimmer(pDimmer);
        }
    }

    void setColorizerIfDifferent(Colorizer* pColorizer)
    {
        if (_pColorizer && _pColorizer->sameAs(pColorizer))
        {
            releaseRef(pColorizer); // already have 'em
        }
        else
        {
            setColorizer(pColorizer);
        }
    }


    void setDimmerBrightness(BRIGHTNESS brightness)
    {
        if (_pDimmer) _pDimmer->setDimmerBrightness(brightness);
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
        if (_pDimmer) _pDimmer->begin();
        if (_pColorizer) _pColorizer->begin();
        _showDeadline.expire();
    }

    virtual void loop()
    {
        if (_pColorizer) _pColorizer->loop();

        if (_showDeadline.hasExpired())
        {
            // Only call the dimmer when we actually are going to show _neopixels: efficiency
            if (_pDimmer) _pDimmer->loop();
            BRIGHTNESS brightness = _pDimmer ? _pDimmer->currentBrightness() : MAX_BRIGHTNESS;

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
        if (_pColorizer) _pColorizer->report();
        if (_pDimmer) _pDimmer->report();
    }


};

#endif

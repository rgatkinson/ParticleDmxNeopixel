//
// PixelSequence.h
//
#ifndef __PIXEL_SEQUENCE_H_
#define __PIXEL_SEQUENCE_H_

#include "neopixel.h"
#include "Util/Deadline.h"
#include "Util/Color.h"
#include "Dimmers/Dimmer.h"
#include "Dimmers/UniformBrightness.h"
#include "Colorizers/Colorizer.h"
#include "Colorizers/UniformColor.h"
#include "Colorizers/RainbowColors.h"
#include "Colorizers/ColorizerSequence.h"

struct PixelSequence : ReferenceCounted, Colorizeable
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:
    int                 _pixelCount;
    Adafruit_NeoPixel   _neopixels;
    Deadline            _showDeadline;
    Dimmer*             _pDimmer;
    Colorizer*          _pColorizer;
    COLOR_INT*          _pixelValues;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    PixelSequence(int pixelCount, int pin, int pixelType)
        : _neopixels(pixelCount, pin, pixelType),
          _showDeadline(10)
    {
        _pixelCount = pixelCount;
        _pDimmer = NULL;
        _pColorizer = NULL;
        _pixelValues = new COLOR_INT[pixelCount];
        setColorizerNoBegin(new UniformColor(Color::BLACK, Deadline::Infinite));
        setDimmerNoBegin(new UniformBrightness(1.0f, Deadline::Infinite));
    }

    DELEGATE_REF_COUNTING

protected:

    virtual ~PixelSequence() override
    {
        releaseRef(_pDimmer);
        releaseRef(_pColorizer);
        delete [] _pixelValues;
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
        if (_pDimmer) _pDimmer->setColorizeable(this);
    }

public:

    inline Dimmer* dimmer()
    {
        return _pDimmer;
    }

    inline Colorizer* colorizer()
    {
        return _pColorizer;
    }

    void setDimmer(Dimmer* pDimmer) override
    {
        setDimmerNoBegin(pDimmer);
        if (_pDimmer) _pDimmer->begin();
    }

    void setColorizer(Colorizer* pColorizer) override
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


    void setDimmerBrightness(BRIGHTNESS brightness) override
    {
        if (_pDimmer) _pDimmer->setDimmerBrightness(brightness);
    }

    void setPixelColor(uint16_t iPixel, COLOR_INT color) override
    {
        _pixelValues[iPixel] = color;
    }

    int numberOfPixels() override
    {
        return _pixelCount;
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    virtual void begin()
    {
        _neopixels.begin();
        for (int iPixel = 0; iPixel < _pixelCount; iPixel++)
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

            for (int iPixel = 0; iPixel < _pixelCount; iPixel++)
            {
                COLOR_INT color = _pixelValues[iPixel];
                if (_pDimmer && _pDimmer->hasPixelizedBrightness())
                {
                    brightness = _pDimmer->currentBrightness(iPixel);
                }
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

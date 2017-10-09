//
// PixelSequence.h
//
#ifndef __PIXEL_SEQUENCE_H_
#define __PIXEL_SEQUENCE_H_

#include "neopixel.h"
#include "Util/Deadline.h"
#include "Util/Color.h"
#include "Lumenizers/Lumenizer.h"
#include "Lumenizers/UniformLuminance.h"
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
    Lumenizer*          _pLumenizer;
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
        _pLumenizer = NULL;
        _pColorizer = NULL;
        _pixelValues = new COLOR_INT[pixelCount];
        ownColorizerNoBegin(new UniformColor(Color::BLACK, Deadline::Infinite));
        ownColorizerNoBegin(new UniformLuminance(1.0f, Deadline::Infinite));
    }

    DELEGATE_REF_COUNTING

protected:

    virtual ~PixelSequence() override
    {
        releaseRef(_pLumenizer);
        releaseRef(_pColorizer);
        delete [] _pixelValues;
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
protected:

    void ownColorizerNoBegin(Colorizer* pColorizer)
    {
        releaseRef(_pColorizer);
        _pColorizer = pColorizer; // takes ownwership
        if (_pColorizer) _pColorizer->noteColorizeable(this);
    }

    void ownColorizerNoBegin(Lumenizer* pLumenizer)
    {
        releaseRef(_pLumenizer);
        _pLumenizer = pLumenizer; // takes ownership
        if (_pLumenizer) _pLumenizer->noteColorizeable(this);
    }

public:

    inline Lumenizer* lumenizer() override
    {
        return _pLumenizer;
    }

    inline Colorizer* colorizer() override
    {
        return _pColorizer;
    }

    void ownLumenizer(Lumenizer* pLumenizer) override
    {
        ownColorizerNoBegin(pLumenizer);
        if (_pLumenizer) _pLumenizer->begin();
    }

    void ownColorizer(Colorizer* pColorizer) override
    {
        ownColorizerNoBegin(pColorizer);
        if (_pColorizer) _pColorizer->begin();
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
        if (_pLumenizer) _pLumenizer->begin();
        if (_pColorizer) _pColorizer->begin();
        _showDeadline.expire();
    }

    virtual void loop()
    {
        if (_pColorizer) _pColorizer->loop();

        if (_showDeadline.hasExpired())
        {
            // Only call the brightness when we actually are going to show _neopixels: efficiency
            BRIGHTNESS brightness = BRIGHTNESS_MAX-1;
            if (_pLumenizer)
            {
                _pLumenizer->loop();
                brightness = _pLumenizer->currentBrightness();
            }

            for (int iPixel = 0; iPixel < _pixelCount; iPixel++)
            {
                COLOR_INT color = _pixelValues[iPixel];
                if (_pLumenizer && _pLumenizer->hasPixelizedBrightness())
                {
                    brightness = _pLumenizer->currentBrightness(iPixel);
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
        if (_pLumenizer) _pLumenizer->report();
    }


};

#endif

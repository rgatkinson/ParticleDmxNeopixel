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
    Adafruit_NeoPixel   neopixels;
    Deadline            showDeadline;
    Dimmer*             pDimmer;
    Colorizer*          pColorizer;
    COLOR_INT           pixelValues[pixelCount];

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    PixelSequence()
        : neopixels(pixelCount, pin, pixelType),
          showDeadline(10)
    {
        pDimmer = NULL;
        pColorizer = NULL;
    }

    virtual void initialize()
    {
        setColorizer(new ConstantColor());
        setDimmer(new ConstantBrightness());
    }

    virtual ~PixelSequence()
    {
        delete pDimmer;
        delete pColorizer;
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    void setDimmer(Dimmer* pDimmer)
    {
        delete this->pDimmer;
        this->pDimmer = pDimmer; // takes ownership
    }
    void setColorizer(Colorizer* pColorizer)
    {
        delete this->pColorizer;
        this->pColorizer = pColorizer; // takes ownwership
        this->pColorizer->setColorizeable(this);
    }
    void setColorizerIfDifferent(Colorizer* pColorizer)
    {
        if (this->pColorizer->sameAs(pColorizer))
        {
            delete pColorizer;
        }
        else
        {
            setColorizer(pColorizer);
            pColorizer->begin();
        }
    }


    void setDimming(BRIGHTNESS brightness)
    {
        pDimmer->setDimming(brightness);
    }

    override void setPixelColor(uint16_t iPixel, COLOR_INT color)
    {
        pixelValues[iPixel] = color;
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
        neopixels.begin();
        for (int iPixel = 0; iPixel < pixelCount; iPixel++)
        {
            pixelValues[iPixel] = Color::BLACK;
        }
        pDimmer->begin();
        pColorizer->begin();
        showDeadline.expire();
    }

    virtual void loop()
    {
        pColorizer->loop();

        if (showDeadline.hasExpired())
        {
            // Only call the dimmer when we actually are going to show neopixels
            pDimmer->loop();
            byte brightness = pDimmer->currentBrightness();

            for (int iPixel = 0; iPixel < pixelCount; iPixel++)
            {
                COLOR_INT color = pixelValues[iPixel];
                neopixels.setColorScaled(iPixel, Color::red(color), Color::green(color), Color::blue(color), brightness);
            }

            neopixels.show();
            showDeadline.reset();
        }
    }

    virtual void report()
    {
        pColorizer->report();
        pDimmer->report();
    }


};

#endif

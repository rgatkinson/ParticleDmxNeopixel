//
// TwinkleBrightness.h
//
#ifndef __TWINKLE_BRIGHTNESS_H__
#define __TWINKLE_BRIGHTNESS_H__

#include "Util/Deadline.h"
#include "Twinkler.h"

//==================================================================================================
// TwinkleBrightness
//==================================================================================================

struct TwinkleBrightness: Dimmer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    int _msInterval;
    ArrayList<Twinkler> _twinklers;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    TwinkleBrightness(float msInterval, int msDuration) : Dimmer(Flavor::Twinkle, msDuration)
    {
        _msInterval = msInterval;
    }

    virtual void setColorizeable(Colorizeable* pColorizeable)
    {
        Dimmer::setColorizeable(pColorizeable);
        _twinklers = ArrayList<Twinkler>();
        for (int i = 0; i < _pixelCount; i++)
        {
            _twinklers.addLast(Twinkler(_msInterval));
        }
    }

    bool sameAs(Dimmer* pThem) override
    {
        return Dimmer::sameAs(pThem) && _msInterval == static_cast<TwinkleBrightness*>(pThem)->_msInterval;
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    bool hasPixelizedBrightness() override
    {
        return true;
    }

    bool usesGammaCorrection() override
    {
        return false;
    }

    BRIGHTNESS currentBrightness(int iPixel) override
    {
        return rawCurrentBrightness(_twinklers[iPixel].currentLevel());
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin() override
    {
        Dimmer::begin();
        for (int i = 0; i < _twinklers.count(); i++)
        {
            _twinklers[i].begin();
        }
    }

    void loop() override
    {
        Dimmer::loop();
        for (int i = 0; i < _twinklers.count(); i++)
        {
            _twinklers[i].loop();
        }
    }

    void report() override
    {
        Dimmer::report();
    }

};

#endif

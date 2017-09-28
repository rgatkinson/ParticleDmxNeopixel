//
// TwinkleLumenizer.h
//
#ifndef __TWINKLE_LUMENIZER_H__
#define __TWINKLE_LUMENIZER_H__

#include "Util/Deadline.h"
#include "Twinkler.h"

//==================================================================================================
// TwinkleLumenizer
//==================================================================================================

struct TwinkleLumenizer: Lumenizer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    int _msPause;
    int _msInterval;
    ArrayList<Twinkler> _twinklers;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    TwinkleLumenizer(float msPause, int msInterval, int msDuration) : Lumenizer(Flavor::Twinkle, msDuration)
    {
        _msPause = msPause;
        _msInterval = msInterval;
    }

    virtual void setColorizeable(Colorizeable* pColorizeable)
    {
        Lumenizer::setColorizeable(pColorizeable);
        _twinklers = ArrayList<Twinkler>();
        for (int i = 0; i < _pixelCount; i++)
        {
            _twinklers.addLast(Twinkler(_msPause, _msInterval));
        }
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
        Lumenizer::begin();
        for (int i = 0; i < _twinklers.count(); i++)
        {
            _twinklers[i].begin();
        }
    }

    void loop() override
    {
        Lumenizer::loop();
        for (int i = 0; i < _twinklers.count(); i++)
        {
            _twinklers[i].loop();
        }
    }

    void report() override
    {
        Lumenizer::report();
    }

};

#endif

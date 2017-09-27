//
// TwinkleBrightness.h
//
#ifndef __TWINKLE_BRIGHTNESS_H__
#define __TWINKLE_BRIGHTNESS_H__

#include "Util/Deadline.h"

//==================================================================================================
// Twinkler
//==================================================================================================

struct Twinkler
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
private:

    int _msInterval;
    float _currentLevel;
    Deadline _timer;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Twinkler()
    {
        _msInterval = 0;
        _currentLevel = 1.0f;
    }

    Twinkler(int msInterval)
    {
        _msInterval = msInterval;
        _currentLevel = 1.0f;
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    float currentLevel()
    {
        return _currentLevel;
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:
    
    void begin()
    {
        resetTimer();
    }

    void loop()
    {
        // Twinkle; see also BreathingBrightness
        int msInterval = _timer.msDuration();
        int ms = _timer.milliseconds() % msInterval;
        float intervalFraction = (float)ms / (float)msInterval;
        //
        _currentLevel = (intervalFraction <= 0.5f)
            ? (intervalFraction * 2.0f)
            : 1.0f - ( (intervalFraction - 0.5f) * 2.0f );

        // Set for the next time
        if (_timer.hasExpired())
        {
            resetTimer();
        }
    }

private:
    void resetTimer()
    {
        int dms   = _msInterval / 3;
        int msMin = _msInterval - dms;
        int msMax = _msInterval + dms;
        int ms = random(msMin, msMax);      // half-open interval, fwiw
        _timer = Deadline(ms);
    }
};

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

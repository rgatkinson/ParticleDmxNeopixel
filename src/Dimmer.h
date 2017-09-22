//
// Brightness.h
//
#ifndef __BRIGHTNESS_H_
#define __BRIGHTNESS_H_

#include "Durable.h"

typedef byte BRIGHTNESS;
const BRIGHTNESS MAX_BRIGHTNESS = 255;

struct Dimmer : Durable
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
public:

    enum DimmerFlavor { DimmerFlavorNone, DimmerFlavorConstant, DimmerFlavorSequence, DimmerFlavorBreathing };

protected:

    DimmerFlavor _flavor;
    BRIGHTNESS _minBrightness;      // min we ever report
    BRIGHTNESS _maxBrightness;      // max we ever report
    BRIGHTNESS _dimmerBrightness;   // controlled externally
    float _currentLevel;            // controlled by us
    float _dimmerLevel;             // controlled externally

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Dimmer(DimmerFlavor flavor, int msDuration) : Durable(msDuration)
    {
        _flavor = flavor;

        // We set a non-zero lower bound in recognition that at lower levels
        // the LEDs simply turn off. Perhaps we just need to tune our PWM curves
        // better, for for the moment, we do this.
        _maxBrightness = 255;
        _minBrightness = 20;          // surprising. maybe voltage & color dependent?
        setCurrentLevel(1.0f);
        setDimmerBrightness(MAX_BRIGHTNESS);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:

    override bool sameAs(Dimmer* pThem)
    {
        return Durable::sameAs(pThem) && _flavor == pThem->_flavor;
    }

    virtual void setMaxBrightness(BRIGHTNESS brightness)
    {
        _maxBrightness = brightness;
    }
    virtual void setMinBrightness(BRIGHTNESS brightness)
    {
        _minBrightness = brightness;
    }

    // Controlled by external faders etc
    virtual void setDimmerBrightness(BRIGHTNESS dimmerBrightness)
    {
        _dimmerBrightness = dimmerBrightness;

        float dimmerLevel = (float)dimmerBrightness / (float)MAX_BRIGHTNESS;
        dimmerLevel = max(0, dimmerLevel);
        dimmerLevel = min(1.0f, dimmerLevel);
        _dimmerLevel = dimmerLevel;
        // Log.info("_dimmerLevel=%f brightness=%d", _dimmerLevel, currentBrightness());
    }

    virtual BRIGHTNESS currentBrightness()
    {
        BRIGHTNESS raw = this->rawCurrentBrightness();
        BRIGHTNESS result = gammaCorrect(raw);
        return result;
    }

protected:

    BRIGHTNESS rawCurrentBrightness()
    {
        if (_dimmerLevel==0.0f)
            return 0;   // honor blackout
        else
            return (BRIGHTNESS)(_currentLevel * _dimmerLevel * (float)(_maxBrightness-_minBrightness) + (float)_minBrightness + 0.5f);
    }

    void setCurrentLevel(float level)
    {
        level = max(0, level);
        level = min(1.0f, level);
        _currentLevel = level;
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    virtual void begin()
    {
        _duration.reset();
    }

    virtual void loop()
    {
    }

    virtual void report()
    {
        Log.info("brightness: cur=%f max=%d", _currentLevel, _maxBrightness);
    }

    //----------------------------------------------------------------------------------------------
    // Internal
    //----------------------------------------------------------------------------------------------
protected:

    BRIGHTNESS gammaCorrect(BRIGHTNESS brightness)
    {
        // from: https://learn.adafruit.com/led-tricks-gamma-correction/the-quick-fix
        // gamma=2.1
        static const byte gammaCorrection[256] =
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
            2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 7, \
            7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 14, 14, \
            14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 20, 20, 21, 21, 22, 22, 23, \
            24, 24, 25, 25, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 33, 33, 34, \
            35, 36, 36, 37, 38, 39, 40, 40, 41, 42, 43, 44, 44, 45, 46, 47, 48, \
            49, 50, 51, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, \
            65, 66, 67, 68, 69, 70, 71, 72, 73, 75, 76, 77, 78, 79, 80, 81, 83, \
            84, 85, 86, 87, 88, 90, 91, 92, 93, 95, 96, 97, 98, 100, 101, 102, \
            104, 105, 106, 107, 109, 110, 112, 113, 114, 116, 117, 118, 120, 121, \
            123, 124, 126, 127, 129, 130, 131, 133, 134, 136, 137, 139, 141, 142, \
            144, 145, 147, 148, 150, 151, 153, 155, 156, 158, 160, 161, 163, 165, \
            166, 168, 170, 171, 173, 175, 176, 178, 180, 182, 183, 185, 187, 189, \
            191, 192, 194, 196, 198, 200, 202, 203, 205, 207, 209, 211, 213, 215, \
            217, 219, 221, 223, 225, 226, 228, 230, 232, 234, 236, 238, 241, 243, \
            245, 247, 249, 251, 253, 255
        };
        return gammaCorrection[brightness];
    }

};

#endif

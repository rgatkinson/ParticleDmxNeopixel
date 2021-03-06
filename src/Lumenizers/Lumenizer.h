//
// Lumenizer.h
//
#ifndef __LUMENIZER_H_
#define __LUMENIZER_H_

#include "Util/Durable.h"
#include "Pixels/Colorizeable.h"
#include "DmxParams/DmxColorLuminanceParameters.h"

const BRIGHTNESS BRIGHTNESS_MIN = 0;
const BRIGHTNESS BRIGHTNESS_OFF = 0;
const BRIGHTNESS BRIGHTNESS_FULL = 255;
const BRIGHTNESS BRIGHTNESS_MAX = 256;

struct Lumenizer : Durable
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
public:

    enum class Flavor
    {
        None,
        Uniform,
        Sequence,
        Breathing,
        Twinkle,
        MorseCode,
        Decimating,
        SelfTest
    };

    static LPCSTR nameOf(Flavor flavor)
    {
        switch (flavor)
        {
            case Flavor::None:          return "None";
            case Flavor::Uniform:       return "Uniform";
            case Flavor::Sequence:      return "Sequence";
            case Flavor::Breathing:     return "Breathing";
            case Flavor::Twinkle:       return "Twinkle";
            case Flavor::Decimating:    return "Decimating";
            case Flavor::SelfTest:      return "SelfTest";
            default:                    return "<unknown>";
        }
    }

protected:

    Flavor          _flavor;
    BRIGHTNESS      _brightnessMin;     // min we ever report
    BRIGHTNESS      _brightnessMax;     // max we ever report
    float           _currentLevel;      // controlled by us
    float           _dimmerLevel;       // controlled externally
    Colorizeable*   _pColorizeable;     // no ref
    int             _pixelCount;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Lumenizer(Flavor flavor, Duration duration) : Durable(duration)
    {
        _flavor = flavor;

        // We set a non-zero lower bound in recognition that at lower levels
        // the LEDs simply turn off. Perhaps we just need to tune our PWM curves
        // better, for for the moment, we do this. [old comment]
        _brightnessMax = BRIGHTNESS_MAX;
        _brightnessMin = 0;
        setCurrentLevel(1.0f);
        setDimmerLevel(1.0f);
        _pColorizeable = nullptr;
        _pixelCount = 0;
    }

    virtual void noteColorizeable(Colorizeable* pColorizeable)
    {
        _pColorizeable = pColorizeable;
        _pixelCount = pColorizeable ? pColorizeable->numberOfPixels() : 0;
    }

protected:
    void delgateTo(Lumenizer* pLumenizer)
    {
        pLumenizer->noteColorizeable(_pColorizeable);
        pLumenizer->setBrightnessMin(_brightnessMin);
        pLumenizer->setBrightnessMax(_brightnessMax);
        pLumenizer->setDimmerLevel(_dimmerLevel);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:

    virtual bool sameAs(Lumenizer* pThem)
    {
        // Do NOT include parameters that will be adjusted by DMX control
        return Durable::sameAs(pThem) && _flavor == pThem->_flavor;
    }

    virtual int msLoopingDuration()
    {
        return msDuration();
    }

    virtual void setBrightnessMax(BRIGHTNESS brightness)
    {
        _brightnessMax = clip(brightness, BRIGHTNESS_MIN, BRIGHTNESS_MAX);
    }
    virtual void setBrightnessMin(BRIGHTNESS brightness)
    {
        _brightnessMin = clip(brightness, BRIGHTNESS_MIN, BRIGHTNESS_MAX-1);
    }

    // Controlled by external faders etc
    virtual void setDimmerLevel(float dimmerLevel)
    {
        dimmerLevel = clip(dimmerLevel, 0, 1);
        _dimmerLevel = dimmerLevel;
        // INFO("setDimmerLevel: %f flavor=%s", dimmerLevel, nameOf(_flavor));
    }

    virtual BRIGHTNESS currentBrightness(int iPixel, bool allowGammaCorrection)
    {
        // Ignore iPixel
        BRIGHTNESS result = this->rawCurrentBrightness(_currentLevel);
        if (allowGammaCorrection && usesGammaCorrection())
        {
            result = gammaCorrect(result);
        }
        return result;
    }

    virtual bool hasPixelizedBrightness()
    {
        return false;
    }

    virtual bool usesGammaCorrection()
    {
        return true;
    }

protected:

    BRIGHTNESS rawCurrentBrightness(float currentLevel)
    {
        if (_dimmerLevel==0.0f)
        {
            return 0;   // honor blackout
        }
        else
        {
            BRIGHTNESS result= clip(
                (BRIGHTNESS)scaleRange(currentLevel * _dimmerLevel, 0, 1, _brightnessMin, _brightnessMax),
                _brightnessMin,
                _brightnessMax-1);
            // INFO("cur=%f dim=%f min=%d max=%d: %d", currentLevel, _dimmerLevel, _brightnessMin, _brightnessMax, result);
            return result;
        }
    }

    void setCurrentLevel(float level)
    {
        _currentLevel = clip(level, 0, 1);
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    void processDmxColorLuminance(const DmxColorLuminanceParameters& parameterBlock)
    {
        processDmxDimmer(parameterBlock.dimmer());
        processDmxEffectSpeedControl(parameterBlock.luminance());
    }

    virtual void processDmxDimmer(const DmxDimmer& dimmer)
    {
        float dimmerLevel = dimmer.dimmerLevel();
        if (_dimmerLevel != dimmerLevel)
        {
            INFO("Lumenizer: dimmerLevel=%f", dimmerLevel);
            setDimmerLevel(dimmerLevel);
        }
    }

    virtual void processDmxEffectSpeedControl(const DmxEffectSpeedControl& luminance)
    {
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    virtual void begin()
    {
        _deadline.reset();
    }

    virtual void loop()
    {
    }

    virtual void report()
    {
        INFO("Lumenizer(%s): level=%f max=%d", nameOf(_flavor), _currentLevel, _brightnessMax);
    }

    //----------------------------------------------------------------------------------------------
    // Internal
    //----------------------------------------------------------------------------------------------
protected:

    BRIGHTNESS gammaCorrect(BRIGHTNESS brightness)
    {
        // from: https://learn.adafruit.com/led-tricks-gamma-correction/the-quick-fix, hand tweaked
        // gamma=2.1
        static const byte gammaCorrection[256] =
        {
        //  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
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

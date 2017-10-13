//
// DecimatingLuminance.h
//
#ifndef __DECIMATOR_LUMINANCE_H__
#define __DECIMATOR_LUMINANCE_H__

#include "Lumenizers/DelegatingLumenizer.h"

struct DecimatingLuminance : DelegatingLumenizer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    int _numerator;     // deicmate this many ...
    int _denominator;   // ... out of this many

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    DecimatingLuminance(int numerator, int denominator, Lumenizer* pLumenizer) : DelegatingLumenizer(Flavor::Decimating, pLumenizer)
    {
        _numerator = numerator;
        _denominator = denominator;
    }

    //----------------------------------------------------------------------------------------------
    // Brightness
    //----------------------------------------------------------------------------------------------

    bool hasPixelizedBrightness() override
    {
        return true;
    }

    BRIGHTNESS currentBrightness(int iPixel, bool allowGammaCorrection) override
    {
        if (_denominator != 0 && modOne(iPixel, _denominator) <= _numerator)
        {
            return 0;
        }
        else
        {
            return DelegatingLumenizer::currentBrightness(iPixel, allowGammaCorrection);
        }
    }
};

#endif

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

    int _decimation;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    DecimatingLuminance(int decimation, Lumenizer* pLumenizer = nullptr) : DecimatingLuminance(Duration::Infinite, decimation, pLumenizer)
    {
    }

    DecimatingLuminance(Duration duration, int decimation, Lumenizer* pLumenizer = nullptr) : DelegatingLumenizer(Flavor::Decimating, duration, pLumenizer)
    {
        _decimation = decimation;
    }

    //----------------------------------------------------------------------------------------------
    // Brightness
    //----------------------------------------------------------------------------------------------

    bool hasPixelizedBrightness() override
    {
        return true;
    }

    BRIGHTNESS currentBrightness(int iPixel) override
    {
        if (iPixel % _decimation == 0)
        {
            return DelegatingLumenizer::currentBrightness(iPixel);
        }
        else
        {
            return 0;
        }
    }
};

#endif

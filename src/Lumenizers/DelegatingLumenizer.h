//
// DelegatingLumenizer.h
//
#ifndef __DELEGATING_LUMENIZER_H__
#define __DELEGATING_LUMENIZER_H__

struct DelegatingLumenizer : Lumenizer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    Lumenizer*  _pLumenizer;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    DelegatingLumenizer(Flavor flavor, Duration duration, Lumenizer* pLumenizer /*nullable, takes ownership*/) : Lumenizer(flavor, duration)
    {
        _pLumenizer = nullptr;
        ownLumenizer(pLumenizer);
    }

    ~DelegatingLumenizer() override
    {
        clear();
    }

    void clear()
    {
        releaseRef(_pLumenizer);
    }

    void ownLumenizer(Lumenizer* pLumenizer /*takes ownwership*/)
    {
        clear();
        if (pLumenizer)
        {
            delgateTo(pLumenizer);
            _pLumenizer = pLumenizer;   // takes ownership
        }
    }

    //----------------------------------------------------------------------------------------------
    // Brightness
    //----------------------------------------------------------------------------------------------

    BRIGHTNESS currentBrightness(int iPixel) override
    {
        if (_pLumenizer)
        {
            return _pLumenizer->currentBrightness(iPixel);
        }
        else
        {
            return Lumenizer::currentBrightness(iPixel);
        }
    }

    bool hasPixelizedBrightness() override
    {
        if (_pLumenizer)
        {
            return _pLumenizer->hasPixelizedBrightness();
        }
        else
        {
            return Lumenizer::hasPixelizedBrightness();
        }
    }

    bool usesGammaCorrection() override
    {
        if (_pLumenizer)
        {
            return _pLumenizer->usesGammaCorrection();
        }
        else
        {
            return Lumenizer::usesGammaCorrection();
        }
    }

    void setBrightnessMax(BRIGHTNESS brightness) override
    {
        Lumenizer::setBrightnessMax(brightness);
        if (_pLumenizer)
        {
            _pLumenizer->setBrightnessMax(brightness);
        }
    }

    void setBrightnessMin(BRIGHTNESS brightness) override
    {
        Lumenizer::setBrightnessMin(brightness);
        if (_pLumenizer)
        {
            _pLumenizer->setBrightnessMin(brightness);
        }
    }

    // Controlled by external faders etc
    void setDimmerLevel(float dimmerLevel) override
    {
        Lumenizer::setDimmerLevel(dimmerLevel);
        if (_pLumenizer)
        {
            _pLumenizer->setDimmerLevel(dimmerLevel);
        }
    }
};

#endif

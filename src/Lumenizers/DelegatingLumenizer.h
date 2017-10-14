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
public:

    DelegatingLumenizer(Flavor flavor, Lumenizer* pLumenizer=nullptr /*nullable, takes ownership*/) : Lumenizer(flavor, Duration(0) /* ignored */)
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
            _pLumenizer = pLumenizer;   // takes ownership
            delgateTo(pLumenizer);
        }
    }

    void setLumenizer(Lumenizer* pLumenizer)
    {
        if (pLumenizer) pLumenizer->addRef();
        ownLumenizer(pLumenizer);
    }

    bool sameAs(Lumenizer* pThemLumenizer) override
    {
        if (Lumenizer::sameAs(pThemLumenizer))
        {
            DelegatingLumenizer* pThem = static_cast<DelegatingLumenizer*>(pThemLumenizer);
            if (_pLumenizer && pThem->_pLumenizer)
            {
                return _pLumenizer->sameAs(pThem->_pLumenizer);
            }
        }
        return false;
    }

    void noteColorizeable(Colorizeable* pColorizeable) override
    {
        Lumenizer::noteColorizeable(pColorizeable);
        if (_pLumenizer)
        {
            _pLumenizer->noteColorizeable(pColorizeable);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Duration
    //----------------------------------------------------------------------------------------------

    void setDuration(Duration duration) override
    {
        if (_pLumenizer)
        {
            _pLumenizer->setDuration(duration);
        }
    }
    int msDuration() override
    {
        if (_pLumenizer)
        {
            return _pLumenizer->msDuration();
        }
        else
        {
            return Duration::Infinite.ms();
        }
    }

    int msLoopingDuration() override
    {
        if (_pLumenizer)
        {
            return _pLumenizer->msDuration();
        }
        else
        {
            return Duration::Infinite.ms();
        }
    }

    bool hasExpired() override
    {
        if (_pLumenizer)
        {
            return _pLumenizer->hasExpired();
        }
        else
        {
            return Lumenizer::hasExpired();
        }
    }

    //----------------------------------------------------------------------------------------------
    // Brightness
    //----------------------------------------------------------------------------------------------

    BRIGHTNESS currentBrightness(int iPixel, bool allowGammaCorrection) override
    {
        if (_pLumenizer)
        {
            return _pLumenizer->currentBrightness(iPixel, allowGammaCorrection);
        }
        else
        {
            return Lumenizer::currentBrightness(iPixel, allowGammaCorrection);
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

    //----------------------------------------------------------------------------------------------
    // Looping
    //----------------------------------------------------------------------------------------------

    void begin() override
    {
        Lumenizer::begin();
        if (_pLumenizer)
        {
            _pLumenizer->begin();
        }
    }

    void loop() override
    {
        Lumenizer::loop();
        if (_pLumenizer)
        {
            _pLumenizer->loop();
        }
    }

    void report() override
    {
        Lumenizer::report();
        if (_pLumenizer)
        {
            _pLumenizer->report();
        }
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------

    virtual void processDmxDimmer(const DmxDimmer& dimmer) override
    {
        Lumenizer::processDmxDimmer(dimmer);
        if (_pLumenizer)
        {
            _pLumenizer->processDmxDimmer(dimmer);
        }
    }

    virtual void processDmxEffectSpeedControl(const DmxEffectSpeedControl& luminance) override
    {
        Lumenizer::processDmxEffectSpeedControl(luminance);
        if (_pLumenizer)
        {
            _pLumenizer->processDmxEffectSpeedControl(luminance);
        }
    }
};

#endif

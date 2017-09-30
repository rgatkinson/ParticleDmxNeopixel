//
// LumenizerSequence.h
//
#ifndef __BRIGHTNESS_SEQUENCE_H__
#define __BRIGHTNESS_SEQUENCE_H__

#include "Util/ArrayList.h"
#include "Lumenizer.h"
#include "Colorizers/UniformColor.h"

//==================================================================================================
// LumenizerSequence
//==================================================================================================

struct LumenizerSequence : Lumenizer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    ArrayList<Lumenizer*> _lumenizers;
    int _currentLumenizer;
    bool _looping;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    LumenizerSequence(Flavor flavor = Flavor::Sequence) : Lumenizer(flavor, 0 /*ignored*/)
    {
        _currentLumenizer = 0;
        _looping = false;
    }

    ~LumenizerSequence() override
    {
        for (int i = 0; i < _lumenizers.count(); i++)
        {
            releaseRef(_lumenizers[i]);
        }
    }

    void addLumenizer(Lumenizer* pLumenizer /*takes ownwership*/)
    {
        pLumenizer->setColorizeable(_pColorizeable);
        pLumenizer->setBrightnessMin(_brightnessMin);
        pLumenizer->setBrightnessMax(_brightnessMax);
        pLumenizer->setDimmerLevel(_dimmerLevel);
        _lumenizers.addLast(pLumenizer);
    }

    void setColorizeable(Colorizeable* pColorizeable) override
    {
        Lumenizer::setColorizeable(pColorizeable);
        for (int i = 0; i < count(); i++)
        {
            _lumenizers[i]->setColorizeable(pColorizeable);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    void setDuration(int msDuration) override
    {
        Log.error("invalid call: LumenizerSequence::setDuration()");
    }
    int msDuration() override
    {
        if (_looping)
        {
            return Deadline::Infinite;
        }
        else
        {
            return msLoopingDuration();
        }
    }

    int msLoopingDuration() override
    {
        int ms = 0;
        for (int i = 0; i < count(); i++)
        {
            ms += _lumenizers[i]->msDuration();
        }
        ms = min(ms, Deadline::Infinite);
        return ms;
    }

    void setLooping(bool looping)
    {
        _looping = looping;
    }
    bool looping()
    {
        return _looping;
    }

    bool isEmpty()
    {
        return _lumenizers.isEmpty();
    }

    int count()
    {
        return _lumenizers.count();
    }

    bool sameAs(Lumenizer* pThemBrightness) override
    {
        bool result = Lumenizer::sameAs(pThemBrightness);
        if (result)
        {
            LumenizerSequence* pThem = static_cast<LumenizerSequence*>(pThemBrightness);
            result = this->count() == pThem->count();
            if (result)
            {
                for (int i = 0; i < count(); i++)
                {
                    Lumenizer* pOurs = this->_lumenizers[i];
                    Lumenizer* pTheirs = pThem->_lumenizers[i];
                    result = pOurs->sameAs(pTheirs);
                    if (!result)
                    {
                        break;
                    }
                }
            }
        }
        return result;
    }

    //----------------------------------------------------------------------------------------------
    // Collections
    //----------------------------------------------------------------------------------------------
public:

    BRIGHTNESS currentBrightness() override
    {
        if (_currentLumenizer < count())
        {
            return _lumenizers[_currentLumenizer]->currentBrightness();
        }
        else
        {
            return Lumenizer::currentBrightness();
        }
    }

    BRIGHTNESS currentBrightness(int iPixel) override
    {
        if (_currentLumenizer < count())
        {
            return _lumenizers[_currentLumenizer]->currentBrightness(iPixel);
        }
        else
        {
            return Lumenizer::currentBrightness(iPixel);
        }
    }

    bool hasPixelizedBrightness() override
    {
        if (_currentLumenizer < count())
        {
            return _lumenizers[_currentLumenizer]->hasPixelizedBrightness();
        }
        else
        {
            return Lumenizer::hasPixelizedBrightness();
        }
    }

    bool usesGammaCorrection() override
    {
        if (_currentLumenizer < count())
        {
            return _lumenizers[_currentLumenizer]->usesGammaCorrection();
        }
        else
        {
            return Lumenizer::usesGammaCorrection();
        }
    }

    void setBrightnessMax(BRIGHTNESS brightness) override
    {
        Lumenizer::setBrightnessMax(brightness);
        for (int i = 0; i < count(); i++)
        {
            _lumenizers[i]->setBrightnessMax(brightness);
        }
    }
    void setBrightnessMin(BRIGHTNESS brightness) override
    {
        Lumenizer::setBrightnessMin(brightness);
        for (int i = 0; i < count(); i++)
        {
            _lumenizers[i]->setBrightnessMin(brightness);
        }
    }

    // Controlled by external faders etc
    void setDimmerLevel(float dimmerLevel) override
    {
        Lumenizer::setDimmerLevel(dimmerLevel);
        for (int i = 0; i < count(); i++)
        {
            _lumenizers[i]->setDimmerLevel(dimmerLevel);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    void processParameterBlock(ColorLuminanceParameterBlock& parameterBlock) override
    {
        Lumenizer::processParameterBlock(parameterBlock);
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    void begin() override
    {
        Lumenizer::begin();
        _currentLumenizer = 0;
        if (_currentLumenizer < count())
        {
            INFO("LumenizerSequence: beginning: %d", _currentLumenizer);
            _lumenizers[_currentLumenizer]->begin();
        }
    }

    bool hasExpired() override
    {
        if (_looping)
        {
            return false;
        }
        else
        {
            return _currentLumenizer >= count();
        }
    }

    void loop() override
    {
        Lumenizer::loop();
        if (_currentLumenizer < count())
        {
            // Run the current guy
            _lumenizers[_currentLumenizer]->loop();

            // If we're done running him, move on to the next
            if (_lumenizers[_currentLumenizer]->hasExpired())
            {
                _currentLumenizer++;
                if (_currentLumenizer == count() && _looping)
                {
                    _currentLumenizer = 0;
                }
                if (_currentLumenizer < count())
                {
                    INFO("LumenizerSequence: beginning: %d", _currentLumenizer);
                    _lumenizers[_currentLumenizer]->begin();
                }
            }
        }
    }

    void report() override
    {
        Lumenizer::report();
        INFO("LumenizerSequence: count=%d cur=%d", count(), _currentLumenizer);
        for (int i = 0; i < count(); i++)
        {
            _lumenizers[i]->report();
        }
    }

};

#endif

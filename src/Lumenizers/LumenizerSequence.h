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

    LumenizerSequence(Flavor flavor = Flavor::Sequence) : Lumenizer(flavor, Duration(0) /*ignored*/)
    {
        _currentLumenizer = 0;
        _looping = false;
    }

    ~LumenizerSequence() override
    {
        releaseLumenizers();
    }

    void releaseLumenizers()
    {
        for (int i = 0; i < _lumenizers.count(); i++)
        {
            ::releaseRef(_lumenizers[i]);
        }
        _lumenizers.clear();
    }

    void ownLumenizer(Lumenizer* pLumenizer /*takes ownwership*/)
    {
        delgateTo(pLumenizer);
        _lumenizers.addLast(pLumenizer);    // takes ownership
    }

    void noteColorizeable(Colorizeable* pColorizeable) override
    {
        Lumenizer::noteColorizeable(pColorizeable);
        for (int i = 0; i < count(); i++)
        {
            _lumenizers[i]->noteColorizeable(pColorizeable);
        }
    }

    bool sameAs(Lumenizer* pThemLumenizer) override
    {
        bool result = Lumenizer::sameAs(pThemLumenizer);
        if (result)
        {
            LumenizerSequence* pThem = static_cast<LumenizerSequence*>(pThemLumenizer);
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
    // Sequence
    //----------------------------------------------------------------------------------------------

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

    //----------------------------------------------------------------------------------------------
    // Duration
    //----------------------------------------------------------------------------------------------

    void setDuration(Duration duration) override
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

    //----------------------------------------------------------------------------------------------
    // Collections
    //----------------------------------------------------------------------------------------------
public:

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
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    void begin() override
    {
        Lumenizer::begin();
        _currentLumenizer = 0;
        if (_currentLumenizer < count())
        {
            onAdvance();
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
                    onAdvance();
                }
            }
        }
    }

    virtual void onAdvance()
    {
        INFO("LumenizerSequence: beginning: %d", _currentLumenizer);
        lumenizer()->begin();
    }

    Lumenizer* lumenizer()
    {
        return _lumenizers[_currentLumenizer];
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

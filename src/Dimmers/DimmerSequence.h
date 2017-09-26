//
// DimmerSequence.h
//
#ifndef __DIMMER_SEQUENCE_H__
#define __DIMMER_SEQUENCE_H__

#include "Util/ArrayList.h"
#include "Dimmer.h"
#include "Colorizers/ConstantColor.h"

//==================================================================================================
// ColorizerSequence
//==================================================================================================

struct DimmerSequence : Dimmer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    ArrayList<Dimmer*> _dimmers;
    int _currentDimmer;
    bool _looping;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    DimmerSequence() : Dimmer(DimmerFlavorSequence, 0 /*ignored*/)
    {
        _currentDimmer = 0;
        _looping = false;
    }

    ~DimmerSequence() override
    {
        for (int i = 0; i < _dimmers.count(); i++)
        {
            releaseRef(_dimmers[i]);
        }
    }

    void addDimmer(Dimmer* pDimmer /*takes ownwership*/)
    {
        pDimmer->setColorizeable(_pColorizeable);
        pDimmer->setMaxBrightness(_maxBrightness);
        pDimmer->setMinBrightness(_minBrightness);
        pDimmer->setDimmerBrightness(_dimmerBrightness);
        _dimmers.addLast(pDimmer);
    }

    void setColorizeable(Colorizeable* pColorizeable) override
    {
        Dimmer::setColorizeable(pColorizeable);
        for (int i = 0; i < count(); i++)
        {
            _dimmers[i]->setColorizeable(pColorizeable);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    void setDuration(int msDuration) override
    {
        Log.error("invalid call: DimmerSequence::setDuration()");
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
            ms += _dimmers[i]->msDuration();
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
        return _dimmers.isEmpty();
    }

    int count()
    {
        return _dimmers.count();
    }

    bool sameAs(Dimmer* pThemAbstract) override
    {
        bool result = Dimmer::sameAs(pThemAbstract);
        if (result)
        {
            DimmerSequence* pThem = static_cast<DimmerSequence*>(pThemAbstract);
            result = this->count() == pThem->count();
            if (result)
            {
                for (int i = 0; i < count(); i++)
                {
                    Dimmer* pOurs = this->_dimmers[i];
                    Dimmer* pTheirs = pThem->_dimmers[i];
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
        if (_currentDimmer < count())
        {
            return _dimmers[_currentDimmer]->currentBrightness();
        }
        else
        {
            return Dimmer::currentBrightness();
        }
    }

    BRIGHTNESS currentBrightness(int iPixel) override
    {
        if (_currentDimmer < count())
        {
            return _dimmers[_currentDimmer]->currentBrightness(iPixel);
        }
        else
        {
            return Dimmer::currentBrightness(iPixel);
        }
    }

    bool hasPixelizedBrightness() override
    {
        if (_currentDimmer < count())
        {
            return _dimmers[_currentDimmer]->hasPixelizedBrightness();
        }
        else
        {
            return Dimmer::hasPixelizedBrightness();
        }
    }

    bool usesGammaCorrection() override
    {
        if (_currentDimmer < count())
        {
            return _dimmers[_currentDimmer]->usesGammaCorrection();
        }
        else
        {
            return Dimmer::usesGammaCorrection();
        }
    }

    void setMaxBrightness(BRIGHTNESS brightness) override
    {
        Dimmer::setMaxBrightness(brightness);
        for (int i = 0; i < count(); i++)
        {
            _dimmers[i]->setMaxBrightness(brightness);
        }
    }
    void setMinBrightness(BRIGHTNESS brightness) override
    {
        Dimmer::setMinBrightness(brightness);
        for (int i = 0; i < count(); i++)
        {
            _dimmers[i]->setMinBrightness(brightness);
        }
    }

    // Controlled by external faders etc
    void setDimmerBrightness(BRIGHTNESS dimmerBrightness) override
    {
        Dimmer::setDimmerBrightness(dimmerBrightness);
        for (int i = 0; i < count(); i++)
        {
            _dimmers[i]->setDimmerBrightness(dimmerBrightness);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    void begin() override
    {
        Dimmer::begin();
        _currentDimmer = 0;
        if (_currentDimmer < count())
        {
            INFO("DimmerSequence: beginning: %d", _currentDimmer);
            _dimmers[_currentDimmer]->begin();
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
            return _currentDimmer >= count();
        }
    }

    void loop() override
    {
        Dimmer::loop();
        if (_currentDimmer < count())
        {
            // Run the current guy
            _dimmers[_currentDimmer]->loop();

            // If we're done running him, move on to the next
            if (_dimmers[_currentDimmer]->hasExpired())
            {
                _currentDimmer++;
                if (_currentDimmer == count() && _looping)
                {
                    _currentDimmer = 0;
                }
                if (_currentDimmer < count())
                {
                    INFO("DimmerSequence: beginning: %d", _currentDimmer);
                    _dimmers[_currentDimmer]->begin();
                }
            }
        }
    }

    void report() override 
    {
        Dimmer::report();
        INFO("Dimmer: #dimmers=%d cur=%d", count(), _currentDimmer);
        for (int i = 0; i < count(); i++)
        {
            _dimmers[i]->report();
        }
    }

};

#endif

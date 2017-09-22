//
// DimmerSequence.h
//
#ifndef __DIMMER_SEQUENCE_H__
#define __DIMMER_SEQUENCE_H__

#include "Dimmer.h"
#include "ConstantColor.h"
#include "ArrayList.h"

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

    override ~DimmerSequence()
    {
        for (int i = 0; i < _dimmers.count(); i++)
        {
            releaseRef(_dimmers[i]);
        }
    }

    void addDimmer(Dimmer* pDimmer /*takes ownwership*/)
    {
        pDimmer->setMaxBrightness(_maxBrightness);
        pDimmer->setMinBrightness(_minBrightness);
        pDimmer->setDimmerBrightness(_dimmerBrightness);
        _dimmers.addLast(pDimmer);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    override void setDuration()
    {
        Log.error("invalid call: DimmerSequence::setDuration()");
    }
    override int msDuration()
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

    override int msLoopingDuration()
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

    override bool sameAs(Dimmer* pThemAbstract)
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

    override BRIGHTNESS currentBrightness()
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

    override void setMaxBrightness(BRIGHTNESS brightness)
    {
        Dimmer::setMaxBrightness(brightness);
        for (int i = 0; i < count(); i++)
        {
            _dimmers[i]->setMaxBrightness(brightness);
        }
    }
    override void setMinBrightness(BRIGHTNESS brightness)
    {
        Dimmer::setMinBrightness(brightness);
        for (int i = 0; i < count(); i++)
        {
            _dimmers[i]->setMinBrightness(brightness);
        }
    }

    // Controlled by external faders etc
    override void setDimmerBrightness(BRIGHTNESS dimmerBrightness)
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

    override void begin()
    {
        Dimmer::begin();
        _currentDimmer = 0;
        if (_currentDimmer < count())
        {
            INFO("DimmerSequence: beginning: %d", _currentDimmer);
            _dimmers[_currentDimmer]->begin();
        }
    }

    override bool hasExpired()
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

    override void loop()
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

    override void report()
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

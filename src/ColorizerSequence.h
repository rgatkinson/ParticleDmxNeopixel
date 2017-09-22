//
// ColorizerSequence.h
//
#ifndef __COLORIZER_SEQUENCE_H__
#define __COLORIZER_SEQUENCE_H__

#include "Colorizer.h"
#include "ConstantColor.h"
#include "ArrayList.h"

//==================================================================================================
// ColorizerSequence
//==================================================================================================

struct ColorizerSequence : Colorizer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    ArrayList<Colorizer*> _colorizers;
    int _currentColorizer;
    bool _looping;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    ColorizerSequence() : Colorizer(ColorizerFlavorSequence, 0 /*ignored*/)
    {
        _currentColorizer = 0;
        _looping = false;
    }

    override ~ColorizerSequence()
    {
        for (int i = 0; i < _colorizers.count(); i++)
        {
            releaseRef(_colorizers[i]);
        }
    }

    void addColorizer(Colorizer* pColorizer /*takes ownwership*/)
    {
        pColorizer->setColorizeable(_pColorizeable);
        _colorizers.addLast(pColorizer);
    }

    virtual void setColorizeable(Colorizeable* pColorizeable)
    {
        Colorizer::setColorizeable(pColorizeable);
        for (int i = 0; i < count(); i++)
        {
            _colorizers[i]->setColorizeable(pColorizeable);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    override void setDuration()
    {
        Log.error("invalid call: ColorizerSequence::setDuration()");
    }
    override int msDuration()
    {
        if (_looping)
        {
            return Deadline::Infinite;
        }
        else
        {
            int ms = 0;
            for (int i = 0; i < count(); i++)
            {
                ms += _colorizers[i]->msDuration();
            }
            ms = min(ms, Deadline::Infinite);
            return ms;
        }
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
        return _colorizers.isEmpty();
    }

    int count()
    {
        return _colorizers.count();
    }

    override bool sameAs(Colorizer* pThemAbstract)
    {
        bool result = Colorizer::sameAs(pThemAbstract);
        if (result)
        {
            ColorizerSequence* pThem = static_cast<ColorizerSequence*>(pThemAbstract);
            result = this->count() == pThem->count();
            if (result)
            {
                for (int i = 0; i < count(); i++)
                {
                    Colorizer* pOurs = this->_colorizers[i];
                    Colorizer* pTheirs = pThem->_colorizers[i];
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

    virtual bool isSequence()  // a one-off dynamic_cast
    {
        return true;
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    override void begin()
    {
        Colorizer::begin();
        _currentColorizer = 0;
        if (_currentColorizer < count())
        {
            _colorizers[_currentColorizer]->begin();
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
            return _currentColorizer >= count();
        }
    }

    override void loop()
    {
        Colorizer::loop();
        if (_currentColorizer < count())
        {
            // Run the current guy
            _colorizers[_currentColorizer]->loop();

            // If we're done running him, move on to the next
            if (_colorizers[_currentColorizer]->hasExpired())
            {
                _currentColorizer++;
                if (_currentColorizer == count() && _looping)
                {
                    _currentColorizer = 0;
                }
                if (_currentColorizer < count())
                {
                    INFO("ColorizerSequence: beginning: %d", _currentColorizer);
                    _colorizers[_currentColorizer]->begin();
                }
            }
        }
    }

    override void report()
    {
        Colorizer::report();
        INFO("ColorizerSequence: %d colorizers cur=%d", count(), _currentColorizer);
        for (int i = 0; i < count(); i++)
        {
            _colorizers[i]->report();
        }
    }

};

#endif

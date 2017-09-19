//
// ColorizerSequence.h
//
#ifndef __COLORIZER_SEQUENCE_H__
#define __COLORIZER_SEQUENCE_H__

#include "Colorizer.h"
#include "ConstantColor.h"
#include "ArrayList.h"

//==================================================================================================
// ColorizerAndDeadline
//==================================================================================================

struct ColorizerAndDeadline
{
    Colorizer*  pColorizer;
    Deadline    deadline;

    ColorizerAndDeadline(Colorizer* pColorizer, int msInterval)
    {
        this->pColorizer = pColorizer; // takes ownership
        this->deadline = Deadline(msInterval);
    }
    ~ColorizerAndDeadline()
    {
        delete pColorizer;
    }

    void begin()
    {
        pColorizer->begin();
        deadline.reset();
    }

    bool sameAs(ColorizerAndDeadline* pThem)
    {
        return this->pColorizer->sameAs(pThem->pColorizer)
            && this->deadline.msDuration() == pThem->deadline.msDuration();
    }
};


//==================================================================================================
// ColorizerSequence
//==================================================================================================

struct ColorizerSequence : Colorizer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    ArrayList<ColorizerAndDeadline*> _colorizers;
    int _currentColorizer;
    bool _looping;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    ColorizerSequence() : Colorizer(ColorizerModeSequence)
    {
        _currentColorizer = 0;
        _looping = false;
    }

    override ~ColorizerSequence()
    {
        for (int i = 0; i < _colorizers.count(); i++)
        {
            delete _colorizers[i];
        }
    }

    void addColorizer(Colorizer* pColorizer /*takes ownwership*/, int ms)
    {
        if (pColorizer->isSequence())
        {
            ColorizerSequence* pSequence = static_cast<ColorizerSequence*>(pColorizer);
            addColorizer(pSequence);    // ignores ms
        }
        else
        {
            ColorizerAndDeadline* pPair = new ColorizerAndDeadline(pColorizer, ms);
            _colorizers.addLast(pPair);
        }
    }

    void addColorizer(ColorizerSequence* pColorizer /*takes ownwership*/)
    {
        for (int i = 0; i < pColorizer->count(); i++)
        {
            _colorizers.addLast(pColorizer->_colorizers[i]);
            pColorizer->_colorizers[i] = NULL;
        }
        delete pColorizer;
    }

    virtual void setColorizeable(Colorizeable* pColorizeable)
    {
        Colorizer::setColorizeable(pColorizeable);
        for (int i = 0; i < count(); i++)
        {
            _colorizers[i]->pColorizer->setColorizeable(pColorizeable);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
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
                    ColorizerAndDeadline* pOurs = this->_colorizers[i];
                    ColorizerAndDeadline* pTheirs = pThem->_colorizers[i];
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
        Log.info("ColorizerSequence: begin: %d remain ms=%d", count(), millis());
        _currentColorizer = 0;
        if (_currentColorizer < count())
        {
            _colorizers[_currentColorizer]->begin();
        }
    }

    override void loop()
    {
        Colorizer::loop();  // pro forma
        if (_currentColorizer < count())
        {
            // Run the current guy
            _colorizers[_currentColorizer]->pColorizer->loop();

            // If we're done running him, move on to the next
            if (_colorizers[_currentColorizer]->deadline.hasExpired())
            {
                _currentColorizer++;
                if (_currentColorizer == count() && _looping)
                {
                    _currentColorizer = 0;
                }
                if (_currentColorizer < count())
                {
                    Log.info("ColorizerSequence: moving to next: %d remain ms=%d", count()-_currentColorizer, millis());
                    _colorizers[_currentColorizer]->begin();
                }
            }
        }
    }

    override void report()
    {
        Colorizer::report(); // pro forma
        Log.info("ColorizerSequence: %d _colorizers cur=%d", count(), _currentColorizer);
        for (int i = 0; i < count(); i++)
        {
            _colorizers[i]->pColorizer->report();
        }
    }

};

#endif

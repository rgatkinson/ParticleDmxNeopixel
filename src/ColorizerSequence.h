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

    ArrayList<ColorizerAndDeadline*> colorizers;
    int iCur;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    ColorizerSequence() : Colorizer(ColorizerModeSequence)
    {
        iCur = 0;
    }

    override ~ColorizerSequence()
    {
        for (int i = 0; i < colorizers.count(); i++)
        {
            delete colorizers[i];
        }
    }

    void addColorizer(Colorizer* pColorizer /*takes ownwership*/, int ms)
    {
        ColorizerAndDeadline* pPair = new ColorizerAndDeadline(pColorizer, ms);
        colorizers.addLast(pPair);
    }

    virtual void setColorizeable(Colorizeable* pColorizeable)
    {
        Colorizer::setColorizeable(pColorizeable);
        for (int i = 0; i < count(); i++)
        {
            colorizers[i]->pColorizer->setColorizeable(pColorizeable);
        }
    }


    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    bool isEmpty()
    {
        return colorizers.isEmpty();
    }

    int count()
    {
        return colorizers.count();
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
                    ColorizerAndDeadline* pOurs = this->colorizers[i];
                    ColorizerAndDeadline* pTheirs = pThem->colorizers[i];
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
    // Loop
    //----------------------------------------------------------------------------------------------

    override void begin()
    {
        Log.info("ColorizerSequence: begin: %d remain ms=%d", count(), millis());
        iCur = 0;
        if (iCur < count())
        {
            colorizers[iCur]->begin();
        }
    }

    override void loop()
    {
        Colorizer::loop();  // pro forma
        if (iCur < count())
        {
            // Run the first guy
            colorizers[iCur]->pColorizer->loop();

            // If we're done running him, move on to the next
            if (colorizers[iCur]->deadline.hasExpired())
            {
                iCur++;
                Log.info("ColorizerSequence: moving to next: %d remain ms=%d", count()-iCur, millis());
                if (iCur < count())
                {
                    colorizers[iCur]->begin();
                }
            }
        }
    }

    override void report()
    {
        Colorizer::report(); // pro forma
        Log.info("ColorizerSequence: %d colorizers cur=%d", count(), iCur);
        for (int i = 0; i < count(); i++)
        {
            colorizers[i]->pColorizer->report();
        }
    }

};

#endif

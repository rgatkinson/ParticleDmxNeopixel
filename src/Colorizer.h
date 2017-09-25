//
// Colorizer.h
//
#ifndef __COLORIZER_H__
#define __COLORIZER_H__

#include "Misc.h"
#include "Color.h"
#include "Deadline.h"
#include "Durable.h"
#include "Colorizeable.h"

//==================================================================================================
// Colorizer
//==================================================================================================

struct Colorizer : Durable
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
public:

    enum ColorizerFlavor { ColorizerFlavorNone, ColorizerFlavorConstant, ColorizerFlavorRainbow, ColorizerFlavorSequence };

protected:

    ColorizerFlavor _flavor;
    Colorizeable*   _pColorizeable;
    int             _pixelCount;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Colorizer(ColorizerFlavor flavor, int msDuration) : Durable(msDuration)
    {
        _flavor = flavor;
        _pColorizeable = NULL;
        _pixelCount = 0;
    }

    virtual void setColorizeable(Colorizeable* pColorizeable)
    {
        _pColorizeable = pColorizeable;
        _pixelCount = pColorizeable==NULL ? 0 : pColorizeable->numberOfPixels();
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:

    override bool sameAs(Colorizer* pThem)
    {
        return Durable::sameAs(pThem) && _flavor == pThem->_flavor;
    }

    virtual int msLoopingDuration()
    {
        return msDuration();
    }

protected:

    void setPixelColor(uint16_t iPixel, COLOR_INT color);

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    virtual void begin()
    {
        _duration.reset();
    }

    virtual void loop()
    {
    }

    virtual void report()
    {
    }
};

#endif

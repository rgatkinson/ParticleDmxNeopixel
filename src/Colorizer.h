//
// Colorizer.h
//
#ifndef __COLORIZER_H__
#define __COLORIZER_H__

#include "Misc.h"
#include "Color.h"
#include "Deadline.h"
#include "ReferenceCounted.h"

//==================================================================================================
// Colorizeable
//==================================================================================================

struct Colorizeable
{
    virtual void setPixelColor(uint16_t iPixel, COLOR_INT color) = 0;
    virtual int  numberOfPixels() = 0;
};

//==================================================================================================
// Colorizer
//==================================================================================================

struct Colorizer : ReferenceCounted
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
    Deadline        _duration;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Colorizer(ColorizerFlavor flavor, int msDuration) : _duration(msDuration)
    {
        _flavor = flavor;
        _pColorizeable = NULL;
        _pixelCount = 0;
    }

    virtual void setDuration(int msDuration)
    {
        _duration = Deadline(msDuration);
    }

    virtual void setColorizeable(Colorizeable* pColorizeable)
    {
        _pColorizeable = pColorizeable;
        _pixelCount = pColorizeable==NULL ? 0 : pColorizeable->numberOfPixels();
    }

protected:

    virtual ~Colorizer()
    {
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:

    virtual int msDuration()
    {
        return _duration.msDuration();
    }

    virtual bool hasExpired()
    {
        return _duration.hasExpired();
    }

    virtual bool sameAs(Colorizer* pThem)
    {
        return _flavor == pThem->_flavor;
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

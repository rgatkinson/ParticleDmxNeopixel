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
    Deadline        _colorUpdateDeadline;
    Colorizeable*   _pColorizeable;
    int             _pixelCount;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Colorizer(ColorizerFlavor flavor)
    {
        _flavor = flavor;
        _pColorizeable = NULL;
    }

    virtual void setColorizeable(Colorizeable* pColorizeable)
    {
        _pColorizeable = pColorizeable;
        _pixelCount = pColorizeable->numberOfPixels();
    }

protected:

    virtual ~Colorizer()
    {
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:

    virtual bool sameAs(Colorizer* pThem)
    {
        return _flavor == pThem->_flavor;
    }

    ColorizerFlavor colorizerMode()
    {
        return _flavor;
    }

    virtual bool isSequence()  // a one-off dynamic_cast
    {
        return false;
    }

protected:

    void setPixelColor(uint16_t iPixel, COLOR_INT color);

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    virtual void begin()
    {
    }

    virtual void loop()
    {
    }

    virtual void report()
    {
    }
};

#endif

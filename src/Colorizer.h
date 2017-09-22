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

    enum ColorizerMode { ColorizerModeNone, ColorizerModeConstant, ColorizerModeRainbow, ColorizerModeSequence };

protected:

    ColorizerMode   _mode;
    Deadline        colorUpdateDeadline;
    Colorizeable*   pColorizeable;
    int             pixelCount;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Colorizer(ColorizerMode mode)
    {
        _mode = mode;
        pColorizeable = NULL;
    }

    virtual void setColorizeable(Colorizeable* pColorizeable)
    {
        this->pColorizeable = pColorizeable;
        this->pixelCount = pColorizeable->numberOfPixels();
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
        return this->_mode == pThem->_mode;
    }

    ColorizerMode mode()
    {
        return _mode;
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

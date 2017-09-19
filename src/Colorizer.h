//
// Colorizer.h
//
#ifndef __COLORIZER_H__
#define __COLORIZER_H__

#include "Misc.h"
#include "Color.h"
#include "Deadline.h"

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

struct Colorizer
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

    virtual ~Colorizer()
    {
    }

    virtual void setColorizeable(Colorizeable* pColorizeable)
    {
        this->pColorizeable = pColorizeable;
        this->pixelCount = pColorizeable->numberOfPixels();
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    virtual bool sameAs(Colorizer* pThem)
    {
        return this->_mode == pThem->_mode;
    }

    ColorizerMode mode()
    {
        return _mode;
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

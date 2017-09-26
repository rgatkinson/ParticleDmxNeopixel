//
// Colorizer.h
//
#ifndef __COLORIZER_H__
#define __COLORIZER_H__

#include "Util/Misc.h"
#include "Util/Color.h"
#include "Util/Deadline.h"
#include "Util/Durable.h"
#include "Pixels/Colorizeable.h"
#include "ColorizableHolder.h"

//==================================================================================================
// Colorizer
//==================================================================================================

struct Colorizer : Durable, ColorizableHolder
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
public:

    enum class Flavor
    {
        None,
        Uniform,
        Rainbow,
        Sequence
    };

protected:

    Flavor _flavor;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Colorizer(Flavor flavor, int msDuration) : Durable(msDuration), ColorizableHolder(false)
    {
        _flavor = flavor;
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:

    virtual bool sameAs(Colorizer* pThem)
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

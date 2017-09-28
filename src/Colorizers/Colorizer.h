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
#include "Pixels/ColorizeableHolder.h"
#include "Artnet/DmxParameterBlock.h"

//==================================================================================================
// Colorizer
//==================================================================================================

struct Colorizer : Durable, ColorizeableHolder
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
        Sequence,
        SelfTest
    };

protected:

    Flavor _flavor;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Colorizer(Flavor flavor, int msDuration) : Durable(msDuration), ColorizeableHolder(false)
    {
        _flavor = flavor;
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:

    virtual bool sameAs(Colorizer* pThem)
    {
        // Do NOT include parameters that will be adjusted by DMX control
        return Durable::sameAs(pThem) && _flavor == pThem->_flavor;
    }

    virtual int msLoopingDuration()
    {
        return msDuration();
    }

protected:

    void setPixelColor(uint16_t iPixel, COLOR_INT color);

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    virtual void processParameterBlock(DmxParameterBlock& parameterBlock)
    {
    }

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

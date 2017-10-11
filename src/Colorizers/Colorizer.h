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
#include "DmxParams/DmxColorLuminanceParameters.h"

//==================================================================================================
// Colorizer
//==================================================================================================

struct Colorizer : Durable, protected ColorizeableHolder
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

    static LPCSTR nameOf(Flavor flavor)
    {
        switch (flavor)
        {
            case Flavor::None:      return "None";
            case Flavor::Uniform:   return "Uniform";
            case Flavor::Sequence:  return "Sequence";
            case Flavor::Rainbow:   return "Rainbow";
            case Flavor::SelfTest:  return "SelfTest";
            default:                return "<unknown>";
        }
    }


protected:

    Flavor _flavor;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Colorizer(Flavor flavor, Duration duration) : Durable(duration), ColorizeableHolder()
    {
        _flavor = flavor;
    }

    virtual void noteColorizeable(Colorizeable* pColorizeable)
    {
        ColorizeableHolder::noteColorizeable(pColorizeable);
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

    virtual void processParameterBlock(DmxColorLuminanceParameters& parameterBlock)
    {
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    virtual void begin()
    {
        _deadline.reset();
    }

    virtual void loop()
    {
    }

    virtual void report()
    {
        INFO("Colorizer(%s)", nameOf(_flavor));
    }
};

#endif

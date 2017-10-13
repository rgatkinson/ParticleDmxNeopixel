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
#include "DmxParams/DmxColorLuminanceParameters.h"

//==================================================================================================
// Colorizer
//==================================================================================================

struct Colorizer : Durable
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

    Flavor          _flavor;
    Colorizeable*   _pColorizeable;     // no ref
    int             _pixelCount;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Colorizer(Flavor flavor, Duration duration) : Durable(duration)
    {
        _flavor = flavor;
        _pColorizeable = nullptr;
        _pixelCount = 0;
    }

    virtual void noteColorizeable(Colorizeable* pColorizeable)
    {
        _pColorizeable = pColorizeable;
        _pixelCount = pColorizeable ? 0 : pColorizeable->numberOfPixels();
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

    void setPixelColor(uint16_t iPixel, Color color);

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    virtual void processDmxColorLuminance(DmxColorLuminanceParameters& parameterBlock)
    {
        // nothing, generically, to do
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

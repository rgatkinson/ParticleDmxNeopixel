//
// ElapsedTime.h
//
#ifndef _ELAPSED_TIME_H_
#define _ELAPSED_TIME_H_

#include "Misc.h"

typedef long TIME;

struct ElapsedTime
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    static const TIME Infinite = 604800000;  // a week in ms

    TIME _msStartTime;
    static TIME loopNow;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    ElapsedTime()
    {
        reset();
    }

    ElapsedTime(TIME _msStartTime)
    {
        this->_msStartTime = _msStartTime;
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    static void begin()
    {
        updateNow();
    }
    static void loop()
    {
        updateNow();
    }

    //----------------------------------------------------------------------------------------------
    // Operations
    //----------------------------------------------------------------------------------------------

    TIME milliseconds()
    {
        return msNow() - _msStartTime;
    }

    virtual void reset()
    {
        _msStartTime = msNow();
    }

protected:

    TIME msNow()
    {
        return computeNow();
        // return loopNow;  // causes problems
    }

private:

    static void updateNow()
    {
        loopNow = computeNow();
    }

    static TIME computeNow()
    {
        return (TIME)(millis() & 0x7FFFFFFF);   // avoid unsigned; ~25 day range
    }
};

TIME ElapsedTime::loopNow = ElapsedTime::computeNow();

#endif

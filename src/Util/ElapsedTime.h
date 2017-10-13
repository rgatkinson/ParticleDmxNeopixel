//
// ElapsedTime.h
//
#ifndef _ELAPSED_TIME_H_
#define _ELAPSED_TIME_H_

#include "Misc.h"

typedef int TIME;

struct ElapsedTime
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    static const TIME Infinite = 604800000;  // a week in ms

    TIME _msStartTime;

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
    // Operations
    //----------------------------------------------------------------------------------------------

    TIME seconds()
    {
        return milliseconds() / 1000;
    }

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
    }

private:

    static TIME computeNow()
    {
        return (TIME)(millis() & 0x7FFFFFFF);   // avoid unsigned; ~25 day range
    }
};

#endif

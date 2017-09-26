//
// Deadline.h
//
#ifndef _DEADLINE_H_
#define _DEADLINE_H_

#include "ElapsedTime.h"

struct Deadline : ElapsedTime
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    TIME _msDuration;
    TIME _msDeadline;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Deadline()
    {
        // This ctor not intended to be functional. Useful for uninitialized variables.
        _msDuration = 0;
        _msDeadline = 0;
    }

    Deadline(TIME _msDuration)
    {
        this->_msDuration = _msDuration;
        this->_msDeadline = _msStartTime + _msDuration;
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    void reset() override
    {
        ElapsedTime::reset();
        _msDeadline = _msStartTime + _msDuration;
    }

    void expire()
    {
        ElapsedTime::reset();
        _msDeadline = _msStartTime;
    }

    TIME msDuration()
    {
        return _msDuration;
    }

    TIME msRemaining()
    {
        TIME result = _msDeadline - msNow();
        return max(0, result);
    }

    bool hasExpired()
    {
        return msRemaining() == 0;
    }

};

#endif

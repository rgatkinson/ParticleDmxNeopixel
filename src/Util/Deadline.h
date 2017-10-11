//
// Deadline.h
//
#ifndef _DEADLINE_H_
#define _DEADLINE_H_

#include "ElapsedTime.h"

//==================================================================================================
// Duration
//==================================================================================================

struct Duration
{
public:
    static Duration Infinite;

private:
    TIME _ms;

public:
    explicit Duration(TIME ms)
    {
        _ms = ms;
    }
    TIME ms()
    {
        return _ms;
    }
};

Duration SELECTANY Duration::Infinite = Duration(ElapsedTime::Infinite);

//==================================================================================================
// Deadline
//==================================================================================================

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

    Deadline(TIME msDuration)
    {
        _msDuration = msDuration;
        _msDeadline = _msStartTime + _msDuration;
    }

    Deadline(Duration duration) : Deadline(duration.ms())
    {
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

    void consume(int ms)
    {
        _msDeadline -= ms;
        _msStartTime -= ms;
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

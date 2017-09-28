//
// Durable.h
//
#ifndef __DURABLE_H__
#define __DURABLE_H__

#include "ReferenceCounted.h"

struct Durable : ReferenceCounted
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    Deadline _duration;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Durable(int msDuration) : _duration(msDuration)
    {
    }

    virtual void setDuration(int msDuration)
    {
        _duration = Deadline(msDuration);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    bool sameAs(Durable* pThem)
    {
        return pThem != nullptr && _duration.msDuration() == pThem->_duration.msDuration();
    }

    virtual int msDuration()
    {
        return _duration.msDuration();
    }

    virtual bool hasExpired()
    {
        return _duration.hasExpired();
    }

};

#endif

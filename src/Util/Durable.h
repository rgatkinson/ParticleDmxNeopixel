//
// Durable.h
//
#ifndef __DURABLE_H__
#define __DURABLE_H__

#include "ReferenceCounted.h"
#include "Util/Deadline.h"

struct Durable : ReferenceCounted
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    Deadline _deadline;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Durable(Duration duration) : _deadline(duration)
    {
    }

    virtual void setDuration(Duration duration)
    {
        _deadline = Deadline(duration);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    bool sameAs(Durable* pThem)
    {
        return pThem != nullptr; // we *don't* count the duration itself
    }

    virtual int msDuration()
    {
        return _deadline.msDuration();
    }

    virtual bool hasExpired()
    {
        return _deadline.hasExpired();
    }

};

#endif

//
// Looper.h
//
#ifndef __LOOPER_H__
#define __LOOPER_H__

#include "Util/ReferenceCounted.h"

//==================================================================================================
// Looper
//==================================================================================================

struct Looper : RefCountable
{
    virtual void begin() = 0;
    virtual void loop() = 0;
    virtual void report() = 0;
};

#endif

//
// DmxEffect.h
//
#ifndef __DMX_EFFECT_H__
#define __DMX_EFFECT_H__

#include "Util/ReferenceCounted.h"

struct DmxEffect : ReferenceCounted
{

    virtual void onRgbi(byte r, byte g, byte b, byte i)
    {

    }

    virtual void onDmxControl(byte b)
    {

    }

    virtual void onDmxSpeedDirection(byte b)
    {

    }
};

#endif

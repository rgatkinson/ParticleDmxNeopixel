//
// DmxColorEffect.h
//
#ifndef __DMX_COLOR_EFFECT_H__
#define __DMX_COLOR_EFFECT_H__

#include "DmxEffect.h"
#include "Colorizers/Colorizer.h"

struct DmxColorEffect : DmxEffect
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    Colorizer* _pColorizer;
};

#endif

//
// DmxKridaParameters.h
//
#ifndef __DMX_KRIDA_PARAMTERS_H__
#define __DMX_KRIDA_PARAMTERS_H__

#include "Util/Misc.h"
#include "DmxParams/DmxDimmer.h"
#include "DmxParams/DmxEffectSpeedControl.h"

struct PACKED DmxKridaChannel
{
    DmxDimmer              dimmer;
    DmxEffectSpeedControl  luminance;
};

struct PACKED DmxKridaDimmer
{
    DmxKridaChannel channels[4];
    const DmxKridaChannel& channel(int i) const
    {
        return channels[i];
    }
};

struct DmxKridaParameters
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    DmxKridaDimmer* _rgDimmers;
    int             _dimmerCount;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    DmxKridaParameters(void* pv, int dimmerCount)
    {
        _rgDimmers = reinterpret_cast<DmxKridaDimmer*>(pv);
        _dimmerCount = dimmerCount;
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:

    const DmxKridaDimmer& dimmer(int i) const
    {
        return _rgDimmers[i];
    }
};

#endif

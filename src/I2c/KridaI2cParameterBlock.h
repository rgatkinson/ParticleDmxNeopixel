//
// KridaParameterBlock.h
//
#ifndef __KDRIDA_I2C_PARAMETER_BLOCK_H__
#define __KDRIDA_I2C_PARAMETER_BLOCK_H__

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

    DmxKridaDimmer* _pData; // how many?

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    DmxKridaParameters(void* pv)
    {
        _pData = reinterpret_cast<DmxKridaDimmer*>(pv);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:

    const DmxKridaChannel& channel(int i) const
    {
        return _pData->channel(i);
    }
};

#endif

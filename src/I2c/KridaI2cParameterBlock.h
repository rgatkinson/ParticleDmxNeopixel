//
// KridaI2cParameterBlock.h
//
#ifndef __KDRIDA_I2C_PARAMETER_BLOCK_H__
#define __KDRIDA_I2C_PARAMETER_BLOCK_H__

#include "Util/Misc.h"
#include "Artnet/DmxColorLuminanceParameters.h"

struct PACKED DmxKridaI2cChannel
{
    DmxDimmer              dimmer;
    DmxEffectSpeedControl  luminance;
};

struct PACKED DmxKridaI2cDimmer
{
    DmxKridaI2cChannel channels[4];

    const DmxKridaI2cChannel& channel(int i) const
    {
        return channels[i];
    }
};

struct DmxKridaI2cParameters
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    DmxKridaI2cDimmer* _pData;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    DmxKridaI2cParameters(void* pv)
    {
        _pData = reinterpret_cast<DmxKridaI2cDimmer*>(pv);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:

    const DmxKridaI2cChannel& channel(int i) const
    {
        return _pData->channel(i);
    }
};

#endif

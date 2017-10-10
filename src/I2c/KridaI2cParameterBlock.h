//
// KridaI2cParameterBlock.h
//
#ifndef __KDRIDA_I2C_PARAMETER_BLOCK_H__
#define __KDRIDA_I2C_PARAMETER_BLOCK_H__

#include "Util/Misc.h"

struct PACKED KridaI2CParameterBlockData
{
    byte    dimmer0;       // 300
    byte    dimmer1;       // 301
    byte    dimmer2;       // 302
    byte    dimmer3;       // 303
};

struct KridaI2CParameterBlock
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    KridaI2CParameterBlockData* _pData;
    byte*                       _pDimmers;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    KridaI2CParameterBlock(void* pv)
    {
        _pData = reinterpret_cast<KridaI2CParameterBlockData*>(pv);
        _pDimmers = &_pData->dimmer0;
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:

    #define declare(memberName)  inline byte memberName() { return _pData->memberName; }

    declare(dimmer0)
    declare(dimmer1)
    declare(dimmer2)
    declare(dimmer3)

    #undef declare

    byte dimmer(int dimmer)
    {
        return _pDimmers[dimmer];
    }
};

#endif

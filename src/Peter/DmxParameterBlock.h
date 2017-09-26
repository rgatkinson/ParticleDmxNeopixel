//
// DmxParameterBlock.h
//
#ifndef __DMX_PARAMTER_BLOCK_H__
#define __DMX_PARAMTER_BLOCK_H__

struct PACKED DmxParameterBlock
{
    byte    red;
    byte    green;
    byte    blue;
    byte    dimmer;
    byte    colorEffect;
    byte    colorSpeed;
    byte    colorControl;
    byte    brightnessEffect;
    byte    brightnessSpeed;
    byte    brightnessControl;
};

#endif

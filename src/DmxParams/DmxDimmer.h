//
// DmxDimmer.h
//
#ifndef __DMX_DIMMER_H__
#define __DMX_DIMMER_H__

struct PACKED DmxDimmer
{
    byte _value;

    float dimmerLevel() const
    {
        return scaleRange(_value, 0, 255, 0, 1);
    }
};

#endif

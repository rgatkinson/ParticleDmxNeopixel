//
// DmxEffectSelector.h
//
#ifndef __DMX_EFFECT_SELECTOR_H__
#define __DMX_EFFECT_SELECTOR_H__

struct DmxEffectSelector : ReferenceCounted, protected ColorizableHolder
{
    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
protected:

    DmxEffectSelector(Colorizeable* pColorizeable) : ColorizableHolder(true)
    {
        setColorizeable(pColorizeable);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    virtual int numEffects() = 0;

    int effectFromDmx(byte dmx)
    {
        // We divy the range [0, 255] up uniformly into [0, numEffects())
        return zeroOneToInt(float(dmx) / 256, numEffects());
    }
};

#endif

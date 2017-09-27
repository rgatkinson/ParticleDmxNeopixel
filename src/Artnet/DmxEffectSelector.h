//
// DmxEffectSelector.h
//
#ifndef __DMX_EFFECT_SELECTOR_H__
#define __DMX_EFFECT_SELECTOR_H__

struct DmxEffectSelector : ReferenceCounted, protected ColorizeableHolder
{
    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
protected:

    DmxEffectSelector(Colorizeable* pColorizeable) : ColorizeableHolder(true)
    {
        setColorizeable(pColorizeable);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    static int effectFromDmx(byte dmx, int numEffects)
    {
        // We divy the range [0, 255] up uniformly into [0, numEffects())
        return zeroOneToInt(float(dmx) / 256, numEffects);
    }
};

#endif

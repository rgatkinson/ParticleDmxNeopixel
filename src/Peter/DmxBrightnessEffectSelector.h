//
// DmxBrightnessEffectSelector.h
//
#ifndef __DMX_BRIGHTNESS_EFFECT_SELECTOR_H__
#define __DMX_BRIGHTNESS_EFFECT_SELECTOR_H__

#include "DmxParameterBlock.h"
#include "DmxEffectSelector.h"

struct DmxBrightnessEffectSelector : DmxEffectSelector
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
public:

    enum class Effect
    {
        None,
        First,
        Uniform = First,
        Max
    };

protected:

    Effect _currentEffect;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    DmxBrightnessEffectSelector(Colorizeable* pColorizeable) : DmxEffectSelector(pColorizeable)
    {
        _currentEffect = Effect::None;
    }

protected:

    ~DmxBrightnessEffectSelector() override
    {
    }

    int numEffects() override
    {
        return (int)Effect::Max - (int)Effect::First;
    }

    //----------------------------------------------------------------------------------------------
    // Operations
    //----------------------------------------------------------------------------------------------
public:

    void processParameterBlock(DmxParameterBlock& parameterBlock)
    {
        Effect effectDesired = Effect(int(Effect::First) + effectFromDmx(parameterBlock.brightnessEffect()));
        if (_currentEffect != effectDesired)
        {
            _currentEffect = effectDesired;
            INFO("switching to brightness effect %d", effectDesired);
            //
            Dimmer* pDimmer = nullptr;
            switch (effectDesired)
            {
                case Effect::Uniform:
                    pDimmer = new UniformBrightness(1.0f, Deadline::Infinite);
                    break;
            }
            if (pDimmer)
            {
                _pColorizeable->setDimmer(pDimmer);
            }
        }
    }

};

#endif

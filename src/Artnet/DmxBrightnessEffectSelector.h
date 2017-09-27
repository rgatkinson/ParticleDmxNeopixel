//
// DmxBrightnessEffectSelector.h
//
#ifndef __DMX_BRIGHTNESS_EFFECT_SELECTOR_H__
#define __DMX_BRIGHTNESS_EFFECT_SELECTOR_H__

#include "DmxParameterBlock.h"
#include "DmxEffectSelector.h"
#include "DmxColorEffectSelector.h"

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
        Breathing,
        Twinkle,
        Max,
        SelfTest,   // AFTER Max, yes
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

    static int numEffects()
    {
        return (int)Effect::Max - (int)Effect::First;
    }

    //----------------------------------------------------------------------------------------------
    // Operations
    //----------------------------------------------------------------------------------------------
public:

    static Effect brightnessEffect(DmxParameterBlock& parameterBlock)
    {
        return Effect(int(Effect::First) + effectFromDmx(parameterBlock.brightnessEffect(), numEffects()));
    }

    void processParameterBlock(DmxParameterBlock& parameterBlock)
    {
        // Use a self test if the COLOR it set to self test
        Effect effectDesired = DmxColorEffectSelector::Effect::SelfTest == DmxColorEffectSelector::colorEffect(parameterBlock)
            ? Effect::SelfTest
            : brightnessEffect(parameterBlock);

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

                case Effect::Breathing:
                    pDimmer = new BreathingBrightness(4000, Deadline::Infinite);
                    break;

                case Effect::Twinkle:
                    pDimmer = new TwinkleBrightness(4000, Deadline::Infinite);
                    break;

                case Effect::SelfTest:
                    pDimmer = new SelfTestBrightness();
                    break;
            }
            if (pDimmer)
            {
                // TODO: check sameAs
                _pColorizeable->setDimmer(pDimmer);
            }
        }
    }

};

#endif

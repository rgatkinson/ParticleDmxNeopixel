//
// DmxLuminanceEffectSelector.h
//
#ifndef __DMX_BRIGHTNESS_EFFECT_SELECTOR_H__
#define __DMX_BRIGHTNESS_EFFECT_SELECTOR_H__

#include "DmxParameterBlock.h"
#include "DmxEffectSelector.h"
#include "DmxColorEffectSelector.h"

struct DmxLuminanceEffectSelector : DmxEffectSelector
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

    DmxLuminanceEffectSelector(Colorizeable* pColorizeable) : DmxEffectSelector(pColorizeable)
    {
        _currentEffect = Effect::None;
    }

protected:

    ~DmxLuminanceEffectSelector() override
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
            //
            Lumenizer* pLumenizer = nullptr;
            switch (effectDesired)
            {
                case Effect::Uniform:
                    pLumenizer = new UniformLuminance(1.0f, Deadline::Infinite);
                    break;

                case Effect::Breathing:
                    pLumenizer = new BreathingLuminance(4000, Deadline::Infinite);
                    break;

                case Effect::Twinkle:
                    pLumenizer = new TwinkleLumenizer(4000, 1000, Deadline::Infinite);
                    break;

                case Effect::SelfTest:
                    pLumenizer = new SelfTestLumenizer();
                    break;
            }
            if (pLumenizer)
            {
                if (!pLumenizer->sameAs(_pColorizeable->lumenizer()))
                {
                    INFO("switching to brightness effect %d", effectDesired);
                    _pColorizeable->setLumenizer(pLumenizer);
                }
                else
                {
                    releaseRef(pLumenizer);
                }
            }
        }
    }

};

#endif

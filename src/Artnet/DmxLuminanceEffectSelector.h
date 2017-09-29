//
// DmxLuminanceEffectSelector.h
//
#ifndef __DMX_BRIGHTNESS_EFFECT_SELECTOR_H__
#define __DMX_BRIGHTNESS_EFFECT_SELECTOR_H__

#include "DmxParameterBlock.h"
#include "DmxEffectSelector.h"
#include "DmxColorEffectSelector.h"
#include "Lumenizers/UniformLuminance.h"
#include "Lumenizers/BreathingLuminance.h"
#include "Lumenizers/SelfTestLuminance.h"
#include "Lumenizers/MorseCodeLuminance.h"

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
        Last=Twinkle,
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

    //----------------------------------------------------------------------------------------------
    // Operations
    //----------------------------------------------------------------------------------------------
public:

    static Effect luminanceEffect(DmxParameterBlock& parameterBlock)
    {
        return scaleRangeDiscrete(parameterBlock.luminanceEffect(), 0, 255, Effect::First, Effect::Last);
    }

    void processParameterBlock(DmxParameterBlock& parameterBlock)
    {
        // Use a self test if the COLOR it set to self test
        Effect effectDesired = DmxColorEffectSelector::Effect::SelfTest == DmxColorEffectSelector::colorEffect(parameterBlock)
            ? Effect::SelfTest
            : luminanceEffect(parameterBlock);

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
                    pLumenizer = new BreathingLuminance(0, 4000, Deadline::Infinite);
                    break;

                case Effect::Twinkle:
                    pLumenizer = new TwinklingLuminance(4000, 1000, Deadline::Infinite);
                    break;

                case Effect::SelfTest:
                    pLumenizer = new SelfTestLuminance(_pColorizeable->colorizer());
                    break;
            }
            if (pLumenizer)
            {
                if (!pLumenizer->sameAs(_pColorizeable->lumenizer()))
                {
                    INFO("switching to luminance effect %d", effectDesired);
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

//
// DmxColorEffectSelector.h
//
#ifndef __DMX_COLOR_EFFECT_SELECTOR_H__
#define __DMX_COLOR_EFFECT_SELECTOR_H__

#include "DmxParameterBlock.h"
#include "DmxEffectSelector.h"

struct DmxColorEffectSelector : DmxEffectSelector
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
        Rainbow,
        SelfTest,
        Max,
    };

protected:

    Effect _currentEffect;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    DmxColorEffectSelector(Colorizeable* pColorizeable) : DmxEffectSelector(pColorizeable)
    {
        _currentEffect = Effect::None;
    }

protected:

    ~DmxColorEffectSelector() override
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

    static Effect colorEffect(DmxParameterBlock& parameterBlock)
    {
        return Effect(int(Effect::First) + effectFromDmx(parameterBlock.colorEffect(), numEffects()));
    }

    void processParameterBlock(DmxParameterBlock& parameterBlock)
    {
        Effect effectDesired = colorEffect(parameterBlock);
        if (_currentEffect != effectDesired)
        {
            _currentEffect = effectDesired;
            //
            Colorizer* pColorizer = nullptr;
            switch (effectDesired)
            {
                case Effect::Uniform:
                    pColorizer = new UniformColor(Color::WHITE, Deadline::Infinite);
                    break;

                case Effect::Rainbow:
                    pColorizer = new RainbowColors(10, Deadline::Infinite);
                    break;

                case Effect::SelfTest:
                    pColorizer = new SelfTestColorizer(Color::WHITE);
                    break;
            }
            if (pColorizer)
            {
                if (!pColorizer->sameAs(_pColorizeable->colorizer()))
                {
                    INFO("switching to color effect %d", effectDesired);
                    _pColorizeable->setColorizer(pColorizer);
                }
                else
                {
                    releaseRef(pColorizer);
                }
            }
        }
    }

};

#endif

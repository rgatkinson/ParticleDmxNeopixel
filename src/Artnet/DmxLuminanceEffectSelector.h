//
// DmxLuminanceEffectSelector.h
//
#ifndef __DMX_LUMINANCE_EFFECT_SELECTOR_H__
#define __DMX_LUMINANCE_EFFECT_SELECTOR_H__

#include "DmxParams/DmxColorLuminanceParameters.h"
#include "Lumenizers/UniformLuminance.h"
#include "Lumenizers/BreathingLuminance.h"
#include "Lumenizers/AmuletSelfTestLuminance.h"
#include "Lumenizers/MorseCodeLuminance.h"
#include "Lumenizers/TwinklingLuminance.h"
#include "DmxColorEffectSelector.h"

template <bool _refCounted>
struct DmxLuminanceEffectSelector : ReferenceCounted
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
        SelfTest,   // excluded from Lightboard availability
    };

    static LPCSTR nameOf(Effect effect)
    {
        switch (effect)
        {
            case Effect::None:      return "None";
            case Effect::Uniform:   return "Uniform";
            case Effect::Breathing: return "Breathing";
            case Effect::Twinkle:   return "Twinkle";
            case Effect::SelfTest:  return "SelfTest";
            default:                return "<unknown>";
        }
    }

protected:

    Effect          _currentEffect;
    Lumenizeable*   _pLumenizeable;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:
    DmxLuminanceEffectSelector(Lumenizeable* pLumenizeable)
        : _pLumenizeable(nullptr)
    {
        setEffect(Effect::None);
        setRef(_pLumenizeable, pLumenizeable, _refCounted);
    }

protected:
    ~DmxLuminanceEffectSelector() override
    {
        releaseRef(_pLumenizeable, _refCounted);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:
    Effect effect()
    {
        return _currentEffect;
    }
    LPCSTR effectName()
    {
        return nameOf(effect());
    }

protected:
    void setEffect(Effect effect)
    {
        _currentEffect = effect;
    }

    //----------------------------------------------------------------------------------------------
    // Operations
    //----------------------------------------------------------------------------------------------
public:

    static Effect luminanceEffect(const DmxEffectSpeedControl& luminance)
    {
        return scaleRangeDiscrete(luminance.effect(), 0, 255, Effect::First, Effect::Last);
    }

    void processDmxColorLuminance(const DmxColorLuminanceParameters& parameterBlock)
    {
        // Use a self test if the COLOR it set to self test
        Effect effectDesired = DmxColorEffectSelector<true>::Effect::SelfTest == DmxColorEffectSelector<true>::colorEffect(parameterBlock.color())
            ? Effect::SelfTest
            : luminanceEffect(parameterBlock.luminance());

        processEffectDesired(effectDesired);
    }

    void processDmxEffectSpeedControl(const DmxEffectSpeedControl& luminance)
    {
        processEffectDesired(luminanceEffect(luminance));
    }

    void processEffectDesired(const Effect effectDesired)
    {
        if (_currentEffect != effectDesired)
        {
            setEffect(effectDesired);
            //
            Lumenizer* pLumenizer = nullptr;
            switch (effectDesired)
            {
                case Effect::Uniform:
                    pLumenizer = new UniformLuminance(1.0f);
                    break;

                case Effect::Breathing:
                    pLumenizer = new BreathingLuminance(0, 4000);
                    break;

                case Effect::Twinkle:
                    pLumenizer = new TwinklingLuminance(4000, 1000);
                    break;

                case Effect::SelfTest:
                    pLumenizer = new AmuletSelfTestLuminance(2,3);
                    break;
            }
            if (pLumenizer)
            {
                if (!pLumenizer->sameAs(_pLumenizeable->lumenizer()))
                {
                    INFO("switching to luminance effect %s", effectName());
                    _pLumenizeable->ownLumenizer(pLumenizer);
                }
                else
                {
                    ::releaseRef(pLumenizer);
                }
            }
        }
    }

};

#endif

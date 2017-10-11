//
// DmxLuminanceEffectSelector.h
//
#ifndef __DMX_BRIGHTNESS_EFFECT_SELECTOR_H__
#define __DMX_BRIGHTNESS_EFFECT_SELECTOR_H__

#include "DmxColorLuminanceParameters.h"
#include "DmxEffectSelector.h"
#include "DmxColorEffectSelector.h"
#include "Lumenizers/UniformLuminance.h"
#include "Lumenizers/BreathingLuminance.h"
#include "Lumenizers/AmuletSelfTestLuminance.h"
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

    Effect _currentEffect;
    VolatileStringSetting _currentEffectName;
    CloudVariable<decltype(_currentEffectName), String> _currentEffectCloud;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    DmxLuminanceEffectSelector(Colorizeable* pColorizeable)
        : DmxEffectSelector(pColorizeable),
          _currentEffectCloud("lumEffect", &_currentEffectName, ReadWriteable::RO)
    {
        setEffect(Effect::None);
    }

protected:

    ~DmxLuminanceEffectSelector() override
    {
    }

    void setEffect(Effect effect)
    {
        _currentEffect = effect;
        _currentEffectName.setValue(nameOf(effect));
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    void begin()
    {
        _currentEffectCloud.begin();
    }

    //----------------------------------------------------------------------------------------------
    // Operations
    //----------------------------------------------------------------------------------------------
public:

    static Effect luminanceEffect(DmxColorLuminanceParameters& parameterBlock)
    {
        return scaleRangeDiscrete(parameterBlock.luminanceEffect(), 0, 255, Effect::First, Effect::Last);
    }

    void processParameterBlock(DmxColorLuminanceParameters& parameterBlock)
    {
        // Use a self test if the COLOR it set to self test
        Effect effectDesired = DmxColorEffectSelector::Effect::SelfTest == DmxColorEffectSelector::colorEffect(parameterBlock)
            ? Effect::SelfTest
            : luminanceEffect(parameterBlock);

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
                if (!pLumenizer->sameAs(_pColorizeable->lumenizer()))
                {
                    INFO("switching to luminance effect %s", _currentEffectName.value().c_str());
                    _pColorizeable->ownLumenizer(pLumenizer);
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

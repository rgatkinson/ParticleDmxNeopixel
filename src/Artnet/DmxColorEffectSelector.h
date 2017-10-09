//
// DmxColorEffectSelector.h
//
#ifndef __DMX_COLOR_EFFECT_SELECTOR_H__
#define __DMX_COLOR_EFFECT_SELECTOR_H__

#include "ColorLuminanceParameterBlock.h"
#include "DmxEffectSelector.h"
#include "System/PersistentSettings.h"
#include "System/CloudVariable.h"

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
        Last=SelfTest,
    };

    static LPCSTR nameOf(Effect effect)
    {
        switch (effect)
        {
            case Effect::None:      return "None";
            case Effect::Uniform:   return "Uniform";
            case Effect::Rainbow:   return "Rainbow";
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

    DmxColorEffectSelector(Colorizeable* pColorizeable)
        : DmxEffectSelector(pColorizeable),
          _currentEffectCloud("colorEffect", &_currentEffectName, false)
    {
        setEffect(Effect::None);
    }

protected:

    ~DmxColorEffectSelector() override
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

    static Effect colorEffect(ColorLuminanceParameterBlock& parameterBlock)
    {
        return scaleRangeDiscrete(parameterBlock.colorEffect(), 0, 255, Effect::First, Effect::Last);
    }

    void processParameterBlock(ColorLuminanceParameterBlock& parameterBlock)
    {
        Effect effectDesired = colorEffect(parameterBlock);
        if (_currentEffect != effectDesired)
        {
            setEffect(effectDesired);
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
                    pColorizer = new AmuletSelfTestColorizer(Color::WHITE);
                    break;
            }
            if (pColorizer)
            {
                if (!pColorizer->sameAs(_pColorizeable->colorizer()))
                {
                    INFO("switching to color effect %s", _currentEffectName.value().c_str());
                    _pColorizeable->ownColorizer(pColorizer);
                }
                else
                {
                    ::releaseRef(pColorizer);
                }
            }
        }
    }

};

#endif

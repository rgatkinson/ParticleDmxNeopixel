//
// DmxColorEffectSelector.h
//
#ifndef __DMX_COLOR_EFFECT_SELECTOR_H__
#define __DMX_COLOR_EFFECT_SELECTOR_H__

#include "System/PersistentSettings.h"
#include "System/PersistentStringSetting.h"
#include "System/CloudVariable.h"
#include "DmxParams/DmxColorLuminanceParameters.h"
#include "Colorizers/RainbowColors.h"
#include "Colorizers/AmuletSelfTestColorizer.h"

template<bool _refCounted>
struct DmxColorEffectSelector : ReferenceCounted
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
        Last=Rainbow,
        SelfTest,   // excluded from Lightboard availability
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

    Effect          _currentEffect;
    Colorizeable*   _pColorizeable;
    int             _pixelCount;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    DmxColorEffectSelector(Colorizeable* pColorizeable)
        : _pColorizeable(nullptr)
    {
        setRef(_pColorizeable, pColorizeable, _refCounted);
        _pixelCount = pColorizeable ? 0 : pColorizeable->numberOfPixels();
        setEffect(Effect::None);
    }
    ~DmxColorEffectSelector()
    {
        releaseRef(_pColorizeable, _refCounted);
    }

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

    static Effect colorEffect(const DmxEffectSpeedControl& color)
    {
        return scaleRangeDiscrete(color.effect(), 0, 255, Effect::First, Effect::Last);
    }

    void processDmxColorLuminance(const DmxColorLuminanceParameters& parameterBlock)
    {
        processDmxEffectSpeedControl(parameterBlock.color());
    }

    void processDmxEffectSpeedControl(const DmxEffectSpeedControl& color)
    {
        Effect effectDesired = colorEffect(color);
        if (_currentEffect != effectDesired)
        {
            setEffect(effectDesired);
            //
            Colorizer* pColorizer = nullptr;
            switch (effectDesired)
            {
                case Effect::Uniform:
                    pColorizer = new UniformColor(Color::White);
                    break;

                case Effect::Rainbow:
                    pColorizer = new RainbowColors(10);
                    break;

                case Effect::SelfTest:
                    pColorizer = new AmuletSelfTestColorizer();
                    break;
            }
            if (pColorizer)
            {
                if (!pColorizer->sameAs(_pColorizeable->colorizer()))
                {
                    INFO("switching to color effect %s", effectName());
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

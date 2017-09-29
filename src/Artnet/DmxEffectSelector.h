//
// DmxEffectSelector.h
//
#ifndef __DMX_EFFECT_SELECTOR_H__
#define __DMX_EFFECT_SELECTOR_H__

#include "Util/Color.h"
#include "Lumenizers/BreathingLuminance.h"
#include "Lumenizers/LumenizerSequence.h"
#include "Lumenizers/TwinklingLuminance.h"
#include "Lumenizers/SelfTestLuminance.h"
#include "Lumenizers/MorseCodeLuminance.h"
#include "Colorizers/ColorizerSequence.h"
#include "Colorizers/SelfTestColorizer.h"
#include "Pixels/Colorizeable.h"

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
    // Demo management
    //----------------------------------------------------------------------------------------------

public:
    enum class Demo
    {
        None,
        First,
        SelfTest=First,
        Rainbow,
        White,
        Twinkle,
        Morse,
        Max,
        Default=SelfTest
    };

    static void setDemo(Colorizeable* pPixels, Demo demo, COLOR_INT indicatorColor)
    {
        Colorizer* pColorizer;
        pPixels->setColorizer(pColorizer = DmxEffectSelector::demoColorizer(demo, indicatorColor));
        pPixels->setLumenizer(DmxEffectSelector::demoLumenizer(demo, pColorizer));
    }

    static Colorizer* demoColorizer(Demo demo, COLOR_INT indicatorColor)
    {
        switch (demo)
        {
            case Demo::Twinkle:
            case Demo::White:
            {
                return new UniformColor(Color::temperature(2550), Deadline::Infinite);
            }
            case Demo::Rainbow:
            {
                return new RainbowColors(RainbowColors::msIntervalDefault, Deadline::Infinite);
            }
            case Demo::Morse:
            {
                return new UniformColor(Color::YELLOW, Deadline::Infinite);
            }
            case Demo::SelfTest:
            default:
            {
                return new SelfTestColorizer(indicatorColor);
            }
        }
    }

    static Lumenizer* demoLumenizer(Demo demo, Colorizer* pColorizer)
    {
        switch (demo)
        {
            case Demo::White:
            {
                LumenizerSequence* pSequence = new LumenizerSequence();
                pSequence->addLumenizer(new UniformLuminance(1.0f, 5000));
                pSequence->addLumenizer(new BreathingLuminance(BreathingLuminance::msPauseDefault, BreathingLuminance::msBreatheDefault, Deadline::Infinite));
                return pSequence;
            }
            case Demo::Twinkle:
            {
                return new TwinklingLuminance(
                    TwinklingLuminance::msTwinklePauseDefault,
                    TwinklingLuminance::msTwinkleBreatheDefault,
                    Deadline::Infinite);

            }
            case Demo::Morse:
            {
                return new MorseCodeLuminance(Deadline::Infinite);
            }
            case Demo::Rainbow:
            {
                return new UniformLuminance(1.0f, Deadline::Infinite);
            }
            case Demo::SelfTest:
            default:
            {
                return new SelfTestLuminance(pColorizer);
            }
        }
    }


};

#endif

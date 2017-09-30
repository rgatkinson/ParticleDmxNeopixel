//
// DmxEffectSelector.h
//
#ifndef __DMX_EFFECT_SELECTOR_H__
#define __DMX_EFFECT_SELECTOR_H__

#include "Util/Color.h"
#include "Lumenizers/BreathingLuminance.h"
#include "Lumenizers/LumenizerSequence.h"
#include "Lumenizers/TwinklingLuminance.h"
#include "Lumenizers/AmuletSelfTestLuminance.h"
#include "Lumenizers/MorseCodeLuminance.h"
#include "Colorizers/ColorizerSequence.h"
#include "Colorizers/AmuletSelfTestColorizer.h"
#include "Pixels/Colorizeable.h"

struct DmxEffectSelector : ReferenceCounted, protected ColorizeableHolder
{
    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
protected:

    DmxEffectSelector(Colorizeable* pColorizeable) : ColorizeableHolder()
    {
        ColorizeableHolder::setColorizeable(pColorizeable);
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
        Morse,
        Twinkle,
        Max,
        Default=SelfTest
    };
    static Demo cycle(Demo demo)
    {
        return ::cycle(demo, Demo::First, Demo::Max);
    }

    static void setDemo(Colorizeable* pPixels, Demo demo, Colorizer* pSelfTestColor, Lumenizer* pSelfTestLuminance) // nb: _usual_ ref counting rules
    {
        pPixels->ownColorizer(DmxEffectSelector::demoColorizer(demo, pSelfTestColor));
        pPixels->ownLumenizer(DmxEffectSelector::demoLumenizer(demo, pSelfTestLuminance));
    }

    static Colorizer* demoColorizer(Demo demo, Colorizer* pSelfTest)
    {
        Colorizer* pResult = nullptr;
        switch (demo)
        {
            case Demo::Twinkle:
            case Demo::White:
            {
                pResult = new UniformColor(Color::temperature(2550), Deadline::Infinite);
                break;
            }
            case Demo::Rainbow:
            {
                pResult = new RainbowColors(RainbowColors::msIntervalDefault, Deadline::Infinite);
                break;
            }
            case Demo::Morse:
            {
                pResult = new UniformColor(Color::YELLOW, Deadline::Infinite);
                break;
            }
            case Demo::SelfTest:
            default:
            {
                pResult = pSelfTest;
                pSelfTest->addRef();
                break;
            }
        }
        return pResult;
    }

    static Lumenizer* demoLumenizer(Demo demo, Lumenizer* pSelfTest)
    {
        Lumenizer* pResult = nullptr;
        switch (demo)
        {
            case Demo::White:
            {
                LumenizerSequence* pSequence = new LumenizerSequence();
                pSequence->ownLumenizer(new UniformLuminance(1.0f, 5000));
                pSequence->ownLumenizer(new BreathingLuminance(BreathingLuminance::msPauseDefault, BreathingLuminance::msBreatheDefault, Deadline::Infinite));
                pResult = pSequence;
                break;
            }
            case Demo::Twinkle:
            {
                pResult = new TwinklingLuminance(
                    TwinklingLuminance::msTwinklePauseDefault,
                    TwinklingLuminance::msTwinkleBreatheDefault,
                    Deadline::Infinite);
                break;
            }
            case Demo::Morse:
            {
                pResult = new MorseCodeLuminance(Deadline::Infinite);
                break;
            }
            case Demo::Rainbow:
            {
                pResult = new UniformLuminance(1.0f, Deadline::Infinite);
                break;
            }
            case Demo::SelfTest:
            default:
            {
                pResult = pSelfTest;
                pSelfTest->addRef();
                break;
            }
        }
        return pResult;
    }


};

#endif

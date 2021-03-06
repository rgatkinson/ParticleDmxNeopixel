//
// DmxDemoManager.h
//
#ifndef __DMX_DEMO_MANAGER_H__
#define __DMX_DEMO_MANAGER_H__

#include "Util/Color.h"
#include "Lumenizers/BreathingLuminance.h"
#include "Lumenizers/LumenizerSequence.h"
#include "Lumenizers/TwinklingLuminance.h"
#include "Lumenizers/AmuletSelfTestLuminance.h"
#include "Lumenizers/MorseCodeLuminance.h"
#include "Colorizers/ColorizerSequence.h"
#include "Colorizers/RainbowColors.h"
#include "Colorizers/AmuletSelfTestColorizer.h"
#include "Pixels/Colorizeable.h"

struct DmxDemoManager
{
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
        pPixels->ownColorizer(DmxDemoManager::demoColorizer(demo, pSelfTestColor));
        pPixels->ownLumenizer(DmxDemoManager::demoLumenizer(demo, pSelfTestLuminance));
    }

    static Colorizer* demoColorizer(Demo demo, Colorizer* pSelfTest)
    {
        Colorizer* pResult = nullptr;
        switch (demo)
        {
            case Demo::Twinkle:
            case Demo::White:
            {
                pResult = new UniformColor(Color::temperature(2550));
                break;
            }
            case Demo::Rainbow:
            {
                pResult = new RainbowColors(RainbowColors::msIntervalDefault);
                break;
            }
            case Demo::Morse:
            {
                pResult = new UniformColor(Color::Yellow);
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
                pSequence->ownLumenizer(new UniformLuminance(Duration(5000), 1.0f));
                pSequence->ownLumenizer(new BreathingLuminance(BreathingLuminance::msPauseDefault, BreathingLuminance::msBreatheDefault));
                pResult = pSequence;
                break;
            }
            case Demo::Twinkle:
            {
                pResult = new TwinklingLuminance(
                    TwinklingLuminance::msTwinklePauseDefault,
                    TwinklingLuminance::msTwinkleBreatheDefault);
                break;
            }
            case Demo::Morse:
            {
                pResult = new MorseCodeLuminance();
                break;
            }
            case Demo::Rainbow:
            {
                pResult = new UniformLuminance(1.0f);
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

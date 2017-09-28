//
// TwinklingLuminance.h
//
#ifndef __TWINKLE_LUMENIZER_H__
#define __TWINKLE_LUMENIZER_H__

#include "Util/Deadline.h"
#include "Twinkler.h"

//==================================================================================================
// TwinklingLuminance
//==================================================================================================

struct TwinklingLuminance: Lumenizer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
public:
    static const int msTwinklePauseDefault = 60000;
    static const int msTwinkleBreatheDefault = 6000;

protected:
    int _msPause;
    int _msBreathe;
    ArrayList<Twinkler> _twinklers;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    TwinklingLuminance(float msPause, int msBreathe, int msDuration) : Lumenizer(Flavor::Twinkle, msDuration)
    {
        _msPause = -1;
        _msBreathe = msBreathe;

        setTwinklePause(msPause);
    }

    virtual void setColorizeable(Colorizeable* pColorizeable)
    {
        Lumenizer::setColorizeable(pColorizeable);
        _twinklers = ArrayList<Twinkler>();
        for (int i = 0; i < _pixelCount; i++)
        {
            _twinklers.addLast(Twinkler(_msPause, _msBreathe));
        }
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    bool hasPixelizedBrightness() override
    {
        return true;
    }

    bool usesGammaCorrection() override
    {
        return false;
    }

    BRIGHTNESS currentBrightness(int iPixel) override
    {
        return rawCurrentBrightness(_twinklers[iPixel].currentLevel());
    }

    void setTwinklePause(int msPause)
    {
        if (_msPause != msPause)
        {
            INFO("setting twinkle pause to %d", msPause);
            _msPause = msPause;
            for (int i = 0; i < _twinklers.count(); i++)
            {
                _twinklers[i].setPauseInterval(msPause);
                _twinklers[i].begin();  // make interval take effect right away
            }
        }
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    void processParameterBlock(DmxParameterBlock& parameterBlock) override
    {
        Lumenizer::processParameterBlock(parameterBlock);

        // 0 is default; otherwise scale from 0 to twice default (so default is also in middle)
        int msTwinklePause = msTwinklePauseDefault;
        if (parameterBlock.luminanceSpeed()!=0)
        {
            float speed = parameterBlock.luminanceSpeedLevel(false);
            msTwinklePause = scaleRange(speed, 0, 1, 0, 2 * msTwinklePauseDefault);
        }

        setTwinklePause(msTwinklePause);
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin() override
    {
        Lumenizer::begin();
        for (int i = 0; i < _twinklers.count(); i++)
        {
            _twinklers[i].begin();
        }
    }

    void loop() override
    {
        Lumenizer::loop();
        for (int i = 0; i < _twinklers.count(); i++)
        {
            _twinklers[i].loop();
        }
    }

    void report() override
    {
        Lumenizer::report();
    }

};

#endif

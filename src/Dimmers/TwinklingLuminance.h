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

    int _msPause;
    int _msBreathe;
    ArrayList<Twinkler> _twinklers;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    TwinklingLuminance(float msPause, int msBreathe, int msDuration) : Lumenizer(Flavor::Twinkle, msDuration)
    {
        _msPause = msPause;
        _msBreathe = msBreathe;
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
        return true;
    }

    BRIGHTNESS currentBrightness(int iPixel) override
    {
        return rawCurrentBrightness(_twinklers[iPixel].currentLevel());
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    void processParameterBlock(DmxParameterBlock& parameterBlock) override
    {
        Lumenizer::processParameterBlock(parameterBlock);

        float twinkleRate = fabs(parameterBlock.luminanceSpeedLevel());
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

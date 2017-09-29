//
// SelfTestLuminance.h
//
#ifndef __SELF_TEST_LUMENIZER_H__
#define __SELF_TEST_LUMENIZER_H__

struct SelfTestLuminance : LumenizerSequence
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    float _initialBrightnessLevel = 1.0f;
    float _idleBrightnessLevel = 0.25f;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    SelfTestLuminance(Colorizer* pColorizer) : LumenizerSequence(Flavor::SelfTest)
    {
        addLumenizer(new UniformLuminance(_initialBrightnessLevel, pColorizer->msLoopingDuration()));
        addLumenizer(new UniformLuminance(_idleBrightnessLevel, Deadline::Infinite));
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    void processParameterBlock(DmxParameterBlock& parameterBlock) override
    {
        Lumenizer::processParameterBlock(parameterBlock);
        // Nothing special to do
    }

    //----------------------------------------------------------------------------------------------
    // Looping
    //----------------------------------------------------------------------------------------------

    void begin() override
    {
        LumenizerSequence::begin();
    }

    void report() override
    {
        LumenizerSequence::report();
        INFO("SelfTestLuminance: first=%d ms", _lumenizers[0]->msDuration());
    }

};

#endif

//
// SelfTestLumenizer.h
//
#ifndef __SELF_TEST_LUMENIZER_H__
#define __SELF_TEST_LUMENIZER_H__

struct SelfTestLumenizer : LumenizerSequence
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    float _initialBrightnessLevel = 1.0f;
    float _idleBrightnessLevel = 0.3f;
    UniformLuminance* _pUniformInitial;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    SelfTestLumenizer() : LumenizerSequence(Flavor::SelfTest)
    {
        addLumenizer(_pUniformInitial = new UniformLuminance(_initialBrightnessLevel, Deadline::Infinite /* adjusted later */));
        addLumenizer(new UniformLuminance(_idleBrightnessLevel, Deadline::Infinite));
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    void processParameterBlock(DmxParameterBlock& parameterBlock) override
    {
        Lumenizer::processParameterBlock(parameterBlock);
    }

    //----------------------------------------------------------------------------------------------
    // Looping
    //----------------------------------------------------------------------------------------------

    void begin() override
    {
        if (_pColorizeable)
        {
            int ms = _pColorizeable->colorizer()->msLoopingDuration();
            _pUniformInitial->setDuration(ms);
        }
        LumenizerSequence::begin();
    }

};

#endif

//
// SelfTestBrightness.h
//
#ifndef __SELF_TEST_BRIGHTNESS_H__
#define __SELF_TEST_BRIGHTNESS_H__

struct SelfTestBrightness : BrightnessSequence
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    float _brightnessLevel = 0.3f;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    SelfTestBrightness(int msQuantum) : BrightnessSequence(Flavor::SelfTest)
    {
        addDimmer(new UniformBrightness(1.0f, msQuantum));
        addDimmer(new UniformBrightness(_brightnessLevel, Deadline::Infinite));
    }

    //----------------------------------------------------------------------------------------------
    // Looping
    //----------------------------------------------------------------------------------------------

    void processParameterBlock(DmxParameterBlock& parameterBlock) override
    {
        Dimmer::processParameterBlock(parameterBlock);
    }

};

#endif

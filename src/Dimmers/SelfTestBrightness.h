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

    float _initialBrightnessLevel = 1.0f;
    float _idleBrightnessLevel = 0.3f;
    UniformBrightness* _pUniformInitial;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    SelfTestBrightness() : BrightnessSequence(Flavor::SelfTest)
    {
        addDimmer(_pUniformInitial = new UniformBrightness(_initialBrightnessLevel, Deadline::Infinite /* adjusted later */));
        addDimmer(new UniformBrightness(_idleBrightnessLevel, Deadline::Infinite));
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    void processParameterBlock(DmxParameterBlock& parameterBlock) override
    {
        Dimmer::processParameterBlock(parameterBlock);
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
        BrightnessSequence::begin();
    }

};

#endif

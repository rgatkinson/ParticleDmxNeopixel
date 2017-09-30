//
// AmuletSelfTestColorizer.h
//
#ifndef __AMULET_ELF_TEST_COLORIZER_H__
#define __AMULET_ELF_TEST_COLORIZER_H__

struct AmuletSelfTestColorizer : ColorizerSequence
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    int _msQuantum = 3000;
    UniformColor* _pUniform = nullptr;
    RainbowColors* _pRainbow = nullptr;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    AmuletSelfTestColorizer(COLOR_INT color) : ColorizerSequence(Flavor::SelfTest)
    {
        ownColorizer(new RainbowColors(10, _msQuantum * 2));
        ownColorizer(_pUniform = new UniformColor(color, _msQuantum * 2));
        ownColorizer(new UniformColor(Color::BLACK, _msQuantum * 3));
        setLooping(true);
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    void processParameterBlock(ColorLuminanceParameterBlock& parameterBlock) override
    {
        Colorizer::processParameterBlock(parameterBlock);
        _pUniform->processParameterBlock(parameterBlock);
        _pRainbow->processParameterBlock(parameterBlock);
    }

};

#endif

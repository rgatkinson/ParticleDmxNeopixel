//
// SelfTestColorizer.h
//
#ifndef __SELF_TEST_COLORIZER_H__
#define __SELF_TEST_COLORIZER_H__

struct SelfTestColorizer : ColorizerSequence
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    int _msQuantum = 1500;
    UniformColor* _pUniformColor = nullptr;
    RainbowColors* _pRainbow = nullptr;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    SelfTestColorizer(COLOR_INT color) : ColorizerSequence(Flavor::SelfTest)
    {
        _pUniformColor = new UniformColor(color, _msQuantum * 2);
        _pRainbow = new RainbowColors(10, _msQuantum * 2);

        addColorizer(new RainbowColors(10, _msQuantum * 2));
        addColorizer(_pUniformColor);
        addColorizer(_pRainbow);
        addColorizer(new UniformColor(Color::BLACK, _msQuantum * 6));
        setLooping(true);
    }

    //----------------------------------------------------------------------------------------------
    // Looping
    //----------------------------------------------------------------------------------------------

    void processParameterBlock(DmxParameterBlock& parameterBlock) override
    {
        Colorizer::processParameterBlock(parameterBlock);
        _pUniformColor->processParameterBlock(parameterBlock);
        _pRainbow->processParameterBlock(parameterBlock);
    }

};

#endif

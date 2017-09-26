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

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    SelfTestColorizer(COLOR_INT color) : ColorizerSequence(Flavor::SelfTest)
    {
        _pUniformColor = new UniformColor(color, _msQuantum * 2);

        addColorizer(new RainbowColors(10,          _msQuantum * 2));
        addColorizer(_pUniformColor);
        addColorizer(new RainbowColors(10,          _msQuantum * 2));
        addColorizer(new UniformColor(Color::BLACK, _msQuantum * 6));
        setLooping(true);
    }

    //----------------------------------------------------------------------------------------------
    // Looping
    //----------------------------------------------------------------------------------------------

    void processParameterBlock(DmxParameterBlock& parameterBlock) override
    {
        Colorizer::processParameterBlock(parameterBlock);
        COLOR_INT colorDesired = parameterBlock.effectiveColor();
        _pUniformColor->setColor(colorDesired);
    }

};

#endif

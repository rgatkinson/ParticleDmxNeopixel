//
// UniformLuminance.h
//
#ifndef _CONSTANT_LUMENIZER_H_
#define _CONSTANT_LUMENIZER_H_

#include "Lumenizer.h"

struct UniformLuminance : Lumenizer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    float _level;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    UniformLuminance(float level, int msDuration) : Lumenizer(Flavor::Uniform, msDuration)
    {
        _level = level;
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
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    void begin() override
    {
        Lumenizer::begin();
        setCurrentLevel(_level);
    }

    void loop() override
    {
        Lumenizer::loop();
    }

    void report() override
    {
        Lumenizer::report();
    }

};

#endif

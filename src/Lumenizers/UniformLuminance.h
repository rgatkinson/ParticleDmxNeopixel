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

    UniformLuminance(float level) : UniformLuminance(Duration::Infinite, level)
    {
    }

    UniformLuminance(Duration duration, float level) : Lumenizer(Flavor::Uniform, duration)
    {
        _level = level;
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    void processDmxColorLuminance(const DmxColorLuminanceParameters& parameterBlock) override
    {
        Lumenizer::processDmxColorLuminance(parameterBlock);
        // Nothing special to do: dimmer will be applied
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

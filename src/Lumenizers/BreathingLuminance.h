//
// BreathingLumenizer.h
//
#ifndef __BREATHING_LUMENIZER_H__
#define __BREATHING_LUMENIZER_H__

#include "Lumenizer.h"
#include "Breather.h"
#include <math.h>

struct BreathingLuminance : Lumenizer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
public:
    static const int msPauseDefault = 0;
    static const int msBreatheMin = 750;
    static const int msBreatheDefault = 5000;

protected:

    int _msPause;
    int _msBreathe;
    Breather<> _breather;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    BreathingLuminance(int msPauseInterval, int msBreatheInterval) : BreathingLuminance(Duration::Infinite, msPauseInterval, msBreatheInterval)
    {
    }

    BreathingLuminance(Duration duration, int msPauseInterval, int msBreatheInterval) : Lumenizer(Flavor::Breathing, duration)
    {
        _msPause = -1;
        _msBreathe = -1;

        setPauseInterval(msPauseInterval);
        setBreatheInterval(msBreatheInterval);
        setBrightnessMin(20);    // empiricly determined
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
protected:

    bool usesGammaCorrection() override
    {
        return false;
    }

    void setPauseInterval(int msPauseInterval) // mostly for debugging / development
    {
        if (_msPause != msPauseInterval)
        {
            _msPause = msPauseInterval;
            _breather.setPauseInterval(msPauseInterval);
        }
    }

    void setBreatheInterval(int msBreatheInterval)
    {
        if (_msBreathe != msBreatheInterval)
        {
            _msBreathe = msBreatheInterval;
            _breather.setBreatheInterval(msBreatheInterval);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Dmx
    //----------------------------------------------------------------------------------------------
public:

    void processParameterBlock(DmxColorLuminanceParameters& parameterBlock) override
    {
        Lumenizer::processParameterBlock(parameterBlock);

        float breathingRate = fabs(parameterBlock.luminanceSpeedLevel());
        float ms = breathingRate == 0
            ? msBreatheDefault
            : scaleRange(breathingRate, 0, 1, msBreatheMin, 2 * msBreatheDefault);
        setBreatheInterval(int(ms));
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    void begin() override
    {
        Lumenizer::begin();
        _breather.begin();
    }

    void loop() override
    {
        Lumenizer::loop();
        _breather.loop();
        setCurrentLevel(_breather.currentLevel());
    }

    void report() override
    {
        Lumenizer::report();
        INFO("BreathingLuminance: breathe interval=%d", _breather.breatheInterval());
    }

};

#endif

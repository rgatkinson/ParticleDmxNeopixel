//
// Breather.h
//
#ifndef __BREATHER_H__
#define __BREATHER_H__

typedef float (*PFN_WAVE)(float, float);

template <PFN_WAVE pfnWave=triWave>
struct Breather
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    Deadline    _timer;
    int         _msPause;
    int         _msBreathe;
    float       _minLevel;
    float       _currentLevel;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Breather(int msDuration = 0)
    {
        _msPause = 0;
        _msBreathe = 0;
        _minLevel = 0.0;
        _currentLevel = 1;
        _timer = Deadline(msDuration);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    void setMinLevel(float minLevel)
    {
        _minLevel = minLevel;
    }
    float minLevel()
    {
        return _minLevel;
    }

    void setBreatheInterval(int msBreathe)
    {
        _msBreathe = msBreathe;
    }
    int breatheInterval()
    {
        return _msBreathe;
    }

    void setPauseInterval(int msPause)
    {
        _msPause = msPause;
    }
    int pauseInterval()
    {
        return _msPause;
    }

    float currentLevel()
    {
        return _currentLevel;
    }

    void setBreathsDuration(int breaths)
    {
        int msDuration = breaths * msBreathCycle();
        _timer = Deadline(msDuration);
    }

    bool hasExpired()
    {
        return _timer.hasExpired();
    }

    int msBreathCycle()
    {
        return _msPause + _msBreathe;
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin(bool randomize = false)
    {
        _timer.reset();
        if (randomize)
        {
            int ms = random(0, msBreathCycle());
            _timer.consume(ms);
        }
        // INFO("millis=%d", _timer.milliseconds());
    }

    void loop()
    {
        int ms = _timer.milliseconds() % msBreathCycle();   // [0, _msPause + _msBreathe)
        if (ms < _msPause)
        {
            _currentLevel = 1.0f;
        }
        else
        {
            ms -= _msPause; // [0, _msBreathe)

            // linear down, linear up: the eye perceives it differently
            _currentLevel = 1.0f - pfnWave(ms, _msBreathe);

            // don't bottom out
            _currentLevel = _minLevel + (1 - _minLevel) * _currentLevel;
        }
    }

};

#endif

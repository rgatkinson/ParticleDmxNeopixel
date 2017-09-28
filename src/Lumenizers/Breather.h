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
        setDurationAndResetTimer(msDuration);
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
    int breathInterval()
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

    void setDurationAndResetTimer(int msDuration)
    {
        _timer = Deadline(msDuration);
    }

    bool hasExpired()
    {
        return _timer.hasExpired();
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin(bool randomize = false)
    {
        _timer.reset();
        if (randomize)
        {
            int ms = random(0, _timer.msDuration());
            _timer.consume(ms);
        }
    }

    void loop()
    {
        int ms = _timer.milliseconds() % (_msPause + _msBreathe);   // [0, _msPause + _msBreathe)
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

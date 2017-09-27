//
// Breather.h
//
#ifndef __BREATHER_H__
#define __BREATHER_H__

struct Breather
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    Deadline    _timer;
    int         _msPause;
    int         _msBreathe;
    float       _currentLevel;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Breather(int msDuration = 0)
    {
        _msPause = 0;
        _msBreathe = 0;
        _currentLevel = 1;
        setDurationAndReset(msDuration);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

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

    void setDurationAndReset(int msDuration)
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

    void begin()
    {
        _timer.reset();
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
            float intervalFraction = float(ms) / float(_msBreathe-1); // [0.0, 1.0]

            // linear up, linear down: the eye perceives it differently
            float level = (intervalFraction <= 0.5f)
                ? (intervalFraction * 2.0f)
                : 1.0f - ( (intervalFraction - 0.5f) * 2.0f );

            _currentLevel = level;
        }
    }

};

#endif

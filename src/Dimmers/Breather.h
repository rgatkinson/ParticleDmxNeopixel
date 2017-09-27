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
    int         _msBreathe;
    float       _currentLevel;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Breather(int msDuration = 0)
    {
        _msBreathe = 0;
        _currentLevel = 1;
        setDuration(msDuration);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    void setBreatheInterval(int msInterval)
    {
        _msBreathe = msInterval;
    }

    int breathInterval()
    {
        return _msBreathe;
    }

    float currentLevel()
    {
        return _currentLevel;
    }

    void setDuration(int msDuration)
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
        int ms = _timer.milliseconds() % _msBreathe;
        float intervalFraction = float(ms) / float(_msBreathe);

        // linear up, linear down: the eye perceives it differently
        float level = (intervalFraction <= 0.5f)
            ? (intervalFraction * 2.0f)
            : 1.0f - ( (intervalFraction - 0.5f) * 2.0f );

        _currentLevel = level;
    }

};

#endif

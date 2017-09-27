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
    int         _msInterval;
    float       _currentLevel;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Breather(int msDuration = 0)
    {
        _msInterval = 0;
        _currentLevel = 1;
        setDuration(msDuration);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    void setInterval(int msInterval)
    {
        _msInterval = msInterval;
    }

    int msInterval()
    {
        return _msInterval;
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
        int ms = _timer.milliseconds() % _msInterval;
        float intervalFraction = float(ms) / float(_msInterval);

        // linear up, linear down: the eye perceives it differently
        float level = (intervalFraction <= 0.5f)
            ? (intervalFraction * 2.0f)
            : 1.0f - ( (intervalFraction - 0.5f) * 2.0f );

        _currentLevel = level;
    }

};

#endif

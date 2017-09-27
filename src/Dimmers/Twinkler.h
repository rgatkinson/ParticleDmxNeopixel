//
// Twinkler.h
//
#ifndef __TWINKLER_H__
#define __TWINKLER_H__

//==================================================================================================
// Twinkler
//==================================================================================================

struct Twinkler
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
private:

    Breather _breather;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Twinkler()
    {
    }

    Twinkler(int msInterval)
    {
        _breather.setBreatheInterval(msInterval);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    float currentLevel()
    {
        return _breather.currentLevel();
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    void begin()
    {
        resetTimer();
        _breather.begin();
    }

    void loop()
    {
        // Twinkle; see also BreathingBrightness
        _breather.loop();

        // Set for the next time
        if (_breather.hasExpired())
        {
            resetTimer();
        }
    }

private:
    void resetTimer()
    {
        // uniform dist'n centered on msInterval
        int msInterval = _breather.breathInterval();
        int dms   = msInterval / 3;
        int msMin = msInterval - dms;
        int msMax = msInterval + dms;
        int msDuration = random(msMin, msMax);      // half-open interval, fwiw
        _breather.setDuration(msDuration);
    }
};

#endif

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

    Breather<sinWave> _breather;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Twinkler()
    {
    }

    Twinkler(int msPause, int msBreathe)
    {
        setPauseInterval(msPause);
        setBreatheInterval(msBreathe);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    float currentLevel()
    {
        return _breather.currentLevel();
    }

    void setPauseInterval(int msPause)
    {
        _breather.setPauseInterval(msPause);
    }

    void setBreatheInterval(int msBreathe)
    {
        _breather.setBreatheInterval(msBreathe);
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    void begin()
    {
        resetTimer();
        _breather.begin(true); // true helps desychronize twinkling, even initially
    }

    void loop()
    {
        // Twinkle; see also BreathingLuminance
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
        // uniform dist'n centered on msPause
        int msPause = _breather.pauseInterval();
        int dms = msPause / 3;
        msPause = random(msPause - dms, msPause + dms);  // half-open interval, fwiw

        // uniform dist'n centered on msBreathe
        int msBreathe = _breather.breatheInterval();
        dms = msBreathe / 4;
        msBreathe = random(msBreathe - dms, msBreathe + dms);

        // Allow only a small number of breaths before we re-randomize
        int breaths = random(1,4);

        // INFO("msPause=%d msBreathe=%d breaths=%d", msPause, msBreathe, breaths);
        _breather.setPauseInterval(msPause);
        _breather.setBreatheInterval(msBreathe);
        _breather.setBreathsDuration(breaths);
    }
};

#endif

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
    int _msPause;
    int _msBreathe;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Twinkler()
    {
    }

    Twinkler(int msPause, int msBreathe)
    {
        _msPause = msPause;
        _msBreathe = msBreathe;
        _breather.setPauseInterval(msPause);
        _breather.setBreatheInterval(msBreathe);
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
        int dms = _msPause / 3;
        int msPause = random(_msPause - dms, _msPause + dms);  // half-open interval, fwiw

        // uniform dist'n centered on msBreathe
        dms = _msBreathe / 4;
        int msBreathe = random(_msBreathe - dms, _msBreathe + dms);

        // Allow only a small number of breaths before we re-randomize
        int breaths = random(1,4);

        INFO("msPause=%d msBreathe=%d breaths=%d", msPause, msBreathe, breaths);

        _breather.setPauseInterval(msPause);
        _breather.setBreatheInterval(msBreathe);
        _breather.setDurationAndResetTimer(breaths * (msPause + msBreathe));
    }
};

#endif

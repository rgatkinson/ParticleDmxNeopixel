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

    Twinkler(int msPause, int msBreathe)
    {
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
        // uniform dist'n centered on msPause
        int msPause = _breather.pauseInterval();
        int dms = msPause / 3;
        msPause = random(msPause - dms, msPause + dms);  // half-open interval, fwiw
        _breather.setPauseInterval(msPause);

        // uniform dist'n centered on msBreathe
        int msBreathe = _breather.breathInterval();
        dms = msBreathe / 4;
        msBreathe = random(msBreathe - dms, msBreathe + dms);
        _breather.setBreatheInterval(msBreathe);

        // Allow only a small number of breaths before we re-randomize
        _breather.setDurationAndReset(3 * (msPause + msBreathe));
    }
};

#endif

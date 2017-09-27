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
        int dms = _msPause / 3;
        int msPause = random(_msPause - dms, _msPause + dms);  // half-open interval, fwiw
        _breather.setPauseInterval(msPause);

        // uniform dist'n centered on msBreathe
        dms = _msBreathe / 4;
        int msBreathe = random(_msBreathe - dms, _msBreathe + dms);
        _breather.setBreatheInterval(msBreathe);

        // Allow only a small number of breaths before we re-randomize
        _breather.setDurationAndReset(3 * (msPause + msBreathe));
    }
};

#endif

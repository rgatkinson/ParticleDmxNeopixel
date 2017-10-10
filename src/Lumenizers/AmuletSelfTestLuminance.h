//
// AmuletSelfTestLuminance.h
//
#ifndef __AMULET_SELF_TEST_LUMENIZER_H__
#define __AMULET_SELF_TEST_LUMENIZER_H__

struct AmuletSelfTestLuminance : LumenizerSequence, SystemEventNotifications
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    static constexpr float _initialDimmerLevel = 0.05f;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    AmuletSelfTestLuminance(Colorizer* pColorizer) : LumenizerSequence(Flavor::SelfTest)
    {
        setDimmerLevel(_initialDimmerLevel);   // this will be overridden if we're if Artnet is up
        steady();
        SystemEventRegistrar::theInstance->registerSystemEvents(this);
    }
    ~AmuletSelfTestLuminance() override
    {
        SystemEventRegistrar::theInstance->unregisterSystemEvents(this);
    }

    //----------------------------------------------------------------------------------------------
    // System
    //----------------------------------------------------------------------------------------------
protected:

    void setSelfTestLumenizer(Lumenizer* pLumenizer)
    {
        releaseLumenizers();
        setLooping(false);
        LumenizerSequence::ownLumenizer(pLumenizer);
    }

    void blinkFast()
    {
        setSelfTestLumenizer(new BreathingLuminance(0, 2000, Deadline::Infinite));
    }
    void breatheSlow()
    {
        setSelfTestLumenizer(new BreathingLuminance(2000, 6000, 16000));
        LumenizerSequence::ownLumenizer(new UniformLuminance(0.0f, 2 * 16000));
        setLooping(true);
    }
    void steady()
    {
        setSelfTestLumenizer(new UniformLuminance(1.0f, 2000));
        LumenizerSequence::ownLumenizer(new UniformLuminance(0.0f, 8000));
        setLooping(true);
    }

    void onNetworkStatus(int netStatus) override
    {
        switch (netStatus)
        {
            case network_status_powering_off:
            case network_status_off:            blinkFast(); break;
            case network_status_powering_on:    breatheSlow(); break;
            case network_status_on:             breatheSlow(); break;
            case network_status_connecting:     blinkFast(); break;
            case network_status_connected:      breatheSlow(); break;
            case network_status_disconnecting:  blinkFast(); break;
            case network_status_disconnected:   breatheSlow(); break;
        }
    }
    void onCloudStatus(int cloudStatus) override
    {
        switch (cloudStatus)
        {
            case cloud_status_connecting:    blinkFast(); break;
            case cloud_status_connected:     breatheSlow(); break;
            case cloud_status_disconnecting: blinkFast(); break;
            case cloud_status_disconnected:  breatheSlow(); break;
        }
    }
    void onTimeChanged(int timeStatus) override
    {
    }
};

#endif

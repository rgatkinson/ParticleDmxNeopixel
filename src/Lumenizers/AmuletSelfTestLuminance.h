//
// AmuletSelfTestLuminance.h
//
#ifndef __AMULET_SELF_TEST_LUMENIZER_H__
#define __AMULET_SELF_TEST_LUMENIZER_H__

#include "Lumenizers/DecimatingLuminance.h"
#include "Lumenizers/UniformLuminance.h"
#include "Lumenizers/LumenizerSequence.h"
#include "System/SystemEventRegistrar.h"

struct AmuletSelfTestLuminance : DelegatingLumenizer, SystemEventNotifications
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    static constexpr float _initialDimmerLevel = 0.05f;

    DecimatingLuminance* _pDecimating;
    LumenizerSequence*   _pSequence;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    AmuletSelfTestLuminance(int numerator, int denominator) : DelegatingLumenizer(Flavor::SelfTest, nullptr)
    {
        _pDecimating = new DecimatingLuminance(numerator, denominator, nullptr);
        _pSequence   = new LumenizerSequence();

        this->setLumenizer(_pDecimating);
        _pDecimating->setLumenizer(_pSequence);

        setDimmerLevel(_initialDimmerLevel);   // this will be overridden if we're if Artnet is up
        steady();
        SystemEventRegistrar::theInstance->registerSystemEvents(this);
    }
    ~AmuletSelfTestLuminance() override
    {
        SystemEventRegistrar::theInstance->unregisterSystemEvents(this);
        releaseRef(_pSequence);
        releaseRef(_pDecimating);
    }

    //----------------------------------------------------------------------------------------------
    // System
    //----------------------------------------------------------------------------------------------
protected:

    void setSelfTestLumenizer(Lumenizer* pLumenizer)
    {
        _pSequence->releaseLumenizers();
        _pSequence->setLooping(false);
        _pSequence->ownLumenizer(pLumenizer);
    }

    void blinkFast()
    {
        setSelfTestLumenizer(new BreathingLuminance(0, 2000));
    }
    void breatheSlow()
    {
        setSelfTestLumenizer(new BreathingLuminance(Duration(16000), 2000, 6000));
        _pSequence->ownLumenizer(new UniformLuminance(Duration(2 * 16000), 0.0f));
        _pSequence->setLooping(true);
    }
    void steady()
    {
        setSelfTestLumenizer(new UniformLuminance(Duration(2000), 1.0f));
        _pSequence->ownLumenizer(new UniformLuminance(Duration(8000), 0.0f));
        _pSequence->setLooping(true);
    }

    void onNetworkStatus(NetworkStatus netStatus) override
    {
        switch (netStatus)
        {
            case NetworkStatus::PoweringOff:
            case NetworkStatus::Off:            blinkFast(); break;
            case NetworkStatus::PoweringOn:     breatheSlow(); break;
            case NetworkStatus::On:             breatheSlow(); break;
            case NetworkStatus::Connecting:     blinkFast(); break;
            case NetworkStatus::Connected:      breatheSlow(); break;
            case NetworkStatus::Disconnecting:  blinkFast(); break;
            case NetworkStatus::Disconnected:   breatheSlow(); break;
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

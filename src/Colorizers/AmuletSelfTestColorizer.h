//
// AmuletSelfTestColorizer.h
//
#ifndef __AMULET_ELF_TEST_COLORIZER_H__
#define __AMULET_ELF_TEST_COLORIZER_H__

#include "ColorizerSequence.h"
#include "System/SystemEventRegistrar.h"

struct AmuletSelfTestColorizer : ColorizerSequence, SystemEventNotifications
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    UniformColor* _pUniform = nullptr;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    AmuletSelfTestColorizer() : ColorizerSequence(Flavor::SelfTest)
    {
        ColorizerSequence::ownColorizer(_pUniform = new UniformColor(Color::Black));
        SystemEventRegistrar::theInstance->registerSystemEvents(this);
    }
    ~AmuletSelfTestColorizer() override
    {
        SystemEventRegistrar::theInstance->unregisterSystemEvents(this);
    }

    //----------------------------------------------------------------------------------------------
    // System
    //----------------------------------------------------------------------------------------------

    void setColor(Color color)
    {
        _pUniform->setColor(color);
    }

    void onNetworkStatus(NetworkStatus netStatus) override
    {
        switch (netStatus)
        {
            case NetworkStatus::PoweringOff:
            case NetworkStatus::Off:            setColor(Color::Red); break;
            case NetworkStatus::PoweringOn:
            case NetworkStatus::On:             setColor(Color::Yellow); break;
            case NetworkStatus::Connecting:
            case NetworkStatus::Connected:      setColor(Color::Green); break;
            case NetworkStatus::Disconnecting:
            case NetworkStatus::Disconnected:   setColor(Color::Magenta); break;
        }
    }
    void onCloudStatus(int cloudStatus) override
    {
        switch (cloudStatus)
        {
            case cloud_status_connecting:
            case cloud_status_connected:     setColor(Color::Cyan); break;
            case cloud_status_disconnecting:
            case cloud_status_disconnected:  setColor(Color::Blue); break;
        }
    }
    void onTimeChanged(int timeStatus) override
    {
    }
};

#endif

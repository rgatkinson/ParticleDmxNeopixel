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

    void onNetworkStatus(int netStatus) override
    {
        switch (netStatus)
        {
            case network_status_powering_off:
            case network_status_off:            setColor(Color::Red); break;
            case network_status_powering_on:
            case network_status_on:             setColor(Color::Yellow); break;
            case network_status_connecting:
            case network_status_connected:      setColor(Color::Green); break;
            case network_status_disconnecting:
            case network_status_disconnected:   setColor(Color::Magenta); break;
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

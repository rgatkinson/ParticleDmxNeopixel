//
// AmuletSelfTestColorizer.h
//
#ifndef __AMULET_ELF_TEST_COLORIZER_H__
#define __AMULET_ELF_TEST_COLORIZER_H__

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
        ColorizerSequence::ownColorizer(_pUniform = new UniformColor(Color::BLACK));
        SystemEventRegistrar::theInstance->registerSystemEvents(this);
    }
    ~AmuletSelfTestColorizer() override
    {
        SystemEventRegistrar::theInstance->unregisterSystemEvents(this);
    }

    //----------------------------------------------------------------------------------------------
    // System
    //----------------------------------------------------------------------------------------------

    void setColor(COLOR_INT color)
    {
        _pUniform->setColor(color);
    }

    void onNetworkStatus(int netStatus) override
    {
        switch (netStatus)
        {
            case network_status_powering_off:
            case network_status_off:            setColor(Color::RED); break;
            case network_status_powering_on:
            case network_status_on:             setColor(Color::YELLOW); break;
            case network_status_connecting:
            case network_status_connected:      setColor(Color::GREEN); break;
            case network_status_disconnecting:
            case network_status_disconnected:   setColor(Color::MAGENTA); break;
        }
    }
    void onCloudStatus(int cloudStatus) override
    {
        switch (cloudStatus)
        {
            case cloud_status_connecting:
            case cloud_status_connected:     setColor(Color::CYAN); break;
            case cloud_status_disconnecting:
            case cloud_status_disconnected:  setColor(Color::BLUE); break;
        }
    }
    void onTimeChanged(int timeStatus) override
    {
    }
};

#endif

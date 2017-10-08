//
// CloudVariables.h
//
#ifndef __CLOUD_VARIABLES_H__
#define __CLOUD_VARIABLES_H__

#include "System/PersistentSettings.h"
#include "System/SystemEventRegistrar.h"

template <typename VALUE>
struct CloudVariable : SystemEventNotifications
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    typedef PersistentSettingTyped<VALUE> SETTING;

    SETTING* _persistentSetting;
    String   _name;
    bool     _begun = false;
    bool     _connected = false;
    bool     _timeSync = false;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    CloudVariable(LPCSTR name, SETTING* pSetting)
        : _persistentSetting(pSetting),
          _name(name)
    {
        SystemEventRegistrar::theInstance->registerSystemEvents(this);
    }

    ~CloudVariable()
    {
        SystemEventRegistrar::theInstance->unregisterSystemEvents(this);
    }

    //----------------------------------------------------------------------------------------------
    // System
    //----------------------------------------------------------------------------------------------

    void onNetworkStatus(system_event_t event, int netStatus) override
    {
    }
    void onCloudStatus(system_event_t event, int cloudStatus) override
    {
        if (cloudStatus==cloud_status_connected)
        {
            _connected = true;
            announce();
        }
    }
    void onTimeChanged(system_event_t event, int timeStatus) override
    {
        if (timeStatus == time_changed_sync)
        {
            _timeSync = true;
            announce();
        }
    }

    //----------------------------------------------------------------------------------------------
    // Cloud
    //----------------------------------------------------------------------------------------------

    const VALUE& valueRef()
    {
        return _persistentSetting->valueRef();
    }
    void setValue(const VALUE& value)
    {
        _persistentSetting->setValue(value);
    }

    void announce()
    {
        if (_begun && _connected)
        {
            INFO("announcing clound variable name=%s value=%s", _name.c_str(), _persistentSetting->valueAsString().c_str());
            bool success = Particle.variable(_name, this->valueRef());
            if (!success)
            {
                ERROR("announcing clound variable FAILED: %s", _name.c_str());
            }
        }
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin()
    {
        _begun = true;
        announce();
    }
};

#endif

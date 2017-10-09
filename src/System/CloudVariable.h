//
// CloudVariable.h
//
#ifndef __CLOUD_VARIABLE_H__
#define __CLOUD_VARIABLE_H__

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
    bool     _announced = false;

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

    void onNetworkStatus(int netStatus) override
    {
    }
    void onCloudStatus(int cloudStatus) override
    {
        if (cloudStatus==cloud_status_connected)
        {
            _connected = true;
            announce();
        }
    }
    void onTimeChanged(int timeStatus) override
    {
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
    int setValue(String value)
    {
        _persistentSetting->setValue(value);
        return 0;
    }

    void announce()
    {
        if (_begun && _connected && !_announced)
        {
            _announced = true;
            INFO("announcing cloud variable name=%s value=%s", _name.c_str(), _persistentSetting->valueAsString().c_str());
            bool success = Particle.variable(_name, this->valueRef());
            if (success)
            {
                success = Particle.function(_name, static_cast<int (CloudVariable::*)(String)>(&CloudVariable::setValue), this);
            }
            if (!success)
            {
                ERROR("announcing cloud variable FAILED: %s", _name.c_str());
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

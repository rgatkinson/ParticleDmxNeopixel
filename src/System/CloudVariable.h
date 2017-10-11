//
// CloudVariable.h
//
#ifndef __CLOUD_VARIABLE_H__
#define __CLOUD_VARIABLE_H__

#include "System/PersistentSettings.h"
#include "System/SystemEventRegistrar.h"

template <typename SETTING, typename VALUE>
struct CloudVariable : SystemEventNotifications
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    SETTING*        _persistentSetting;
    String          _name;
    ReadWriteable   _writeable;
    bool            _begun = false;
    bool            _connected = false;
    bool            _announced = false;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    CloudVariable(LPCSTR name, SETTING* pSetting, ReadWriteable writeable = ReadWriteable::RW)
        : _persistentSetting(pSetting),
          _name(name),
          _writeable(writeable)
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
        } else if (cloudStatus==cloud_status_disconnected)
        {
            _connected = false;
            _announced = false;
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
            if (success && _writeable==ReadWriteable::RW)
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

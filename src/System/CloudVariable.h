//
// CloudVariable.h
//
#ifndef __CLOUD_VARIABLE_H__
#define __CLOUD_VARIABLE_H__

#include <functional>
#include "System/PersistentSettings.h"
#include "System/SystemEventRegistrar.h"

//==================================================================================================
// AbstractCloudVariable
//==================================================================================================

template <typename VALUE>
struct AbstractCloudVariable : SystemEventNotifications
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
public:
    String                  _name;
    bool                    _begun = false;
    bool                    _connected = false;
    bool                    _announced = false;
    std::function<VALUE()>  _fnGetValue;
    VALUE                   _lastValue;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    AbstractCloudVariable(LPCSTR name)
        : _name(name)
    {
        _fnGetValue = []() { return VALUE{}; };
    }

    void registerSystemEvents()
    {
        SystemEventRegistrar::theInstance->registerSystemEvents(this);
    }

    virtual ~AbstractCloudVariable()
    {
        SystemEventRegistrar::theInstance->unregisterSystemEvents(this);
    }

    //----------------------------------------------------------------------------------------------
    // System
    //----------------------------------------------------------------------------------------------
public:

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
        else if (cloudStatus==cloud_status_disconnected)
        {
            _connected = false;
            _announced = false;
        }
    }
    void onTimeChanged(int timeStatus) override
    {
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    void begin()
    {
        // Do this late because we want to be fully constructed. And there's little
        // point in doing earlier. So here works fine.
        registerSystemEvents();

        _begun = true;
        announce();
    }

    //----------------------------------------------------------------------------------------------
    // Cloud
    //----------------------------------------------------------------------------------------------
protected:

    virtual bool announce()
    {
        bool success = false;
        if (_begun && _connected && !_announced)
        {
            _announced = true;
            INFO("announcing cloud variable name=%s", _name.c_str());
            success = announceVariable(_name, VALUE{});
        }
        return success;
    }

    void* onUpdateIntVariable()
    {
        _lastValue = _fnGetValue();
        INFO("variable %s=%d", _name.c_str(), _lastValue);
        return &_lastValue;
    }
    void* onUpdateStringVariable()
    {
        _lastValue = _fnGetValue();
        INFO("variable %s=%s", _name.c_str(), _lastValue);
        return const_cast<LPSTR>(_lastValue);
    }

    bool announceVariable(LPCSTR name, int var)
    {
        spark_variable_t extra;
        extra.size = sizeof(extra);
        extra.update = updateIntVariable;
        return spark_variable(name, reinterpret_cast<void*>(this), CloudVariableTypeInt::value(), &extra);
    }
    bool announceVariable(LPCSTR name, LPCSTR var)
    {
        spark_variable_t extra;
        extra.size = sizeof(extra);
        extra.update = updateStringVariable;
        return spark_variable(name, reinterpret_cast<void*>(this), CloudVariableTypeString::value(), &extra);
    }

    // called as: result = item->update(item->userVarKey, item->userVarType, item->userVar, nullptr);
    static const void* updateIntVariable(const char* name, Spark_Data_TypeDef type, const void* pvThis, void* reserved)
    {
        AbstractCloudVariable* pThis = reinterpret_cast<AbstractCloudVariable*>(const_cast<void*>(pvThis));
        return pThis->onUpdateIntVariable();
    }
    static const void* updateStringVariable(const char* name, Spark_Data_TypeDef type, const void* pvThis, void* reserved)
    {
        AbstractCloudVariable* pThis = reinterpret_cast<AbstractCloudVariable*>(const_cast<void*>(pvThis));
        return pThis->onUpdateStringVariable();
    }
};

//==================================================================================================
// CloudVariable
//==================================================================================================

template <typename VALUE>
struct CloudVariable : AbstractCloudVariable<VALUE>
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:
    typedef AbstractCloudVariable<VALUE> super;

    typedef CloudSetting<VALUE> SETTING;
    ReadWriteable       _writeable;
    SETTING*            _setting;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    CloudVariable(LPCSTR name, SETTING* pSetting, ReadWriteable writeable = ReadWriteable::RW)
        : AbstractCloudVariable<VALUE>(name),
          _setting(pSetting),
          _writeable(writeable)
    {
        super::_fnGetValue = [this]() { return _setting->value(); };
    }

    //----------------------------------------------------------------------------------------------
    // Cloud
    //----------------------------------------------------------------------------------------------
protected:
    int cloudSetValue(String value)
    {
        _setting->setValueString(value);
        return 0;
    }

    bool announce() override
    {
        bool success = super::announce();
        if (success && _writeable==ReadWriteable::RW)
        {
            success = Particle.function(super::_name, static_cast<int (CloudVariable::*)(String)>(&CloudVariable::cloudSetValue), this);
        }
        return success;
    }
};

#endif

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
    bool                    _systemEventsRegistered = false;
    bool                    _begun = false;
    bool                    _connected = false;
    bool                    _announced = false;
    std::function<VALUE()>  _fnGetValue;
    VALUE                   _lastValue;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    AbstractCloudVariable()
    {
    }

    void initialize(LPCSTR name)
    {
        _name = name;
        _fnGetValue = []() { return VALUE{}; };
    }

    void registerSystemEvents()
    {
        if (!_systemEventsRegistered)
        {
            SystemEventRegistrar::theInstance->registerSystemEvents(this);
            _systemEventsRegistered = true;
        }
    }

    virtual ~AbstractCloudVariable()
    {
        SystemEventRegistrar::theInstance->unregisterSystemEvents(this);
    }

    //----------------------------------------------------------------------------------------------
    // System
    //----------------------------------------------------------------------------------------------
public:

    void onNetworkStatus(NetworkStatus netStatus) override
    {
    }
    void onCloudStatus(int cloudStatus) override
    {
        if (cloudStatus==cloud_status_connected)
        {
            _connected = true;
            announceIfReady();
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
        registerAndAnnounce();
    }

    void loop()
    {
        registerAndAnnounce();
    }

    void report()
    {
    }

    void registerAndAnnounce()
    {
        // Do this late because we want to be fully constructed. And there's little
        // point in doing earlier. So here works fine.
        registerSystemEvents();

        _begun = true;
        announceIfReady();
    }


    //----------------------------------------------------------------------------------------------
    // Cloud
    //----------------------------------------------------------------------------------------------
protected:

    virtual bool announceIfReady()
    {
        bool success = false;
        if (_begun && _connected && !_announced)
        {
            _announced = true;
            INFO("announcing cloud variable name=%s", _name.c_str());
            success = announceVariable(_name, VALUE{});
            if (!success)
            {
                WARN("failed to announce cloud variable name=%s", _name.c_str());
            }
        }
        return success;
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
        extra.update = updateLPCSTRVariable;
        return spark_variable(name, reinterpret_cast<void*>(this), CloudVariableTypeString::value(), &extra);
    }
    bool announceVariable(LPCSTR name, const String& var)
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
    static const void* updateLPCSTRVariable(const char* name, Spark_Data_TypeDef type, const void* pvThis, void* reserved)
    {
        AbstractCloudVariable* pThis = reinterpret_cast<AbstractCloudVariable*>(const_cast<void*>(pvThis));
        return pThis->onUpdateLPCSTRVariable();
    }
    static const void* updateStringVariable(const char* name, Spark_Data_TypeDef type, const void* pvThis, void* reserved)
    {
        AbstractCloudVariable* pThis = reinterpret_cast<AbstractCloudVariable*>(const_cast<void*>(pvThis));
        return pThis->onUpdateStringVariable();
    }

    void* onUpdateIntVariable()
    {
        _lastValue = _fnGetValue();
        // INFO("read %s=%d", _name.c_str(), _lastValue);
        return &_lastValue;
    }
    void* onUpdateLPCSTRVariable()
    {
        _lastValue = _fnGetValue();
        // INFO("read %s=%s", _name.c_str(), _lastValue);
        return const_cast<LPSTR>(_lastValue);
    }
    void* onUpdateStringVariable()
    {
        _lastValue = _fnGetValue();
        // INFO("read %s=%s", _name.c_str(), _lastValue.c_str());
        return const_cast<LPSTR>(_lastValue.c_str());
    }
};

//==================================================================================================
// ComputedCloudVariable
//==================================================================================================

template <typename VALUE>
struct ComputedCloudVariable : AbstractCloudVariable<VALUE>
{
    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:
    ComputedCloudVariable()
    {
    }

    void initialize(LPCSTR name, std::function<VALUE()> fnGetValue)
    {
        AbstractCloudVariable<VALUE>::initialize(name);
        AbstractCloudVariable<VALUE>::_fnGetValue = fnGetValue;
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
    typedef CloudSetting<VALUE> Setting;

    ReadWriteable       _writeable;
    Setting*            _setting;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    CloudVariable()
    {
    }

    void initialize(LPCSTR name, Setting* pSetting, ReadWriteable writeable = ReadWriteable::RW)
    {
        super::initialize(name);
        _setting = pSetting;
        _writeable = writeable;
        super::_fnGetValue = [this]() { return _setting->value(); };
    }

    //----------------------------------------------------------------------------------------------
    // Cloud
    //----------------------------------------------------------------------------------------------
protected:
    int cloudSetValue(String value)
    {
        INFO("write %s=%s", super::_name.c_str(), value.c_str());
        return _setting->setValueString(value);
    }

    bool announceIfReady() override
    {
        bool success = super::announceIfReady();
        if (success && _writeable==ReadWriteable::RW)
        {
            success = Particle.function(super::_name, static_cast<int (CloudVariable::*)(String)>(&CloudVariable::cloudSetValue), this);
        }
        return success;
    }
};

#endif

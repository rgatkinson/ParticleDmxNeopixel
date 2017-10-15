//
// PersistentValueSetting.h
//
#ifndef __PERSISTENT_VALUE_SETTING_H__
#define __PERSISTENT_VALUE_SETTING_H__

#include "System/PersistentSettings.h"

//==================================================================================================
// PersistentValueSetting
//==================================================================================================

template <typename T>
struct PersistentValueSetting : PersistentSetting, NotifyableSetting<T>
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

private:
    struct State
    {
    private:
        T    _value;
        byte _flag;
    public:
        State()
        {
            _flag = PersistentSettings::INVALID_BYTE;
        }
        bool isValid() const
        {
            return _flag != PersistentSettings::INVALID_BYTE;
        }
        const T& value() const
        {
            return _value;
        }
        void setValue(const T& value)
        {
            _value = value;
            _flag = PersistentSettings::VALID_BYTE;
        }
    };

    T       _defaultValue;
    State   _state;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    PersistentValueSetting() : PersistentValueSetting(T{})
    {
    }
    PersistentValueSetting(const T& defaultValue)
    {
        setDefault(defaultValue);
        PersistentSettings::theInstance->addSetting(this);    // note: we lay out in declaration order!
    }

    void setDefault(const T& defaultValue)
    {
        _defaultValue = defaultValue;
    }

    bool ensureLoaded() override
    {
        bool result = _state.isValid();
        if (!result)
        {
            result = PersistentSettings::theInstance->load(this);
        }
        if (!result)
        {
            result = loadDefault();
            if (result)
            {
                save();
            }
        }
        return result;
    }

    bool loadDefault()
    {
        _state.setValue(defaultValue());
        trace("loaded default", _state.value());
        return true;
    }
    bool load(void* pv, int cb) override
    {
        if (cb == size())
        {
            memcpy(pointer(), pv, cb);
            if (_state.isValid())
            {
                trace("loaded", _state.value());
                return true;
            }
        }
        return false;
    }

    int size() override
    {
        return sizeof(_state);
    }
    void* pointer() override
    {
        return &_state;
    }
    void save()
    {
        PersistentSettings::theInstance->save(this);
        trace("saved", _state.value());
    }

    virtual void trace(LPCSTR message, T t)
    {
        INFO("PersistentValueSetting: %s: <subclass responsibility>", message);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    T defaultValue() const
    {
        return _defaultValue;
    }

    T value() override
    {
        ensureLoaded();
        return _state.value();
    }
    void setValue(const T& value) override
    {
        T oldValue = this->value();
        if (oldValue != value)
        {
            _state.setValue(value);
            save();
            notifyChanged(oldValue);
        }
    }
    void notifyChanged(const T& oldValue) override
    {
        NotifyableSetting<T>::notifyChanged(oldValue);
    }

    virtual int setValueString(const String& string) override
    {
        return 0; // subclass responsibility
    }
    virtual String valueAsString() override
    {
        return String("<subclass responsibility>");
    }
};

//==================================================================================================
// PersistentIntSetting
//==================================================================================================

struct PersistentIntSetting : PersistentValueSetting<int>
{
    PersistentIntSetting() : PersistentIntSetting(0) {}
    PersistentIntSetting(int defaultValue) : PersistentValueSetting<int>(defaultValue) { }

    virtual String valueAsString() override
    {
        return String::format("%d", value());
    }
    virtual int setValueString(const String& value) override
    {
        int intValue = value.toInt();
        PersistentValueSetting<int>::setValue(intValue);
        return intValue;
    }
    void trace(LPCSTR message, int value) override
    {
        INFO("PersistentIntSetting: %s: %d", message, value);
    }
};

#endif

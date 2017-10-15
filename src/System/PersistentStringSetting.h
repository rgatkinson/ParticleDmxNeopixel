//
// PersistentStringSetting.h
//
#ifndef __PERSISTENT_STRING_SETTING_H__
#define __PERSISTENT_STRING_SETTING_H__

//==================================================================================================
// PersistentStringSetting
//==================================================================================================

template <int _cchValue>
struct PersistentStringSetting : PersistentSetting, NotifyableSetting<LPCSTR>
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
private:
    struct State
    {
    private:
        byte _flag;
        char _rgchValue[_cchValue];  // always null terminated, include that in your count!
    public:
        State()
        {
            _flag = PersistentSettings::INVALID_BYTE;
            zero();
        }
        bool isValid()
        {
            return _flag != PersistentSettings::INVALID_BYTE;
        }
        LPCSTR value()
        {
            return &_rgchValue[0];
        }
        void setValue(LPCSTR value)
        {
            zero();
            safeStrncpy(_rgchValue, _cchValue, value);
            _flag = PersistentSettings::VALID_BYTE;
        }
        void zero()
        {
            ::zero(&_rgchValue[0], _cchValue);
        }
    };

    String  _defaultValue;
    State   _state;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:
    PersistentStringSetting() : PersistentStringSetting("")
    {
    }
    PersistentStringSetting(const String& defaultValue) : PersistentStringSetting(defaultValue.c_str())
    {
    }
    PersistentStringSetting(LPCSTR defaultValue)
    {
        setDefault(defaultValue);
        PersistentSettings::theInstance->addSetting(this);    // note: we lay out in declaration order!
    }
    void setDefault(LPCSTR defaultValue)
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
        _state.setValue(_defaultValue.c_str());
        INFO("PersistentStringSetting: loaded default: %s", valueAsString().c_str());
        return true;
    }
    bool load(void* pv, int cb) override
    {
        if (cb == size())
        {
            memcpy(pointer(), pv, cb);
            if (_state.isValid())
            {
                INFO("PersistentStringSetting: loaded: %s", valueAsString().c_str());
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
        INFO("PersistentStringSetting: saved: %s", valueAsString().c_str());
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    String valueAsString() override
    {
        return value();
    }
    LPCSTR value() override
    {
        ensureLoaded();
        return _state.value();
    }
    void setValue(const LPCSTR& value) override
    {
        LPCSTR oldValue = this->value();
        if (strcmp(value, oldValue) != 0)
        {
            String capturedOldValue(oldValue);
            _state.setValue(value);
            save();
            notifyChanged(capturedOldValue.c_str());
        }
    }
    int setValueString(const String& value) override
    {
        setValue(value.c_str());
        return value.length();
    }

};

#endif

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

    String  _defaultValue;
    char    _rgchValue[_cchValue];  // always null terminated

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    PersistentStringSetting() : PersistentStringSetting("")
    {
    }
    PersistentStringSetting(const String& defaultValue) : PersistentStringSetting(defaultValue.c_str())
    {
    }
    PersistentStringSetting(LPCSTR defaultValue)
        : _defaultValue(defaultValue)
    {
        zero();
        loadDefault();
        PersistentSettings::theInstance->addSetting(this);    // note: we lay out in declaration order!
    }
    void zero()
    {
        ::zero(&_rgchValue[0], _cchValue);
    }
    bool loadDefault() override
    {
        zero();
        safeStrncpy(_rgchValue, _cchValue, _defaultValue.c_str());
        INFO("PersistentStringSetting: loaded default: %s", valueAsString().c_str());
        return true;
    }
    bool load(void* pv, int cb) override
    {
        zero();
        memcpy(&_rgchValue[0], pv, min(cb, _cchValue-1));  // ALWAYS null terminate
        INFO("PersistentStringSetting: loaded: %s", valueAsString().c_str());
        return true;
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
        return &_rgchValue[0];
    }
    void setValue(const LPCSTR& sz) override
    {
        if (strcmp(sz, value()) != 0)
        {
            String oldValue(value());
            zero();
            safeStrncpy(_rgchValue, _cchValue, sz);
            save();
            notifyChanged(oldValue.c_str());
        }
    }
    int setValueString(const String& value) override
    {
        setValue(value.c_str());
        return value.length();
    }

    int size() override
    {
        return _cchValue;
    }
    void* pointer() override
    {
        return &_rgchValue[0];
    }
    void save()
    {
        PersistentSettings::theInstance->save(this);
    }
};

#endif

//
// PersistentStringSetting.h
//
#ifndef __PERSISTENT_STRING_SETTING_H__
#define __PERSISTENT_STRING_SETTING_H__

//==================================================================================================
// PersistentStringSetting
//==================================================================================================

template <int _cchValue>
struct PersistentStringSetting : PersistentTypedSetting<LPCSTR>
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    String  _defaultValue;
    LPCSTR  _value;
    char    _rgchValue[_cchValue];

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
        : _defaultValue(defaultValue),
        _value(&_rgchValue[0])
    {
        zero();
        loadDefault();
        PersistentSettings::theInstance->add(this);    // note: we lay out in declaration order!
    }
    void zero()
    {
        ::zero(&_rgchValue[0], _cchValue);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    String valueAsString() override
    {
        return String::format("%s", value());
    }
    LPCSTR value() override
    {
        return _value;
    }
    const LPCSTR& valueRef() override
    {
        return _value;
    }
    void setValue(const LPCSTR& sz) override
    {
        zero();
        safeStrncpy(_rgchValue, _cchValue, sz);
        save();
        INFO("PersistentStringSetting: setValue: %s", valueAsString().c_str());
    }
    void setValue(const String& value) override
    {
        setValue(value.c_str());
    }

    int size() override
    {
        return _cchValue;
    }
    void* pointer() override
    {
        return &_rgchValue[0];
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
    void save()
    {
        PersistentSettings::theInstance->save(this);
    }
};

//==================================================================================================
// VolatileStringSetting
//==================================================================================================

struct VolatileStringSetting
{
    String _value;

    VolatileStringSetting() : VolatileStringSetting("") {}
    VolatileStringSetting(LPCSTR defaultValue) : _value(defaultValue) {}

    String          value()                         { return _value; }
    const String&   valueRef()                      { return _value; }
    void            setValue(const String& value)   { _value = value; }
    void            setValue(LPCSTR value)          { _value = value; }
    String          valueAsString()                 { return value(); }
};

#endif

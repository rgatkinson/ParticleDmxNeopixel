//
// PersistentValueSetting.h
//
#ifndef __PERSISTENT_VALUE_SETTING_H__
#define __PERSISTENT_VALUE_SETTING_H__

//==================================================================================================
// PersistentValueSetting
//==================================================================================================

template <typename T>
struct PersistentValueSetting : PersistentTypedSetting<T>
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    T _value;
    T _default;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    PersistentValueSetting() : PersistentValueSetting(T{})
    {
    }
    PersistentValueSetting(const T& defaultValue) : _default(defaultValue)
    {
        loadDefault();
        PersistentSettings::theInstance->add(this);    // note: we lay out in declaration order!
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    T value() override
    {
        return _value;
    }
    const T& valueRef() override
    {
        return _value;
    }
    void setValue(const T& value) override
    {
        _value = value;
        save();
        INFO("PersistentValueSetting: setValue: %s", valueAsString().c_str());
    }
    virtual void setValue(const String& string) override
    {
        // subclass responsibility
    }
    virtual String valueAsString() override
    {
        return String("<subclass responsibility>");
    }

    int size() override
    {
        return sizeof(_value);
    }
    void* pointer() override
    {
        return &_value;
    }
    bool loadDefault() override
    {
        _value = _default;
        INFO("PersistentValueSetting: loaded default: %s", valueAsString().c_str());
        return true;
    }
    bool load(void* pv, int cb) override
    {
        if (cb == size())
        {
            memcpy(pointer(), pv, cb);
            INFO("PersistentValueSetting: loaded: %s", valueAsString().c_str());
            return true;
        }
        else
        {
            ERROR("PersistentValueSetting: invalid size: %d", cb);
            return false;
        }
    }
    void save()
    {
        PersistentSettings::theInstance->save(this);
    }
};

//==================================================================================================
// PersistentIntSetting
//==================================================================================================

struct PersistentIntSetting : PersistentValueSetting<int>
{
    PersistentIntSetting() : PersistentIntSetting(0) {}
    PersistentIntSetting(int defaultValue) : PersistentValueSetting<int>(defaultValue) {}

    virtual String valueAsString() override
    {
        return String::format("%d", value());
    }
    virtual void setValue(const String& value) override
    {
        PersistentValueSetting<int>::setValue(value.toInt());
    }
};

//==================================================================================================
// VolatileValueSetting
//==================================================================================================

template <typename T>
struct VolatileValueSetting
{
    T _value;

    VolatileValueSetting() : VolatileValueSetting(T{}) {}
    VolatileValueSetting(const T& defaultValue) : _value(defaultValue) {}

    T               value()                         { return _value; }
    const T&        valueRef()                      { return _value; }
    void            setValue(const T& value)        { _value = value; }
    virtual String  valueAsString()                 { return "<subclassResponsibility>"; }
    virtual void    setValue(const String& string)  { /*subclassResponsibility */ }
};

//==================================================================================================
// VolatileIntSetting
//==================================================================================================

struct VolatileIntSetting : VolatileValueSetting<int>
{
    VolatileIntSetting() : VolatileIntSetting(0) {}
    VolatileIntSetting(int defaultValue) : VolatileValueSetting<int>(defaultValue) {}

    virtual String valueAsString() override
    {
        return String::format("%d", value());
    }
    virtual void setValue(const String& value) override
    {
        VolatileValueSetting<int>::setValue(value.toInt());
    }
};
#endif

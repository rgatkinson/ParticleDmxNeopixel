//
// PersistentValueSetting.h
//
#ifndef __PERSISTENT_VALUE_SETTING_H__
#define __PERSISTENT_VALUE_SETTING_H__

//==================================================================================================
// PersistentValueSetting
//==================================================================================================

template <typename T>
struct PersistentValueSetting : PersistentSetting, NotifyableSetting<T>
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
        if (_value != value)
        {
            T oldValue = _value;
            _value = value;
            save();
            notifyChanged(oldValue);
        }
    }
    void notifyChanged(const T& oldValue) override
    {
        NotifyableSetting<T>::notifyChanged(oldValue);
    }

    virtual void setValueString(const String& string) override
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
    virtual void setValueString(const String& value) override
    {
        PersistentValueSetting<int>::setValue(value.toInt());
    }
};

//==================================================================================================
// VolatileValueSetting
//==================================================================================================

template <typename T>
struct VolatileValueSetting : TypedSetting<T>
{
    T _value;

    VolatileValueSetting() : VolatileValueSetting(T{}) {}
    VolatileValueSetting(const T& defaultValue) : _value(defaultValue) {}

    T               value()                         { return _value; }
    const T&        valueRef()                      { return _value; }
    virtual String  valueAsString()                 { return "<subclassResponsibility>"; }
    virtual void    setValueString(const String& string)  { /*subclassResponsibility */ }

    void setValue(const T& value)
    {
        if (_value != value)
        {
            _value = value;
        }
    }
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
    virtual void setValueString(const String& value) override
    {
        VolatileValueSetting<int>::setValue(value.toInt());
    }
};
#endif

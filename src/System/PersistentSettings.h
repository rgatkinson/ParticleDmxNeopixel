//
// PersistentSettings.h
//
#ifndef __PERSISTENT_SETTINGS_H__
#define __PERSISTENT_SETTINGS_H__

#include <vector>
#include "Util/Misc.h"

//==================================================================================================
// PersistentSetting
//==================================================================================================

struct PersistentSetting
{
    virtual int     size() = 0;
    virtual void*   pointer() = 0;
    virtual bool    loadDefault() = 0;
    virtual bool    load(void* pv, int cb) = 0;
};

template <typename T>
struct PersistentSettingTyped : PersistentSetting
{
    virtual T           value() = 0;
    virtual String      valueAsString() = 0;
    virtual void        setValue(const T& value) = 0;
};

//==================================================================================================
// PersistentSettings
//==================================================================================================

struct PersistentSettings
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    static const int ibVersion = 0;
    std::vector<PersistentSetting*> _persistentSettings;

public:
    static PersistentSettings* theInstance;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    PersistentSettings()
    {
        theInstance = this;
    }

    void add(PersistentSetting* persistentSetting)
    {
        _persistentSettings.push_back(persistentSetting);
    }

    //----------------------------------------------------------------------------------------------
    // Storage
    //----------------------------------------------------------------------------------------------

    bool isInitialized()
    {
        return (int)EEPROM.read(ibVersion) != 0xFF;
    }
    void saveInitialized()
    {
        EEPROM.write(ibVersion, 0);
    }
    void clear()
    {
        INFO("clearing EEPROM");
        EEPROM.clear();
    }

    int ibFirst()
    {
        return ibVersion + 1;
    }

    int ibFirst(PersistentSetting* persistentSetting)
    {
        int ib = ibFirst();
        for (auto it = _persistentSettings.begin(); it != _persistentSettings.end(); it++)
        {
            if ((*it)==persistentSetting)
            {
                return ib;
            }
            ib += (*it)->size();
        }
        return -1;
    }

    void save(PersistentSetting* persistentSetting)
    {
        int ibFirst = this->ibFirst(persistentSetting);
        if (ibFirst >= 0)
        {
            byte* pb = reinterpret_cast<byte*>(persistentSetting->pointer());
            int cb = persistentSetting->size();
            for (int dib = 0; dib < cb; dib++)
            {
                EEPROM.write(ibFirst + dib, pb[dib]);
            }
        }
    }

    void load(PersistentSetting* persistentSetting)
    {
        int ibFirst = this->ibFirst(persistentSetting);
        if (ibFirst >= 0)
        {
            int cb = persistentSetting->size();
            byte* pb = reinterpret_cast<byte*>(mallocNoFail(cb));
            for (int dib = 0; dib < cb; dib++)
            {
                pb[dib] = EEPROM.read(ibFirst + dib);
            }
            persistentSetting->load(pb, cb);
            free(pb);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin()
    {
        if (!isInitialized())
        {
            // Tell all the settings that we're starting anew
            for (auto it = _persistentSettings.begin(); it != _persistentSettings.end(); it++)
            {
                (*it)->loadDefault();
            }

            // Persistently store that new state
            INFO("saving EEPROM");
            for (auto it = _persistentSettings.begin(); it != _persistentSettings.end(); it++)
            {
                save(*it);
            }

            // Appear initialized next time
            saveInitialized();
        }
        else
        {
            // Load from persistent storage
            INFO("loading EEPROM");
            for (auto it = _persistentSettings.begin(); it != _persistentSettings.end(); it++)
            {
                load(*it);
            }
        }
    }

    void loop()
    {
    }

    void report()
    {
    }

};

//==================================================================================================
// PersistentValueSetting
//==================================================================================================

template <typename T>
struct PersistentValueSetting : PersistentSettingTyped<T>
{
    T _value;
    T _default;

    PersistentValueSetting() : PersistentValueSetting(T{})
    {
    }
    PersistentValueSetting(const T& defaultValue) : _default(defaultValue)
    {
        loadDefault();
        PersistentSettings::theInstance->add(this);    // note: we lay out in declaration order!
    }
    T value() override
    {
        return _value;
    }
    void setValue(const T& value) override
    {
        _value = value;
        save();
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
    PersistentIntSetting() : PersistentIntSetting(0)
    {
    }
    PersistentIntSetting(int defaultValue) : PersistentValueSetting<int>(defaultValue)
    {
    }

    virtual String valueAsString() override
    {
        return String::format("%d", value());
    }
};

//==================================================================================================
// PersistentStringSetting
//==================================================================================================

template <int _cchValue>
struct PersistentStringSetting : PersistentSettingTyped<LPCSTR>
{
    String _defaultValue;
    char _rgchValue[_cchValue];

    PersistentStringSetting() : PersistentStringSetting("")
    {
    }
    PersistentStringSetting(const String& defaultValue) : PersistentStringSetting(defaultValue.c_str())
    {
    }
    PersistentStringSetting(LPCSTR defaultValue) : _defaultValue(defaultValue)
    {
        zero();
        loadDefault();
        PersistentSettings::theInstance->add(this);    // note: we lay out in declaration order!
    }
    void zero()
    {
        ::zero(&_rgchValue[0], _cchValue);
    }

    String valueAsString() override
    {
        return String::format("%s", value());
    }
    LPCSTR value() override
    {
        return &_rgchValue[0];
    }
    void setValue(const LPCSTR& sz) override
    {
        zero();
        safeStrncpy(_rgchValue, _cchValue, sz);
        save();
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

#endif

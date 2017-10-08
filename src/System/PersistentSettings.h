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
    virtual void    loadDefault() = 0;
    virtual void    load(void* pv, int cb) = 0;
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
struct PersistentValueSetting : PersistentSetting
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
    T& value()
    {
        return _value;
    }
    void setValue(const T& value)
    {
        _value = value;
        PersistentSettings::theInstance->save(this);
    }

    int size() override
    {
        return sizeof(_value);
    }
    void* pointer() override
    {
        return &_value;
    }
    void loadDefault() override
    {
        _value = _default;
    }
    void load(void* pv, int cb) override
    {
        if (cb == size())
        {
            memcpy(pointer(), pv, cb);
        }
        else
        {
            ERROR("invalid setting size: %d", cb);
        }
    }
};

//==================================================================================================
// PersistentStringSetting
//==================================================================================================

template <int _cchBuffer>
struct PersistentStringSetting : PersistentSetting
{
    String _default;
    char _rgchBuffer[_cchBuffer];

    PersistentStringSetting() : PersistentStringSetting("")
    {
    }
    PersistentStringSetting(const String& defaultValue) : PersistentStringSetting(defaultValue.c_str())
    {
    }
    PersistentStringSetting(LPCSTR defaultValue) : _default(defaultValue)
    {
        zero();
        loadDefault();
        PersistentSettings::theInstance->add(this);    // note: we lay out in declaration order!
    }
    void zero()
    {
        ::zero(&_rgchBuffer[0], _cchBuffer);
    }

    LPCSTR value()
    {
        return &_rgchBuffer[0];
    }
    void setValue(LPCSTR sz)
    {
        zero();
        safeStrncpy(_rgchBuffer, _cchBuffer, sz);
        PersistentSettings::theInstance->save(this);
    }

    int size() override
    {
        return _cchBuffer;
    }
    void* pointer() override
    {
        return &_rgchBuffer[0];
    }
    void loadDefault() override
    {
        zero();
        safeStrncpy(_rgchBuffer, _cchBuffer, _default.c_str());
    }
    void load(void* pv, int cb) override
    {
        zero();
        memcpy(&_rgchBuffer[0], pv, min(cb, _cchBuffer-1));  // ALWAYS null terminate
    }
};

#endif

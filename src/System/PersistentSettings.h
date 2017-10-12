//
// PersistentSettings.h
//
#ifndef __PERSISTENT_SETTINGS_H__
#define __PERSISTENT_SETTINGS_H__

#include <vector>
#include <functional>
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
struct CloudSetting
{
    virtual T      value() = 0;
    virtual String valueAsString() = 0;
    virtual void   setValueString(const String& string) = 0;
};

template <typename T>
struct TypedSetting : CloudSetting<T>
{
    virtual void setValue(const T& value) = 0;
};

template <typename T>
struct NotifyableSetting : TypedSetting<T>
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    typedef std::function<void(const T&)> Notifyee;
    Notifyee _notifyee = static_cast<Notifyee>(nullptr);

    //----------------------------------------------------------------------------------------------
    // Notifications
    //----------------------------------------------------------------------------------------------

    template <typename U>
    void setChangeNotification(U notifyee)
    {
        _notifyee = static_cast<Notifyee>(notifyee);
    }
    virtual void notifyChanged(const T& oldValue)
    {
        if (_notifyee)
        {
            _notifyee(oldValue);
        }
    }
};

//==================================================================================================
// PersistentSettings
//==================================================================================================

#define USE_HAL 1

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
            #if USE_HAL
                HAL_EEPROM_Put(ibFirst, pb, cb);
            #else
                for (int dib = 0; dib < cb; dib++)
                {
                    EEPROM.write(ibFirst + dib, pb[dib]);
                }
            #endif
        }
    }

    void load(PersistentSetting* persistentSetting)
    {
        int ibFirst = this->ibFirst(persistentSetting);
        if (ibFirst >= 0)
        {
            int cb = persistentSetting->size();
            byte* pb = reinterpret_cast<byte*>(mallocNoFail(cb));
            #if USE_HAL
                HAL_EEPROM_Get(ibFirst, pb, cb);
            #else
                for (int dib = 0; dib < cb; dib++)
                {
                    pb[dib] = EEPROM.read(ibFirst + dib);
                }
            #endif
            persistentSetting->load(pb, cb);
            free(pb);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Cloud
    //----------------------------------------------------------------------------------------------

    int factoryReset(String value)
    {
        clear();
        System.reset();
        return 0;
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin()
    {
        Particle.function("factoryReset", &PersistentSettings::factoryReset, this);

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
};



#undef USE_HAL
#endif

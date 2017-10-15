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
    virtual bool    ensureLoaded() = 0;
    virtual bool    load(void* pv, int cb) = 0;
};

template <typename T>
struct CloudSetting
{
    virtual T      value() = 0;
    virtual String valueAsString() = 0;
    virtual int    setValueString(const String& string) = 0;
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
    void registerNotifyChanged(U notifyee)
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

#define USE_HAL 0 // nice hack, but it doesn't work reliably as currently manifest. we don't know why
// seemed to work for most vars, but isn't working for the header.

struct PersistentSettings
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    std::vector<PersistentSetting*> _persistentSettings;
    int                             _ibNext;

public:
    static InstanceHolder<PersistentSettings> theInstance;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    PersistentSettings()
    {
        _ibNext = ibDataFirst();
    }

    int addSetting(PersistentSetting* persistentSetting)
    {
        _persistentSettings.push_back(persistentSetting);
        int result = _ibNext;
        _ibNext += persistentSetting->size();
        return result;
    }

    //----------------------------------------------------------------------------------------------
    // Header management
    //----------------------------------------------------------------------------------------------

    static const byte INVALID_BYTE = (byte)0xFF;
    static const byte VALID_BYTE   = (byte)0;

    static const int ibHeader = 0;

    int ibDataFirst()
    {
        return ibHeader + sizeof(Header);
    }
    int ibValidMax()
    {
        return readHeader().ibValidMax();
    }

    struct Header
    {
        int  _ibValidMax;
        byte _flag;

        bool isInitialized()
        {
            return _flag != INVALID_BYTE;
        }
        bool initialize()
        {
            bool result = false;
            if (!isInitialized())
            {
                _ibValidMax = 0;
                _flag = VALID_BYTE;
                result = true;
            }
            return result;
        }
        int ibValidMax()
        {
            return isInitialized() ? _ibValidMax : 0;
        }
        bool noteValidMax(int ibValidMax)
        {
            bool result = initialize();
            if (_ibValidMax < ibValidMax)
            {
                _ibValidMax = ibValidMax;
                result = true;
            }
            return result;
        }
    };

    Header readHeader()
    {
        Header header;
        readBytes(ibHeader, &header, sizeof(header));
        return header;
    }
    void writeHeader(const Header& header)
    {
        writeBytes(ibHeader, &header, sizeof(header));
    }

    bool isInitialized()
    {
        return readHeader().isInitialized();
    }
    void saveInitialized()
    {
        Header header = readHeader();
        if (!isInitialized())
        {
            header.initialize();
            writeHeader(header);
            INFO("saveInitialized: isInitialized()=%d", isInitialized());
        }
    }
    void noteValid(int ibValidMax)
    {
        Header header = readHeader();
        if (header.noteValidMax(ibValidMax))
        {
            writeHeader(header);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Storage
    //----------------------------------------------------------------------------------------------

    void save(PersistentSetting* persistentSetting, int ibFirst)
    {
        if (ibFirst >= 0)
        {
            persistentSetting->ensureLoaded();
            byte* pb = reinterpret_cast<byte*>(persistentSetting->pointer());
            int cb = persistentSetting->size();
            writeBytes(ibFirst, pb, cb);
            int ibMax = ibFirst + cb;
            noteValid(ibMax);
        }
    }

    bool load(PersistentSetting* persistentSetting, int ibFirst)
    {
        bool result = false;
        if (ibFirst >= 0)
        {
            int cb = persistentSetting->size();
            int ibMax = ibFirst + cb;

            // Is the space occupied by the setting a valid region of data?
            if (ibMax <= ibValidMax())
            {
                byte* pb = reinterpret_cast<byte*>(mallocNoFail(cb));
                readBytes(ibFirst, pb, cb);
                result = persistentSetting->load(pb, cb);
                free(pb);
            }
        }
        return result;
    }

    //----------------------------------------------------------------------------------------------
    // Low level
    //----------------------------------------------------------------------------------------------

    void clear()
    {
        INFO("clearing EEPROM");
        EEPROM.clear();
    }

    void writeBytes(int ibFirst, const byte* pb, int cb)
    {
        #if USE_HAL
            HAL_EEPROM_Put(ibFirst, pb, cb);
        #else
            for (int dib = 0; dib < cb; dib++)
            {
                EEPROM.write(ibFirst + dib, pb[dib]);
            }
        #endif
    }

    void readBytes(int ibFirst, byte*pb, int cb)
    {
        #if USE_HAL
            HAL_EEPROM_Get(ibFirst, pb, cb);
        #else
            for (int dib = 0; dib < cb; dib++)
            {
                pb[dib] = EEPROM.read(ibFirst + dib);
            }
        #endif
    }

    template <typename T>
    void writeBytes(int ibFirst, T* pt, int cb)
    {
        return writeBytes(ibFirst, reinterpret_cast<const byte*>(pt), cb);
    }

    template <typename T>
    void readBytes(int ibFirst, T* pt, int cb)
    {
        return readBytes(ibFirst, reinterpret_cast<byte*>(pt), cb);
    }

    //----------------------------------------------------------------------------------------------
    // Cloud
    //----------------------------------------------------------------------------------------------

    int factoryReset(String value)
    {
        // Make caller put in a parameter as a safety check
        if (value.length() > 0)
        {
            clear();
            System.reset();
        }
        else
        {
            INFO("empty factoryReset request ignored");
        }
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
            INFO("initializing EEPROM");
            saveInitialized();
        }
        else
        {
            INFO("loading EEPROM");
        }
    }
};

decltype(PersistentSettings::theInstance) SELECTANY PersistentSettings::theInstance;

#undef USE_HAL
#endif

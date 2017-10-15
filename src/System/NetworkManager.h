//
// NetworkManager.h
//
#ifndef __NETWORK_MANAGER_H_
#define __NETWORK_MANAGER_H_

#include <initializer_list>
#include <functional>
#include <map>
#include <vector>
#include "Util/Misc.h"
#include "System/Looper.h"
#include "System/SystemEventRegistrar.h"
#include "System/NetworkStatusMonitor.h"
#include "System/PersistentValueSetting.h"
#include "System/PersistentStringSetting.h"
#include "System/CloudVariable.h"

//==================================================================================================
// WiFiPassword
//==================================================================================================

struct WiFiPassword
{
    String          ssid;
    String          password;
    SecurityType    securityType;

    WiFiPassword(LPCSTR ssid="", LPCSTR password="", SecurityType securityType = WPA2)
    {
        this->ssid = String(ssid);
        this->password = String(password);
        this->securityType = securityType;
    }

    WiFiPassword(const WiFiPassword& them)
    {
        this->ssid = them.ssid;
        this->password = them.password;
        this->securityType = them.securityType;
    }

    ~WiFiPassword()
    {
    }
};

//==================================================================================================
// NetworkManager
//==================================================================================================

struct NetworkManager : SystemEventNotifications
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    typedef String                                         AppNameType;
    typedef std::function<Looper*()>                       CreateLooperType;
    typedef std::pair<const AppNameType, CreateLooperType> AppMapPairType;
          const LPCSTR            _nullAppName           = "null";
          const LPCSTR            _defaultAppName        = _nullAppName;

    /*
     * wakeUpPin: the wakeup pin number. supports external interrupts on the following pins:
     *   D1, D2, D3, D4, A0, A1, A3, A4, A6, A7 (WKP)
     * The same pin limitations as attachInterrupt apply
    */
    static const int              _sSleepTimeoutDefault  = 0;
    static const int              _sSleepLengthDefault   = 600;
    static const uint16_t         _wakeUpPin             = A7;
    static const PinMode          _wakeUpPinMode         = INPUT_PULLUP;
    static const InterruptMode    _wakeUpEdgeMode        = FALLING;
           const SleepNetworkFlag _wakeUpNetworkMode     = SLEEP_NETWORK_OFF;

    bool                            _connectAttempted = false;
    bool                            _clearCredentials = false;
    rga::vector<WiFiPassword>       _passwords;
    ElapsedTime                     _upTime;
    Deadline                        _sleepTimer;

    CloudVariable<LPCSTR>           _cloudAppName;
    CloudVariable<int>              _cloudSleepTimeout;
    CloudVariable<int>              _cloudSleepLength;
    ComputedCloudVariable<int>      _cloudUptime;

    std::map<AppNameType, CreateLooperType> _appMap;
    Looper*                         _pLooper;
    bool                            _begun;

    static PersistentStringSetting<14> _appName;
    static PersistentIntSetting        _sleepTimeout;
    static PersistentIntSetting        _sleepLength;

public:
    static InstanceHolder<NetworkManager> theInstance;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    typedef std::initializer_list<AppMapPairType> InitializerType;

    NetworkManager()
        : _cloudAppName("app", &_appName),
          _cloudSleepTimeout("sleepTimeout", &_sleepTimeout),
          _cloudSleepLength("sleepLength", &_sleepLength),
          _cloudUptime("uptime", [this]() { return _upTime.seconds(); }),
          _appMap(),
          _pLooper(nullptr),
          _begun(false)
    {
        _sleepTimeout.setDefault(_sSleepTimeoutDefault);
        _sleepLength.setDefault(_sSleepLengthDefault);
        _appName.setDefault(_defaultAppName);

        pinMode(_wakeUpPin, _wakeUpPinMode);
        WiFi.selectAntenna(ANT_INTERNAL);   // persistently remembered
        _sleepTimeout.registerNotifyChanged([this](int oldValue) { resetSleepTimer("sleepTimeout changed"); });
        _appName.registerNotifyChanged([this](LPCSTR oldValue) { onAppNameChanged(); });
        SystemEventRegistrar::theInstance->registerSystemEvents(this);

    }
    ~NetworkManager()
    {
        SystemEventRegistrar::theInstance->unregisterSystemEvents(this);
        releaseRef(_pLooper);
    }

    void setApplications(InitializerType applications)
    {
        _appMap = std::map<AppNameType, CreateLooperType>(applications);
        _appMap[_nullAppName] = []() { return nullptr; };
    }

    void addPassword(WiFiPassword& password)
    {
        _passwords.push_back(password);
    }

    void setClearCredentials(bool clearCredentials)
    {
        _clearCredentials = clearCredentials;
    }

    void addPasswords(WiFiPassword wifiPasswords[], int cb)
    {
        int passwordCount = cb / sizeof(wifiPasswords[0]);
        for (int i = 0; i < passwordCount; i++)
        {
            addPassword(wifiPasswords[i]);
        }
    }

    //----------------------------------------------------------------------------------------------
    // System Events
    //----------------------------------------------------------------------------------------------

    void onNetworkStatus(NetworkStatus netStatus) override
    {
        INFO("network: %s", nameOf(netStatus));
        if (netStatus == NetworkStatus::Connected)
        {
            resetSleepTimer("NetworkManager::onNetworkStatus");
        }
    }

    void onCloudStatus(int cloudStatus) override
    {
        switch (cloudStatus)
        {
            case cloud_status_connecting:    INFO("cloud: connecting"); break;
            case cloud_status_connected:     INFO("cloud: connected"); break;
            case cloud_status_disconnecting: INFO("cloud: disconnecting"); break;
            case cloud_status_disconnected:  INFO("cloud: disconnected"); break;
        }
    }

    void onTimeChanged(int timeStatus) override
    {
        switch (timeStatus)
        {
            case time_changed_manually:     INFO("time: manual change"); break;
            case time_changed_sync:         INFO("time: synhcronized"); break;
        }
    }

    //----------------------------------------------------------------------------------------------
    // Sleep
    //----------------------------------------------------------------------------------------------

    bool sleepEnabled()
    {
        if (_sleepTimeout.value() > 0)
        {
            if (NetworkStatusMonitor::theInstance->status() == NetworkStatus::Connected)
            {
                return true;
            }
        }
        return false;
    }

    void resetSleepTimer(LPCSTR caller)
    {
        _sleepTimer = Deadline(_sleepTimeout.value() * 1000);
    }

    void checkForSleep()
    {
        if (sleepEnabled() && _sleepTimer.hasExpired())
        {
            sleep();
        }
    }

    void sleep()
    {
        int sSleep = _sleepLength.value();
        WARN("*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* system sleeping for %ds on pin %d=%s", sSleep, _wakeUpPin, nameOfPin(_wakeUpPin));
        System.sleep(_wakeUpPin, _wakeUpEdgeMode, sSleep, _wakeUpNetworkMode);  // doesn't return
        announceLog();
        resetSleepTimer("sleep");
    }

    //----------------------------------------------------------------------------------------------
    // App management
    //----------------------------------------------------------------------------------------------

    void onAppNameLoad()
    {
        ensureApp();
    }

    void onAppNameChanged()
    {
        if (ensureApp())
        {
            // Start over to ensure clean code path. In particular, to ensure
            // that the cloud variables get published correctly (we don't seem)
            // to be able to add dynamically after our initial publishing bunch;
            // don't understand why.
            INFO("restarting system");
            System.reset();
        }
    }

    bool ensureApp()
    {
        bool result = false;
        const String name = String(_appName.value()).toLowerCase();

        auto query = _appMap.find(name);
        if (query != _appMap.end())
        {
            INFO("creating app name '%s'", name.c_str());

            CreateLooperType& creator = query->second;
            releaseRef(_pLooper);
            _pLooper = creator();
            if (_begun)
            {
                if (_pLooper) _pLooper->begin();
            }
            result = true;
        }
        else
        {
            WARN("unrecognized app name '%s': ignored", name.c_str());
        }
        return result;
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin()
    {
        onAppNameLoad();
        resetSleepTimer("NetworkManager::begin");

        WiFi.on();  // must be on first before we call setCredentials()

        if (_clearCredentials)
        {
            WiFi.clearCredentials();    // will issue WARN
        }
        for (int i = 0; i < _passwords.count(); i++)
        {
            WiFiPassword& password = _passwords[i];
            INFO("adding credential: ssid=\"%s\" pw=\"%s\" security=%d cipher=%d", password.ssid.c_str(), password.password.c_str(), password.securityType, WLAN_CIPHER_AES);
            WiFi.setCredentials(password.ssid, password.password, password.securityType, WLAN_CIPHER_AES);
        }

        WiFi.connect(); // no point in WIFI_CONNECT_SKIP_LISTEN: that only matters if WiFi lacks credentials
        _cloudAppName.begin();
        _cloudUptime.begin();
        _cloudSleepTimeout.begin();
        _cloudSleepLength.begin();
        report();

        if (_pLooper) _pLooper->begin();
        _begun = true;
    }

    void loop()
    {
        if (WiFi.connecting())
        {
        }
        else if (WiFi.ready() && !_connectAttempted)
        {
            _connectAttempted = true;
            Particle.connect();
        }
        Particle.process();
        checkForSleep();

        _cloudAppName.loop();
        _cloudUptime.loop();
        _cloudSleepTimeout.loop();
        _cloudSleepLength.loop();
        if (_pLooper) _pLooper->loop();
    }

    void report()
    {
        WiFiAccessPoint ap[5];
        int credentialCount = WiFi.getCredentials(ap, 5);
        INFO("%d credentials reported", credentialCount);
        for (int i = 0; i < credentialCount; i++)
        {
            INFO("credential: ssid=\"%s\" security=%d cipher=%d", ap[i].ssid, ap[i].security, ap[i].cipher);
        }

        _cloudAppName.report();
        _cloudUptime.report();
        _cloudSleepTimeout.report();
        _cloudSleepLength.report();
        if (_pLooper) _pLooper->report();
    }

};

decltype(NetworkManager::theInstance) SELECTANY NetworkManager::theInstance;

inline void resetSleepTimer(LPCSTR sz)
{
    NetworkManager::theInstance->resetSleepTimer(sz);
}

#endif

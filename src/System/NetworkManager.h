//
// NetworkManager.h
//
#ifndef __NETWORK_MANAGER_H_
#define __NETWORK_MANAGER_H_

#include <vector>
#include "Util/Misc.h"
#include "System/SystemEventRegistrar.h"
#include "System/NetworkStatusMonitor.h"
#include "System/PersistentValueSetting.h"
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
    String                          _appName;
    ElapsedTime                     _upTime;
    Deadline                        _sleepTimer;
    PersistentIntSetting            _sleepTimeout;
    PersistentIntSetting            _sleepLength;

    ComputedCloudVariable<LPCSTR>   _cloudAppName;
    ComputedCloudVariable<int>      _cloudUptime;
    CloudVariable<int>              _cloudSleepTimeout;
    CloudVariable<int>              _cloudSleepLength;

public:
    static NetworkManager* theInstance;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:
    NetworkManager(LPCSTR szAppName)
        : _sleepTimeout(_sSleepTimeoutDefault),
          _sleepLength(_sSleepLengthDefault),
          _appName(szAppName),
          _cloudAppName("app", [this]() { return _appName.c_str(); }),
          _cloudUptime("uptime", [this]() { return _upTime.seconds(); }),
          _cloudSleepTimeout("sleepTimeout", &_sleepTimeout),
          _cloudSleepLength("sleepLength", &_sleepLength)
    {
        pinMode(_wakeUpPin, _wakeUpPinMode);
        theInstance = this;
        WiFi.selectAntenna(ANT_INTERNAL);   // persistently remembered
        _sleepTimeout.registerNotifyChanged([this](int oldValue) { resetSleepTimer("sleepTimeout changed"); });
        SystemEventRegistrar::theInstance->registerSystemEvents(this);
    }
    ~NetworkManager()
    {
        _sleepTimeout.registerNotifyChanged(nullptr);
        SystemEventRegistrar::theInstance->unregisterSystemEvents(this);
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
        // INFO("resetSleepTimer(%s)", caller);
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
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin()
    {
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
    }

};

inline void resetSleepTimer(LPCSTR sz)
{
    NetworkManager::theInstance->resetSleepTimer(sz);
}

#endif

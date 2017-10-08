//
// NetworkManager.h
//
#ifndef __NETWORK_MANAGER_H_
#define __NETWORK_MANAGER_H_

#include "Util/Misc.h"
#include "Util/ArrayList.h"
#include "SystemEventRegistrar.h"

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

    bool _connectAttempted = false;
    bool _clearCredentials = false;
    ArrayList<WiFiPassword> _passwords;

public:
    static NetworkManager* theInstance;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:
    NetworkManager()
    {
        theInstance = this;
        WiFi.selectAntenna(ANT_INTERNAL);   // persistently remembered
        SystemEventRegistrar::theInstance->registerSystemEvents(this);
    }

    void addPassword(WiFiPassword& password)
    {
        _passwords.addLast(password);
    }

    void addPasswords(bool clearWifiPasswords, WiFiPassword wifiPasswords[], int cb)
    {
        _clearCredentials = clearWifiPasswords;
        int passwordCount = cb / sizeof(wifiPasswords[0]);
        for (int i = 0; i < passwordCount; i++)
        {
            addPassword(wifiPasswords[i]);
        }
    }

    //----------------------------------------------------------------------------------------------
    // System Events
    //----------------------------------------------------------------------------------------------

    void onNetworkStatus(system_event_t ignored, int netStatus) override
    {
        switch (netStatus)
        {
            case network_status_powering_off:   INFO("network: powering off"); break;
            case network_status_off:            INFO("network: off"); break;
            case network_status_powering_on:    INFO("network: powering on"); break;
            case network_status_on:             INFO("network: on"); break;
            case network_status_connecting:     INFO("network: connecting"); break;
            case network_status_connected:      INFO("network: connected"); break;      // DHCP acquired
            case network_status_disconnecting:  INFO("network: disconnecting"); break;
            case network_status_disconnected:   INFO("network: disconnected"); break;
        }
    }

    void onCloudStatus(system_event_t event, int cloudStatus) override
    {
        switch (cloudStatus)
        {
            case cloud_status_connecting:    INFO("cloud: connecting"); break;
            case cloud_status_connected:     INFO("cloud: connected"); break;
            case cloud_status_disconnecting: INFO("cloud: disconnecting"); break;
            case cloud_status_disconnected:  INFO("cloud: disconnected"); break;
        }
    }

    void onTimeChanged(system_event_t event, int timeStatus) override
    {
        switch (timeStatus)
        {
            case time_changed_manually:     INFO("time: manual change"); break;
            case time_changed_sync:         INFO("time: synhcronized"); break;
        }
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    void begin()
    {
        INFO("NetworkManager::begin");

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

        // no point in WIFI_CONNECT_SKIP_LISTEN: that only matters if WiFi lacks credentials
        WiFi.connect();

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

#endif

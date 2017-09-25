//
// NetworkManager.h
//
#ifndef __NETWORK_MANAGER_H_
#define __NETWORK_MANAGER_H_

#include "Util/Misc.h"
#include "Util/ArrayList.h"

//==================================================================================================
// Globals
//==================================================================================================

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(MANUAL);

//==================================================================================================
// WiFiPassword
//==================================================================================================

struct WiFiPassword
{
    LPSTR           ssid;
    LPSTR           password;
    SecurityType    securityType;

    WiFiPassword(LPCSTR ssid=NULL, LPCSTR password=NULL, SecurityType securityType = WPA2)
    {
        this->ssid = strdup(ssid);
        this->password = strdup(password);
        this->securityType = securityType;
    }

    WiFiPassword(const WiFiPassword& them)
    {
        this->ssid = strdup(them.ssid);
        this->password = strdup(them.password);
        this->securityType = them.securityType;
    }

    ~WiFiPassword()
    {
        strfree(ssid);
        strfree(password);
    }
};

//==================================================================================================
// NetworkManager
//==================================================================================================

struct NetworkManager
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    int  _loopCount = 0;
    bool _connectAttempted = false;
    int  _connectAttemptLoopCount = 0;
    bool _clearCredentials = false;
    ArrayList<WiFiPassword> _passwords;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:
    NetworkManager()
    {
        WiFi.selectAntenna(ANT_INTERNAL);   // persistently remembered
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
    // Accessing
    //----------------------------------------------------------------------------------------------

    void begin()
    {
        WiFi.on();  // must be on first before we call setCredentials()

        if (_clearCredentials)
        {
            WiFi.clearCredentials();
        }
        for (int i = 0; i < _passwords.count(); i++)
        {
            WiFiPassword& password = _passwords[i];
            WiFi.setCredentials(password.ssid, password.password, password.securityType, WLAN_CIPHER_AES);
        }

        // no point in WIFI_CONNECT_SKIP_LISTEN: that only matters if WiFi lacks credentials
        WiFi.connect();
    }

    void loop()
    {
        if (WiFi.ready() && !_connectAttempted)
        {
            Log.info("calling Particle.connect()...");
            Particle.connect();
            Log.info("...done");
            _connectAttempted = true;
            _connectAttemptLoopCount = _loopCount;
        }
        if (_connectAttempted && _connectAttemptLoopCount == _loopCount-1)
        {
            if (!Particle.connected())
            {
                Log.warn("Particle.connect() failed");
            }
            else
            {
                Log.info("Particle.connect() succeeded");
            }
        }
        Particle.process();
        _loopCount++;
    }

    void report()
    {
    }

};

#endif

//
// NetworkManager.h
//
#ifndef __NETWORK_MANAGER_H_
#define __NETWORK_MANAGER_H_

#include "Misc.h"
#include "ArrayList.h"

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(MANUAL);

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

struct NetworkManager
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    int  loopCount = 0;
    bool connectAttempted = false;
    int  connectAttemptLoopCount = 0;
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

    void addPasswords(WiFiPassword wifiPasswords[], int cb)
    {
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
        if (WiFi.ready() && !connectAttempted)
        {
            Log.info("calling Particle.connect()...");
            Particle.connect();
            Log.info("...done");
            connectAttempted = true;
            connectAttemptLoopCount = loopCount;
        }
        if (connectAttempted && connectAttemptLoopCount == loopCount-1)
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
        loopCount++;
    }

    void report()
    {

    }

};

#endif

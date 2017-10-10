//
// Globals.h
//
#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "SystemEventRegistrar.h"
#include "PersistentSettings.h"
#include "NetworkManager.h"
#include "CloudVariable.h"
#include "Util/ElapsedTime.h"

extern void setCredentials();

struct Globals
{
public:
    static Globals* theInstance;

private:
    SerialLogHandler        _logHandler;
    SystemEventRegistrar    _systemEventRegistrar;
    PersistentSettings      _persistentSettings;
    NetworkManager          _networkManager;
    int                     _buttonToken;

public:
    Globals()
    {
        theInstance = this;
        System.enableFeature(FEATURE_RESET_INFO);
        if (System.resetReason() == RESET_REASON_PANIC)
        {
            System.enterSafeMode();
        }

        Serial.blockOnOverrun(false);   // its just debug; don't hold things up
        Serial.begin(115200);
        while (!Serial.isEnabled())
        {
            // spin
        }

        setCredentials(); // see AfterPasswords.h

        _buttonToken = SystemEventRegistrar::theInstance->registerButtonFinalClick(
            [this](int clickCount)
            {
                report();
            });
    }
    ~Globals()
    {
        SystemEventRegistrar::theInstance->unregisterButtonFinalClick(_buttonToken);
    }

public:
    void begin()
    {
        delay(500); // try to allow the Log to become ready for output (naybe no longer needed w/ wait on serial above?)

        _persistentSettings.begin();
        _systemEventRegistrar.begin();
        _networkManager.begin();
    }

    void loop()
    {
        _systemEventRegistrar.loop();
        _networkManager.loop();
    }

    void report()
    {
        INFO("--------------------------------------------");
        INFO("free memory: %lu", System.freeMemory());
        _networkManager.report();
    }
};

#endif

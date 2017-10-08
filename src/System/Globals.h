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

struct Globals
{
public:
    static Globals* theInstance;

private:
    SerialLogHandler        _logHandler;
    SystemEventRegistrar    _systemEventRegistrar;
    PersistentSettings      _persistentSettings;
    NetworkManager          _networkManager;

public:
    Globals()
    {
        theInstance = this;

        Serial.begin(115200);
        if (System.resetReason() == RESET_REASON_PANIC)
        {
            System.enterSafeMode();
        }
    }

public:
    void begin()
    {
        delay(500); // try to allow the Log to become ready for output

        _persistentSettings.begin();
        _networkManager.begin();
    }

    void loop()
    {
        _persistentSettings.loop();
        _networkManager.loop();
    }

    void report()
    {
        _persistentSettings.report();
        _networkManager.report();
    }
};

#endif

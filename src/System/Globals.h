//
// Globals.h
//
#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "SystemEventRegistrar.h"
#include "PersistentSettings.h"
#include "NetworkManager.h"
#include "Util/ElapsedTime.h"

struct Globals
{
    static Globals* theInstance;

    SerialLogHandler        logHandler;
    SystemEventRegistrar    systemEventRegistrar;
    PersistentSettings      persistentSettings;
    NetworkManager          networkManager;

    Globals()
    {
        theInstance = this;
        startup();
    }

private:
    void startup()
    {
        Serial.begin(115200);
        if (System.resetReason() == RESET_REASON_PANIC)
        {
            System.enterSafeMode();
        }
    }

public:
    void begin()
    {
        ElapsedTime::begin();
        persistentSettings.begin();
        networkManager.begin();
    }

    void loop()
    {
        ElapsedTime::loop();
        persistentSettings.loop();
        networkManager.loop();
    }

    void report()
    {
        persistentSettings.report();
        networkManager.report();
    }
};

#endif

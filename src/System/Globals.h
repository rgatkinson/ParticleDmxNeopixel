//
// Globals.h
//
#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "SystemEventRegistrar.h"
#include "NetworkManager.h"
#include "Util/ElapsedTime.h"

struct Globals
{
    SystemEventRegistrar    systemEventRegistrar;
    NetworkManager          networkManager;

    static void setup()
    {
        Serial.begin(115200);
        if (System.resetReason() == RESET_REASON_PANIC)
        {
            System.enterSafeMode();
        }
    }

    static void startup()
    {
        ElapsedTime::begin();
    }

    static void loop()
    {
        ElapsedTime::loop();
    }
};

#endif

//
// Globals.h
//
#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "System/Misc.h"
#include "System/SystemEventRegistrar.h"
#include "System/NetworkStatusMonitor.h"
#include "System/PersistentSettings.h"
#include "System/NetworkManager.h"
#include "System/CloudVariable.h"
#include "Util/ElapsedTime.h"

extern void setCredentials();   // See AfterPasswords.h

//==================================================================================================
// Globals
//==================================================================================================

struct Globals
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

public:
    static Globals* theInstance;

private:
    SerialLogHandler        _logHandler;
    int                     _buttonToken;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

public:
    Globals(NetworkManager::InitializerType applications)
    {
        theInstance = this;
        System.enableFeature(FEATURE_RESET_INFO);
        if (System.resetReason() == RESET_REASON_PANIC)
        {
            System.enterSafeMode();
        }

        Serial.blockOnOverrun(true);   // while it's just debug, it's harder to debug if we miss things
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

        NetworkManager::theInstance->setApplications(applications);
    }

public:
    ~Globals()
    {
        SystemEventRegistrar::theInstance->unregisterButtonFinalClick(_buttonToken);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

public:

    void announceLog()
    {
        delay(500); // try to allow the Log to become ready for output
        INFO("*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*");
    }

    void begin()
    {
        announceLog();
        PersistentSettings::theInstance->begin();
        SystemEventRegistrar::theInstance->begin();
        NetworkManager::theInstance->begin();
    }

    void loop()
    {
        SystemEventRegistrar::theInstance->loop();
        NetworkManager::theInstance->loop();
    }

    void report()
    {
        INFO("--------------------------------------------");
        INFO("free memory: %lu", System.freeMemory());
        NetworkManager::theInstance->report();
    }
};

inline void announceLog()
{
    Globals::theInstance->announceLog();
}

#endif

//
// Globals.cpp
//
// One struct containing all our necessary global instances in order to allow
// us to control initialization order.

#include "Particle.h"
#include "Globals.h"

//==================================================================================================
// Globals
//==================================================================================================

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(MANUAL);
STARTUP(System.enableFeature(FEATURE_RESET_INFO));

SystemEventRegistrar*   SystemEventRegistrar::theInstance = nullptr;
NetworkManager*         NetworkManager::theInstance = nullptr;
Globals                 globals;

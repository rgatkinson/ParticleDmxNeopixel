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

SystemEventRegistrar*   SystemEventRegistrar::theInstance;
PersistentSettings*     PersistentSettings::theInstance;
NetworkManager*         NetworkManager::theInstance;
Globals*                Globals::theInstance;

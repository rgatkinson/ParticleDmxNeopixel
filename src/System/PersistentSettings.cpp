//
// PersistentSettings.cpp
//
#include "Particle.h"
#include "PersistentValueSetting.h"
#include "PersistentStringSetting.h"
#include "Artnet/ArtnetDevice.h"
#include "I2c/KridaGlobals.h"
#include "I2c/KridaDimmersDevice.h"
#include "System/NetworkManager.h"

// Defined in one file so that their relative initializatio order is defined and fixed.
// That is important as it determines the order of layout in EEPROM.

decltype(NetworkManager::_appName)      NetworkManager::_appName;
decltype(NetworkManager::_sleepTimeout) NetworkManager::_sleepTimeout;
decltype(NetworkManager::_sleepLength)  NetworkManager::_sleepLength;

decltype(ArtnetDevice::_dmxUniverse) ArtnetDevice::_dmxUniverse;
decltype(ArtnetDevice::_dmxAddress)  ArtnetDevice::_dmxAddress;
decltype(ArtnetDevice::_name)        ArtnetDevice::_name;
decltype(ArtnetDevice::_description) ArtnetDevice::_description;

decltype(KridaGlobals::_percentFirst) KridaGlobals::_percentFirst;
decltype(KridaGlobals::_percentLast)  KridaGlobals::_percentLast;

decltype(KridaDimmersDevice::_dimmerCount) KridaDimmersDevice::_dimmerCount;

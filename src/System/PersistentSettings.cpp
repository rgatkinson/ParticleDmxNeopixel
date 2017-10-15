//
// PersistentSettings.cpp
//
#include "Particle.h"
#include "PersistentValueSetting.h"
#include "PersistentStringSetting.h"
#include "Artnet/ArtnetDevice.h"

decltype(ArtnetDevice::_dmxUniverse) ArtnetDevice::_dmxUniverse;
decltype(ArtnetDevice::_dmxAddress)  ArtnetDevice::_dmxAddress;
decltype(ArtnetDevice::_name)        ArtnetDevice::_name;
decltype(ArtnetDevice::_description) ArtnetDevice::_description;

//
// KridaDimmerDevice.h
//
// An Artnet device that controls zero or more Krida I2c dimmers

#ifndef __KRIDA_DIMMER_DEVICE_H__
#define __KRIDA_DIMMER_DEVICE_H__

#include "Artnet/DmxPacketConsumer.h"
#include "KridaDimmer.h"

struct KridaDimmerDevice : DmxPacketConsumer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------
    // Packets
    //----------------------------------------------------------------------------------------------

    void onDmxPacket(ArtDmxPacket& packet) override
    {
    }
};

#endif

//
// DmxPacketConsumer.h
//
#ifndef __DMX_PACKET_CONSUMER_H__
#define __DMX_PACKET_CONSUMER_H__

struct ArtnetDmxPacket;

struct DmxPacketConsumer
{
    virtual void onDmxPacket(ArtnetDmxPacket& packet) = 0;
};

#endif

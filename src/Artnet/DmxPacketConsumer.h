//
// DmxPacketConsumer.h
//
#ifndef __DMX_PACKET_CONSUMER_H__
#define __DMX_PACKET_CONSUMER_H__

struct ArtDmxPacket;

struct DmxPacketConsumer
{
    virtual void onDmxPacket(ArtDmxPacket& packet) = 0;
};

#endif

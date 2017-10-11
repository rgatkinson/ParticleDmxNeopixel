//
// Krida I2C Dimmmer
//
#ifndef __KRIDA_I2C_DIMMER_H__
#define __KRIDA_I2C_DIMMER_H__

#include "Util/Deadline.h"
#include "Artnet/Artnet.h"
#include "Artnet/DmxPacketConsumer.h"
#include "I2c/KridaI2cParameterBlock.h"
#include "KridaDimmerChannel.h"

// All the documentation we have :-( :
//
// KRIDA Electronics I2C AC Light Dimmer
//
// I2C Protocol -> first byte - device address
//                 second byte - channel select
//                 third byte - dimming value
//
// Device address -> 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20
// Device address -> 0x3F, 0x3E, 0x3D, 0x3C, 0x3B, 0x3A, 0x39, 0x38
//
// Channels -> 0x80 1st channel
//             0x81 2nd channel
//             0x82 3rd channel
//             0x83 4th channel
//
// Dimming value range 0...100
//                     0 - Fully ON
//                     100 - Fully OFF
//
struct KridaDimmer
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    static const int i2cAddressPCF8574T  = 0x20;   // '7 bit addr'
    static const int i2cAddressPCF8574AT = 0x38;   // '7 bit addr'

protected:

    static const int _channelCount = 4;
    static const int _channelAddressFirst = 0x80;
    static const int _msTransmissionInterval = 100;

    Deadline           _transmissionDeadline;
    bool               _channelDirty;
    int                _i2cAddress;
    KridaDimmerChannel _channels[_channelCount];

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    KridaDimmer(int i2cAddressBase, int addressDipSwitch)
        : _transmissionDeadline(_msTransmissionInterval),
          _channels { {this}, {this}, {this}, {this} }
    {
        Wire.setSpeed(CLOCK_SPEED_100KHZ);
        Wire.begin();
        _i2cAddress = i2cAddressBase + addressDipSwitch;
    }

    ~KridaDimmer()
    {
        Wire.end();
    }

    //----------------------------------------------------------------------------------------------
    // Transmission
    //----------------------------------------------------------------------------------------------

    void transmit()
    {
        Wire.beginTransmission(_i2cAddress);
        for (int channel = 0; channel < _channelCount; channel++)
        {
            Wire.write(_channelAddressFirst + channel);
            Wire.write(_channels[channel].value());
        }
        Wire.endTransmission();
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void noteChannelValueChange(KridaDimmerChannel* pChannel)
    {
        _channelDirty = true;
    }

    virtual void begin()
    {
        _transmissionDeadline.expire();
        _channelDirty = true;
    }

    virtual void loop()
    {
        if (_transmissionDeadline.hasExpired())
        {
            transmit();
            _transmissionDeadline.reset();
            _channelDirty = false;
        }
    }

    virtual void report()
    {
    }
};

#endif

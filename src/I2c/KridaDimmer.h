//
// Krida I2C Dimmmer
//
#ifndef __KRIDA_I2C_DIMMER_H__
#define __KRIDA_I2C_DIMMER_H__

#include "Util/Deadline.h"
#include "Util/ReferenceCounted.h"
#include "KridaDimmerChannel.h"
#include "DmxParams/DmxKridaParameters.h"

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
struct KridaDimmer : ReferenceCounted
{
    //----------------------------------------------------------------------------------------------
    // Types
    //----------------------------------------------------------------------------------------------

    enum class Type
    {
        PCF8574T = 0x20,    // '7 bit addr'
        PCF8574AT = 0x38,   // '7 bit addr'
    };

    static Type other(Type type) { return type==Type::PCF8574AT ? Type::PCF8574T : Type::PCF8574AT; }

    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    static const int _dipSwitchMax = 8;
    static const int _channelCount = 4;
    static const int _channelAddressFirst = 0x80;
    static const int _msTransmissionInterval = 100;

    Deadline            _transmissionDeadline;
    bool                _channelDirty;
    Type                _type;
    int                 _dipSwitch;
    KridaDimmerChannel* _channels[_channelCount];

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    KridaDimmer(int dipSwitch, Type type = Type::PCF8574AT)
        : _transmissionDeadline(_msTransmissionInterval),
          _channels { new KridaDimmerChannel{this}, new KridaDimmerChannel{this}, new KridaDimmerChannel{this}, new KridaDimmerChannel{this} }
    {
        _dipSwitch = dipSwitch;
        _type = type;
    }

protected:

    ~KridaDimmer() override
    {
        for (int i = 0; i < _channelCount; i++)
        {
            releaseRef(_channels[i]);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:

    int i2cAddress()
    {
        return (int)_type + (_dipSwitchMax-1 - _dipSwitch);
    }

    //----------------------------------------------------------------------------------------------
    // Transmission
    //----------------------------------------------------------------------------------------------
protected:

    static bool detect(int i2cAddress)
    {
        Wire.beginTransmission(i2cAddress);
        return Wire.endTransmission() == 0;
    }

    void detect()
    {
        // try to correct for erroneous type
        if (!detect(i2cAddress()))
        {
            _type = other(_type);
            if (!detect(i2cAddress()))
            {
                _type = other(_type);  // may as well use what he said
                WARN("KRIDA: unable to detect dimmer addr=0x%02x", i2cAddress());
            }
            else
            {
                INFO("KRIDA: detected alt-dimmer addr=0x%02x", i2cAddress());
            }
        }
        else
        {
            INFO("KRIDA: detected dimmer addr=0x%02x", i2cAddress());
        }
    }

    void transmit()
    {
        Wire.beginTransmission(i2cAddress());
        for (int channel = 0; channel < _channelCount; channel++)
        {
            Wire.write(_channelAddressFirst + channel);
            Wire.write(_channels[channel]->value());
        }
        Wire.endTransmission();
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------
public:

    void noteChannelValueChange(KridaDimmerChannel* pChannel)
    {
        _channelDirty = true;
    }

    void begin()
    {
        detect();
        _transmissionDeadline.expire();
        _channelDirty = true;
        for (int i = 0; i < _channelCount; i++)
        {
            _channels[i]->begin();
        }
    }

    void loop()
    {
        for (int i = 0; i < _channelCount; i++)
        {
            _channels[i]->loop();
        }
        if (_transmissionDeadline.hasExpired())
        {
            transmit();
            _transmissionDeadline.reset();
            _channelDirty = false;
        }
    }

    void report()
    {
        for (int i = 0; i < _channelCount; i++)
        {
            _channels[i]->report();
        }
    }

    //----------------------------------------------------------------------------------------------
    // Packets
    //----------------------------------------------------------------------------------------------

    void processDmxKridaDimmer(const DmxKridaDimmer& parameters)
    {
        for (int i = 0; i < _channelCount; i++)
        {
            _channels[i]->processDmxKridaChannel(parameters.channel(i));
        }
    }
};

#endif

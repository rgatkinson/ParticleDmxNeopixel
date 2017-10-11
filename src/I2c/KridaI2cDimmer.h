//
// Krida I2C Dimmmer
//
#ifndef __KRIDA_I2C_DIMMER_H__
#define __KRIDA_I2C_DIMMER_H__

#include "Artnet/Artnet.h"
#include "Artnet/DmxPacketConsumer.h"
#include "I2c/KridaI2cParameterBlock.h"
#include "Util/Deadline.h"

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
struct KridaI2cDimmer : DmxPacketConsumer
{
    //----------------------------------------------------------------------------------------------
    // Types
    //----------------------------------------------------------------------------------------------
protected:
    typedef int ChannelValue;
    static const ChannelValue _channelValueFirst = 0;
    static const ChannelValue _channelValueOff   = _channelValueFirst;
    static const ChannelValue _channelValueLast  = 100;

    struct Channel
    {
    private:
        KridaI2cDimmer* _pDimmer;
        ChannelValue    _channelValue;
    public:
        Channel(KridaI2cDimmer* pDimmer)
        {
            _pDimmer       = pDimmer;
            _channelValue  = _channelValueOff;
        }
        ChannelValue channelValue() const
        {
            return _channelValue;
        }
        void setChannelValue(ChannelValue channelValue)
        {
            if (_channelValue != channelValue)
            {
                _channelValue = channelValue;
                _pDimmer->noteChannelValueChange(this);
            }
        }
    };

    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    static const int i2cAddressPCF8574T  = 0x20;   // '7 bit addr'
    static const int i2cAddressPCF8574AT = 0x38;   // '7 bit addr'

protected:

    static const int _channelCount = 4;
    static const int _channelAddressFirst = 0x80;
    static const int _msTransmissionInterval = 100;

    ArtnetDevice _artnet;
    int          _i2cAddress;
    Channel      _channels[_channelCount];
    Deadline     _transmissionDeadline;
    bool         _transmissionNeeded;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    KridaI2cDimmer(int i2cAddressBase, int addressDipSwitch, LPCSTR shortName)
        : _artnet(this, DMX_ADDRESS_DEFAULT, dmxCount(), shortName),
          _transmissionDeadline(_msTransmissionInterval),
          _channels { {this}, {this}, {this}, {this} }
    {
        Wire.setSpeed(CLOCK_SPEED_100KHZ);
        Wire.begin();
        _i2cAddress = i2cAddressBase + addressDipSwitch;
    }

    ~KridaI2cDimmer()
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
            Wire.write(_channels[channel].channelValue());
        }
        Wire.endTransmission();
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void noteChannelValueChange(Channel* pChannel)
    {
        _transmissionNeeded = true;
    }

    virtual void begin()
    {
        _artnet.begin();
        _transmissionDeadline.expire();
        _transmissionNeeded = true;
    }

    virtual void loop()
    {
        _artnet.loop();
        if (_transmissionDeadline.hasExpired())
        {
            if (_transmissionNeeded)
            {
                transmit();
                _transmissionDeadline.reset();
            }
        }
    }

    virtual void report()
    {
        _artnet.report();
    }

    //----------------------------------------------------------------------------------------------
    // Packets
    //----------------------------------------------------------------------------------------------

    static int dmxCount()
    {
        return sizeof(DmxColorLuminanceParametersData);
    }

    ChannelValue channelValueFromDimmerLevel(float dimmerLevel)
    {
        return scaleRangeDiscrete(dimmerLevel, 0, 1, _channelValueFirst, _channelValueLast);
    }

    void onDmxPacket(ArtDmxPacket& packet) override
    {
        byte* dmxValuesPointer = packet.dmxValuesPointer(_artnet.dmxAddress(), dmxCount());
        if (dmxValuesPointer)
        {
            DmxKridaI2cParameters parameterBlock = DmxKridaI2cParameters(dmxValuesPointer);
            for (int channel = 0; channel < _channelCount; channel++)
            {
                float dimmerLevel = parameterBlock.channel(channel).dimmer.dimmerLevel();
                ChannelValue channelValue = channelValueFromDimmerLevel(channelValue);
                _channels[channel].setChannelValue(channelValue);
            }
        }
    }

};

#endif

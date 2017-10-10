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
    typedef int DIMMER_VALUE;

    struct Dimmer
    {
    private:

        KridaI2cDimmer* _parent;
        DIMMER_VALUE    _dimmerValue;
        int             _channelOffset;
    public:
        Dimmer(KridaI2cDimmer* parent, int channelOffset)
        {
            _parent        = parent;
            _dimmerValue   = _dimmerValueOff;
            _channelOffset = channelOffset;
        }
        int channelOffset()
        {
            return _channelOffset;
        }
        DIMMER_VALUE dimmerValue()
        {
            return _dimmerValue;
        }
        void setDimmerValue(DIMMER_VALUE dimmerValue)
        {
            if (_dimmerValue != dimmerValue)
            {
                _dimmerValue = dimmerValue;
                _parent->noteChangedDimmer();
            }
        }
    };

    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
    static const int i2cAddressPCF8574T  = 0x20;   // '7 bit addr'
    static const int i2cAddressPCF8574AT = 0x38;   // '7 bit addr'

protected:

    static const DIMMER_VALUE _dimmerValueFirst = 0;
    static const DIMMER_VALUE _dimmerValueOff   = _dimmerValueFirst;
    static const DIMMER_VALUE _dimmerValueLast  = 100;

    static const int _channelCount = 4;
    static const int _channelAddressFirst = 0x80;
    static const int _msTransmissionInterval = 500;

    bool         _transmissionNeeded;
    int          _i2cAddress;
    ArtnetDevice _artnet;
    Deadline     _deadline;
    Dimmer       _dimmers[_channelCount];

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    KridaI2cDimmer(int i2cAddressBase, int addressDipSwitch, LPCSTR shortName)
        : _artnet(this, DMX_ADDRESS_DEFAULT, shortName),
          _deadline(_msTransmissionInterval),
          _dimmers { {this,0}, {this,1}, {this,2}, {this,3} }
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
        for (int i = 0; i < _channelCount; i++)
        {
            Wire.write(_dimmers[i].channelOffset() + _channelAddressFirst);
            Wire.write(_dimmers[i].dimmerValue());
        }
        Wire.endTransmission();
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void noteChangedDimmer()
    {
        _transmissionNeeded = true;
    }

    virtual void begin()
    {
        _artnet.begin();
        _deadline.expire();
        _transmissionNeeded = true;
    }

    virtual void loop()
    {
        _artnet.loop();
        if (_deadline.hasExpired())
        {
            if (_transmissionNeeded)
            {
                transmit();
                _deadline.reset();
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

    DIMMER_VALUE dimmerValueFromDmx(byte dmx)
    {
        return scaleRangeDiscrete(dmx, 0, 255, _dimmerValueFirst, _dimmerValueLast);
    }

    void onDmxPacket(ArtDmxPacket& packet) override
    {
        byte* dmxValuesPointer = packet.dmxValuesPointer(_artnet.dmxAddress(), sizeof(KridaI2CParameterBlockData));
        if (dmxValuesPointer)
        {
            KridaI2CParameterBlock parameterBlock = KridaI2CParameterBlock(dmxValuesPointer);
            for (int channel = 0; channel < _channelCount; channel++)
            {
                DIMMER_VALUE dimmerValue = dimmerValueFromDmx(parameterBlock.dimmer(channel));
                _dimmers[channel].setDimmerValue(dimmerValue);
            }
        }
    }

};

#endif

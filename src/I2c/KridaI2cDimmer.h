//
// Krida I2C Dimmmer
//
#ifndef __KRIDA_I2C_DIMMER_H__
#define __KRIDA_I2C_DIMMER_H__

#include "Artnet/Artnet.h"
#include "Artnet/DmxPacketConsumer.h"
#include "I2c/KridaI2cParameterBlock.h"
#include "Util/Deadline.h"

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
protected:

    static const DIMMER_VALUE _dimmerValueFirst = 0;
    static const DIMMER_VALUE _dimmerValueOff   = _dimmerValueFirst;
    static const DIMMER_VALUE _dimmerValueLast  = 100;

    static const int _channelCount = 4;
    static const int _channelAddressFirst = 0x80;
    static const int _msTransmissionInterval = 500;

    bool         _transmissionNeeded;
    int          _i2cAddress = 0x3f;
    ArtnetDevice _artnet;
    Deadline     _deadline;
    Dimmer       _dimmers[_channelCount];

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    KridaI2cDimmer(LPCSTR shortName)
        : _artnet(this, DMX_ADDRESS_DEFAULT, shortName),
          _deadline(_msTransmissionInterval),
          _dimmers { {this,0}, {this,1}, {this,2}, {this,3} }
    {
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

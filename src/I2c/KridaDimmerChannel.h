//
// KridaDimmerChannel.h
//
#ifndef __KRIDA_I2C_CHANNEL_H__
#define __KRIDA_I2C_CHANNEL_H__

struct KridaDimmer;

struct KridaDimmerChannel
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    typedef int Value;
    static const Value ValueFirst = 0;
    static const Value ValueOff   = ValueFirst;
    static const Value ValueLast  = 100;

private:

    KridaDimmer* _pDimmer;
    Value        _value;

public:

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    KridaDimmerChannel(KridaDimmer* pDimmer)
    {
        _pDimmer = pDimmer;
        _value  = ValueOff;
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    Value value() const
    {
        return _value;
    }

    void setValue(Value value);
};

#endif

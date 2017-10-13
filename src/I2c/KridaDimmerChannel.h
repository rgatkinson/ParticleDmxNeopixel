//
// KridaDimmerChannel.h
//
#ifndef __KRIDA_DIMMER_CHANNEL_H__
#define __KRIDA_DIMMER_CHANNEL_H__

#include "DmxParams/DmxKridaParameters.h"
#include "Artnet/DmxLuminanceEffectSelector.h"

struct KridaDimmer;

// Dimming value range 0...100
//                     0 - Fully ON
//                     100 - Fully OFF
struct KridaDimmerChannel : ReferenceCounted, Lumenizeable
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

    typedef int Value;
    static const Value ValueFirst = 0;
    static const Value ValueOff   = ValueFirst;
    static const Value ValueLast  = 100;

private:

    KridaDimmer*                        _pParent;
    Value                               _value;
    Lumenizer*                          _pLumenizer;
    DmxLuminanceEffectSelector<false>*  _pLuminanceEffectSelector;

public:

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------

    KridaDimmerChannel(KridaDimmer* pDimmer)
    {
        _pParent = pDimmer;
        _value = ValueOff;
        _pLumenizer = nullptr;
        _pLuminanceEffectSelector = new DmxLuminanceEffectSelector<false>(this);
    }

    ~KridaDimmerChannel()
    {
        releaseRef(_pLuminanceEffectSelector);
        releaseRef(_pLumenizer);
    }

    DELEGATE_REF_COUNTING

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    void ownLumenizer(Lumenizer* pLumenizer) override
    {
        setRef(_pLumenizer, pLumenizer);
    }
    Lumenizer* lumenizer() override
    {
        return _pLumenizer;
    }

    Value value() const
    {
        return _value;
    }

    void setValue(Value value);

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin()
    {
        if (_pLumenizer)
        {
            _pLumenizer->begin();
        }
    }

    void loop()
    {
        if (_pLumenizer)
        {
            _pLumenizer->loop();
        }
    }

    void report()
    {
        if (_pLumenizer)
        {
            _pLumenizer->report();
        }
    }

    //----------------------------------------------------------------------------------------------
    // Packets
    //----------------------------------------------------------------------------------------------

    void processDmxKridaChannel(const DmxKridaChannel& parameterBlock)
    {
        _pLuminanceEffectSelector->processDmxEffectSpeedControl(parameterBlock.luminance);

        if (_pLumenizer)
        {
            _pLumenizer->processDmxDimmer(parameterBlock.dimmer);
        }
    }
};

#endif

//
// KridaDimmerChannel.h
//
#ifndef __KRIDA_DIMMER_CHANNEL_H__
#define __KRIDA_DIMMER_CHANNEL_H__

#include "DmxParams/DmxKridaParameters.h"
#include "Artnet/DmxLuminanceEffectSelector.h"
#include "I2c/KridaGlobals.h"

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
    static const Value ValueLast  = 100;
    static const Value ValueOff   = ValueFirst;
    static const Value ValueFull  = ValueLast;

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
        KridaGlobals::createInstance();
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

    Value transmissionValue() const
    {
        // In actuality, 0 is full and 100 is off. Silly. But there you have it.
        return ValueLast - _value;
    }

    void setValue(Value value);

    void setBrightness(int brightness)
    {
        if (brightness == BRIGHTNESS_OFF)
        {
            // Honor blackout
            setValue(ValueOff);
        }
        else
        {
            // Percent is of the range [ValueFirst, ValueLast]
            int percentFirst = KridaGlobals::theInstance->percentFirst();
            int percentLast  = KridaGlobals::theInstance->percentLast();

            // Sanitize
            percentFirst = max(0, min(100, percentFirst));
            percentLast  = max(0, min(100, percentLast));
            percentFirst = min(percentFirst, percentLast);
            percentLast  = max(percentFirst, percentLast);

            int valueFirst = scaleRangeDiscrete(percentFirst, 0, 100, ValueFirst, ValueLast);
            int valueLast  = scaleRangeDiscrete(percentLast,  0, 100, ValueFirst, ValueLast);

            Value value = scaleRangeDiscrete(brightness, BRIGHTNESS_OFF, BRIGHTNESS_FULL, valueFirst, valueLast);
            setValue(value);
        }
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin()
    {
        KridaGlobals::theInstance->begin();
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
            BRIGHTNESS brightness = _pLumenizer->currentBrightness(0, false);
            setBrightness(brightness);
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

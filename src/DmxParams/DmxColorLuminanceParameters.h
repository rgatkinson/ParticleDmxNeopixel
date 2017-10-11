//
// DmxColorLuminanceParameters.h
//
#ifndef __COLOR_LUMINANCE_PARAMTER_BLOCK_H__
#define __COLOR_LUMINANCE_PARAMTER_BLOCK_H__

#include "DmxEffectSpeedControl.h"
#include "DmxRedGreenBlue.h"
#include "DmxDimmer.h"

struct PACKED DmxColorLuminanceParametersData
{
    byte                    colorTemperature;       // 300
    DmxRedGreenBlue         redGreenBlue;           // 301-303
    DmxDimmer               dimmer;                 // 304
    DmxEffectSpeedControl   color;                  // 305, 306, 307
    DmxEffectSpeedControl   luminance;              // 308, 309, 310
};

struct DmxColorLuminanceParameters
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    DmxColorLuminanceParametersData* _pData;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    DmxColorLuminanceParameters(void* pv)
    {
        _pData = reinterpret_cast<DmxColorLuminanceParametersData*>(pv);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:

    byte colorTemperature() const { return _pData->colorTemperature; }

    byte red() const              { return _pData->redGreenBlue.red(); }
    byte green() const            { return _pData->redGreenBlue.green(); }
    byte blue() const             { return _pData->redGreenBlue.blue(); }

    const DmxDimmer& dimmer() const { return _pData->dimmer; }

    byte colorEffect() const      { return _pData->color.effect(); }
    byte colorSpeed() const       { return _pData->color.speed(); }
    byte colorControl() const     { return _pData->color.control(); }

    byte luminanceEffect() const  { return _pData->luminance.effect(); }
    byte luminanceSpeed() const   { return _pData->luminance.speed(); }
    byte luminanceControl() const { return _pData->luminance.control(); }

    float colorSpeedLevel(bool directional = true) const
    {
        return _pData->color.speedLevel(directional);
    }

    float luminanceSpeedLevel(bool directional = true) const
    {
        return _pData->luminance.speedLevel(directional);
    }

public:

    // color temperature zero allows rgb
    // otherwise, color temperature max is the sweet spot
    // othewise, we interpolate color temperature
    COLOR_INT effectiveColor() const
    {
        const byte dmxColorTemperature = colorTemperature();
        const float kelvinSweet = 2550;   // soft white incandesent

        if (dmxColorTemperature == 0)
        {
            return _pData->redGreenBlue.color();
        }
        else if (dmxColorTemperature == 255)
        {
            return Color::temperature(kelvinSweet);
        }
        else
        {
            // 1-254 map to kelvin. We use a uniform distribution from first to last,
            // but have a little plateau there in the middle where the warm sweet spot lies
            // https://en.wikipedia.org/wiki/Color_temperature

            const float kelvinFirst = 1700;   // a flame from a match
            const float kelvinLast = 6500;    // daylight
            const float kelvinRange = kelvinLast - kelvinFirst + 1;
            const float sweetFraction = (kelvinSweet - kelvinFirst) / kelvinRange;

            const int dmxFirst = 1;
            const int dmxLast  = 254;
            const int dmxRange = dmxLast - dmxFirst + 1;

            const int dmxPlateauLength = 5;
            const int dmxPlateauFirst  = int(sweetFraction * (dmxRange - dmxPlateauLength));
            const int dmxPlateauMax    = dmxPlateauFirst + dmxPlateauLength;

            float kelvin;

            if (dmxColorTemperature < dmxPlateauFirst)
            {
                kelvin = scaleRange(dmxColorTemperature, dmxFirst, dmxPlateauFirst-1, kelvinFirst, kelvinSweet);
            }
            else if (dmxColorTemperature < dmxPlateauMax)
            {
                kelvin = kelvinSweet;
            }
            else
            {
                kelvin = scaleRange(dmxColorTemperature, dmxPlateauMax, dmxLast, kelvinSweet, kelvinLast);
            }

            return Color::temperature(kelvin);
        }
    }
};

#endif

//
// DmxColorLuminanceParameters.h
//
#ifndef __COLOR_LUMINANCE_PARAMTER_BLOCK_H__
#define __COLOR_LUMINANCE_PARAMTER_BLOCK_H__

#include "Util/Misc.h"

struct PACKED DmxEffectSpeedControl
{
    byte    effect;
    byte    speed;
    byte    control;
};

struct PACKED DmxRedGreenBlue
{
    byte    _red;
    byte    _green;
    byte    _blue;

    byte red()   const { return _red;   }
    byte green() const { return _green; }
    byte blue()  const { return _blue;  }

    COLOR_INT color() const { return Color::rgb(red(), green(), blue()); }
};

struct PACKED DmxDimmer
{
    byte _value;

    float dimmerLevel() const
    {
        return scaleRange(_value, 0, 255, 0, 1);
    }
};

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

    byte colorTemperature() { return _pData->colorTemperature; }

    byte red()              { return _pData->redGreenBlue.red(); }
    byte green()            { return _pData->redGreenBlue.green(); }
    byte blue()             { return _pData->redGreenBlue.blue(); }

    const DmxDimmer& dimmer()   { return _pData->dimmer; }

    byte colorEffect()      { return _pData->color.effect; }
    byte colorSpeed()       { return _pData->color.speed; }
    byte colorControl()     { return _pData->color.control; }

    byte luminanceEffect()  { return _pData->luminance.effect; }
    byte luminanceSpeed()   { return _pData->luminance.speed; }
    byte luminanceControl() { return _pData->luminance.control; }

    float colorSpeedLevel(bool directional = true)
    {
        return speedLevel(directional, colorSpeed());
    }

    float luminanceSpeedLevel(bool directional = true)
    {
        return speedLevel(directional, luminanceSpeed());
    }

    // directional:     [-1,1], neg=CW, pos=CCW
    // non-directional: [0,1]
    // zero is stopped
    static float speedLevel(bool directional, byte dmx)
    {
        if (dmx==0)
        {
            return 0;
        }
        else
        {
            const int dmxFirst  = 1;
            const int dmxMid    = 128;
            const int dmxLast   = 255;
            const int dmxMax    = dmxLast + 1;

            if (directional)
            {
                const int dmxPlateauLength = 5;
                const int dmxPlateauFirst  = dmxMid - dmxPlateauLength / 2;
                const int dmxPlateauLast   = dmxMid + dmxPlateauLength / 2;
                const int dmxPlateauMax    = dmxPlateauLast + 1;
                if (dmx < dmxPlateauFirst)
                {
                    // We want [-1,0), so invert
                    dmx = dmxPlateauFirst - 1 - (dmx - dmxFirst);
                    return -fraction(dmx, dmxFirst, dmxPlateauFirst);
                }
                else if (dmx <= dmxPlateauLast)
                {
                    // on the plateau
                    return 0;
                }
                else
                {
                    // (0,1]
                    return fraction(dmx, dmxPlateauMax, dmxMax);
                }
            }
            else
            {
                return scaleRange(dmx, dmxFirst, dmxLast, 0, 1);
            }
        }
    }

    static inline float fraction(float position, float first, float max)
    {
        return (position - first + 1) / (max - first);
    }

public:

    // color temperature zero allows rgb
    // otherwise, color temperature max is the sweet spot
    // othewise, we interpolate color temperature
    COLOR_INT effectiveColor()
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

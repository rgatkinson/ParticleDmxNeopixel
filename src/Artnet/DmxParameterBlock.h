//
// DmxParameterBlock.h
//
#ifndef __DMX_PARAMTER_BLOCK_H__
#define __DMX_PARAMTER_BLOCK_H__

#include "Util/Misc.h"

struct PACKED DmxParameterBlockData
{
    byte    colorTemperature;
    byte    red;
    byte    green;
    byte    blue;
    byte    dimmer;
    byte    colorEffect;
    byte    colorSpeed;
    byte    colorControl;
    byte    luminanceEffect;
    byte    luminanceSpeed;
    byte    luminanceControl;
};

struct DmxParameterBlock
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    DmxParameterBlockData* _pData;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    DmxParameterBlock(void* pv)
    {
        _pData = reinterpret_cast<DmxParameterBlockData*>(pv);
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:

    #define declare(memberName)  inline byte memberName() { return _pData->memberName; }

    declare(colorTemperature)
    declare(red)
    declare(green)
    declare(blue)
    declare(dimmer)
    declare(colorEffect)
    declare(colorSpeed)
    declare(colorControl)
    declare(luminanceEffect)
    declare(luminanceSpeed)
    declare(luminanceControl)

    #undef declare

    float colorSpeedLevel()
    {
        return speedLevel(colorSpeed());
    }

    float luminanceSpeedLevel()
    {
        return speedLevel(luminanceSpeed());
    }

private:

    static float speedLevel(byte dmx)   // [-1,1], neg=CW, pos=CCW
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
            const int dmxPlateauLength = 5;
            const int dmxPlateauFirst  = dmxMid - dmxPlateauLength / 2;
            const int dmxPlateauLast   = dmxMid + dmxPlateauLength / 2;
            const int dmxPlateauMax    = dmxPlateauLast + 1;
            if (dmx < dmxPlateauFirst)
            {
                // We want fast->slow, so invert
                dmx = dmxPlateauFirst - 1 - (dmx - dmxFirst);
                return -fraction(dmx, dmx, dmxPlateauFirst);
            }
            else if (dmx <= dmxPlateauLast)
            {
                return 0;
            }
            else
            {
                // slow->fast
                return fraction(dmx, dmxPlateauMax, dmxMax);
            }
        }
    }

    static inline float fraction(float dmx, float first, float max)
    {
        return (dmx - first + 1) / (max - first);
    }

public:

    COLOR_INT effectiveColor()
    {
        const byte dmxColorTemperature = colorTemperature();
        const float kelvinSweet = 2550;   // soft white incandesent

        if (dmxColorTemperature == 0)
        {
            return Color::rgb(red(), green(), blue());
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

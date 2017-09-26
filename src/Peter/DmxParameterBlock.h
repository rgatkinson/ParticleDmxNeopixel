//
// DmxParameterBlock.h
//
#ifndef __DMX_PARAMTER_BLOCK_H__
#define __DMX_PARAMTER_BLOCK_H__

#include "Util/Misc.h"

struct PACKED DmxParameterBlockData
{
    byte    red;
    byte    green;
    byte    blue;
    byte    dimmerBrightness;
    byte    colorEffect;
    byte    colorSpeed;
    byte    colorControl;
    byte    brightnessEffect;
    byte    brightnessSpeed;
    byte    brightnessControl;
    byte    colorTemperature;
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

    declare(red)
    declare(green)
    declare(blue)
    declare(dimmerBrightness)
    declare(colorEffect)
    declare(colorSpeed)
    declare(colorControl)
    declare(brightnessEffect)
    declare(brightnessSpeed)
    declare(brightnessControl)
    declare(colorTemperature)

    #undef declare

    COLOR_INT effectiveColor()
    {
        if (colorTemperature() == 0)
        {
            return Color::rgb(red(), green(), blue());
        }
        else
        {
            // 1-255 map to kelvin. We use a uniform distribution from first to last,
            // but have a little plateau there in the middle where the warm sweet spot
            // lies
            // https://en.wikipedia.org/wiki/Color_temperature

            constexpr float kelvinFirst = 1700;   // a flame from a match
            constexpr float kelvinSweet = 2550;   // soft white incandesent
            constexpr float kelvinLast = 6500;     // daylight
            constexpr float kelvinRange = kelvinLast - kelvinFirst + 1;
            constexpr float plateauFraction = (kelvinSweet - kelvinFirst) / kelvinRange;

            constexpr int dmxFirst = 1;
            constexpr int dmxLast  = 255;
            constexpr int dmxRange = dmxLast - dmxFirst; + 1;

            constexpr int dmxPlateauLength = 4;

            float kelvin = 2550;
            return Color::temperature(kelvin);
        }
    }
};

#endif

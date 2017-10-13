//
// DmxEffectSpeedControl.h
//
#ifndef __DMX_EFFECT_SPEED_CONTROL_H__
#define __DMX_EFFECT_SPEED_CONTROL_H__

#include "Util/Misc.h"

struct PACKED DmxEffectSpeedControl
{
protected:

    byte    _effect;
    byte    _speed;
    byte    _control;

    static const int _dmxFirst  = 1;
    static const int _dmxMid    = 128;
    static const int _dmxLast   = 255;
    static const int _dmxMax    = _dmxLast + 1;

public:

    byte effect()  const { return _effect;  }
    byte speed()   const { return _speed;   }
    byte control() const { return _control; }

    // directional:     [-1,1], neg=CW, pos=CCW
    // non-directional: [0,1]
    // zero is stopped
    float speedLevel(bool directional = true) const
    {
        return directional
            ? directionalSpeedLevel(speed())
            : nonDirectionalSpeedLevel(speed());
    }

    static float directionalSpeedLevel(byte dmx)
    {
        if (dmx==0)
        {
            return 0;
        }
        else
        {
            const int dmxPlateauLength = 5;
            const int dmxPlateauFirst  = _dmxMid - dmxPlateauLength / 2;
            const int dmxPlateauLast   = _dmxMid + dmxPlateauLength / 2;
            const int dmxPlateauMax    = dmxPlateauLast + 1;
            if (dmx < dmxPlateauFirst)
            {
                // We want [-1,0), so invert
                dmx = dmxPlateauFirst - 1 - (dmx - _dmxFirst);
                return -fraction(dmx, _dmxFirst, dmxPlateauFirst);
            }
            else if (dmx <= dmxPlateauLast)
            {
                // on the plateau
                return 0;
            }
            else
            {
                // (0,1]
                return fraction(dmx, dmxPlateauMax, _dmxMax);
            }
        }
    }

    static float nonDirectionalSpeedLevel(byte dmx)
    {
        if (dmx==0)
        {
            return 0;
        }
        else
        {
            return scaleRange(dmx, _dmxFirst, _dmxLast, 0, 1);
        }
    }

protected:
    static float fraction(float position, float first, float max)
    {
        return (position - first + 1) / (max - first);
    }
};

#endif

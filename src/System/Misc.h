//
// System/Misc.h
//
#ifndef __SYSTEM_MISC_H__
#define __SYSTEM_MISC_H__

#include "Util/Misc.h"

extern void resetSleepTimer(LPCSTR);
extern void announceLog();

inline LPCSTR nameOfPin(int pin)
{
    switch (pin)
    {
        #define PIN(pin) case pin: return #pin;
        PIN(D0)
        PIN(D1)
        PIN(D2)
        PIN(D3)
        PIN(D4)
        PIN(D5)
        PIN(D6)
        PIN(D7)

        PIN(A0)
        PIN(A1)
        PIN(A2)
        PIN(A3)
        PIN(A4)
        PIN(A5)
        PIN(A6)
        PIN(A7)

        PIN(RX)
        PIN(TX)
        #undef PIN
        default: return "<unknown>";
    }
}

#endif

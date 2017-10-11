//
// DmxRedGreenBlue.h
//
#ifndef __DMX_RED_GREEN_BLUE_H__
#define __DMX_RED_GREEN_BLUE_H__

struct PACKED DmxRedGreenBlue
{
    byte    _red;
    byte    _green;
    byte    _blue;

    byte red()   const { return _red;   }
    byte green() const { return _green; }
    byte blue()  const { return _blue;  }

    Color color() const
    {
        return Color::rgb(red(), green(), blue());
    }
};

#endif

//
// Color.h
//
#ifndef __COLOR_H_
#define __COLOR_H_

typedef int COLOR_INT;

struct Color
{
    static const COLOR_INT BLACK       = 0xFF000000;
    static const COLOR_INT DKGRAY      = 0xFF444444;
    static const COLOR_INT GRAY        = 0xFF888888;
    static const COLOR_INT LTGRAY      = 0xFFCCCCCC;
    static const COLOR_INT WHITE       = 0xFFFFFFFF;
    static const COLOR_INT RED         = 0xFFFF0000;
    static const COLOR_INT GREEN       = 0xFF00FF00;
    static const COLOR_INT BLUE        = 0xFF0000FF;
    static const COLOR_INT YELLOW      = 0xFFFFFF00;
    static const COLOR_INT CYAN        = 0xFF00FFFF;
    static const COLOR_INT MAGENTA     = 0xFFFF00FF;
    static const COLOR_INT TRANSPARENT = 0;

    static int alpha(COLOR_INT color)
    {
        return (color >> 24) & 0xFF;
    }
    static int red(COLOR_INT color)
    {
        return (color >> 16) & 0xFF;
    }
    static int green(COLOR_INT color)
    {
        return (color >> 8) & 0xFF;
    }
    static int blue(COLOR_INT color)
    {
        return (color >> 0) & 0xFF;
    }

    static COLOR_INT rgb(int red, int green, int blue)
    {
        return argb(0xFF, red, green, blue);
    }
    static COLOR_INT argb(int alpha, int red, int green, int blue)
    {
        return (alpha << 24) | (red << 16) | (green << 8) | blue;
    }
};

#endif

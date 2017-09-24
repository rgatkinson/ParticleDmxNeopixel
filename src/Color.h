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
        return (clip(alpha) << 24) | (clip(red) << 16) | (clip(green) << 8) | clip(blue);
    }

    static COLOR_INT rgb(float red, float green, float blue)
    {
        return argb(1.0f, red, green, blue);
    }
    static COLOR_INT argb(float alpha, float red, float green, float blue)
    {
        return argb(asByte(alpha), asByte(red), asByte(green), asByte(blue));
    }

    static COLOR_INT temperature(float kelvin)
    {
        return rgb(blackBodyRed(kelvin), blackBodyGreen(kelvin), blackBodyBlue(kelvin));
    }

protected:
    static int clip(int b)
    {
        return min(255, max(0, b));
    }
    static int asByte(float f)
    {
        return (int)(f * 255.0f + 0.5f);
    }

    // black body curve pieces fitted to Mathematica's ColorData["BlackBodySpectrum"] function

    static float blackBodyRed(float kelvin)
    {
        if (kelvin <= 6500.f)
        {
            return 1;
        }
        else if (kelvin <= 10000.f)
        {
            kelvin -= 6500.f;
            const float a = 3.11973f;
            const float b = -0.000467144f;
            const float c = 2.16043e-8f;
            return ((c * kelvin) + b) * kelvin + a;
        }
    }

    static float blackBodyGreen(float kelvin)
    {
        if (kelvin < 1000.f)
        {
            return 0;
        }
        else if (kelvin <= 6500.f)
        {
            kelvin -= 1000.f;
            const float a = -0.00206477f;
            const float b = 0.000259848f;
            const float c = -1.62618e-8f;
            return ((c * kelvin) + b) * kelvin + a;
        }
        else
        {
            kelvin -= 6500.f;
            const float a = 0.938026f;
            const float b = -0.000107892f;
            const float c = 1.10377e-8f;
            return ((c * kelvin) + b) * kelvin + a;
        }
    }

    static float blackBodyBlue(float kelvin)
    {
        if (kelvin <= 1500.f)
        {
            return 0;
        }
        else if (kelvin <= 6500.f)
        {
            kelvin -= 1500.f;
            const float a = -0.0026023f;
            const float b = 5.49685e-6f;
            const float c = 7.40119e-8f;
            const float d =-7.13431e-12f;
            return ((((d * kelvin) + c) * kelvin) + b) * kelvin + a;
        }
        else if (kelvin <= 7000.f)
        {
            kelvin -= 6500.f;
            const float a = 0.988f;
            const float b = 0.000024f;
            return b * kelvin + a;
        }
        else
        {
            return 1;
        }
    }

};

#endif

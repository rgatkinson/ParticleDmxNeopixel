//
// Color.h
//
#ifndef __COLOR_H_
#define __COLOR_H_

#include "math.h"

typedef int COLOR_INT;

struct Color
{
    //----------------------------------------------------------------------------------------------
    // Public function
    //----------------------------------------------------------------------------------------------

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

    static inline int alpha(COLOR_INT color)
    {
        return (color >> 24) & 0xFF;
    }
    static inline int red(COLOR_INT color)
    {
        return (color >> 16) & 0xFF;
    }
    static inline int green(COLOR_INT color)
    {
        return (color >> 8) & 0xFF;
    }
    static inline int blue(COLOR_INT color)
    {
        return (color >> 0) & 0xFF;
    }


    static inline COLOR_INT rgb(int red, int green, int blue)
    {
        return argb(0xFF, red, green, blue);
    }
    static COLOR_INT argb(int alpha, int red, int green, int blue)
    {
        return (clipByte(alpha) << 24) | (clipByte(red) << 16) | (clipByte(green) << 8) | clipByte(blue);
    }


    static inline COLOR_INT rgb(float red, float green, float blue)
    {
        return argb(1.0f, red, green, blue);
    }
    static COLOR_INT argb(float alpha, float red, float green, float blue)
    {
        return argb(zeroOneToByte(alpha), zeroOneToByte(red), zeroOneToByte(green), zeroOneToByte(blue));
    }


    static COLOR_INT temperature(float kelvin)
    {
        return rgb(blackBodyRed(kelvin), blackBodyGreen(kelvin), blackBodyBlue(kelvin));
    }


    static inline COLOR_INT wheelTri(float f)
    {
        return wheel<triUp, triDown>(f);
    }

    static inline COLOR_INT wheelSin(float f)
    {
        return wheel<sinUp, sinDown>(f);
    }

    //----------------------------------------------------------------------------------------------
    // Internal
    //----------------------------------------------------------------------------------------------

protected:

    typedef float (*PFN)(float);

    template <PFN pfn>
    inline static byte scalePfn(float f, float scale)
    {
        return zeroOneToByte(scale * pfn(f));
    }

    //----------------------------------------------------------------------------------------------
    // Color Wheel
    //----------------------------------------------------------------------------------------------

    // For a given (fractional) angle around the color wheel, returns
    // the color associated therewith. Red is at 0 (and 1); green is
    // at 1/3, and blue is at 2/3.
    template <PFN pfnUp, PFN pfnDown>
    static COLOR_INT wheel(float f)
    {
        constexpr float oneThird = 1.f / 3.f;
        constexpr float twoThirds = 2.f / 3.f;
        constexpr float threeHalves = 1.5f;

        // Red appears less intense; we can compensate
        constexpr float redScale = 1.0f;
        constexpr float greenScale = 1.0f;
        constexpr float blueScale = 1.0f;

        f -= floorf(f); // remove integer part

        if (f <= oneThird)
        {
            return Color::rgb(
                scalePfn<pfnDown>((f + oneThird) * threeHalves, redScale),
                scalePfn<pfnUp>(f * threeHalves, greenScale),
                0);
        }
        else if (f <= twoThirds)
        {
            return Color::rgb(
                0,
                scalePfn<pfnDown>(f * threeHalves, greenScale),
                scalePfn<pfnUp>((f-oneThird) * threeHalves, blueScale));
        }
        else
        {
            return Color::rgb(
                scalePfn<pfnUp>((f - twoThirds) * threeHalves, redScale),
                0,
                scalePfn<pfnDown>((f-oneThird) * threeHalves, blueScale));
        }
    }

    //-----------------------------------------------------------------------
    // Triangle height as a function of fraction across the base
public:
    inline static float tri(float f, float triangleBase)
    {
        return tri(f / triangleBase);
    }
    inline static float triUp(float f, float triangleBase)
    {
        return triUp(f / triangleBase);
    }
    inline static float triDown(float f, float triangleBase)
    {
        return triDown(f / triangleBase);
    }

    inline static float tri(float f)
    {
        return f <= 0.5f
            ? triUp(f)
            : triDown(f);
    }
    inline static float triUp(float f)
    {
        return f + f;
    }
    inline static float triDown(float f)
    {
        float result = (1 - f);
        return result + result;
    }

    //-----------------------------------------------------------------------
    // Similar, but for a sinusoidial shape instead of linear triangle sides

    inline static float sinUp(float f)
    {
        float result = sinf(f * PiF);
        return result * result;
    }
    inline static float sinDown(float f)
    {
        return sinUp(f);
    }

    //----------------------------------------------------------------------------------------------
    // Black body radiation
    //----------------------------------------------------------------------------------------------

    static float blackBodyRed(float kelvin)
    {
        const float r0 = 6500;
        if (kelvin <= r0)
        {
            return 1;
        }
        else
        {
            kelvin -= r0;
            const float a = 1.0018;
            const float b = -0.000191353;
            const float c = 2.27577e-8;
            const float d = 0;
            return ((((d * kelvin) + c) * kelvin) + b) * kelvin + a;
        }
    }

    static float blackBodyGreen(float kelvin)
    {
        const float g0 = 1000;
        const float g2 = 6600;
        if (kelvin <= g2)
        {
            kelvin -= g0;
            const float a = 0.0374249;
            const float b = 0.00022693;
            const float c = -7.10085e-9;
            const float d = -7.87308e-13;
            return ((((d * kelvin) + c) * kelvin) + b) * kelvin + a;
        }
        else
        {
            kelvin -= g2;
            const float a = 0.945357;
            const float b = -0.000121171;
            const float c = 2.21965e-8;
            const float d = -2.03433e-12;
            return ((((d * kelvin) + c) * kelvin) + b) * kelvin + a;
        }
    }

    static float blackBodyBlue(float kelvin)
    {
        const float b1 = 1900;
        const float b2 = 6600;
        if (kelvin <= b1)
        {
            return 0;
        }
        else if (kelvin <= b2)
        {
            kelvin -= b1;
            const float a = -0.0058959;
            const float b = 0.0000885465;
            const float c = 5.48613e-8;
            const float d = -5.96726e-12;
            return ((((d * kelvin) + c) * kelvin) + b) * kelvin + a;
        }
        else
        {
            return 1;
        }
    }

};

#endif

//
// Color.h
//
#ifndef __COLOR_H_
#define __COLOR_H_

#include "math.h"

struct Color
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    int _value;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    Color() : Color(0)
    {
    }
    explicit Color(int color) : _value(color)
    {
    }

    Color& operator=(const Color& them)
    {
        _value = them.value();
        return *this;
    }

    //----------------------------------------------------------------------------------------------
    // Public function
    //----------------------------------------------------------------------------------------------

    int value() const
    {
        return _value;
    }

    static const Color Black, DarkGray, Gray, LightGray, White, Red, Green, Blue, Yellow, Cyan, Magenta, Transparent;

    int alpha() { return (_value >> 24) & 0xFF; }
    int red()   { return (_value >> 16) & 0xFF; }
    int green() { return (_value >>  8) & 0xFF; }
    int blue()  { return (_value >>  0) & 0xFF; }

    static Color rgb(int red, int green, int blue)
    {
        return argb(0xFF, red, green, blue);
    }
    static Color argb(int alpha, int red, int green, int blue)
    {
        return Color((clipByte(alpha) << 24) | (clipByte(red) << 16) | (clipByte(green) << 8) | clipByte(blue));
    }

    static inline Color rgb(float red, float green, float blue)
    {
        return argb(1.0f, red, green, blue);
    }
    static Color argb(float alpha, float red, float green, float blue)
    {
        return argb(zeroOneToByte(alpha), zeroOneToByte(red), zeroOneToByte(green), zeroOneToByte(blue));
    }

    static Color temperature(float kelvin)
    {
        return rgb(blackBodyRed(kelvin), blackBodyGreen(kelvin), blackBodyBlue(kelvin));
    }

    static inline Color wheelTri(float f)
    {
        return wheel<triWaveUp, triWaveDown>(f);
    }

    static inline Color wheelSin(float f)
    {
        return wheel<sinWaveUp, sinWaveDown>(f);
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
    static Color wheel(float f)
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

//----------------------------------------------------------------------------------------------
// Comparision
//----------------------------------------------------------------------------------------------

static inline bool operator==(const Color& left, const Color& right)
{
    return left.value() == right.value();
}
static inline bool operator!=(const Color& left, const Color& right)
{
    return left.value() != right.value();
}

//----------------------------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------------------------

const Color SELECTANY Color::Black      { 0xFF000000 };
const Color SELECTANY Color::DarkGray   { 0xFF444444 };
const Color SELECTANY Color::Gray       { 0xFF888888 };
const Color SELECTANY Color::LightGray  { 0xFFCCCCCC };

const Color SELECTANY Color::White      { 0xFFFFFFFF };
const Color SELECTANY Color::Red        { 0xFFFF0000 };
const Color SELECTANY Color::Green      { 0xFF00FF00 };
const Color SELECTANY Color::Blue       { 0xFF0000FF };

const Color SELECTANY Color::Yellow     { 0xFFFFFF00 };
const Color SELECTANY Color::Cyan       { 0xFF00FFFF };
const Color SELECTANY Color::Magenta    { 0xFFFF00FF };
const Color SELECTANY Color::Transparent { 0 };

#endif

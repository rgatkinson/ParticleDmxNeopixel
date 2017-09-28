//
// Misc.h
//
#ifndef _MISC_H_
#define _MISC_H_

#include <math.h>

//--------------------------------------------------------------------------------------------------
// Misc
//--------------------------------------------------------------------------------------------------

typedef long long Int64;

// https://gcc.gnu.org/onlinedocs/gcc-4.0.2/gcc/Type-Attributes.html
#define PACKED __attribute__((__packed__))

//--------------------------------------------------------------------------------------------------
// Logging / tracing
//--------------------------------------------------------------------------------------------------

// https://gcc.gnu.org/onlinedocs/gcc-4.4.2/gcc/Diagnostic-Pragmas.html
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#pragma GCC diagnostic error   "-Wreturn-type"
// #pragma GCC diagnostic error "-Wsuggest-override" // not in the version of GCC we use, I think

#undef TRACE
#undef INFO
#undef WARN
#undef ERROR

#define TRACE(fmt, ...)      Log.trace(fmt, ##__VA_ARGS__)
#define INFO(fmt, ...)       Log.info(fmt, ##__VA_ARGS__)
#define WARN(fmt, ...)       Log.warn(fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...)      Log.error(fmt, ##__VA_ARGS__)

//--------------------------------------------------------------------------------------------------
// Memory
//--------------------------------------------------------------------------------------------------

inline void* mallocNoFail(size_t cb)
{
    void* result = malloc(cb);
    if (result == NULL)
    {
        ERROR("out of memory");
        System.enterSafeMode();
    }
    return result;
}

//--------------------------------------------------------------------------------------------------
// Math / numerics
//--------------------------------------------------------------------------------------------------

static const float Pi  = 3.1415926535897932384626433832795;
static const float PiF = 3.1415926535897932384626433832795f;
static const float TwoPi  = 2.0 * Pi;
static const float TwoPiF = 2.0f * PiF;

template<typename T, T _first, T _last>
inline T clip(T t)
{
    if (t < _first) return _first;
    if (t > _last)  return _last;
    return t;
}

template<typename T>
inline T clip(T t, T _first, T _last)
{
    if (t < _first) return _first;
    if (t > _last)  return _last;
    return t;
}

template <typename T>
inline T clipByte(T t)
{
    return clip<T, T(0), T(255)>(t);
}


inline int zeroOneToInt(float f, int iMax)
{
    // [0-1) should clearly be [0-iMax)
    // Additionally, we assign the pesky 1.0f ALSO to iMax-1
    return clip((int)(f * iMax), 0, iMax-1);
}

inline int zeroOneToByte(float f)
{
    // [0-1) should clearly be [0-255]
    // Additionally, we assign the pesky 1.0f ALSO to 255
    return clipByte(int(f * 256.0f));
}


 /**
   * Scale a number in the range of fromFirst to fromLast, to the range of toFirst to toLast
   * @param from number to scale
   * @param fromFirst lower bound range of value
   * @param fromLast upper bound range of value
   * @param toFirst lower bound of scale
   * @param toLast upper bound of scale
   * @return a float scaled to a value between toFirst and toLast, inclusive
   */
inline float scaleRange(float from, float fromFirst, float fromLast, float toFirst, float toLast)
{
    const float a = (toFirst-toLast) / (fromFirst-fromLast);
    const float b = toFirst - fromFirst*a;
    return a*from + b;
}

//--------------------------------------------------------------------------------------------------
// Ups & downs
//--------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------
// Triangle height as a function of fraction across the base

inline float triWaveUp(float f)
{
    return f + f;
}
inline float triWaveDown(float f)
{
    float result = (1 - f);
    return result + result;
}
inline float triWave(float f)
{
    return f <= 0.5f
        ? triWaveUp(f)
        : triWaveDown(f);
}

inline  float triWaveUp(float f, float triangleBase)
{
    return triWaveUp(f / triangleBase);
}
inline float triWaveDown(float f, float triangleBase)
{
    return triWaveDown(f / triangleBase);
}
inline float triWave(float f, float triangleBase)
{
    return triWave(f / triangleBase);
}

//-----------------------------------------------------------------------
// Similar, but for a sinusoidial shape instead of linear triangle sides

inline float sinWaveUp(float f)
{
    float result = sinf(f * PiF);
    return result * result;
}
inline float sinWaveDown(float f)
{
    return sinWaveUp(f);
}
inline float sinWave(float f)
{
    return sinWaveUp(f);    // up and down are same
}

inline  float sinWaveUp(float f, float triangleBase)
{
    return sinWaveUp(f / triangleBase);
}
inline float sinWaveDown(float f, float triangleBase)
{
    return sinWaveDown(f / triangleBase);
}
inline float sinWave(float f, float triangleBase)
{
    return sinWave(f / triangleBase);
}



//--------------------------------------------------------------------------------------------------
// Strings
//--------------------------------------------------------------------------------------------------

typedef char* LPSTR;
typedef const char* LPCSTR;

inline void safeStrNCpy(char*rgchBuffer, int cchBuffer, LPCSTR sz)
{
    const char* pchFrom = &sz[0];
    char* pchTo         = &rgchBuffer[0];
    char* pchToLast     = &rgchBuffer[cchBuffer-1];
    while (pchTo < pchToLast)
    {
        if ('\0' == *pchFrom) break;
        *pchTo++ = *pchFrom++;
    }
    while (pchTo <= pchToLast)
    {
        *pchTo++ = '\0';
    }
}

inline void strfree(LPSTR& sz)
{
    free(sz);
    sz = NULL;
}

inline LPSTR strdup(LPCSTR sz)
{
    if (sz == NULL)
    {
        return NULL;
    }
    else
    {
        int cch = strlen(sz);
        LPSTR result = (LPSTR)mallocNoFail(cch + 1);
        strcpy(result, sz);
        return result;
    }
}

inline void setString(LPSTR& variable, LPCSTR sz)
{
    strfree(variable);
    variable = strdup(sz);
}


//--------------------------------------------------------------------------------------------------
// Assert
//--------------------------------------------------------------------------------------------------

#define assert(value) ((value) ? 0 : (assertFailed(__FILE__, __LINE__), 0))

inline void assertFailed(LPCSTR file, int line)
{
    Serial.printf("(%s,%d): assertion failed\r\n", file, line);
    System.enterSafeMode();
}

#endif

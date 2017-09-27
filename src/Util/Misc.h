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
inline int clip(T t)
{
    if (t < _first) return _first;
    if (t > _last)  return _last;
    return t;
}

template<typename T>
inline int clip(T t, T _first, T _last)
{
    if (t < _first) return _first;
    if (t > _last)  return _last;
    return t;
}

template <typename T>
inline int clipByte(T t)
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
   * Scale a number in the range of x1 to x2, to the range of y1 to y2
   * @param value number to scale
   * @param x1 lower bound range of value
   * @param x2 upper bound range of value
   * @param y1 lower bound of scale
   * @param y2 upper bound of scale
   * @return a float scaled to a value between y1 and y2, inclusive
   */
inline float scaleRange(float value, float x1, float x2, float y1, float y2)
{
    float a = (y1-y2)/(x1-x2);
    float b = y1 - x1*(y1-y2)/(x1-x2);
    return a*value+b;
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

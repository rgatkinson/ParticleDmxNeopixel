//
// Misc.h
//
#ifndef _MISC_H_
#define _MISC_H_

//--------------------------------------------------------------------------------------------------
// Misc
//--------------------------------------------------------------------------------------------------

typedef long long Int64;

static const double Pi  = 3.1415926535897932384626433832795;
static const float  PiF = 3.1415926535897932384626433832795f;
static const double TwoPi  = 2.0 * Pi;
static const float  TwoPiF = 2.0f * PiF;

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

template<typename T, T _first, T _last>
inline int clip(T t)
{
    return min(_last, max(_first, t));
}

template<typename T>
    inline int clip(T t, T _first, T _last)
{
    return min(_last, max(_first, t));
}

template <typename T>
inline int clipByte(T b)
{
    return min(255, max(0, b));
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

//
// Misc.h
//
#ifndef _MISC_H_
#define _MISC_H_

//--------------------------------------------------------------------------------------------------
// Misc
//--------------------------------------------------------------------------------------------------

typedef long long Int64;

#define override virtual
#define offset_of(type, field) (reinterpret_cast<size_t>(&reinterpret_cast<type*>(0)->field))

static const double Pi  = 3.1415926535897932384626433832795;
static const float  PiF = 3.1415926535897932384626433832795f;
static const double TwoPi  = 2.0 * Pi;
static const float  TwoPiF = 2.0f * PiF;

//--------------------------------------------------------------------------------------------------
// Tracing
//--------------------------------------------------------------------------------------------------

#define TRACE(...)      Log.info(__VA_ARGS__)
#define INFO(...)       Log.info(__VA_ARGS__)

//--------------------------------------------------------------------------------------------------
// Memory
//--------------------------------------------------------------------------------------------------

inline void* mallocNoFail(size_t cb)
{
    void* result = malloc(cb);
    if (result == NULL) System.enterSafeMode();
    return result;
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

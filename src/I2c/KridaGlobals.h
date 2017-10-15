//
// KridaGlobals.h
//
#ifndef __KRIDA_GLOBALS_H__
#define __KRIDA_GLOBALS_H__

#include "System/PersistentValueSetting.h"
#include "System/CloudVariable.h"

struct KridaGlobals
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

public:
    static KridaGlobals* theInstance;

    PersistentIntSetting  _percentFirst;
    PersistentIntSetting  _percentLast;

    CloudVariable<int>    _percentFirstCloudVar;
    CloudVariable<int>    _percentLastCloudVar;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    KridaGlobals()
        : _percentFirst(0),
          _percentLast(100),
          _percentFirstCloudVar("dim100First", &_percentFirst),
          _percentLastCloudVar("dim100Last", &_percentLast)
    {
        theInstance = this;
    }

    static void createInstance()
    {
        if (theInstance==nullptr)
        {
            new KridaGlobals();
        }
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------

    int percentFirst()
    {
        return _percentFirst.value();
    }
    int percentLast()
    {
        return _percentLast.value();
    }

    //----------------------------------------------------------------------------------------------
    // Loop
    //----------------------------------------------------------------------------------------------

    void begin()
    {
        _percentFirstCloudVar.begin();
        _percentLastCloudVar.begin();
    }

    void loop()
    {
        _percentFirstCloudVar.loop();
        _percentLastCloudVar.loop();
    }

    void report()
    {
        _percentFirstCloudVar.report();
        _percentLastCloudVar.report();
    }
};

KridaGlobals* SELECTANY KridaGlobals::theInstance = nullptr;

#endif

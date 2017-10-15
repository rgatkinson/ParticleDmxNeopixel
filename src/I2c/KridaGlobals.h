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
    static InstanceHolder<KridaGlobals> theInstance;

    static PersistentIntSetting  _percentFirst;
    static PersistentIntSetting  _percentLast;

    CloudVariable<int>    _percentFirstCloudVar;
    CloudVariable<int>    _percentLastCloudVar;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    KridaGlobals()
    {
        _percentFirst.setDefault(0);
        _percentLast.setDefault(100);

        _percentFirstCloudVar.initialize("dim100First", &_percentFirst);
        _percentLastCloudVar.initialize("dim100Last", &_percentLast);
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

decltype(KridaGlobals::theInstance)   SELECTANY KridaGlobals::theInstance;

#endif

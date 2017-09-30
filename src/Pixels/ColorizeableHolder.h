//
// ColorizeableHolder.h
//
#ifndef __COLORIZABLE_HOLDER_H__
#define __COLORIZABLE_HOLDER_H__

#include "Colorizeable.h"

struct ColorizeableHolder
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
protected:

    bool            _refCounted;
    Colorizeable*   _pColorizeable;
    int             _pixelCount;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    ColorizeableHolder()
    {
        _refCounted = false;
        _pColorizeable = NULL;
        _pixelCount = 0;
    }

    virtual ~ColorizeableHolder()
    {
        releaseColorizable();
    }

    void releaseColorizable()
    {
        if (_refCounted)
        {
            ::releaseRef(_pColorizeable);
        }
        else
        {
            _pColorizeable = nullptr;
        }
    }

    virtual void noteColorizeable(Colorizeable* pColorizeable)
    {
        releaseColorizable();
        _refCounted = false;
        _pColorizeable = pColorizeable;
        _pixelCount = pColorizeable==NULL ? 0 : pColorizeable->numberOfPixels();
    }

    virtual void setColorizeable(Colorizeable* pColorizeable)
    {
        releaseColorizable();
        _refCounted = true;
        setRef(_pColorizeable, pColorizeable);
        _pixelCount = pColorizeable==NULL ? 0 : pColorizeable->numberOfPixels();
    }
};

#endif

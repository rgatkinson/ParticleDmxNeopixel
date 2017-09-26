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

    ColorizeableHolder(bool refCounted)
    {
        _refCounted = refCounted;
        _pColorizeable = NULL;
        _pixelCount = 0;
    }

    virtual ~ColorizeableHolder()
    {
        if (_refCounted)
        {
            releaseRef(_pColorizeable);
        }
    }

    virtual void setColorizeable(Colorizeable* pColorizeable)
    {
        if (_refCounted)
        {
            setRef(_pColorizeable, pColorizeable);
        }
        else
        {
            _pColorizeable = pColorizeable;
        }

        _pixelCount = pColorizeable==NULL ? 0 : pColorizeable->numberOfPixels();
    }
};

#endif

//
// ColorizableHolder.h
//
#ifndef __COLORIZABLE_HOLDER_H__
#define __COLORIZABLE_HOLDER_H__

#include "Pixels/Colorizeable.h"

struct ColorizableHolder
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

    ColorizableHolder(bool refCounted)
    {
        _refCounted = refCounted;
        _pColorizeable = NULL;
        _pixelCount = 0;
    }

    virtual ~ColorizableHolder()
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

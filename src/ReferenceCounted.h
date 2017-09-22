//
// ReferenceCounted.h
//
#ifndef __REFERENCE_COUNTED_H__
#define __REFERENCE_COUNTED_H__

struct ReferenceCounted
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------
private:

    int _refCount;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    ReferenceCounted()
    {
        _refCount = 1;
    }

protected:

    virtual ~ReferenceCounted()
    {
        // not public
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:

    void addRef()
    {
        _refCount++;
    }

    void release()
    {
        if (--_refCount == 0)
        {
            delete this;
        }
    }

};

template <typename T>
inline void releaseRef(T*& pt)
{
    if (pt != NULL)
    {
        pt->release();
        pt = NULL;
    }
}

#endif

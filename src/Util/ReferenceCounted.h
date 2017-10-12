//
// ReferenceCounted.h
//
#ifndef __REFERENCE_COUNTED_H__
#define __REFERENCE_COUNTED_H__

struct RefCountable
{
    virtual void addRef() = 0;
    virtual void release() = 0;
};

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

#define DELEGATE_REF_COUNTING \
    virtual void addRef() override { ReferenceCounted::addRef(); }  \
    virtual void release() override { ReferenceCounted::release(); }  \

template <typename T>
inline void releaseRef(T*& pt)
{
    if (pt != nullptr)
    {
        pt->release();
        pt = nullptr;
    }
}

template <typename T>
inline void releaseRef(T*& pt, bool counted)
{
    if (pt != nullptr)
    {
        if (counted) pt->release();
        pt = nullptr;
    }
}

template <typename T>
inline void addRef(T* pt)
{
    if (pt != nullptr)
    {
        pt->addRef();
    }
}

template <typename T>
inline void setRef(T*& variable, T* value)
{
    addRef(value);          // do first in case value and variable have same value
    releaseRef(variable);
    variable = value;
}

template <typename T>
inline void setRef(T*& variable, T* value, bool counted)
{
    if (counted)
    {
        addRef(value);          // do first in case value and variable have same value
        releaseRef(variable);
    }
    variable = value;
}

#endif

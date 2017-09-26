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
    if (pt != NULL)
    {
        pt->release();
        pt = NULL;
    }
}

template <typename T>
inline void addRef(T* pt)
{
    if (pt != NULL)
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

#endif

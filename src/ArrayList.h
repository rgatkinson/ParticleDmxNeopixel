//
// ArrayList.h
// Simple for now
//
#ifndef __ARRAY_LIST_H_
#define __ARRAY_LIST_H_

template <typename ITEM>
struct ArrayList
{
    //----------------------------------------------------------------------------------------------
    // State
    //----------------------------------------------------------------------------------------------

protected:

    ITEM*   items;
    int     cItemAllocated;
    int     iItemFirst;
    int     iItemMax;

    //----------------------------------------------------------------------------------------------
    // Construction
    //----------------------------------------------------------------------------------------------
public:

    ArrayList()
    {
        initialize();
    }

    ~ArrayList()
    {
        free();
    }

    void addLast(ITEM item)
    {
        // Do we have space at the end? If not, then grow.
        if (iItemMax == cItemAllocated)
        {
            grow(count() + 10);
        }
        items[iItemMax++] = item;
    }

    ITEM removeFirst()
    {
        return items[iItemFirst++];
    }

protected:

    void initialize()
    {
        items = 0;
        cItemAllocated = 0;
        iItemFirst = 0;
        iItemMax = 0;
    }

    void free()
    {
        // We don't own our items
        delete items;
        initialize();
    }

    void grow(int newSize)
    {
        ITEM* newItems = new ITEM[newSize];
        int count = this->count();
        for (int i = 0; i < count; i++)
        {
            newItems[i] = (*this)[i];
        }
        if (items)
        {
            delete [] items;
        }
        items = newItems;
        cItemAllocated = newSize;
        iItemFirst = 0;
        iItemMax = count;
    }

    //----------------------------------------------------------------------------------------------
    // Accessing
    //----------------------------------------------------------------------------------------------
public:

    bool isEmpty()
    {
        return count()==0;
    }

    int count()
    {
        return iItemMax - iItemFirst;
    }

    ITEM operator[](int iItem)
    {
        return items[iItemFirst + iItem];
    }
};


#endif

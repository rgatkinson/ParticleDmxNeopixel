//
// LinkedList.h
//
#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

template <typename Container, int dibOffset>
struct ListEntry
{
private:
    ListEntry *pPrev, *pNext;

public:
    ListEntry()
    {
        clear();
    }

    void clear()
    {
        pPrev = pNext = this;
    }

    Container* prev()
    {
        return prevEntry()->getContainer();
    }
    Container* next()
    {
        return nextEntry()->getContainer();
    }
    ListEntry* prevEntry()
    {
        return pPrev;
    }
    ListEntry* nextEntry()
    {
        return pNext;
    }

    Container* getContainer()
    {
        return reinterpret_cast<Container*>(reinterpret_cast<byte*>(this) - dibOffset);
    }
    static ListEntry* getListEntry(Container *pContainer)
    {
        return reinterpret_cast<ListEntry*>(reinterpret_cast<byte*>(pContainer) + dibOffset);
    }

    void insertAfter(Container* pThem)
    {
        insertAfterEntry(getListEntry(pThem));
    }
    void insertBefore(Container* pThem)
    {
        insertBeforeEntry(getListEntry(pThem));
    }

    void remove()
    {
        ListEntry* pPrevPrev = this->pPrev;
        ListEntry* pPrevNext = this->pNext;

        pPrevPrev->pNext = pPrevNext;
        pPrevNext->pPrev = pPrevPrev;

        this->pPrev = this->pNext = this;
    }

    /* Note: the head of the list ISN'T an actual field in Container at the indicated
     * offset. Rather, it (usually) lives in some other structure.*/
    struct Head
    {
    private:
        ListEntry entry;

    public:
        Head()
        {
            clear();
        }

        void append(Container* pNew)
        {
            getListEntry(pNew)->insertAfterEntry(entry.pPrev);
        }

        void prepend(Container *pNew)
        {
            getListEntry(pNew)->insertBeforeEntry(entry.pNext);
        }

        bool isEmpty()
        {
            return entry.pNext == &entry;
        }

        void clear()
        {
            entry.clear();
        }

        /* You MUST NOT use this pointer if it's equal to stop(). */
        Container* first()
        {
            return entry.pNext->getContainer();
        }

        /* You MUST NOT use this pointer if it's equal to stop(). */
        Container* last()
        {
            return entry.pPrev->getContainer();
        }

        /* NOT A VALID Container*. Just a sentinel. */
        Container* stop()
        {
            return entry.getContainer();
        }
        };

private:

    void insertAfterEntry(ListEntry *pBefore)
    {
        this->pNext = pBefore->pNext;
        this->pPrev = pBefore;
        pBefore->pNext = this;
        this->pNext->pPrev = this;
    }

    void insertBeforeEntry(ListEntry *pAfter)
    {
        this->pNext = pAfter;
        this->pPrev = pAfter->pPrev;
        pAfter->pPrev = this;
        this->pPrev->pNext = this;
    }
};


template <typename Container /*extends DoublyLinkable*/>
struct DoublyLinkable : ListEntry<Container, 0>
    {
    };

#endif

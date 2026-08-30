#ifndef NL_DL_LIST_CONTAINER_H
#define NL_DL_LIST_CONTAINER_H

#include "NL/nlDLRing.h"
#include "NL/nlList.h"
#include "NL/nlSlotPool.h"

template <typename T, typename Adapter>
class DLListContainerBase
{
public:
    typedef void (DLListContainerBase::*EntryCallback)(DLListEntry<T>*);

    DLListContainerBase()
        : m_Head(0)
    {
    }

    DLListContainerBase(int initial, int delta)
        : m_Allocator(initial, delta)
        , m_Head(0)
    {
    }

    ~DLListContainerBase()
    {
        Clear();
    }

    void Clear()
    {
        nlWalkDLRing(m_Head, this, &DLListContainerBase::DeleteEntry);
        m_Head = 0;
    }

    DLListEntry<T>* Allocate(const T& data)
    {
        DLListEntry<T> value(data);
        DLListEntry<T>* entry = m_Allocator.Allocate();
        if (entry != 0)
        {
            *entry = value;
        }
        return entry;
    }

    void AddEnd(const T& data)
    {
        DLListEntry<T>* entry = Allocate(data);
        nlDLRingAddEnd(&m_Head, entry);
    }

    void AddStart(const T& data)
    {
        DLListEntry<T>* entry = Allocate(data);
        nlDLRingAddStart(&m_Head, entry);
    }

    void Deallocate(DLListEntry<T>* entry, T* outData)
    {
        if (outData != 0)
        {
            *outData = entry->entry;
        }
        m_Allocator.DeleteEntry(entry);
    }

    void RemoveStart(T* outData)
    {
        DLListEntry<T>* entry = nlDLRingRemoveStart(&m_Head);
        Deallocate(entry, outData);
    }

    void Remove(nlDLListIterator<T>* position)
    {
        DLListEntry<T>* entry = position->next();
        nlDLRingRemove(&m_Head, entry);
        m_Allocator.DeleteEntry(entry);
    }

    nlDLListIterator<T> Begin() const
    {
        return nlDLListIterator<T>(m_Head, nlDLRingGetStart(m_Head));
    }

    nlDLListIterator<T> Begin(DLListEntry<T>* current) const
    {
        return nlDLListIterator<T>(m_Head, current);
    }

    void DeleteEntry(DLListEntry<T>* entry);

    /* 0x00 */ Adapter m_Allocator;
    /* 0x04 */ DLListEntry<T>* m_Head;
}; // size: 0x08

template <typename T>
class nlDLListContainer
    : public DLListContainerBase<T, NewAdapter<DLListEntry<T> > >
{
public:
    nlDLListContainer()
        : DLListContainerBase<T, NewAdapter<DLListEntry<T> > >()
    {
    }
};

template <typename T>
class nlDLListSlotPool
    : public DLListContainerBase<T, BasicSlotPool<DLListEntry<T> > >
{
public:
    nlDLListSlotPool()
        : DLListContainerBase<T, BasicSlotPool<DLListEntry<T> > >()
    {
        this->m_Allocator.Initialize(16, 16);
    }

    ~nlDLListSlotPool()
    {
        this->Clear();
        this->m_Allocator.FreeBlocks();
    }

    nlDLListSlotPool(const int initial)
        : DLListContainerBase<T, BasicSlotPool<DLListEntry<T> > >()
    {
        this->m_Allocator.Initialize(initial, 0);
    }

    nlDLListSlotPool(const int initial, const int delta)
        : DLListContainerBase<T, BasicSlotPool<DLListEntry<T> > >(initial, delta)
    {
    }
};

template <typename T, typename Adapter>
void DLListContainerBase<T, Adapter>::DeleteEntry(
    DLListEntry<T>* entry)
{
    if (entry != 0)
    {
        entry->entry.~T();
    }
    m_Allocator.DeleteEntry(entry);
}

#endif

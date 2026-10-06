#ifndef NL_DL_LIST_CONTAINER_INL
#define NL_DL_LIST_CONTAINER_INL
#include "NL/nlDLListContainer.h"

template <typename T>
inline typename nlDLListIterator<T>::Reference nlDLListIterator<T>::operator*() const
{
    return m_Curr->entry;
}

template <typename T>
inline typename nlDLListIterator<T>::Pointer nlDLListIterator<T>::CurrentEntry() const
{
    return m_Curr;
}

template <typename T>
inline bool nlDLListIterator<T>::hasNext() const
{
    return m_Curr != 0;
}

template <typename T>
inline bool nlDLListIterator<T>::IsDone() const
{
    return m_Curr == 0;
}

template <typename T>
inline bool nlDLListIterator<T>::IsStart() const
{
    return nlDLRingIsStart(m_Head, m_Curr);
}

template <typename T>
inline bool nlDLListIterator<T>::IsEnd() const
{
    return nlDLRingIsEnd(m_Head, m_Curr);
}

template <typename T>
inline void nlDLListIterator<T>::Step()
{
    if (nlDLRingIsEnd(m_Head, m_Curr) || m_Curr == 0)
    {
        m_Curr = 0;
    }
    else
    {
        m_Curr = m_Curr->m_next;
    }
}

template <typename T>
inline typename nlDLListIterator<T>::Pointer nlDLListIterator<T>::next()
{
    Pointer result = m_Curr;
    Step();
    return result;
}

template <typename T>
inline void nlDLListIterator<T>::Retreat()
{
    if (nlDLRingIsStart(m_Head, m_Curr))
    {
        m_Curr = 0;
    }
    else
    {
        m_Curr = m_Curr->m_prev;
    }
}

template <typename T, typename Adapter>
inline DLListContainerBase<T, Adapter>::DLListContainerBase()
    : m_Head(0)
{
}

template <typename T, typename Adapter>
inline DLListContainerBase<T, Adapter>::DLListContainerBase(int initial, int delta)
    : m_Allocator(initial, delta)
    , m_Head(0)
{
}

template <typename T, typename Adapter>
inline DLListContainerBase<T, Adapter>::DLListContainerBase(Adapter allocator)
    : m_Allocator(allocator)
    , m_Head(0)
{
}

template <typename T, typename Adapter>
inline DLListContainerBase<T, Adapter>::~DLListContainerBase()
{
    Clear();
}

template <typename T, typename Adapter>
inline void DLListContainerBase<T, Adapter>::Clear()
{
    nlWalkDLRing(m_Head, this, &DLListContainerBase::DeleteEntry);
    m_Head = 0;
}

template <typename T, typename Adapter>
inline void DLListContainerBase<T, Adapter>::Copy(DLListContainerBase& other)
{
    Clear();
    nlDLListIterator<T> iterator;
    iterator = other.Begin();
    while (!iterator.IsDone())
    {
        AddEnd(*iterator);
        iterator.Step();
    }
}

template <typename T, typename Adapter>
inline DLListEntry<T>* DLListContainerBase<T, Adapter>::Allocate(const T& data)
{
    DLListEntry<T> value(data);
    DLListEntry<T>* entry = m_Allocator.Allocate();
    new (entry) DLListEntry<T>(value);
    return entry;
}

template <typename T, typename Adapter>
inline unsigned long DLListContainerBase<T, Adapter>::AddEnd(const T& data)
{
    DLListEntry<T>* entry = Allocate(data);
    nlDLRingAddEnd(&m_Head, entry);
    return (unsigned long)entry;
}

template <typename T, typename Adapter>
inline unsigned long DLListContainerBase<T, Adapter>::AddAfter(nlDLListIterator<T>& position, const T& data)
{
    DLListEntry<T>* entry = Allocate(data);
    nlDLRingInsert(&m_Head, position.CurrentEntry(), entry);
    return (unsigned long)entry;
}

template <typename T, typename Adapter>
inline void DLListContainerBase<T, Adapter>::AddStart(const T& data)
{
    DLListEntry<T>* entry = Allocate(data);
    nlDLRingAddStart(&m_Head, entry);
}

template <typename T, typename Adapter>
inline void DLListContainerBase<T, Adapter>::Deallocate(DLListEntry<T>* entry, T* outData)
{
    if (outData != 0)
    {
        *outData = entry->entry;
    }
    m_Allocator.DeleteEntry(entry);
}

template <typename T, typename Adapter>
inline void DLListContainerBase<T, Adapter>::RemoveStart(T* outData)
{
    DLListEntry<T>* entry = nlDLRingRemoveStart(&m_Head);
    Deallocate(entry, outData);
}

template <typename T, typename Adapter>
inline void DLListContainerBase<T, Adapter>::Remove(nlDLListIterator<T>* position)
{
    DLListEntry<T>* entry = position->next();
    nlDLRingRemove(&m_Head, entry);
    m_Allocator.DeleteEntry(entry);
}

template <typename T, typename Adapter>
inline void DLListContainerBase<T, Adapter>::Remove(nlDLListIterator<T>* position, T* outData)
{
    DLListEntry<T>* entry = position->next();
    nlDLRingRemove(&m_Head, entry);
    Deallocate(entry, outData);
}

template <typename T, typename Adapter>
inline T DLListContainerBase<T, Adapter>::RemoveEntry(DLListEntry<T>* entry)
{
    T data = entry->entry;
    nlDLRingRemove(&m_Head, entry);
    m_Allocator.DeleteEntry(entry);
    return data;
}

template <typename T, typename Adapter>
inline nlDLListIterator<T> DLListContainerBase<T, Adapter>::Begin()
{
    nlDLListIterator<T> result;
    result.m_Curr = nlDLRingGetStart(m_Head);
    result.m_Head = m_Head;
    return result;
}

template <typename T, typename Adapter>
inline nlDLListIterator<T> DLListContainerBase<T, Adapter>::Begin() const
{
    nlDLListIterator<T> result;
    result.m_Curr = nlDLRingGetStart(m_Head);
    result.m_Head = m_Head;
    return result;
}

template <typename T, typename Adapter>
inline nlDLListIterator<T> DLListContainerBase<T, Adapter>::Begin(DLListEntry<T>* current) const
{
    nlDLListIterator<T> result;
    result.m_Curr = current;
    result.m_Head = m_Head;
    return result;
}

template <typename T, typename Adapter>
inline nlDLListIterator<T> DLListContainerBase<T, Adapter>::End()
{
    nlDLListIterator<T> result;
    result.m_Curr = nlDLRingGetEnd(m_Head);
    result.m_Head = m_Head;
    return result;
}

template <typename T, typename Adapter>
inline bool DLListContainerBase<T, Adapter>::IsEmpty()
{
    return m_Head == 0;
}

template <typename T, typename Adapter>
inline u32 DLListContainerBase<T, Adapter>::CountElements() const
{
    return nlDLRingCountElements(m_Head);
}

template <typename T, typename Adapter>
inline DLListContainerBase<T, Adapter&>::DLListContainerBase()
    : m_Head(0)
{
}

template <typename T, typename Adapter>
inline DLListContainerBase<T, Adapter&>::DLListContainerBase(Adapter& allocator)
    : m_Head(0)
{
    m_Allocator = &allocator;
}

template <typename T, typename Adapter>
inline DLListContainerBase<T, Adapter&>::~DLListContainerBase()
{
    Clear();
}

template <typename T, typename Adapter>
inline void DLListContainerBase<T, Adapter&>::Clear()
{
    nlWalkDLRing(m_Head, this, &DLListContainerBase::DeleteEntry);
    m_Head = 0;
}

template <typename T, typename Adapter>
inline DLListEntry<T>* DLListContainerBase<T, Adapter&>::Allocate(const T& data)
{
    DLListEntry<T> value(data);
    DLListEntry<T>* entry = m_Allocator->Allocate();
    new (entry) DLListEntry<T>(value);
    return entry;
}

template <typename T, typename Adapter>
inline unsigned long DLListContainerBase<T, Adapter&>::AddEnd(const T& data)
{
    DLListEntry<T>* entry = Allocate(data);
    nlDLRingAddEnd(&m_Head, entry);
    return (unsigned long)entry;
}

template <typename T, typename Adapter>
inline unsigned long DLListContainerBase<T, Adapter&>::AddAfter(nlDLListIterator<T>& position, const T& data)
{
    DLListEntry<T>* entry = Allocate(data);
    nlDLRingInsert(&m_Head, position.CurrentEntry(), entry);
    return (unsigned long)entry;
}

template <typename T, typename Adapter>
inline void DLListContainerBase<T, Adapter&>::AddStart(const T& data)
{
    DLListEntry<T>* entry = Allocate(data);
    nlDLRingAddStart(&m_Head, entry);
}

template <typename T, typename Adapter>
inline void DLListContainerBase<T, Adapter&>::Deallocate(DLListEntry<T>* entry, T* outData)
{
    if (outData != 0)
    {
        *outData = entry->entry;
    }
    m_Allocator->DeleteEntry(entry);
}

template <typename T, typename Adapter>
inline void DLListContainerBase<T, Adapter&>::RemoveStart(T* outData)
{
    DLListEntry<T>* entry = nlDLRingRemoveStart(&m_Head);
    Deallocate(entry, outData);
}

template <typename T, typename Adapter>
inline void DLListContainerBase<T, Adapter&>::Remove(nlDLListIterator<T>* position)
{
    DLListEntry<T>* entry = position->next();
    nlDLRingRemove(&m_Head, entry);
    m_Allocator->DeleteEntry(entry);
}

template <typename T, typename Adapter>
inline nlDLListIterator<T> DLListContainerBase<T, Adapter&>::Begin() const
{
    nlDLListIterator<T> result;
    result.m_Curr = nlDLRingGetStart(m_Head);
    result.m_Head = m_Head;
    return result;
}

template <typename T, typename Adapter>
inline nlDLListIterator<T> DLListContainerBase<T, Adapter&>::Begin(DLListEntry<T>* current) const
{
    nlDLListIterator<T> result;
    result.m_Curr = current;
    result.m_Head = m_Head;
    return result;
}

template <typename T, typename Adapter>
inline nlDLListIterator<T> DLListContainerBase<T, Adapter&>::End() const
{
    nlDLListIterator<T> result;
    result.m_Curr = nlDLRingGetEnd(m_Head);
    result.m_Head = m_Head;
    return result;
}

template <typename T, typename Adapter>
inline bool DLListContainerBase<T, Adapter&>::IsEmpty() const
{
    return m_Head == 0;
}

template <typename T, typename Adapter>
inline T* DLListContainerBase<T, Adapter&>::AllocateAtEnd(unsigned long* outEntry)
{
    DLListEntry<T>* result;
    m_Allocator->Allocate(result);
    new (result) DLListEntry<T>;
    nlDLRingAddEnd(&m_Head, result);

    if (outEntry != 0)
    {
        *outEntry = (unsigned long)result;
    }

    return &result->entry;
}

template <typename T, typename Adapter>
inline bool DLListContainerBase<T, Adapter&>::Walk(const Function1<bool, T&>& callback)
{
    WalkCallback adapter(callback);
    return nlWalkRing(m_Head, &adapter, &WalkCallback::Call);
}

template <typename T, typename Adapter>
inline void DLListContainerBase<T, Adapter&>::DeleteEntry(DLListEntry<T>* entry)
{
    if (entry != 0)
    {
        entry->entry.~T();
    }
    m_Allocator->DeleteEntry(entry);
}

template <typename T>
inline nlDLListContainer<T>::nlDLListContainer()
    : DLListContainerBase<T, NewAdapter<DLListEntry<T> > >()
{
}

template <typename T>
inline nlDLListContainer<T>::nlDLListContainer(int initial)
    : DLListContainerBase<T, NewAdapter<DLListEntry<T> > >()
{
}

template <typename T>
inline void nlDLListSlotPool<T>::Free()
{
    DLListContainerBase<T, BasicSlotPool<DLListEntry<T> > >::Clear();
    this->m_Allocator.FreeBlocks();
}

template <typename T>
inline nlDLListSlotPool<T>::nlDLListSlotPool()
    : DLListContainerBase<T, BasicSlotPool<DLListEntry<T> > >()
{
    this->m_Allocator.Initialize(16, 16);
}

template <typename T>
inline nlDLListSlotPool<T>::~nlDLListSlotPool()
{
    this->Clear();
    this->m_Allocator.FreeBlocks();
}

template <typename T>
inline nlDLListSlotPool<T>::nlDLListSlotPool(const int initial)
    : DLListContainerBase<T, BasicSlotPool<DLListEntry<T> > >()
{
    this->m_Allocator.Initialize(initial, 0);
}

template <typename T>
inline nlDLListSlotPool<T>::nlDLListSlotPool(const int initial, const int delta)
    : DLListContainerBase<T, BasicSlotPool<DLListEntry<T> > >()
{
    this->m_Allocator.Initialize(initial, delta);
}

template <typename T>
inline BorrowedDLListSlotPool<T>::BorrowedDLListSlotPool(BasicSlotPool<DLListEntry<T> >& allocator)
    : DLListContainerBase<T, BasicSlotPool<DLListEntry<T> >&>(allocator)
{
}

template <typename T, typename Adapter>
inline T* DLListContainerBase<T, Adapter>::AllocateAtEnd(
    unsigned long* outEntry)
{
    DLListEntry<T>* result;
    m_Allocator.Allocate(result);
    new (result) DLListEntry<T>;
    nlDLRingAddEnd(&m_Head, result);

    if (outEntry != 0)
    {
        *outEntry = (unsigned long)result;
    }

    return &result->entry;
}

template <typename T, typename Adapter>
bool DLListContainerBase<T, Adapter>::Walk(const Function1<bool, T&>& callback)
{
    WalkCallback adapter(callback);
    return nlWalkRing(m_Head, &adapter, &WalkCallback::Call);
}

template <typename T>
void nlDLListIterator<T>::Copy(const nlDLListIterator& other)
{
    m_Head = other.m_Head;
    m_Curr = other.m_Curr;
}

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

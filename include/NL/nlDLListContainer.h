#ifndef NL_DL_LIST_CONTAINER_H
#define NL_DL_LIST_CONTAINER_H

#include "NL/nlDLRing.h"
#include "NL/nlList.h"
#include "NL/nlFunction.h"
#include "NL/nlSlotPool.h"

template <typename T>
class nlDLListIterator
{
public:
    typedef DLListEntry<T>* Pointer;
    typedef T& Reference;

    void Copy(const nlDLListIterator& other);

    inline Reference operator*() const;

    inline Pointer CurrentEntry() const;

    inline bool hasNext() const;

    inline bool IsDone() const;

    inline bool IsStart() const;

    inline bool IsEnd() const;

    inline void Step();

    inline Pointer next();

    inline void Retreat();

    Pointer m_Head;
    Pointer m_Curr;
};

template <typename T, typename Adapter>
class DLListContainerBase
{
public:
    typedef void (DLListContainerBase::*EntryCallback)(DLListEntry<T>*);

    inline DLListContainerBase();

    inline DLListContainerBase(int initial, int delta);

    inline DLListContainerBase(Adapter allocator);

    inline ~DLListContainerBase();

    inline void Clear();

    // Not const: a const source lets the compiler hoist its head read above
    // this list's clearing store, which retail keeps below it.
    inline void Copy(DLListContainerBase& other);

    inline DLListEntry<T>* Allocate(const T& data);

    inline unsigned long AddEnd(const T& data);

    inline unsigned long AddAfter(nlDLListIterator<T>& position, const T& data);

    inline void AddStart(const T& data);

    inline void Deallocate(DLListEntry<T>* entry, T* outData);

    inline void RemoveStart(T* outData);

    inline void Remove(nlDLListIterator<T>* position);

    inline void Remove(nlDLListIterator<T>* position, T* outData);

    inline T RemoveEntry(DLListEntry<T>* entry);

    inline nlDLListIterator<T> Begin();

    inline nlDLListIterator<T> Begin() const;

    inline nlDLListIterator<T> Begin(DLListEntry<T>* current) const;

    inline nlDLListIterator<T> End();

    inline bool IsEmpty();

    inline u32 CountElements() const;

    T* AllocateAtEnd(unsigned long* outEntry);

    struct WalkCallback
    {
        const Function1<bool, T&>& m_Callback;
        WalkCallback(const Function1<bool, T&>& callback)
            : m_Callback(callback)
        {
        }
        bool Call(DLListEntry<T>* entry) { return m_Callback(entry->entry); }
    };

    bool Walk(const Function1<bool, T&>& callback);

    inline void DeleteEntry(DLListEntry<T>* entry);

    /* 0x00 */ Adapter m_Allocator;
    /* 0x04 */ DLListEntry<T>* m_Head;
}; // size: 0x08

template <typename T, typename Adapter>
class DLListContainerBase<T, Adapter&>
{
public:
    typedef void (DLListContainerBase::*EntryCallback)(DLListEntry<T>*);

    inline DLListContainerBase();

    inline DLListContainerBase(Adapter& allocator);

    inline ~DLListContainerBase();

    inline void Clear();

    inline DLListEntry<T>* Allocate(const T& data);

    inline unsigned long AddEnd(const T& data);

    inline unsigned long AddAfter(nlDLListIterator<T>& position, const T& data);

    inline void AddStart(const T& data);

    inline void Deallocate(DLListEntry<T>* entry, T* outData);

    inline void RemoveStart(T* outData);

    inline void Remove(nlDLListIterator<T>* position);

    inline nlDLListIterator<T> Begin() const;

    inline nlDLListIterator<T> Begin(DLListEntry<T>* current) const;

    inline nlDLListIterator<T> End() const;

    inline bool IsEmpty() const;

    inline T* AllocateAtEnd(unsigned long* outEntry);

    struct WalkCallback
    {
        const Function1<bool, T&>& m_Callback;
        WalkCallback(const Function1<bool, T&>& callback)
            : m_Callback(callback)
        {
        }
        bool Call(DLListEntry<T>* entry) { return m_Callback(entry->entry); }
    };

    inline bool Walk(const Function1<bool, T&>& callback);

    inline void DeleteEntry(DLListEntry<T>* entry);

    /* 0x00 */ Adapter* m_Allocator;
    /* 0x04 */ DLListEntry<T>* m_Head;
}; // size: 0x08

template <typename T>
class nlDLListContainer
    : public DLListContainerBase<T, NewAdapter<DLListEntry<T> > >
{
public:
    inline nlDLListContainer();

    // Mirrors the nlDLListSlotPool(int initial) form; a NewAdapter list has
    // no pool to size, so the count is accepted and ignored.
    inline nlDLListContainer(int initial);
};

template <typename T>
class nlDLListSlotPool
    : public DLListContainerBase<T, BasicSlotPool<DLListEntry<T> > >
{
public:
    inline void Free();

    inline nlDLListSlotPool();

    inline ~nlDLListSlotPool();

    inline nlDLListSlotPool(const int initial);

    inline nlDLListSlotPool(const int initial, const int delta);
};

// The list borrows its node pool and does not free the pool's blocks.
template <typename T>
class BorrowedDLListSlotPool
    : public DLListContainerBase<T, BasicSlotPool<DLListEntry<T> >&>
{
public:
    inline BorrowedDLListSlotPool(BasicSlotPool<DLListEntry<T> >& allocator);
};

#endif

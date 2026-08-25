#ifndef NL_DL_LIST_CONTAINER_H
#define NL_DL_LIST_CONTAINER_H

#include "NL/nlDLRing.h"
#include "NL/nlList.h"

template <typename T, typename Adapter>
class DLListContainerBase
{
public:
    DLListContainerBase()
        : m_Head(0)
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

    void DeleteEntry(DLListEntry<T>* entry);

    nlDLListIterator<T> Begin() const
    {
        return nlDLListIterator<T>(m_Head, nlDLRingGetStart(m_Head));
    }

    /* 0x00 */ Adapter m_Allocator;
    /* 0x04 */ DLListEntry<T>* m_Head;
};

template <typename T>
class nlDLListContainer
    : public DLListContainerBase<T, NewAdapter<DLListEntry<T> > >
{
};

template <typename T, typename Adapter>
inline void DLListContainerBase<T, Adapter>::DeleteEntry(
    DLListEntry<T>* entry)
{
    if (entry != 0)
    {
        entry->entry.~T();
    }
    m_Allocator.DeleteEntry(entry);
}

#endif // NL_DL_LIST_CONTAINER_H

#ifndef NL_NL_LIST_H
#define NL_NL_LIST_H

#include "NL/nlMemory.h"

template <typename T>
class ListEntry
{
public:
    /* 0x00 */ ListEntry<T>* next;
    /* 0x04 */ T entry;

    ListEntry()
        : next(0)
        , entry(0)
    {
    }

    ListEntry(const T& value)
        : next(0)
        , entry(value)
    {
    }
};

template <typename T>
class nlListIterator;

template <typename T>
class NewAdapter
{
public:
    T* New(const T& value)
    {
        T* entry = (T*)nlMalloc(sizeof(T), 8, false);
        if (entry != 0)
        {
            *entry = value;
        }
        return entry;
    }

    static void DeleteEntry(T* entry)
    {
        delete entry;
    }
};

template <typename EntryT, typename ContainerT>
inline void nlWalkList(
    EntryT* entry, ContainerT* container, void (ContainerT::*callback)(EntryT*))
{
    while (entry != 0)
    {
        EntryT* next = entry->next;
        (container->*callback)(entry);
        entry = next;
    }
}

template <typename T>
inline void nlListAddStart(T** head, T* entry, T** tail)
{
    if (tail != 0 && *head == 0)
    {
        *tail = entry;
    }
    entry->next = *head;
    *head = entry;
}

template <typename T, typename Adapter>
class ListContainerBase
{
public:
    typedef void (ListContainerBase::*EntryCallback)(ListEntry<T>*);

    ListContainerBase()
        : m_Head(0)
        , m_Tail(0)
    {
    }

    ~ListContainerBase()
    {
        Clear();
    }

    void Clear()
    {
        EntryCallback callback = &ListContainerBase::DeleteEntry;
        nlWalkList(m_Head, this, callback);
        m_Head = 0;
        m_Tail = 0;
    }

    void DeleteEntry(ListEntry<T>* entry)
    {
        m_Allocator.DeleteEntry(entry);
    }

    void AddEntry(const T& value)
    {
        ListEntry<T> local(value);
        ListEntry<T>* entry = m_Allocator.New(local);
        nlListAddStart(&m_Head, entry, &m_Tail);
    }

    nlListIterator<T> Begin();

    /* 0x00 */ Adapter m_Allocator;
    /* 0x04 */ ListEntry<T>* m_Head;
    /* 0x08 */ ListEntry<T>* m_Tail;
};

template <typename T>
class nlListIterator
{
public:
    nlListIterator(ListEntry<T>* current)
        : m_Curr(current)
    {
    }

    bool IsValid() const
    {
        return m_Curr != 0;
    }

    T& Current() const
    {
        return m_Curr->entry;
    }

    void Next()
    {
        m_Curr = m_Curr->next;
    }

    ListEntry<T>* CurrentEntry() const
    {
        return m_Curr;
    }

private:
    ListEntry<T>* m_Curr;
};

template <typename T, typename Adapter>
inline nlListIterator<T> ListContainerBase<T, Adapter>::Begin()
{
    return nlListIterator<T>(m_Head);
}

template <typename T>
class nlListContainer
    : public ListContainerBase<T, NewAdapter<ListEntry<T> > >
{
};

#endif

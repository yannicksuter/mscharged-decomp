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
    T* Allocate()
    {
        return (T*)nlMalloc(sizeof(T), 8, false);
    }

    void Allocate(T*& out)
    {
        out = (T*)nlMalloc(sizeof(T), 8, false);
    }

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

    void Free(T* entry)
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

template <typename T>
inline void nlListAddEnd(T** head, T** tail, T* entry)
{
    entry->next = 0;
    T* oldTail = *tail;
    if (oldTail != 0)
    {
        oldTail->next = entry;
        *tail = entry;
        return;
    }

    *tail = entry;
    *head = entry;
}

template <typename T>
inline T* nlListRemoveStart(T** head, T** tail)
{
    T* first = *head;
    if (first == 0)
    {
        return 0;
    }

    if (tail != 0 && *tail == first)
    {
        *tail = 0;
    }

    T* tmp = *head;
    *head = tmp->next;
    return tmp;
}

#include "NL/nlListContainer.h"

#endif

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
inline void nlListAddEnd(T** head, T** tail, T* node)
{
    node->next = 0;
    T* temp_r6 = *tail;
    if (temp_r6 != 0)
    {
        temp_r6->next = node;
        *tail = node;
        return;
    }
    *tail = node;
    *head = node;
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

template <typename T>
class nlList
{
public:
    nlList(T* start, T* end)
    {
        m_pEnd = end;
        m_pStart = start;
    }

    nlList() { }

    T* m_pStart;
    T* m_pEnd;
};

template <typename T>
T* nlListRemoveElement(T** head, T* element, T** tail)
{
    if (head == 0)
        return 0;

    if (*head == element)
    {
        if (tail != 0 && *tail == *head)
        {
            *tail = 0;
        }
        T* first = *head;
        *head = first->next;
        return 0;
    }

    T* prev = *head;
    T* current = prev->next;
    while (current != 0)
    {
        if (current == element)
        {
            prev->next = current->next;
            if (tail != 0 && *tail == current)
            {
                *tail = prev;
            }
            return prev;
        }
        prev = current;
        current = current->next;
    }
    return 0;
}

template <typename T>
inline void nlDeleteList(T** head)
{
    T* next;
    while (*head != 0)
    {
        next = (*head)->next;
        delete *head;
        *head = next;
    }
    *head = 0;
}

#include "NL/nlListContainer.h"

#endif

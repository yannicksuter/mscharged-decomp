#ifndef NL_RING_H
#define NL_RING_H

#include "types.h"

template <typename T>
inline void nlRingAddStart(T** list, T* item)
{
    T* head = *list;
    if (head == 0)
    {
        *list = item;
        item->m_next = item;
        return;
    }

    item->m_next = head->m_next;
    head = *list;
    head->m_next = item;
}

template <typename T>
inline void nlRingAddEnd(T** list, T* item)
{
    nlRingAddStart(list, item);
    *list = item;
}

template <typename T>
inline void nlDeleteRing(T** head)
{
    T* next;
    T* current;

    T* headPtr = *head;
    if (headPtr != 0)
    {
        current = headPtr->m_next;
        for (;;)
        {
            next = current->m_next;
            delete current;
            if (current != *head)
            {
                current = next;
            }
            else
            {
                break;
            }
        }
        *head = 0;
    }
}

template <typename T>
inline u32 nlRingCountElements(T* head)
{
    T* current;
    u32 count = 0;

    if (head == 0)
    {
        return 0;
    }

    current = head->m_next;
    while (true)
    {
        T* next = current->m_next;
        count++;
        if (current == head)
        {
            break;
        }
        current = next;
    }
    return count;
}

template <typename T, typename CallbackType>
inline void nlWalkRing(T* head, CallbackType* callback, void (CallbackType::*callbackFunc)(T*))
{
    if (head == 0)
    {
        return;
    }

    T* current = head->m_next;
    while (true)
    {
        T* next = current->m_next;
        (callback->*callbackFunc)(current);
        if (current == head)
        {
            break;
        }
        current = next;
    }
}

#endif // NL_RING_H

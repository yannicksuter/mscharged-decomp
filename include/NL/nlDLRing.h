#ifndef NL_DL_RING_H
#define NL_DL_RING_H

#include "NL/nlRing.h"

template <typename T>
class DLListEntry
{
public:
    DLListEntry()
        : m_next(0)
        , m_prev(0)
        , entry(0)
    {
    }

    DLListEntry(const T& data)
        : m_next(0)
        , m_prev(0)
        , entry(data)
    {
    }

    /* 0x00 */ DLListEntry<T>* m_next;
    /* 0x04 */ DLListEntry<T>* m_prev;
    /* 0x08 */ T entry;
}; // size: 0x0C

template <typename T>
inline void nlDLRingInsert(T** head, T* afterNode, T* newNode)
{
    afterNode->m_next->m_prev = newNode;
    newNode->m_next = afterNode->m_next;
    newNode->m_prev = afterNode;
    afterNode->m_next = newNode;
    if ((void*)*head == afterNode)
    {
        *head = newNode;
    }
}

template <typename T>
inline void nlDLRingAddStart(T** head, T* newNode)
{
    T* current = *head;
    if (current == 0)
    {
        *head = newNode;
        newNode->m_next = newNode;
        newNode->m_prev = newNode;
        return;
    }

    current->m_next->m_prev = newNode;
    newNode->m_next = current->m_next;
    newNode->m_prev = current;
    current->m_next = newNode;
}

template <typename T>
inline void nlDLRingAddEnd(T** head, T* newNode)
{
    nlDLRingAddStart(head, newNode);
    *head = newNode;
}

template <typename T>
inline void nlDLRingRemove(T** head, T* current)
{
    T* next = current->m_next;

    if (next == current)
    {
        *head = 0;
        return;
    }

    current->m_prev->m_next = next;
    current->m_next->m_prev = current->m_prev;

    if (*head == current)
    {
        *head = current->m_prev;
    }
}

template <typename T>
inline T* nlDLRingGetStart(T* current)
{
    if (current == 0)
    {
        return 0;
    }
    return current->m_next;
}

template <typename T>
inline bool nlDLRingIsEnd(T* head, T* current)
{
    if (head == 0)
    {
        return true;
    }
    return head == current;
}

template <typename T>
inline bool nlDLRingIsStart(T* head, T* current)
{
    if (head == 0)
    {
        return false;
    }
    return head->m_next == current;
}

template <typename T>
inline T* nlDLRingRemoveStart(T** current)
{
    T* temp_r31;
    temp_r31 = (*current)->m_next;
    nlDLRingRemove<T>(current, temp_r31);
    return temp_r31;
}

template <typename T>
inline T* nlDLRingRemoveEnd(T** head)
{
    T* node = *head;
    nlDLRingRemove<T>(head, node);
    return node;
}

template <typename T>
inline u32 nlDLRingCountElements(T* head)
{
    return nlRingCountElements(head);
}

template <typename T>
inline bool nlDLRingValidateContainsElement(T* head, const T* node)
{
    if (head == 0)
    {
        return false;
    }

    T* current = head->m_next;

    for (;;)
    {
        if (node == current)
        {
            return true;
        }

        T* next = current->m_next;

        if (current == head)
        {
            break;
        }

        current = next;
    }

    return false;
}

template <typename T>
inline bool nlDLRingRemoveSafely(T** head, const T* node)
{
    T* next;

    if (nlDLRingValidateContainsElement(*head, node) == 0)
    {
        return false;
    }

    next = node->m_next;
    if (next == node)
    {
        *head = 0;
        return true;
    }

    node->m_prev->m_next = next;
    node->m_next->m_prev = node->m_prev;
    if (*head == node)
    {
        *head = node->m_prev;
        return true;
    }

    return true;
}

template <typename T>
inline void nlDLRingAppendRing(T** head, T* current)
{
    T* currentHead = *head;
    T* nextAfterHead;

    if (current != 0)
    {
        if (currentHead == 0)
        {
            *head = current;
            return;
        }

        nextAfterHead = currentHead->m_next;
        currentHead->m_next = current->m_next;
        current->m_next->m_prev = *head;
        current->m_next = nextAfterHead;
        nextAfterHead->m_prev = current;
        *head = current;
    }
}

template <typename T>
inline T* nlDLRingGetEnd(T* current)
{
    if (current == 0)
    {
        return 0;
    }
    return current;
}

template <typename T>
inline void nlDeleteDLRing(T** head)
{
    nlDeleteRing<T>(head);
}

template <typename T, typename CallbackType>
inline void nlWalkDLRing(T* head, CallbackType* callback, void (CallbackType::*callbackFunc)(T*))
{
    void (CallbackType::*func)(T*) = callbackFunc;
    nlWalkRing(head, callback, func);
}

template <typename T>
class nlDLListIterator
{
public:
    typedef DLListEntry<T>* Pointer;
    typedef T& Reference;

    nlDLListIterator() { }

    nlDLListIterator(Pointer head, Pointer current)
    {
        m_Curr = current;
        m_Head = head;
    }

    Reference operator*() const { return m_Curr->entry; }

    Pointer CurrentEntry() const { return m_Curr; }

    bool hasNext() const { return m_Curr != 0; }

    bool IsEnd() const { return nlDLRingIsEnd(m_Head, m_Curr); }

    void Step() { next(); }

    Pointer next()
    {
        Pointer result = m_Curr;
        if (nlDLRingIsEnd(m_Head, m_Curr) || m_Curr == 0)
        {
            m_Curr = 0;
        }
        else
        {
            m_Curr = m_Curr->m_next;
        }
        return result;
    }

    Pointer m_Head;
    Pointer m_Curr;
};

#endif // NL_DL_RING_H

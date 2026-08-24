#ifndef NL_DL_RING_H
#define NL_DL_RING_H

template <typename T>
inline void nlDLRingInsert(T** head, T* afterNode, T* newNode)
{
    afterNode->mNext->mPrev = newNode;
    newNode->mNext = afterNode->mNext;
    newNode->mPrev = afterNode;
    afterNode->mNext = newNode;
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
        newNode->mNext = newNode;
        newNode->mPrev = newNode;
        return;
    }

    current->mNext->mPrev = newNode;
    newNode->mNext = current->mNext;
    newNode->mPrev = current;
    current->mNext = newNode;
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
    T* next = current->mNext;

    if (next == current)
    {
        *head = 0;
        return;
    }

    current->mPrev->mNext = next;
    current->mNext->mPrev = current->mPrev;

    if (*head == current)
    {
        *head = current->mPrev;
    }
}

template <typename T>
inline T* nlDLRingGetStart(T* current)
{
    if (current == 0)
    {
        return 0;
    }
    return current->mNext;
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
inline bool nlDLRingValidateContainsElement(T* head, const T* node)
{
    if (head == 0)
    {
        return false;
    }

    T* current = head->mNext;

    for (;;)
    {
        if (node == current)
        {
            return true;
        }

        T* next = current->mNext;

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

    next = node->mNext;
    if (next == node)
    {
        *head = 0;
        return true;
    }

    node->mPrev->mNext = next;
    node->mNext->mPrev = node->mPrev;
    if (*head == node)
    {
        *head = node->mPrev;
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

        nextAfterHead = currentHead->mNext;
        currentHead->mNext = current->mNext;
        current->mNext->mPrev = *head;
        current->mNext = nextAfterHead;
        nextAfterHead->mPrev = current;
        *head = current;
    }
}

#endif // NL_DL_RING_H

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

#endif // NL_DL_RING_H

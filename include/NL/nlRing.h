#ifndef NL_RING_H
#define NL_RING_H

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

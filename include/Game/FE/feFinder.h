#ifndef _FEFINDER_H_
#define _FEFINDER_H_

template <class T>
T* FindItemByHashID(T* list, unsigned long hashID)
{
    if (list == 0)
        return 0;
    T* curr = list->m_next;
    for (;;)
    {
        unsigned long id = curr->m_hash;
        T* next = curr->m_next;
        if (hashID == id)
            return curr;
        if (curr == list)
            break;
        curr = next;
    }
    return 0;
}

#endif // _FEFINDER_H_

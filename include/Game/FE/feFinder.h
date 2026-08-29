#ifndef GAME_FE_FINDER_H
#define GAME_FE_FINDER_H

template <class T>
inline T* FindItemByHashID(T* list, unsigned long hashID)
{
    if (list == 0)
    {
        return 0;
    }

    T* curr = list->m_next;
    for (;;)
    {
        unsigned long id = curr->m_hash;
        T* next = curr->m_next;
        if (hashID == id)
        {
            return curr;
        }
        if (curr == list)
        {
            break;
        }
        curr = next;
    }
    return 0;
}

#endif // GAME_FE_FINDER_H

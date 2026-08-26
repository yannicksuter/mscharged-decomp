#ifndef NL_ALGORITHM_H
#define NL_ALGORITHM_H

#include "stdlib.h"

template <typename T, typename Key>
T* nlBSearch(const Key& key, T* array, int size)
{
    int high = size - 1;
    int low = -1;
    while (high - low > 1)
    {
        int probe = (high + low) / 2;
        if ((unsigned long)array[probe] > (unsigned long)key)
            high = probe;
        else
            low = probe;
    }
    unsigned long highValue = (unsigned long)array[high];
    if (highValue == (unsigned long)key)
        return &array[high];
    if (low == -1)
        return NULL;
    unsigned long lowValue = (unsigned long)array[low];
    if (lowValue == (unsigned long)key)
        return &array[low];
    return NULL;
}

template <typename T>
int nlDefaultQSortComparer(const T* pa, const T* pb)
{
    if ((unsigned long)*pa > (unsigned long)*pb)
        return 1;
    if ((unsigned long)*pa == (unsigned long)*pb)
        return 0;
    return -1;
}

template <typename T>
inline void nlQSort(T* array, int count, int (*comparefunc)(const T*, const T*))
{
    qsort(array, count, sizeof(T), (int (*)(const void*, const void*))comparefunc);
}

#endif // NL_ALGORITHM_H

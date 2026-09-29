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
        if (array[probe] > key)
            high = probe;
        else
            low = probe;
    }
    if (array[high] == key)
        return &array[high];
    if (low == -1)
        return NULL;
    if (array[low] == key)
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

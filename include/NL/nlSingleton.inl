#ifndef NL_SINGLETON_INL
#define NL_SINGLETON_INL

#include "NL/nlSingleton.h"

template <class T>
inline T* nlSingleton<T>::GetInstance()
{
    return s_pInstance;
}

#endif // NL_SINGLETON_INL

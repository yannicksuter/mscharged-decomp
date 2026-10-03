#ifndef GAME_TWEAK_BINDING_FLOAT_H
#define GAME_TWEAK_BINDING_FLOAT_H

#include "Game/TweakValueFloat.h"

template <typename T>
inline int TweakBinding<T>::GetValueType()
{
    return 5;
}

template <typename T>
inline int TweakBinding<T>::GetStorageKind()
{
    return 2;
}

template <typename T>
inline T TweakBinding<T>::GetDefault()
{
    return 0.0f;
}

template <typename T>
inline TweakValueBase* TweakBinding<T>::CreateValue(const char* name, void* entry)
{
    TweakValueFloat* created = new (gTweakValueAllocator->Allocate(sizeof(TweakValueFloat))) TweakValueFloat(name, 0.0f);
    AddTweakValue((TweakEntry*)entry, created);
    return created;
}

template <typename T>
inline void TweakBinding<T>::CopyValueFrom(TweakValueBase* other)
{
    switch (other->GetStorageKind())
    {
    case 1:
        *m_pValue = ((TweakValueFloat*)other)->value;
        break;
    case 2:
        *m_pValue = *((TweakFloatBinding*)other)->m_pValue;
        break;
    }
}

template <typename T>
inline void* TweakBinding<T>::GetValueAddress()
{
    return m_pValue;
}

extern char gTweakFloatBindingFormat[];

template <typename T>
inline void TweakBinding<T>::FormatValue(char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, gTweakFloatBindingFormat, *m_pValue);
}

template <typename T>
inline void TweakBinding<T>::ParseValue(const char* string)
{
    *m_pValue = (float)atof(string);
}

template <typename T>
inline int TweakBinding<T>::IsBound()
{
    return m_pValue != 0;
}

template <typename T>
inline void TweakBinding<T>::UnidentifiedVirtual14(float* minimum, float* maximum, float* increment)
{
    *minimum = 0.0f;
    *maximum = 0.0f;
    *increment = 0.0f;
}

template <typename T>
inline void TweakBinding<T>::BindValueAddress(void* value)
{
    m_pValue = (T*)value;
}

#endif // GAME_TWEAK_BINDING_FLOAT_H

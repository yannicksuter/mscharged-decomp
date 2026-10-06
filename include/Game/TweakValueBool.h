#ifndef GAME_TWEAK_VALUE_BOOL_H
#define GAME_TWEAK_VALUE_BOOL_H
#include "Game/TweakValue.h"
inline void ParseOwnedTweakValue(bool& result, const char* value)
{
    if (TweakValueStringEquals(value, "true") || TweakValueStringEquals(value, "triggered")
        || TweakValueStringEquals(value, "on"))
    {
        result = true;
    }
    if (TweakValueStringEquals(value, "false") || TweakValueStringEquals(value, "off"))
    {
        result = false;
    }
}

template <typename T>
class TweakValue : public TweakValueBase
{
public:
    virtual void CopyValueFrom(TweakValueBase*);
    virtual int GetStorageKind();
    virtual int GetValueType();
    virtual void* GetValueAddress();
    virtual void FormatValue(char*, unsigned long);
    virtual void ParseValue(const char*);
    virtual ~TweakValue();
    virtual void UnidentifiedVirtual14(float*, float*, float*);
    virtual void UnidentifiedVirtual18();

    static void operator delete(void* pointer)
    {
        gTweakValueAllocator->m_Pool1.Free(pointer);
    }

    TweakValue(const char* name, const char* category, T value,
        bool formatName = true)
    {
        mValue = value;
        mName = name;
        mFormatName = formatName;
        if (IsTweakRegistryInitialized() == 0)
        {
            void* entry = nlMalloc(0x18, 8, true);
            if (entry != 0)
            {
                QueueTweakValue((TweakPendingValue*)entry, this, category);
            }
        }
        else
        {
            TweakEntry* config = GetTweakRoot();
            TweakEntry* entry = FindOrCreateTweakPath(config, category, 0);
            if (entry != 0)
            {
                AddTweakValue(entry, this);
            }
        }
        gLastTweakCategory = category;
    }

    TweakValue(
        const char* name, const char* category)
        : mValue(T())
    {
        mName = name;
        mFormatName = false;

        if (IsTweakRegistryInitialized() == 0)
        {
            void* entry = nlMalloc(0x18, 8, true);
            if (entry != 0)
                QueueTweakValue((TweakPendingValue*)entry, this, category);
        }
        else
        {
            TweakEntry* config = GetTweakRoot();
            TweakEntry* entry = FindOrCreateTweakPath(config, category, 0);
            if (entry != 0)
                AddTweakValue(entry, this);
        }

        gLastTweakCategory = category;
    }

    TweakValue(const char* name, T value)
    {
        mValue = value;
        mName = name;
    }

    T GetValue() const
    {
        return mValue;
    }

    operator T() const
    {
        return mValue;
    }

    const T& operator=(const T& value)
    {
        mValue = value;
        return mValue;
    }

    T mValue;
};
template <typename T>
inline void TweakValue<T>::CopyValueFrom(
    TweakValueBase* other)
{
    switch (other->GetStorageKind())
    {
    case 1:
        mValue = ((TweakValue<T>*)other)->mValue;
        break;
    case 2:
        mValue = *((typename TweakType<T>::Binding*)other)->m_pValue;
        break;
    }
}

template <typename T>
inline int TweakValue<T>::GetStorageKind()
{
    return 1;
}

template <typename T>
inline int TweakValue<T>::GetValueType()
{
    return TweakType<T>::ID;
}

template <typename T>
inline void* TweakValue<T>::GetValueAddress()
{
    return &mValue;
}

template <typename T>
inline void TweakValue<T>::FormatValue(
    char* buffer, unsigned long size)
{
    FormatOwnedTweakValue(buffer, size, mValue);
}

template <typename T>
inline void TweakValue<T>::ParseValue(const char* value)
{
    ParseOwnedTweakValue(mValue, value);
}

template <typename T>
inline TweakValue<T>::~TweakValue()
{
}

template <typename T>
inline void TweakValue<T>::UnidentifiedVirtual14(
    float* minimum, float* maximum, float* increment)
{
    *minimum = 0.0f;
    *maximum = 0.0f;
    *increment = 0.0f;
}

template <typename T>
inline void TweakValue<T>::UnidentifiedVirtual18()
{
}

#endif // GAME_TWEAK_VALUE_BOOL_H

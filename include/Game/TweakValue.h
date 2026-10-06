#ifndef GAME_TWEAK_VALUE_H
#define GAME_TWEAK_VALUE_H

#include "Game/TweakValueBase.h"
#include "Game/TweakRegistration.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlSmallBlockAllocator.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"
#include "types.h"
#include <stdlib.h>

class InterpreterCore;
class TweakIntBinding;

extern const char* gLastTweakCategory;

typedef nlSmallBlockAllocator<0x10, 0x20, 0x40, 1> TweakValueAllocator3;
typedef nlSmallBlockAllocator<0x10, 0x20, 1, 1> TweakValueAllocator2;
extern TweakValueAllocator3* gTweakValueAllocator;
extern TweakValueAllocator2* gTweakBindingAllocator;

// Shared base of the pool-allocated pointer-backed values. Retail keeps no
// vtable for it: derived constructors elide its vtable store, and derived
// destructors inline its empty destructor.
// It owns the type-independent registration entry points, which only use the
// base fields and the virtuals below.
class TweakBindingBase : public TweakValueBase
{
public:
    virtual int IsBound() = 0;
    virtual TweakValueBase* CreateValue(const char* name,
        void* entry) = 0;
    virtual void BindValueAddress(void* value) = 0;

    bool Bind(const char* path);
    bool Bind(const char*, const char*, bool, float, float, float);

    static void operator delete(void* pointer)
    {
        gTweakBindingAllocator->m_Pool1.Free(pointer);
    }
};

inline void* AllocateTweakValue(unsigned long size)
{
    return gTweakValueAllocator->Allocate(size);
}

inline bool TweakValueStringEquals(const char* value, const char* expected)
{
    return nlStrICmp(value, expected) == 0;
}

inline void FormatTweakBindingValue(char* buffer, unsigned long size, bool* value)
{
    nlSNPrintf(buffer, size, *value ? "true" : "false");
}

inline void ParseTweakBindingValue(bool*& result, const char* value)
{
    if (TweakValueStringEquals(value, "true"))
    {
        *result = true;
    }
    if (TweakValueStringEquals(value, "false"))
    {
        *result = false;
    }
}

inline void FormatOwnedTweakValue(char* buffer, unsigned long size, bool value)
{
    nlSNPrintf(buffer, size, value ? "true" : "false");
}

extern char gTweakFloatBindingFormat[];
inline void FormatTweakBindingValue(char* buffer, unsigned long size, float* value)
{
    nlSNPrintf(buffer, size, gTweakFloatBindingFormat, *value);
}
inline void ParseTweakBindingValue(float*& result, const char* value)
{
    *result = (float)atof(value);
}

template <typename T>
class TweakBinding;
template <typename T>
class TweakValue;
typedef TweakValue<bool> TweakValueBool;
class TweakValueFloat;
template <typename T>
struct TweakType;
template <>
struct TweakType<bool>
{
    enum
    {
        ID = 2
    };
    typedef TweakValueBool OwnedValue;
    typedef TweakBinding<bool> Binding;
    static bool ReadOwned(TweakValueBase* value);
};
template <>
struct TweakType<float>
{
    enum
    {
        ID = 5
    };
    typedef TweakValueFloat OwnedValue;
    typedef TweakBinding<float> Binding;
    static float ReadOwned(TweakValueBase* value);
};
template <>
struct TweakType<int>
{
    enum
    {
        ID = 3
    };
    typedef TweakValue<int> OwnedValue;
    typedef TweakIntBinding Binding;
};
template <typename T>
inline void FormatOwnedTweakValue(char*, unsigned long, T);
inline void ParseOwnedTweakValue(int&, const char*);

template <typename T>
class TweakBinding : public TweakBindingBase
{
public:
    inline TweakBinding(T* value = 0);
    TweakBinding(const char* path, T defaultValue)
        : m_pValue(0)
    {
        mName = 0;
        if (IsTweakRegistryInitialized() && !Bind(path))
        {
            *m_pValue = defaultValue;
        }
    }
    TweakBinding(const char* name, const char* category,
        T* value, bool formatName = false)
        : m_pValue(value)
    {
        mName = name;
        mFormatName = formatName;

        if (IsTweakRegistryInitialized() == 0)
        {
            void* entry = nlMalloc(0x18, 8, true);
            if (entry != 0)
            {
                QueueTweakValue((TweakPendingValue*)entry, this, category);
            }
            gLastTweakCategory = category;
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
    }
    virtual ~TweakBinding()
    {
    }
    virtual int GetValueType()
    {
        return TweakType<T>::ID;
    }
    virtual int GetStorageKind()
    {
        return 2;
    }
    virtual T GetDefault()
    {
        return T();
    }
    virtual TweakValueBase* CreateValue(
        const char* name, void* entry)
    {
        typedef typename TweakType<T>::OwnedValue Value;
        Value* created = new (AllocateTweakValue(sizeof(Value))) Value(name, T());
        AddTweakValue((TweakEntry*)entry, created);
        return created;
    }
    virtual void CopyValueFrom(
        TweakValueBase* other)
    {
        switch (other->GetStorageKind())
        {
        case 1:
            *m_pValue = TweakType<T>::ReadOwned(other);
            break;
        case 2:
            *m_pValue = *((TweakBinding<T>*)other)->m_pValue;
            break;
        }
    }
    virtual void* GetValueAddress()
    {
        return m_pValue;
    }
    virtual void FormatValue(
        char* buffer, unsigned long size)
    {
        FormatTweakBindingValue(buffer, size, m_pValue);
    }
    virtual void ParseValue(const char* value)
    {
        ParseTweakBindingValue(m_pValue, value);
    }
    virtual int IsBound()
    {
        return m_pValue != 0;
    }
    virtual void UnidentifiedVirtual14(
        float* minimum, float* maximum, float* increment)
    {
        *minimum = 0.0f;
        *maximum = 0.0f;
        *increment = 0.0f;
    }
    virtual void BindValueAddress(void* value)
    {
        m_pValue = (T*)value;
    }

    bool Bind(const char* path)
    {
        return TweakBindingBase::Bind(path);
    }

    bool Bind(const char* name, float value,
        const char* group, bool reload, float min, float max)
    {
        bool found = TweakBindingBase::Bind(name, group, reload, value, min, max);
        if (!found)
        {
            *m_pValue = GetDefaultValue();
            return found;
        }
        return found;
    }

    bool BindWithDefault(const char* name, T defaultValue,
        const char* group, bool reload, float value, float min, float max)
    {
        bool found = Bind(name, value, group, reload, min, max);
        if (!found)
        {
            *m_pValue = defaultValue;
        }
        return found;
    }

    const T& operator=(const T& value)
    {
        *m_pValue = value;
        return *m_pValue;
    }

    T GetDefaultValue()
    {
        return GetDefault();
    }

    const T& GetValue() const
    {
        return *m_pValue;
    }

    operator T() const
    {
        return *m_pValue;
    }

public:
    /* 0x0C */ T* m_pValue;

    friend class InterpreterCore;
}; // total size: 0x10
typedef TweakBinding<bool> TweakBoolBinding;
typedef TweakBinding<float> TweakFloatBinding;
class TweakIntBinding : public TweakBindingBase
{
public:
    TweakIntBinding(int* value = 0);
    TweakIntBinding(const char* name, const char* category, int* value,
        bool formatName = false)
    {
        m_pValue = value;
        mName = name;
        mFormatName = formatName;
        if (IsTweakRegistryInitialized() == 0)
        {
            void* entry = nlMalloc(0x18, 8, true);
            if (entry != 0)
            {
                QueueTweakValue((TweakPendingValue*)entry, this, category);
            }
            gLastTweakCategory = category;
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
    }
    virtual int GetValueType();
    virtual int GetStorageKind();
    virtual void UnidentifiedVirtual14(float*, float*, float*);
    virtual void* GetValueAddress();
    virtual void FormatValue(char*, unsigned long);
    virtual void ParseValue(const char*);
    virtual void CopyValueFrom(TweakValueBase*);
    virtual int IsBound();
    virtual TweakValueBase* CreateValue(const char* name,
        void* entry);
    virtual void BindValueAddress(void* value);
    virtual int GetDefault();
    ~TweakIntBinding();

    using TweakBindingBase::Bind;

    bool Bind(const char* name, float value,
        const char* group, bool reload, float min, float max)
    {
        bool found = TweakBindingBase::Bind(name, group, reload, value, min, max);
        if (!found)
        {
            *m_pValue = GetDefault();
            return found;
        }
        return found;
    }

    bool BindWithDefault(const char*, int, const char*, bool, float, float, float);

    const int& GetValue() const
    {
        return *m_pValue;
    }

    operator int() const
    {
        return *m_pValue;
    }

    const int& operator=(const int& value)
    {
        *m_pValue = value;
        return *m_pValue;
    }

public:
    /* 0x0C */ int* m_pValue;

    friend class InterpreterCore;
}; // total size: 0x10

#include "Game/TweakBindingInline.h"
#include "Game/TweakValueBool.h"
#include "Game/TweakValueFloat.h"
inline bool TweakType<bool>::ReadOwned(TweakValueBase* value)
{
    return ((OwnedValue*)value)->mValue;
}
#endif // GAME_TWEAK_VALUE_H

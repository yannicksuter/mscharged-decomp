#ifndef GAME_TWEAK_VALUE_H
#define GAME_TWEAK_VALUE_H

#include "Game/TweakValueBase.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlSmallBlockAllocator.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"
#include "types.h"
#include <stdlib.h>

class InterpreterCore;
class TweakEntry;
class TweakNode;
struct TweakPendingValue;

int IsTweakRegistryInitialized(void);
TweakEntry* GetTweakRoot(void);
void QueueTweakValue(TweakPendingValue*, TweakValueBase*, const char*);
TweakEntry* FindOrCreateTweakPath(TweakEntry*, const char*, int);
void AddTweakValue(TweakEntry*, TweakValueBase*);
TweakNode* FindTweakChild(TweakEntry*, const char*);

extern const char* gLastTweakCategory;

typedef nlSmallBlockAllocator<0x10, 0x20, 0x40, 1> TweakValueAllocator3;
typedef nlSmallBlockAllocator<0x10, 0x20, 1, 1> TweakValueAllocator2;
extern TweakValueAllocator3* gTweakValueAllocator;
extern TweakValueAllocator2* gTweakBindingAllocator;

// Shared base of the pool-allocated pointer-backed values. Retail keeps no
// vtable for it: its constructor and destructor are implicit, so every derived
// constructor elides its vtable store and every derived destructor inlines it.
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
    bool Bind(const char*, float, const char*, bool, float, float);

    static void operator delete(void* pointer)
    {
        gTweakBindingAllocator->m_Pool1.Free(pointer);
    }
};

class TweakFloatBinding : public TweakBindingBase
{
public:
    TweakFloatBinding(float* value = 0)
        : m_pValue(value)
    {
    }
    TweakFloatBinding(const char* path, float defaultValue)
        : m_pValue(0)
    {
        mName = 0;
        if (IsTweakRegistryInitialized() && !Bind(path))
        {
            *m_pValue = defaultValue;
        }
    }
    TweakFloatBinding(const char* name, const char* category, float* value,
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
    virtual float GetDefault();

    using TweakBindingBase::Bind;

    bool Bind(const char* name, float value, const char* group,
        bool reload, float min, float max)
    {
        bool found = TweakBindingBase::Bind(name, value, group, reload, min, max);
        if (!found)
        {
            *m_pValue = GetDefaultValue();
            return found;
        }
        return found;
    }

    bool BindWithDefault(const char* name, float defaultValue,
        const char* group, bool reload, float value, float min, float max)
    {
        bool found = Bind(name, value, group, reload, min, max);
        if (!found)
        {
            *m_pValue = defaultValue;
        }
        return found;
    }

    const float& operator=(const float& value)
    {
        *m_pValue = value;
        return *m_pValue;
    }

    float GetDefaultValue()
    {
        return GetDefault();
    }

    const float& GetValue() const
    {
        return *m_pValue;
    }

    operator float() const
    {
        return *m_pValue;
    }

public:
    /* 0x0C */ float* m_pValue;

    friend class InterpreterCore;
}; // total size: 0x10

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

    using TweakBindingBase::Bind;

    bool Bind(const char* name, float value,
        const char* group, bool reload, float min, float max)
    {
        bool found = TweakBindingBase::Bind(name, value, group, reload, min, max);
        if (!found)
        {
            *m_pValue = GetDefault();
            return found;
        }
        return found;
    }

    bool BindWithDefault(const char*, int, const char*, bool, float, float, float);

    operator int() const
    {
        return *m_pValue;
    }

public:
    /* 0x0C */ int* m_pValue;

    friend class InterpreterCore;
}; // total size: 0x10

class TweakBoolBinding : public TweakBindingBase
{
public:
    TweakBoolBinding(bool* value = 0)
        : m_pValue(value)
    {
    }
    TweakBoolBinding(const char* name, const char* category,
        bool* value, bool formatName)
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
    virtual int GetValueType();
    virtual int GetStorageKind();
    virtual bool GetDefault();
    virtual TweakValueBase* CreateValue(const char* name,
        void* entry);
    virtual void CopyValueFrom(TweakValueBase*);
    virtual void* GetValueAddress();
    virtual void FormatValue(char*, unsigned long);
    virtual void ParseValue(const char*);
    virtual int IsBound();
    virtual void UnidentifiedVirtual14(float*, float*, float*);
    virtual void BindValueAddress(void* value);

    using TweakBindingBase::Bind;

    bool Bind(const char* name, float value,
        const char* group, bool reload, float min, float max)
    {
        bool found = TweakBindingBase::Bind(name, value, group, reload, min, max);
        if (!found)
        {
            *m_pValue = GetDefault();
            return found;
        }
        return found;
    }

    bool BindWithDefault(const char* name, bool defaultValue,
        const char* group, bool reload, float value, float min, float max)
    {
        bool found = Bind(name, value, group, reload, min, max);
        if (!found)
        {
            *m_pValue = defaultValue;
        }
        return found;
    }

public:
    /* 0x0C */ bool* m_pValue;

    friend class InterpreterCore;
}; // total size: 0x10

class TweakValueBool : public TweakValueBase
{
public:
    virtual void CopyValueFrom(TweakValueBase*);
    virtual int GetStorageKind();
    virtual int GetValueType();
    virtual void* GetValueAddress();
    virtual void FormatValue(char*, unsigned long);
    virtual void ParseValue(const char*);
    virtual ~TweakValueBool();
    virtual void UnidentifiedVirtual14(float*, float*, float*);
    virtual void UnidentifiedVirtual18();

    static void operator delete(void* pointer)
    {
        gTweakValueAllocator->m_Pool1.Free(pointer);
    }

    TweakValueBool(const char* name, const char* category, bool value,
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

    TweakValueBool(const char* name, bool value)
    {
        mValue = value;
        mName = name;
    }

    bool GetValue() const
    {
        return mValue;
    }

    operator bool() const
    {
        return mValue;
    }

    const bool& operator=(const bool& value)
    {
        mValue = value;
        return mValue;
    }

    /* 0x0A */ bool mValue;
}; // total size: 0x0C

// Retail Game/tu_80009B34.cpp keeps the bool family's virtual bodies as a
// weak block behind its static initializer, in the order below, and no unit
// defines them out of line.

inline int TweakBoolBinding::GetValueType()
{
    return 2;
}

inline int TweakBoolBinding::GetStorageKind()
{
    return 2;
}

inline bool TweakBoolBinding::GetDefault()
{
    return false;
}

inline TweakValueBase* TweakBoolBinding::CreateValue(
    const char* name, void* entry)
{
    TweakValueBool* created = new (
        gTweakValueAllocator->Allocate(sizeof(TweakValueBool)))
        TweakValueBool(name, false);
    AddTweakValue((TweakEntry*)entry, created);
    return created;
}

inline void TweakBoolBinding::CopyValueFrom(
    TweakValueBase* other)
{
    switch (other->GetStorageKind())
    {
    case 1:
        *m_pValue = ((TweakValueBool*)other)->mValue;
        break;
    case 2:
        *m_pValue = *((TweakBoolBinding*)other)->m_pValue;
        break;
    }
}

inline void* TweakBoolBinding::GetValueAddress()
{
    return m_pValue;
}

inline void TweakBoolBinding::FormatValue(
    char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, *m_pValue ? "true" : "false");
}

inline void TweakBoolBinding::ParseValue(const char* value)
{
    if (nlStrICmp(value, "true") == 0)
    {
        *m_pValue = true;
    }
    if (nlStrICmp(value, "false") == 0)
    {
        *m_pValue = false;
    }
}

inline int TweakBoolBinding::IsBound()
{
    return m_pValue != 0;
}

inline void TweakBoolBinding::UnidentifiedVirtual14(
    float* minimum, float* maximum, float* increment)
{
    *minimum = 0.0f;
    *maximum = 0.0f;
    *increment = 0.0f;
}

inline void TweakBoolBinding::BindValueAddress(void* value)
{
    m_pValue = (bool*)value;
}

inline void TweakValueBool::CopyValueFrom(
    TweakValueBase* other)
{
    switch (other->GetStorageKind())
    {
    case 1:
        mValue = ((TweakValueBool*)other)->mValue;
        break;
    case 2:
        mValue = *((TweakBoolBinding*)other)->m_pValue;
        break;
    }
}

inline int TweakValueBool::GetStorageKind()
{
    return 1;
}

inline int TweakValueBool::GetValueType()
{
    return 2;
}

inline void* TweakValueBool::GetValueAddress()
{
    return &mValue;
}

inline void TweakValueBool::FormatValue(
    char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, mValue ? "true" : "false");
}

inline void TweakValueBool::ParseValue(const char* value)
{
    if (nlStrICmp(value, "true") == 0 || nlStrICmp(value, "triggered") == 0
        || nlStrICmp(value, "on") == 0)
    {
        mValue = true;
    }
    if (nlStrICmp(value, "false") == 0 || nlStrICmp(value, "off") == 0)
    {
        mValue = false;
    }
}

inline TweakValueBool::~TweakValueBool()
{
}

inline void TweakValueBool::UnidentifiedVirtual14(
    float* minimum, float* maximum, float* increment)
{
    *minimum = 0.0f;
    *maximum = 0.0f;
    *increment = 0.0f;
}

inline void TweakValueBool::UnidentifiedVirtual18()
{
}

// NOTE: removed stale GXMaterialFloatTweak_804F4190 / TweakValueImpl_804F4DC8 tail
// (origin/main 30bf4de2) - superseded by TweakValueFloat.h (TweakValueFloat)
// and TweakFloatBinding renames above; remote detail preserved in symbols.txt
// (__sinit_Ball_cpp, __arraydtor, full UnidentifiedVirtual set) and in the
// full TweakFloatBinding vtable declaration.

#endif // GAME_TWEAK_VALUE_H

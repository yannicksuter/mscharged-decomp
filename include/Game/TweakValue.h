#ifndef GAME_TWEAK_VALUE_H
#define GAME_TWEAK_VALUE_H

#include "NL/nlMemory.h"
#include "NL/nlSmallBlockAllocator.h"
#include "NL/nlString.h"
#include "types.h"

int nlSNPrintf(char* buffer, unsigned long size, const char* format, ...);

class InterpreterCore;
class TweakEntry_8052BF00;
class TweakNode_8052BEB0;
class TweakValueBase_8052BF70;
struct TweakPendingValue;

extern "C"
{
    int fn_802C0F04(void);
    TweakEntry_8052BF00* fn_802C0E30(void);
    void fn_802C2DF4(TweakPendingValue*, TweakValueBase_8052BF70*, const char*);
    TweakEntry_8052BF00* fn_802C4504(TweakEntry_8052BF00*, const char*, int);
    void fn_802C5780(TweakEntry_8052BF00*, TweakValueBase_8052BF70*);
    TweakNode_8052BEB0* fn_802C5884(TweakEntry_8052BF00*, const char*);
}

extern const char* lbl_806E1E90;

typedef nlSmallBlockAllocator<0x10, 0x20, 0x40, 1> TweakValueAllocator3;
typedef nlSmallBlockAllocator<0x10, 0x20, 1, 1> TweakValueAllocator2;
extern TweakValueAllocator3* lbl_806E1E58;
extern TweakValueAllocator2* lbl_806E1E5C;

class TweakValueBase_8052BF70
{
public:
    TweakValueBase_8052BF70();
    virtual ~TweakValueBase_8052BF70();
    virtual int UnidentifiedVirtual0C();
    virtual int UnidentifiedVirtual10();
    virtual void UnidentifiedVirtual14(
        float* minimum, float* maximum, float* increment)
    {
        *minimum = 0.0f;
        *maximum = 0.0f;
        *increment = 0.0f;
    }
    virtual void UnidentifiedVirtual18()
    {
    }
    virtual void* UnidentifiedVirtual1C()
    {
        return 0;
    }
    virtual void* UnidentifiedVirtual20();
    virtual void UnidentifiedVirtual24(char* buffer, unsigned long size)
    {
        buffer[0] = '\0';
    }
    virtual void UnidentifiedVirtual28(const char* value)
    {
    }
    virtual void UnidentifiedVirtual2C(TweakValueBase_8052BF70*);

public:
    /* 0x04 */ const char* mName;
    /* 0x08 */ u8 mUnidentified008;
    /* 0x09 */ bool mUnidentified009;
}; // total size: 0x0C (0x0A..0x0C tail padding, reused by derived classes)

// Shared base of the pool-allocated pointer-backed values. Retail keeps no
// vtable for it: its constructor and destructor are implicit, so every derived
// constructor elides its vtable store and every derived destructor inlines it.
// It owns the type-independent registration entry points, which only use the
// base fields and the virtuals below.
class UnidentifiedTweakValueImplBase : public TweakValueBase_8052BF70
{
public:
    virtual int UnidentifiedVirtual30() = 0;
    virtual TweakValueBase_8052BF70* UnidentifiedVirtual34(const char* name,
        void* entry) = 0;
    virtual void UnidentifiedVirtual38(void* value) = 0;

    bool fn_802C4F94(const char* path);
    bool fn_802C4FEC(const char*, float, const char*, bool, float, float);

    static void operator delete(void* pointer)
    {
        lbl_806E1E5C->m_Pool1.Free(pointer);
    }
};

class TweakValueImpl_804F4DC8 : public UnidentifiedTweakValueImplBase
{
public:
    TweakValueImpl_804F4DC8(float* value = 0)
        : m_pValue(value)
    {
    }
    TweakValueImpl_804F4DC8(const char* name, const char* category, float* value,
        bool unidentified = false)
    {
        m_pValue = value;
        mName = name;
        mUnidentified009 = unidentified;

        if (fn_802C0F04() == 0)
        {
            void* entry = nlMalloc(0x18, 8, true);
            if (entry != 0)
            {
                fn_802C2DF4((TweakPendingValue*)entry, this, category);
            }
            lbl_806E1E90 = category;
        }
        else
        {
            TweakEntry_8052BF00* config = fn_802C0E30();
            TweakEntry_8052BF00* entry = fn_802C4504(config, category, 0);
            if (entry != 0)
            {
                fn_802C5780(entry, this);
            }
        }
    }
    virtual int UnidentifiedVirtual30();
    virtual TweakValueBase_8052BF70* UnidentifiedVirtual34(const char* name,
        void* entry);
    virtual void UnidentifiedVirtual38(void* value);
    virtual float UnidentifiedVirtual3C();

    bool BindWithDefault(const char* name, float defaultValue,
        const char* group, bool reload, float value, float min, float max)
    {
        bool found = fn_802C4FEC(name, value, group, reload, min, max);
        if (!found)
        {
            *m_pValue = GetDefaultValue();
        }
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
        return UnidentifiedVirtual3C();
    }

    const float& UnidentifiedGetValue() const
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

class TweakValueIntImpl_804FD898 : public UnidentifiedTweakValueImplBase
{
public:
    TweakValueIntImpl_804FD898(int* value = 0);
    TweakValueIntImpl_804FD898(const char* name, const char* category, int* value,
        bool unidentified = false)
    {
        m_pValue = value;
        mName = name;
        mUnidentified009 = unidentified;
        if (fn_802C0F04() == 0)
        {
            void* entry = nlMalloc(0x18, 8, true);
            if (entry != 0)
            {
                fn_802C2DF4((TweakPendingValue*)entry, this, category);
            }
            lbl_806E1E90 = category;
        }
        else
        {
            TweakEntry_8052BF00* config = fn_802C0E30();
            TweakEntry_8052BF00* entry = fn_802C4504(config, category, 0);
            if (entry != 0)
            {
                fn_802C5780(entry, this);
            }
        }
    }
    virtual int UnidentifiedVirtual0C();
    virtual int UnidentifiedVirtual10();
    virtual void UnidentifiedVirtual14(float*, float*, float*);
    virtual void* UnidentifiedVirtual20();
    virtual void UnidentifiedVirtual24(char*, unsigned long);
    virtual void UnidentifiedVirtual28(const char*);
    virtual void UnidentifiedVirtual2C(TweakValueBase_8052BF70*);
    virtual int UnidentifiedVirtual30();
    virtual TweakValueBase_8052BF70* UnidentifiedVirtual34(const char* name,
        void* entry);
    virtual void UnidentifiedVirtual38(void* value);
    virtual int UnidentifiedVirtual3C();

    bool BindWithDefault(const char*, int, const char*, bool, float, float, float);

    operator int() const
    {
        return *m_pValue;
    }

public:
    /* 0x0C */ int* m_pValue;

    friend class InterpreterCore;
}; // total size: 0x10

class TweakValueBoolImpl_804F4538 : public UnidentifiedTweakValueImplBase
{
public:
    TweakValueBoolImpl_804F4538(bool* value = 0);
    TweakValueBoolImpl_804F4538(const char* name, const char* category,
        bool* value, bool defaultValue)
        : m_pValue(value)
    {
        mName = name;
        mUnidentified009 = defaultValue;

        if (fn_802C0F04() == 0)
        {
            void* entry = nlMalloc(0x18, 8, true);
            if (entry != 0)
            {
                fn_802C2DF4((TweakPendingValue*)entry, this, category);
            }
            lbl_806E1E90 = category;
        }
        else
        {
            TweakEntry_8052BF00* config = fn_802C0E30();
            TweakEntry_8052BF00* entry = fn_802C4504(config, category, 0);
            if (entry != 0)
            {
                fn_802C5780(entry, this);
            }
        }
    }
    virtual int UnidentifiedVirtual0C();
    virtual int UnidentifiedVirtual10();
    virtual bool UnidentifiedVirtual3C();
    virtual TweakValueBase_8052BF70* UnidentifiedVirtual34(const char* name,
        void* entry);
    virtual void UnidentifiedVirtual2C(TweakValueBase_8052BF70*);
    virtual void* UnidentifiedVirtual20();
    virtual void UnidentifiedVirtual24(char*, unsigned long);
    virtual void UnidentifiedVirtual28(const char*);
    virtual int UnidentifiedVirtual30();
    virtual void UnidentifiedVirtual14(float*, float*, float*);
    virtual void UnidentifiedVirtual38(void* value);

public:
    /* 0x0C */ bool* m_pValue;

    friend class InterpreterCore;
}; // total size: 0x10

class TweakValueBool_804F4578 : public TweakValueBase_8052BF70
{
public:
    virtual void UnidentifiedVirtual2C(TweakValueBase_8052BF70*);
    virtual int UnidentifiedVirtual10();
    virtual int UnidentifiedVirtual0C();
    virtual void* UnidentifiedVirtual20();
    virtual void UnidentifiedVirtual24(char*, unsigned long);
    virtual void UnidentifiedVirtual28(const char*);
    virtual ~TweakValueBool_804F4578();
    virtual void UnidentifiedVirtual14(float*, float*, float*);
    virtual void UnidentifiedVirtual18();

    static void operator delete(void* pointer)
    {
        lbl_806E1E58->m_Pool1.Free(pointer);
    }

    TweakValueBool_804F4578(const char* name, const char* category, bool value,
        bool unidentified = true)
    {
        mValue = value;
        mName = name;
        mUnidentified009 = unidentified;
        if (fn_802C0F04() == 0)
        {
            void* entry = nlMalloc(0x18, 8, true);
            if (entry != 0)
            {
                fn_802C2DF4((TweakPendingValue*)entry, this, category);
            }
        }
        else
        {
            TweakEntry_8052BF00* config = fn_802C0E30();
            TweakEntry_8052BF00* entry = fn_802C4504(config, category, 0);
            if (entry != 0)
            {
                fn_802C5780(entry, this);
            }
        }
        lbl_806E1E90 = category;
    }

    TweakValueBool_804F4578(const char* name, bool value)
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

    /* 0x0A */ bool mValue;
}; // total size: 0x0C

// Retail Game/tu_80009B88.cpp keeps the bool family's virtual bodies as a
// weak block behind its static initializer, in the order below, and no unit
// defines them out of line.

inline int TweakValueBoolImpl_804F4538::UnidentifiedVirtual0C()
{
    return 2;
}

inline int TweakValueBoolImpl_804F4538::UnidentifiedVirtual10()
{
    return 2;
}

inline bool TweakValueBoolImpl_804F4538::UnidentifiedVirtual3C()
{
    return false;
}

inline TweakValueBase_8052BF70* TweakValueBoolImpl_804F4538::UnidentifiedVirtual34(
    const char* name, void* entry)
{
    TweakValueBool_804F4578* created = new (
        lbl_806E1E58->Allocate(sizeof(TweakValueBool_804F4578)))
        TweakValueBool_804F4578(name, false);
    fn_802C5780((TweakEntry_8052BF00*)entry, created);
    return created;
}

inline void TweakValueBoolImpl_804F4538::UnidentifiedVirtual2C(
    TweakValueBase_8052BF70* other)
{
    switch (other->UnidentifiedVirtual10())
    {
    case 1:
        *m_pValue = ((TweakValueBool_804F4578*)other)->mValue;
        break;
    case 2:
        *m_pValue = *((TweakValueBoolImpl_804F4538*)other)->m_pValue;
        break;
    }
}

inline void* TweakValueBoolImpl_804F4538::UnidentifiedVirtual20()
{
    return m_pValue;
}

inline void TweakValueBoolImpl_804F4538::UnidentifiedVirtual24(
    char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, *m_pValue ? "true" : "false");
}

inline void TweakValueBoolImpl_804F4538::UnidentifiedVirtual28(const char* value)
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

inline int TweakValueBoolImpl_804F4538::UnidentifiedVirtual30()
{
    return m_pValue != 0;
}

inline void TweakValueBoolImpl_804F4538::UnidentifiedVirtual14(
    float* minimum, float* maximum, float* increment)
{
    *minimum = 0.0f;
    *maximum = 0.0f;
    *increment = 0.0f;
}

inline void TweakValueBoolImpl_804F4538::UnidentifiedVirtual38(void* value)
{
    m_pValue = (bool*)value;
}

inline void TweakValueBool_804F4578::UnidentifiedVirtual2C(
    TweakValueBase_8052BF70* other)
{
    switch (other->UnidentifiedVirtual10())
    {
    case 1:
        mValue = ((TweakValueBool_804F4578*)other)->mValue;
        break;
    case 2:
        mValue = *((TweakValueBoolImpl_804F4538*)other)->m_pValue;
        break;
    }
}

inline int TweakValueBool_804F4578::UnidentifiedVirtual10()
{
    return 1;
}

inline int TweakValueBool_804F4578::UnidentifiedVirtual0C()
{
    return 2;
}

inline void* TweakValueBool_804F4578::UnidentifiedVirtual20()
{
    return &mValue;
}

inline void TweakValueBool_804F4578::UnidentifiedVirtual24(
    char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, mValue ? "true" : "false");
}

inline void TweakValueBool_804F4578::UnidentifiedVirtual28(const char* value)
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

inline TweakValueBool_804F4578::~TweakValueBool_804F4578()
{
}

inline void TweakValueBool_804F4578::UnidentifiedVirtual14(
    float* minimum, float* maximum, float* increment)
{
    *minimum = 0.0f;
    *maximum = 0.0f;
    *increment = 0.0f;
}

inline void TweakValueBool_804F4578::UnidentifiedVirtual18()
{
}

#endif // GAME_TWEAK_VALUE_H

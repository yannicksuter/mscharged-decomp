#ifndef GAME_TWEAK_REGISTRY_H
#define GAME_TWEAK_REGISTRY_H

#include "Game/TweakValue.h"
#include "NL/nlSmallBlockAllocator.h"
#include "types.h"

// Interned-string storage kinds. Kind 5 resolves to static or dynamic
// depending on whether the registry state has been pushed.
enum TweakStringKind
{
    kTweakStringStatic = 0,
    kTweakStringDynamic = 1,
    kTweakStringValue = 2,
    kTweakStringFolder = 3,
    kTweakStringCurrent = 5
};

class TweakEntry_8052BF00;
class TweakNode_8052BEB0;
struct TweakPendingValue;
struct TweakRecycledName;

typedef nlSmallBlockAllocator<0x10, 0x20, 1, 1> TweakValueAllocator2;

extern "C"
{
    extern nlSlotPoolFixed<0x10> lbl_8057C66C;
    extern nlSlotPoolFixed<0x20> lbl_8057C6E4;
    extern nlSlotPoolFixed<0x2C> lbl_8057C734;

    extern TweakValueAllocator3* lbl_806E1E58;
    extern TweakValueAllocator2* lbl_806E1E5C;
    extern TweakPendingValue* lbl_806E1E60;
    extern TweakPendingValue* lbl_806E1E64;
    extern TweakRecycledName* lbl_806E1E68;
    extern TweakRecycledName* lbl_806E1E6C;
    extern u8 lbl_806E1E42;

    void fn_802C0CCC(void);
    void* fn_802C0E3C(void);
    TweakEntry_8052BF00* fn_802C0E44(void);
    TweakEntry_8052BF00* fn_802C0E4C(TweakValueBase_8052BF70* value, TweakEntry_8052BF00* parent);
    void fn_802C0F24(int fromEnd, u8 flag, unsigned int* sizes);
    void fn_802C1B98(void);
    void fn_802C1BB0(void);
    void fn_802C1D30(void);
    const char* fn_802C1EBC(const char* str, int kind);
    void fn_802C2080(TweakEntry_8052BF00* entry, const char* name, const char* valueStr);
    int fn_802C250C(const char* str, bool* out);
    int fn_802C269C(const char* str, unsigned int count, int index);
    int fn_802C278C(const char* name, int* outLength);
    const char* fn_802C2914(const char* name, int kind);
    void* fn_802C2B38(const char* name);
    float fn_802C2B48(const char* path, float defaultValue);
    int fn_802C2BE8(const char* path, int defaultValue);
    u8 fn_802C2C84(const char* path, u8 defaultValue);
    const char* fn_802C2D20(const char* path, const char* defaultValue);
    int fn_802C2DBC(const char* path);

    // TU3: node and path management.
    TweakEntry_8052BF00* fn_802C3FF8(TweakEntry_8052BF00* entry, const char* name, int noCreate);
    TweakEntry_8052BF00* fn_802C41B4(TweakEntry_8052BF00* entry, const char* path);
    void fn_802C47E4(TweakNode_8052BEB0* node);

    // Config-file TU.
    void fn_802C56E8(TweakEntry_8052BF00* entry, TweakEntry_8052BF00* child);
    void fn_802C5D74(TweakEntry_8052BF00* entry);
    void* fn_802C595C(TweakEntry_8052BF00* entry, const char* name);
    void fn_802C7480(const char* path, const char** name, char* dir);
    void fn_802C7534(const char* a, const char* b, char* out);

    // Recycled-name TU.
    void fn_802C3970(void);

    // Node TU.
    void fn_802C46C0(TweakNode_8052BEB0* node, char* buffer, unsigned long size);
}

class TweakNode_8052BEB0
{
public:
    TweakNode_8052BEB0();
    virtual ~TweakNode_8052BEB0();
    virtual int UnidentifiedVirtual0C();
    virtual int UnidentifiedVirtual10() { return 1; }
    virtual int UnidentifiedVirtual14() { return 0; }
    virtual TweakEntry_8052BF00* UnidentifiedVirtual18();
    virtual void UnidentifiedVirtual1C() = 0;

    static void operator delete(void* ptr);

    /* 0x04 */ TweakNode_8052BEB0* m_Next;
    /* 0x08 */ TweakEntry_8052BF00* m_Parent;
    /* 0x0C */ TweakValueBase_8052BF70* m_Value;
    /* 0x10 */ int m_Unk10;
    /* 0x14 */ u32 m_PathHash;
    /* 0x18 */ int m_State;
    /* 0x1C */ int m_Unk1C;
}; // size: 0x20

class TweakEntry_8052BF00 : public TweakNode_8052BEB0
{
public:
    TweakEntry_8052BF00();
    virtual ~TweakEntry_8052BF00();
    virtual int UnidentifiedVirtual0C();
    virtual int UnidentifiedVirtual14();
    virtual TweakEntry_8052BF00* UnidentifiedVirtual18();
    virtual void UnidentifiedVirtual1C();

    /* 0x20 */ TweakNode_8052BEB0* m_ChildHead;
    /* 0x24 */ TweakNode_8052BEB0* m_ChildTail;
    /* 0x28 */ int m_Unk28;
}; // size: 0x2C

inline const char* fn_802C3FDC(TweakNode_8052BEB0* node)
{
    return node->m_Value != 0 ? node->m_Value->mName : "ROOT";
}

struct TweakPendingValue
{
    /* 0x00 */ TweakValueBase_8052BF70* m_Value;
    /* 0x04 */ const char* m_Category;
    /* 0x08 */ int m_Unk8;
    /* 0x0C */ TweakPendingValue* m_Next;
    /* 0x10 */ u8 m_Registered;
    /* 0x14 */ union
    {
        u8 m_Default8;
        int m_Default32;
        float m_DefaultFloat;
    };

    static TweakPendingValue* PopHead()
    {
        TweakPendingValue* head = lbl_806E1E60;
        if (head != 0)
        {
            if (head == lbl_806E1E64)
            {
                lbl_806E1E60 = 0;
                lbl_806E1E64 = 0;
            }
            else
            {
                lbl_806E1E60 = head->m_Next;
            }
        }
        return head;
    }
}; // size: 0x18

struct TweakRecycledName
{
    /* 0x00 */ u32 m_Unk0;
    /* 0x04 */ u32 m_Unk4;
    /* 0x08 */ u32 m_Unk8;
    /* 0x0C */ TweakRecycledName* m_Next;
}; // size: 0x10

int nlSNPrintf(char* buffer, unsigned long size, const char* format, ...);

class TweakValueString_8052BD48 : public TweakValueBase_8052BF70
{
public:
    TweakValueString_8052BD48(const char* name, const char* value)
    {
        m_Value = value;
        mName = name;
    }
    virtual ~TweakValueString_8052BD48() { }
    virtual int UnidentifiedVirtual0C() { return 8; }
    virtual int UnidentifiedVirtual10() { return 1; }
    virtual void UnidentifiedVirtual14(float* value, float* min, float* max)
    {
        *value = 0.0f;
        *min = 0.0f;
        *max = 0.0f;
    }
    virtual void UnidentifiedVirtual18() { }
    virtual void* UnidentifiedVirtual20() { return &m_Value; }
    virtual void UnidentifiedVirtual24(char* buffer, unsigned long size)
    {
        nlSNPrintf(buffer, size, "%s", m_Value);
    }
    virtual void UnidentifiedVirtual28(const char* str)
    {
        m_Value = fn_802C1EBC(str, kTweakStringValue);
    }
    virtual void UnidentifiedVirtual2C(TweakValueBase_8052BF70* other)
    {
        switch (other->UnidentifiedVirtual10())
        {
        case 1:
            m_Value = ((TweakValueString_8052BD48*)other)->m_Value;
            break;
        case 2:
            m_Value = *(const char**)((TweakValueImpl_804F4DC8*)other)->m_pValue;
            break;
        }
    }

    static void operator delete(void* ptr) { lbl_806E1E58->m_Pool1.Free(ptr); }

    /* 0x0C */ const char* m_Value;
}; // size: 0x10

// Name-only value attached to folder entries.
class TweakValueName_8052BE78 : public TweakValueBase_8052BF70
{
public:
    TweakValueName_8052BE78(const char* name)
    {
        mName = name;
    }
    static void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }
    virtual ~TweakValueName_8052BE78() { }
    virtual int UnidentifiedVirtual0C() { return 1; }
    virtual int UnidentifiedVirtual10() { return 3; }
    virtual void* UnidentifiedVirtual20() { return 0; }
    virtual void UnidentifiedVirtual24(char*, unsigned long) { }
    virtual void UnidentifiedVirtual28(const char*) { }
    virtual void UnidentifiedVirtual2C(TweakValueBase_8052BF70*) { }
    virtual int UnidentifiedVirtual30() { return 1; }
    virtual int UnidentifiedVirtual34() { return 0; }
}; // size: 0x0C

template <typename T>
T* TweakNodeListRemove(T** head, T* node, T** tail)
{
    if (head == 0)
    {
        return 0;
    }
    T* first = *head;
    if (first == node)
    {
        if (tail != 0 && *tail == first)
        {
            *tail = 0;
        }
        *head = (T*)(*head)->m_Next;
        return 0;
    }
    T* previous = first;
    for (T* current = (T*)first->m_Next; current != 0; current = (T*)current->m_Next)
    {
        if (current == node)
        {
            previous->m_Next = current->m_Next;
            if (tail != 0 && *tail == current)
            {
                *tail = previous;
            }
            return previous;
        }
        previous = current;
    }
    return 0;
}

#endif // GAME_TWEAK_REGISTRY_H

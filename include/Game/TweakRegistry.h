#ifndef GAME_TWEAK_REGISTRY_H
#define GAME_TWEAK_REGISTRY_H

#include "Game/TweakValue.h"
#include "NL/nlPrint.h"
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

class TweakEntry;
class TweakNode;
struct TweakPendingValue;
struct TweakRecycledName;

extern nlSlotPoolFixed<0x10> gTweakNamePool;
extern nlSlotPoolFixed<0x20> gTweakNodePool;
extern nlSlotPoolFixed<0x2C> gTweakEntryPool;

extern TweakValueAllocator3* gTweakValueAllocator;
extern TweakValueAllocator2* gTweakBindingAllocator;
extern TweakPendingValue* gPendingTweakHead;
extern TweakPendingValue* gPendingTweakTail;
extern TweakRecycledName* gRecycledTweakNameHead;
extern TweakRecycledName* gRecycledTweakNameTail;
extern u8 gTweakStatePushed;
extern u8 gDeletePersistentTweakValues;

void ResetDynamicTweaks(void);
TweakNode* GetTweakPriorityNode(void);
TweakEntry* GetUserTweakEntry(void);
TweakEntry* CreateTweakEntry(TweakValueBase* value, TweakEntry* parent);
void ClearTweakRegistryReset(void);
void RegisterPendingTweaks(void);
void BindPendingTweaks(void);
const char* InternTweakString(const char* str, int kind);
void CreateTweakValueFromString(TweakEntry* entry, const char* name, const char* valueStr);
int ParseTweakBool(const char* str, bool* out);
int IsTweakNamePrefix(const char* str, unsigned int count, int index);
int NeedsTweakNameFormatting(const char* name, int* outLength);
const char* FormatTweakName(const char* name, int kind);
void UnregisterTweakValue(TweakValueBase* value);

// TU3: node and path management.
TweakEntry* FindOrCreateTweakChildEntry(TweakEntry* entry, const char* name, int noCreate);
const char* GetTweakNodeName(TweakNode* node);
TweakNode* FindTweakNode(TweakNode* entry, const char* path);
void UpdateTweakNodePathHash(TweakNode* node);

// Entry TU.
void InsertTweakChildSorted(TweakEntry* entry, TweakNode* child);
void AddTweakChild(TweakEntry* entry, TweakNode* child);
void RemoveDynamicTweakChildren(TweakEntry* entry);
void RemoveTweakValue(TweakEntry* entry, TweakValueBase* value);
void ClearTweakChildren(TweakEntry* entry);
void SplitTweakPath(const char* path, const char** leafName, char* directory);
void JoinTweakPath(const char* parentPath, const char* childPath, char* buffer);
int IsTweakNameOnStack(const char* name);

// Recycled-name TU.
void RecycleTweakNames(void);

// Node TU.
void GetTweakNodePath(TweakNode* node, char* buffer, unsigned long size);

void InitializeTweakRegistry(int fromEnd, u8 flag, unsigned int* sizes);
float GetTweakFloat(const char* path, float defaultValue);
int GetTweakInt(const char* path, int defaultValue);
bool GetTweakBool(const char* path, bool defaultValue);
const char* GetTweakString(const char* path, const char* defaultValue);
bool TweakExists(const char* path);

class TweakNode
{
public:
    TweakNode();
    virtual ~TweakNode();
    virtual int UnidentifiedVirtual0C();
    virtual int UnidentifiedVirtual10() { return 1; }
    virtual int UnidentifiedVirtual14() { return 0; }
    virtual TweakEntry* UnidentifiedVirtual18();

    static void operator delete(void* ptr) { gTweakNodePool.Free(ptr); }

    /* 0x04 */ TweakNode* m_Next;
    /* 0x08 */ TweakEntry* m_Parent;
    /* 0x0C */ TweakValueBase* m_Value;
    /* 0x10 */ int m_Depth;
    /* 0x14 */ u32 m_PathHash;
    /* 0x18 */ int m_State;
    /* 0x1C */ int m_Unk1C;
}; // size: 0x20

class TweakEntry : public TweakNode
{
public:
    TweakEntry();
    virtual ~TweakEntry();
    virtual int UnidentifiedVirtual0C();
    virtual int UnidentifiedVirtual14();
    virtual TweakEntry* UnidentifiedVirtual18();
    virtual void UnidentifiedVirtual1C();

    static void operator delete(void* ptr) { gTweakEntryPool.Free(ptr); }

    /* 0x20 */ TweakNode* m_ChildHead;
    /* 0x24 */ TweakNode* m_ChildTail;
    /* 0x28 */ bool m_Unk28;
    /* 0x29 */ bool m_SortChildren;
    /* 0x2A */ u8 m_Pad2A[2];
}; // size: 0x2C

struct TweakPendingValue
{
    /* 0x00 */ TweakValueBase* m_Value;
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
        TweakPendingValue* head = gPendingTweakHead;
        if (head != 0)
        {
            if (head == gPendingTweakTail)
            {
                gPendingTweakHead = 0;
                gPendingTweakTail = 0;
            }
            else
            {
                gPendingTweakHead = head->m_Next;
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


class TweakValueString : public TweakValueBase
{
public:
    TweakValueString(const char* name, const char* value)
    {
        m_Value = value;
        mName = name;
    }
    virtual ~TweakValueString() { }
    virtual int GetValueType() { return 8; }
    virtual int GetStorageKind() { return 1; }
    virtual void UnidentifiedVirtual14(float* value, float* min, float* max)
    {
        *value = 0.0f;
        *min = 0.0f;
        *max = 0.0f;
    }
    virtual void UnidentifiedVirtual18() { }
    virtual void* GetValueAddress() { return &m_Value; }
    virtual void FormatValue(char* buffer, unsigned long size)
    {
        nlSNPrintf(buffer, size, "%s", m_Value);
    }
    virtual void ParseValue(const char* str)
    {
        m_Value = InternTweakString(str, kTweakStringValue);
    }
    virtual void CopyValueFrom(TweakValueBase* other)
    {
        switch (other->GetStorageKind())
        {
        case 1:
            m_Value = ((TweakValueString*)other)->m_Value;
            break;
        case 2:
            m_Value = *(const char**)((TweakFloatBinding*)other)->m_pValue;
            break;
        }
    }

    static void operator delete(void* ptr) { gTweakValueAllocator->m_Pool1.Free(ptr); }

    /* 0x0C */ const char* m_Value;
}; // size: 0x10

// Name-only value attached to folder entries.
class TweakValueName : public TweakValueBase
{
public:
    TweakValueName(const char* name)
    {
        mName = name;
    }
    static void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }
    virtual ~TweakValueName() { }
    virtual int GetValueType() { return 1; }
    virtual int GetStorageKind() { return 3; }
    virtual void* GetValueAddress() { return 0; }
    virtual void FormatValue(char*, unsigned long) { }
    virtual void ParseValue(const char*) { }
    virtual void CopyValueFrom(TweakValueBase*) { }
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

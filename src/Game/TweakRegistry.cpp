#include "Game/TweakRegistry.h"

#include "Game/UnidentifiedStaticStorage.h"

#include "Game/TweakValue.h"
#include "Game/TweakValueFloat.h"
#include "Game/TweakValueInt.h"
#include "NL/nlBasicString.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

#include <ctype.h>
#include <stdlib.h>
#include "NL/nlstring_tmpl.h"

static TweakEntry sTweakRootEntry;

// Interned string storage, indexed by TweakStringKind.
char* gTweakStringBuffers[4];
char* gTweakStringNext[4];
unsigned int gTweakStringCapacities[4];
int gTweakStringBytesUsed[4];
int gTweakStringCounts[4];

static u8 sTweakRegistryStateUninitialized = 1;

TweakNode* gTweakPriorityNode;
TweakEntry* gUserTweakEntry;
u8 gTweakRegistryInitialized;
u8 gDeletePersistentTweakValues;
u8 gTweakStatePushed;
u8 gTweakRegistryResetPending;
u8 gTweakStatePushEnabled;
u8 gResetTweakValueStrings;
unsigned int gTweakStaticStringCapacity;
unsigned int gTweakDynamicStringCapacity;
unsigned int gTweakValueStringCapacity;
unsigned int gTweakFolderStringCapacity;
TweakValueAllocator3* gTweakValueAllocator;
TweakValueAllocator2* gTweakBindingAllocator;
TweakPendingValue* gPendingTweakHead;
TweakPendingValue* gPendingTweakTail;

void ResetDynamicTweaks(void)
{
    RemoveDynamicTweakChildren(&sTweakRootEntry);
    RecycleTweakNames();
    gTweakNamePool.FreeBlocks();
    gTweakEntryPool.FreeBlocks();
    gTweakNodePool.FreeBlocks();
    gTweakValueAllocator->m_Pool1.FreeBlocks();
    gTweakValueAllocator->m_Pool2.FreeBlocks();
    gTweakValueAllocator->m_Pool3.FreeBlocks();
    gTweakBindingAllocator->m_Pool1.FreeBlocks();
    gTweakBindingAllocator->m_Pool2.FreeBlocks();
    nlZeroMemory(gTweakStringBuffers[kTweakStringDynamic], gTweakStringCapacities[kTweakStringDynamic]);
    gTweakStringNext[kTweakStringDynamic] = gTweakStringBuffers[kTweakStringDynamic];
    if (gResetTweakValueStrings)
    {
        nlZeroMemory(gTweakStringBuffers[kTweakStringValue], gTweakStringCapacities[kTweakStringValue]);
        gTweakStringNext[kTweakStringValue] = gTweakStringBuffers[kTweakStringValue];
    }
    gTweakRegistryResetPending = true;
}

TweakEntry* GetTweakRoot(void)
{
    return &sTweakRootEntry;
}

TweakNode* GetTweakPriorityNode(void)
{
    return gTweakPriorityNode;
}

TweakEntry* GetUserTweakEntry(void)
{
    return gUserTweakEntry;
}

TweakEntry* CreateTweakEntry(TweakValueBase* value, TweakEntry* parent)
{
    TweakEntry* entry = new (gTweakEntryPool.Allocate()) TweakEntry;
    entry->m_Value = value;
    UpdateTweakNodePathHash(entry);
    if (parent != 0)
    {
        AddTweakChild(parent, entry);
    }
    return entry;
}

int IsTweakRegistryInitialized(void)
{
    if (sTweakRegistryStateUninitialized)
    {
        gTweakRegistryInitialized = 0;
        sTweakRegistryStateUninitialized = 0;
    }
    return gTweakRegistryInitialized;
}

void InitializeTweakRegistry(int fromEnd, u8 flag, unsigned int* sizes)
{
    gTweakStaticStringCapacity = 0x7800;
    gTweakDynamicStringCapacity = 0x2800;
    gTweakValueStringCapacity = 0x800;
    gTweakFolderStringCapacity = 0x3000;
    if (sizes != 0)
    {
        if (sizes[0] != 0)
        {
            gTweakStaticStringCapacity = sizes[0];
        }
        if (sizes[1] != 0)
        {
            gTweakDynamicStringCapacity = sizes[1];
        }
        if (sizes[2] != 0)
        {
            gTweakValueStringCapacity = sizes[2];
        }
        if (sizes[3] != 0)
        {
            gTweakFolderStringCapacity = sizes[3];
        }
    }
    gTweakStatePushEnabled = flag;

    gTweakValueAllocator = new (nlMalloc(sizeof(TweakValueAllocator3), 8, false)) TweakValueAllocator3;

    TweakValueAllocator2* dynamic_pools = new (nlMalloc(sizeof(TweakValueAllocator2), 8, false)) TweakValueAllocator2;
    gTweakRegistryInitialized = 1;
    gTweakBindingAllocator = dynamic_pools;

    unsigned int i;
    for (i = 0; i < 4; i++)
    {
        switch (i)
        {
        case 0:
            gTweakStringCapacities[i] = gTweakStaticStringCapacity;
            break;
        case 1:
            gTweakStringCapacities[i] = gTweakDynamicStringCapacity;
            break;
        case 2:
            gTweakStringCapacities[i] = gTweakValueStringCapacity;
            break;
        case 3:
            gTweakStringCapacities[i] = gTweakFolderStringCapacity;
            break;
        default:
            gTweakStringCapacities[i] = 0;
            break;
        }
    }

    if (fromEnd == 0)
    {
        for (int j = 0; j < 4; j++)
        {
            gTweakStringBuffers[j] = (char*)nlMalloc(gTweakStringCapacities[j], 8, false);
        }
    }
    else
    {
        for (int j = 0; j < 4; j++)
        {
            gTweakStringBuffers[j] = (char*)nlMalloc(gTweakStringCapacities[j], 8, true);
        }
    }

    for (int j = 0; j < 4; j++)
    {
        gTweakStringNext[j] = gTweakStringBuffers[j];
        gTweakStringBuffers[j][gTweakStringCapacities[j] - 1] = '\0';
    }

    gUserTweakEntry = FindOrCreateTweakChildEntry(&sTweakRootEntry, "User", 0);
    RegisterPendingTweaks();
    BindPendingTweaks();

    TweakPendingValue* pending = TweakPendingValue::PopHead();
    while (pending != 0)
    {
        delete pending;
        pending = TweakPendingValue::PopHead();
    }

    if (flag != 0)
    {
        gTweakEntryPool.PushState();
        gTweakNodePool.PushState();
        gTweakValueAllocator->m_Pool1.PushState();
        gTweakValueAllocator->m_Pool2.PushState();
        gTweakValueAllocator->m_Pool3.PushState();
        gTweakBindingAllocator->m_Pool1.PushState();
        gTweakBindingAllocator->m_Pool2.PushState();
        gTweakNamePool.PushState();
        gTweakStatePushed = 1;
    }
}

void ClearTweakRegistryReset(void)
{
    if (gTweakRegistryResetPending)
    {
        gTweakRegistryResetPending = 0;
    }
}

void RegisterPendingTweaks(void)
{
    TweakValueBase* value;
    TweakPendingValue* pending = gPendingTweakHead;
    while (pending != 0)
    {
        value = pending->m_Value;
        value->UnidentifiedVirtual0C();
        int kind = value->UnidentifiedVirtual10();
        if ((!pending->m_Registered && kind == 1) || (kind == 2 && ((TweakBindingBase*)value)->UnidentifiedVirtual30()))
        {
            if (value->mUnidentified009)
            {
                if (NeedsTweakNameFormatting(value->mName, 0))
                {
                    value->mName = FormatTweakName(value->mName, kTweakStringStatic);
                }
            }
            TweakEntry* entry;
            if (nlStrChr(value->mName, '/') != 0)
            {
                const char* name;
                char dir[0x100];
                char combined[0x100];
                SplitTweakPath(value->mName, &name, dir);
                JoinTweakPath(pending->m_Category, dir, combined);
                entry = FindOrCreateTweakPath(&sTweakRootEntry, combined, 0);
                value->mName = name;
            }
            else
            {
                entry = FindOrCreateTweakPath(&sTweakRootEntry, pending->m_Category, 0);
            }
            if (entry != 0)
            {
                AddTweakValue(entry, value);
            }
            pending->m_Registered = true;
        }
        pending = pending->m_Next;
    }
}

void BindPendingTweaks(void)
{
    TweakPendingValue* pending = TweakPendingValue::PopHead();
    for (; pending != 0; pending = pending->m_Next)
    {
        TweakValueBase* value = pending->m_Value;
        int type = value->UnidentifiedVirtual0C();
        int kind = value->UnidentifiedVirtual10();
        if (!pending->m_Registered && kind == 2)
        {
            if (value->mUnidentified009)
            {
                if (NeedsTweakNameFormatting(value->mName, 0))
                {
                    value->mName = FormatTweakName(value->mName, kTweakStringStatic);
                }
            }
            if (!((TweakBindingBase*)value)
                    ->Bind(value->mName, 0.0f, pending->m_Category, false, 0.0f, 0.0f))
            {
                TweakFloatBinding* impl = (TweakFloatBinding*)value;
                switch (type)
                {
                case 5:
                    *impl->m_pValue = pending->m_DefaultFloat;
                    break;
                case 3:
                    *(int*)impl->m_pValue = pending->m_Default32;
                    break;
                case 2:
                    *(u8*)impl->m_pValue = pending->m_Default8;
                    break;
                case 8:
                    *(u32*)impl->m_pValue = (u32)pending->m_Default32;
                    break;
                }
            }
            pending->m_Registered = true;
        }
    }
}

const char* InternTweakString(const char* str, int kind)
{
    if (kind == kTweakStringCurrent)
    {
        kind = gTweakStatePushed != 0;
    }
    if (kind != kTweakStringValue)
    {
        const char* existing = gTweakStringBuffers[kind];
        while (existing < gTweakStringNext[kind])
        {
            if (nlStrICmp(str, existing) == 0)
            {
                return existing;
            }
            existing += nlStrLen(existing) + 2;
        }
    }
    const char* start = gTweakStringNext[kind];
    while (*str != '\0')
    {
        if (gTweakStringNext[kind] - gTweakStringBuffers[kind] >= (int)gTweakStringCapacities[kind] - 1)
        {
            return start;
        }
        *gTweakStringNext[kind] = *str;
        str++;
        gTweakStringNext[kind]++;
    }
    *gTweakStringNext[kind] = '\0';
    gTweakStringNext[kind]++;
    *gTweakStringNext[kind] = 1;
    gTweakStringNext[kind]++;
    gTweakStringBytesUsed[kind] = gTweakStringNext[kind] - gTweakStringBuffers[kind];
    gTweakStringCounts[kind]++;
    return start;
}

// Interns the name, allocates the value from the shared pool and registers it
// under the entry. Retail evaluates the value argument before the interning
// call in every branch, which only a call boundary reproduces.
template <typename T, typename V>
static T* UnidentifiedCreateValue(TweakEntry* entry, const char* name, V value)
{
    const char* interned = InternTweakString(name, kTweakStringCurrent);
    T* created = new (gTweakValueAllocator->Allocate(sizeof(T))) T(interned, value);
    AddTweakValue(entry, created);
    return created;
}

void CreateTweakValueFromString(TweakEntry* entry, const char* name, const char* valueStr)
{
    int intValue = 0;
    TweakValueBase* value;
    bool boolValue = 0;
    const char* scan;
    float floatValue = 0.0f;
    int isInt;

    for (scan = valueStr; *scan != '\0'; scan++)
    {
        if (!isdigit(*scan) && *scan != '-')
        {
            isInt = 0;
            goto scannedInt;
        }
    }
    intValue = (int)atof(valueStr);
    isInt = 1;
scannedInt:
    if (isInt)
    {
        value = UnidentifiedCreateValue<TweakValueInt>(entry, name, intValue);
    }
    else
    {
        int isFloat = 0;
        for (scan = valueStr; *scan != '\0'; scan++)
        {
            if (*scan == '.' || *scan == ',')
            {
                isFloat = 1;
            }
            else if (!isdigit(*scan) && *scan != '-')
            {
                isFloat = 0;
                goto scannedFloat;
            }
        }
        floatValue = (float)atof(valueStr);
    scannedFloat:
        if (isFloat)
        {
                value = UnidentifiedCreateValue<TweakValueFloat>(entry, name, floatValue);
        }
        else if (ParseTweakBool(valueStr, &boolValue))
        {
                value = UnidentifiedCreateValue<TweakValueBool>(entry, name, boolValue);
        }
        else
        {
                value = UnidentifiedCreateValue<TweakValueString>(entry, name, "");
        }
    }
    value->UnidentifiedVirtual28(valueStr);
}

static const char* sTweakBoolStrings[] = {
    "true",
    "yes",
    "enable",
    "false",
    "no",
    "disable",
    "off",
};

int ParseTweakBool(const char* str, bool* out)
{
    int index = -1;
    switch (tolower((signed char)*str))
    {
    case 't':
        index = 0;
        break;
    case 'y':
        index = 1;
        break;
    case 'e':
        index = 2;
        break;
    case 'f':
        index = 3;
        break;
    case 'n':
        index = 4;
        break;
    case 'd':
        index = 5;
        break;
    case 'o':
        if (tolower((signed char)str[1]) == 'n' && str[2] == '\0')
        {
            *out = 1;
            return 1;
        }
        index = 6;
        break;
    }
    if (index > -1)
    {
        const char* match = sTweakBoolStrings[index];
        for (;;)
        {
            str++;
            match++;
            if (*match != tolower((signed char)*str))
            {
                break;
            }
            if (*match == '\0')
            {
                *out = index <= 2;
                return 1;
            }
        }
    }
    return 0;
}

int IsTweakNamePrefix(const char* str, unsigned int count, int index)
{
    if (count == 0)
    {
        return 1;
    }
    if (index == 0)
    {
        if (str[0] == 'g' || str[0] == 'm' || str[0] == 's')
        {
            if (count == 1)
            {
                return str[1] >= 'A' && str[1] <= 'Z';
            }
            return IsTweakNamePrefix(str, count - 1, index + 1);
        }
    }
    if (str[index] == '_')
    {
        return IsTweakNamePrefix(str, count - 1, index + 1);
    }
    if (str[index] == 'f' || str[index] == 'b' || str[index] == 'i' || str[index] == 'n')
    {
        if (count == 1)
        {
            return str[index + 1] >= 'A' && str[index + 1] <= 'Z';
        }
        return IsTweakNamePrefix(str, count - 1, index + 1);
    }
    return 0;
}

int NeedsTweakNameFormatting(const char* name, int* outLength)
{
    int found = 0;
    unsigned int i;

    for (i = 0; i < nlStrLen(name) - 1; i++)
    {
        if (name[i] == '_')
        {
            found = 1;
        }
    }
    for (i = 0; i < nlStrLen(name) - 1; i++)
    {
        bool lower = name[i] >= 'a' && name[i] <= 'z';
        if (lower)
        {
            bool upper = name[i + 1] >= 'A' && name[i + 1] <= 'Z';
            if (upper)
            {
                found = 1;
            }
        }
    }
    if (found)
    {
        int prefixLength;
        for (int length = 3; length >= 1; length--)
        {
            if (IsTweakNamePrefix(name, length, 0))
            {
                prefixLength = length;
                goto haveLength;
            }
        }
        prefixLength = 0;
    haveLength:
        if (outLength != 0)
        {
            *outLength = prefixLength;
        }
        return 1;
    }
    return 0;
}

const char* FormatTweakName(const char* name, int kind)
{
    int prefix = 0;
    NeedsTweakNameFormatting(name, &prefix);

    char buffer[0x48];
    unsigned int out = 0;
    unsigned int pos;

    for (pos = prefix; pos < nlStrLen(name); pos++)
    {
        if (pos > (unsigned int)(prefix + 1) && name[pos] >= 'A' && name[pos] <= 'Z' && name[pos - 1] >= 'a' && name[pos - 1] <= 'z' && ((pos < nlStrLen(name) - 1 && name[pos + 1] >= 'a' && name[pos + 1] <= 'z') || pos == nlStrLen(name) - 1))
        {
            buffer[out++] = ' ';
        }
        if (pos != 0 && name[pos] >= '0' && name[pos] < '9' && name[pos - 1] >= 'A' && name[pos - 1] <= 'z')
        {
            buffer[out++] = ' ';
        }
        if (name[pos] == '_')
        {
            buffer[out++] = '/';
        }
        else
        {
            buffer[out++] = name[pos];
        }
    }
    buffer[out] = '\0';
    return InternTweakString(buffer, kind);
}

void UnregisterTweakValue(TweakValueBase* value)
{
    RemoveTweakValue(&sTweakRootEntry, value);
}

float GetTweakFloat(const char* path, float defaultValue)
{
    TweakNode* entry = FindTweakNode(&sTweakRootEntry, path);
    if (entry == 0)
    {
        return defaultValue;
    }
    int kind = entry->m_Value->UnidentifiedVirtual10();
    if (kind == 1)
    {
        return ((TweakValueFloat*)entry->m_Value)->value;
    }
    if (kind == 2)
    {
        return *((TweakFloatBinding*)entry->m_Value)->m_pValue;
    }
    return defaultValue;
}

int GetTweakInt(const char* path, int defaultValue)
{
    TweakNode* entry = FindTweakNode(&sTweakRootEntry, path);
    if (entry == 0)
    {
        return defaultValue;
    }
    int kind = entry->m_Value->UnidentifiedVirtual10();
    if (kind == 1)
    {
        return ((TweakValueInt*)entry->m_Value)->value;
    }
    if (kind == 2)
    {
        return *(int*)((TweakFloatBinding*)entry->m_Value)->m_pValue;
    }
    return defaultValue;
}

bool GetTweakBool(const char* path, bool defaultValue)
{
    TweakNode* entry = FindTweakNode(&sTweakRootEntry, path);
    if (entry == 0)
    {
        return defaultValue;
    }
    int kind = entry->m_Value->UnidentifiedVirtual10();
    if (kind == 1)
    {
        return ((TweakValueBool*)entry->m_Value)->mValue;
    }
    if (kind == 2)
    {
        return *(bool*)((TweakFloatBinding*)entry->m_Value)->m_pValue;
    }
    return defaultValue;
}

const char* GetTweakString(const char* path, const char* defaultValue)
{
    TweakNode* entry = FindTweakNode(&sTweakRootEntry, path);
    if (entry == 0)
    {
        return defaultValue;
    }
    int kind = entry->m_Value->UnidentifiedVirtual10();
    if (kind == 1)
    {
        return ((TweakValueString*)entry->m_Value)->m_Value;
    }
    if (kind == 2)
    {
        return *(const char**)((TweakFloatBinding*)entry->m_Value)->m_pValue;
    }
    return defaultValue;
}

bool TweakExists(const char* path)
{
    return FindTweakNode(&sTweakRootEntry, path) != 0;
}

void QueueTweakValue(TweakPendingValue* pending, TweakValueBase* value, const char* category)
{
    pending->m_Value = value;
    pending->m_Category = category;
    pending->m_Unk8 = 0;
    pending->m_Next = 0;
    pending->m_Registered = 0;
    if (gPendingTweakHead == 0 && gPendingTweakTail == 0)
    {
        gPendingTweakHead = pending;
        gPendingTweakTail = pending;
    }
    else
    {
        gPendingTweakTail->m_Next = pending;
        gPendingTweakTail = pending;
    }
}

int TweakNode::UnidentifiedVirtual0C()
{
    return 0;
}

TweakEntry* TweakNode::UnidentifiedVirtual18()
{
    return 0;
}

static TweakIntBinding sStaticStringBytes("Static String Mem Used", "/Registry/Stats/Bytes", &gTweakStringBytesUsed[kTweakStringStatic]);
static TweakIntBinding sDynamicStringBytes("Dynamic String Mem Used", gLastTweakCategory, &gTweakStringBytesUsed[kTweakStringDynamic]);
static TweakIntBinding sValueStringBytes("String Value Mem Used", gLastTweakCategory, &gTweakStringBytesUsed[kTweakStringValue]);
static TweakIntBinding sFolderNameBytes("Folder Name Mem Used", gLastTweakCategory, &gTweakStringBytesUsed[kTweakStringFolder]);
static TweakIntBinding sStaticStringCount("Num static strings", "/Registry/Stats/Number", &gTweakStringCounts[kTweakStringStatic]);
static TweakIntBinding sDynamicStringCount("Num dynamic strings", gLastTweakCategory, &gTweakStringCounts[kTweakStringDynamic]);
static TweakIntBinding sValueStringCount("Num value strings", gLastTweakCategory, &gTweakStringCounts[kTweakStringValue]);
static TweakIntBinding sFolderNameCount("Num folder names", gLastTweakCategory, &gTweakStringCounts[kTweakStringFolder]);

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;

#include "Game/TweakRegistry.h"

#include "Game/UnidentifiedStaticStorage.h"

#include "Game/TweakValue.h"
#include "NL/glx/GXMaterialCrystalTweaks.h"
#include "NL/glx/GXMaterialShadowTweaks.h"
#include "NL/nlBasicString.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

#include <ctype.h>
#include <stdlib.h>

static TweakEntry_8052BF00 lbl_8057C4E4;

// Interned string storage, indexed by TweakStringKind.
char* lbl_8057C510[4];
char* lbl_8057C520[4];
unsigned int lbl_8057C530[4];
int lbl_8057C540[4];
int lbl_8057C550[4];

static u8 lbl_806DF310 = 1;

extern "C"
{
    void* lbl_806E1E38;
    TweakEntry_8052BF00* lbl_806E1E3C;
    u8 lbl_806E1E40;
    u8 lbl_806E1E41;
    u8 lbl_806E1E42;
    u8 lbl_806E1E43;
    u8 lbl_806E1E44;
    u8 lbl_806E1E45;
    unsigned int lbl_806E1E48;
    unsigned int lbl_806E1E4C;
    unsigned int lbl_806E1E50;
    unsigned int lbl_806E1E54;
    TweakValueAllocator3* lbl_806E1E58;
    TweakValueAllocator2* lbl_806E1E5C;
    TweakPendingValue* lbl_806E1E60;
    TweakPendingValue* lbl_806E1E64;
}

void fn_802C0CCC(void)
{
    fn_802C5D74(&lbl_8057C4E4);
    fn_802C3970();
    lbl_8057C66C.FreeBlocks();
    lbl_8057C734.FreeBlocks();
    lbl_8057C6E4.FreeBlocks();
    lbl_806E1E58->m_Pool1.FreeBlocks();
    lbl_806E1E58->m_Pool2.FreeBlocks();
    lbl_806E1E58->m_Pool3.FreeBlocks();
    lbl_806E1E5C->m_Pool1.FreeBlocks();
    lbl_806E1E5C->m_Pool2.FreeBlocks();
    nlZeroMemory(lbl_8057C510[kTweakStringDynamic], lbl_8057C530[kTweakStringDynamic]);
    lbl_8057C520[kTweakStringDynamic] = lbl_8057C510[kTweakStringDynamic];
    if (lbl_806E1E45)
    {
        nlZeroMemory(lbl_8057C510[kTweakStringValue], lbl_8057C530[kTweakStringValue]);
        lbl_8057C520[kTweakStringValue] = lbl_8057C510[kTweakStringValue];
    }
    lbl_806E1E43 = true;
}

TweakEntry_8052BF00* fn_802C0E30(void)
{
    return &lbl_8057C4E4;
}

void* fn_802C0E3C(void)
{
    return lbl_806E1E38;
}

TweakEntry_8052BF00* fn_802C0E44(void)
{
    return lbl_806E1E3C;
}

TweakEntry_8052BF00* fn_802C0E4C(TweakValueBase_8052BF70* value, TweakEntry_8052BF00* parent)
{
    TweakEntry_8052BF00* entry = new (lbl_8057C734.Allocate()) TweakEntry_8052BF00;
    entry->m_Value = value;
    fn_802C47E4(entry);
    if (parent != 0)
    {
        fn_802C56E8(parent, entry);
    }
    return entry;
}

int fn_802C0F04(void)
{
    if (lbl_806DF310)
    {
        lbl_806E1E40 = 0;
        lbl_806DF310 = 0;
    }
    return lbl_806E1E40;
}

void fn_802C0F24(int fromEnd, u8 flag, unsigned int* sizes)
{
    lbl_806E1E48 = 0x7800;
    lbl_806E1E4C = 0x2800;
    lbl_806E1E50 = 0x800;
    lbl_806E1E54 = 0x3000;
    if (sizes != 0)
    {
        if (sizes[0] != 0)
        {
            lbl_806E1E48 = sizes[0];
        }
        if (sizes[1] != 0)
        {
            lbl_806E1E4C = sizes[1];
        }
        if (sizes[2] != 0)
        {
            lbl_806E1E50 = sizes[2];
        }
        if (sizes[3] != 0)
        {
            lbl_806E1E54 = sizes[3];
        }
    }
    lbl_806E1E44 = flag;

    lbl_806E1E58 = new (nlMalloc(sizeof(TweakValueAllocator3), 8, false)) TweakValueAllocator3;

    TweakValueAllocator2* dynamic_pools = new (nlMalloc(sizeof(TweakValueAllocator2), 8, false)) TweakValueAllocator2;
    lbl_806E1E40 = 1;
    lbl_806E1E5C = dynamic_pools;

    unsigned int i;
    for (i = 0; i < 4; i++)
    {
        switch (i)
        {
        case 0:
            lbl_8057C530[i] = lbl_806E1E48;
            break;
        case 1:
            lbl_8057C530[i] = lbl_806E1E4C;
            break;
        case 2:
            lbl_8057C530[i] = lbl_806E1E50;
            break;
        case 3:
            lbl_8057C530[i] = lbl_806E1E54;
            break;
        default:
            lbl_8057C530[i] = 0;
            break;
        }
    }

    if (fromEnd == 0)
    {
        for (int j = 0; j < 4; j++)
        {
            lbl_8057C510[j] = (char*)nlMalloc(lbl_8057C530[j], 8, false);
        }
    }
    else
    {
        for (int j = 0; j < 4; j++)
        {
            lbl_8057C510[j] = (char*)nlMalloc(lbl_8057C530[j], 8, true);
        }
    }

    for (int j = 0; j < 4; j++)
    {
        lbl_8057C520[j] = lbl_8057C510[j];
        lbl_8057C510[j][lbl_8057C530[j] - 1] = '\0';
    }

    lbl_806E1E3C = fn_802C3FF8(&lbl_8057C4E4, "User", 0);
    fn_802C1BB0();
    fn_802C1D30();

    TweakPendingValue* pending = TweakPendingValue::PopHead();
    while (pending != 0)
    {
        delete pending;
        pending = TweakPendingValue::PopHead();
    }

    if (flag != 0)
    {
        lbl_8057C734.PushState();
        lbl_8057C6E4.PushState();
        lbl_806E1E58->m_Pool1.PushState();
        lbl_806E1E58->m_Pool2.PushState();
        lbl_806E1E58->m_Pool3.PushState();
        lbl_806E1E5C->m_Pool1.PushState();
        lbl_806E1E5C->m_Pool2.PushState();
        lbl_8057C66C.PushState();
        lbl_806E1E42 = 1;
    }
}

void fn_802C1B98(void)
{
    if (lbl_806E1E43)
    {
        lbl_806E1E43 = 0;
    }
}

void fn_802C1BB0(void)
{
    TweakValueBase_8052BF70* value;
    TweakPendingValue* pending = lbl_806E1E60;
    while (pending != 0)
    {
        value = pending->m_Value;
        value->UnidentifiedVirtual0C();
        int kind = value->UnidentifiedVirtual10();
        if ((!pending->m_Registered && kind == 1) || (kind == 2 && ((UnidentifiedTweakValueImplBase*)value)->UnidentifiedVirtual30()))
        {
            if (value->mUnidentified009)
            {
                if (fn_802C278C(value->mName, 0))
                {
                    value->mName = fn_802C2914(value->mName, kTweakStringStatic);
                }
            }
            TweakEntry_8052BF00* entry;
            if (nlStrChr(value->mName, '/') != 0)
            {
                const char* name;
                char dir[0x100];
                char combined[0x100];
                fn_802C7480(value->mName, &name, dir);
                fn_802C7534(pending->m_Category, dir, combined);
                entry = fn_802C4504(&lbl_8057C4E4, combined, 0);
                value->mName = name;
            }
            else
            {
                entry = fn_802C4504(&lbl_8057C4E4, pending->m_Category, 0);
            }
            if (entry != 0)
            {
                fn_802C5780(entry, value);
            }
            pending->m_Registered = true;
        }
        pending = pending->m_Next;
    }
}

void fn_802C1D30(void)
{
    TweakPendingValue* pending = TweakPendingValue::PopHead();
    for (; pending != 0; pending = pending->m_Next)
    {
        TweakValueBase_8052BF70* value = pending->m_Value;
        int type = value->UnidentifiedVirtual0C();
        int kind = value->UnidentifiedVirtual10();
        if (!pending->m_Registered && kind == 2)
        {
            if (value->mUnidentified009)
            {
                if (fn_802C278C(value->mName, 0))
                {
                    value->mName = fn_802C2914(value->mName, kTweakStringStatic);
                }
            }
            if (!((UnidentifiedTweakValueImplBase*)value)
                    ->fn_802C4FEC(value->mName, 0.0f, pending->m_Category, false, 0.0f, 0.0f))
            {
                TweakValueImpl_804F4DC8* impl = (TweakValueImpl_804F4DC8*)value;
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

const char* fn_802C1EBC(const char* str, int kind)
{
    if (kind == kTweakStringCurrent)
    {
        kind = lbl_806E1E42 != 0;
    }
    if (kind != kTweakStringValue)
    {
        const char* existing = lbl_8057C510[kind];
        while (existing < lbl_8057C520[kind])
        {
            if (nlStrICmp(str, existing) == 0)
            {
                return existing;
            }
            existing += nlStrLen(existing) + 2;
        }
    }
    const char* start = lbl_8057C520[kind];
    while (*str != '\0')
    {
        if (lbl_8057C520[kind] - lbl_8057C510[kind] >= (int)lbl_8057C530[kind] - 1)
        {
            return start;
        }
        *lbl_8057C520[kind] = *str;
        str++;
        lbl_8057C520[kind]++;
    }
    *lbl_8057C520[kind] = '\0';
    lbl_8057C520[kind]++;
    *lbl_8057C520[kind] = 1;
    lbl_8057C520[kind]++;
    lbl_8057C540[kind] = lbl_8057C520[kind] - lbl_8057C510[kind];
    lbl_8057C550[kind]++;
    return start;
}

// Interns the name, allocates the value from the shared pool and registers it
// under the entry. Retail evaluates the value argument before the interning
// call in every branch, which only a call boundary reproduces.
template <typename T, typename V>
static T* UnidentifiedCreateValue(TweakEntry_8052BF00* entry, const char* name, V value)
{
    const char* interned = fn_802C1EBC(name, kTweakStringCurrent);
    T* created = new (lbl_806E1E58->Allocate(sizeof(T))) T(interned, value);
    fn_802C5780(entry, created);
    return created;
}

void fn_802C2080(TweakEntry_8052BF00* entry, const char* name, const char* valueStr)
{
    int intValue = 0;
    TweakValueBase_8052BF70* value;
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
        value = UnidentifiedCreateValue<GXMaterialColourTweak_804FC520>(entry, name, intValue);
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
                value = UnidentifiedCreateValue<GXMaterialFloatTweak_804F4190>(entry, name, floatValue);
        }
        else if (fn_802C250C(valueStr, &boolValue))
        {
                value = UnidentifiedCreateValue<TweakValueBool_804F4578>(entry, name, boolValue);
        }
        else
        {
                value = UnidentifiedCreateValue<TweakValueString_8052BD48>(entry, name, "");
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

int fn_802C250C(const char* str, bool* out)
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

int fn_802C269C(const char* str, unsigned int count, int index)
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
            return fn_802C269C(str, count - 1, index + 1);
        }
    }
    if (str[index] == '_')
    {
        return fn_802C269C(str, count - 1, index + 1);
    }
    if (str[index] == 'f' || str[index] == 'b' || str[index] == 'i' || str[index] == 'n')
    {
        if (count == 1)
        {
            return str[index + 1] >= 'A' && str[index + 1] <= 'Z';
        }
        return fn_802C269C(str, count - 1, index + 1);
    }
    return 0;
}

int fn_802C278C(const char* name, int* outLength)
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
            if (fn_802C269C(name, length, 0))
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

const char* fn_802C2914(const char* name, int kind)
{
    int prefix = 0;
    fn_802C278C(name, &prefix);

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
    return fn_802C1EBC(buffer, kind);
}

void fn_802C2B38(TweakValueBase_8052BF70* value)
{
    fn_802C595C(&lbl_8057C4E4, value);
}

float fn_802C2B48(const char* path, float defaultValue)
{
    TweakEntry_8052BF00* entry = fn_802C41B4(&lbl_8057C4E4, path);
    if (entry == 0)
    {
        return defaultValue;
    }
    int kind = entry->m_Value->UnidentifiedVirtual10();
    if (kind == 1)
    {
        return ((GXMaterialFloatTweak_804F4190*)entry->m_Value)->value;
    }
    if (kind == 2)
    {
        return *((TweakValueImpl_804F4DC8*)entry->m_Value)->m_pValue;
    }
    return defaultValue;
}

int fn_802C2BE8(const char* path, int defaultValue)
{
    TweakEntry_8052BF00* entry = fn_802C41B4(&lbl_8057C4E4, path);
    if (entry == 0)
    {
        return defaultValue;
    }
    int kind = entry->m_Value->UnidentifiedVirtual10();
    if (kind == 1)
    {
        return ((GXMaterialColourTweak_804FC520*)entry->m_Value)->value;
    }
    if (kind == 2)
    {
        return *(int*)((TweakValueImpl_804F4DC8*)entry->m_Value)->m_pValue;
    }
    return defaultValue;
}

u8 fn_802C2C84(const char* path, u8 defaultValue)
{
    TweakEntry_8052BF00* entry = fn_802C41B4(&lbl_8057C4E4, path);
    if (entry == 0)
    {
        return defaultValue;
    }
    int kind = entry->m_Value->UnidentifiedVirtual10();
    if (kind == 1)
    {
        return ((TweakValueBool_804F4578*)entry->m_Value)->mValue;
    }
    if (kind == 2)
    {
        return *(u8*)((TweakValueImpl_804F4DC8*)entry->m_Value)->m_pValue;
    }
    return defaultValue;
}

const char* fn_802C2D20(const char* path, const char* defaultValue)
{
    TweakEntry_8052BF00* entry = fn_802C41B4(&lbl_8057C4E4, path);
    if (entry == 0)
    {
        return defaultValue;
    }
    int kind = entry->m_Value->UnidentifiedVirtual10();
    if (kind == 1)
    {
        return ((TweakValueString_8052BD48*)entry->m_Value)->m_Value;
    }
    if (kind == 2)
    {
        return *(const char**)((TweakValueImpl_804F4DC8*)entry->m_Value)->m_pValue;
    }
    return defaultValue;
}

int fn_802C2DBC(const char* path)
{
    return fn_802C41B4(&lbl_8057C4E4, path) != 0;
}

void fn_802C2DF4(TweakPendingValue* pending, TweakValueBase_8052BF70* value, const char* category)
{
    pending->m_Value = value;
    pending->m_Category = category;
    pending->m_Unk8 = 0;
    pending->m_Next = 0;
    pending->m_Registered = 0;
    if (lbl_806E1E60 == 0 && lbl_806E1E64 == 0)
    {
        lbl_806E1E60 = pending;
        lbl_806E1E64 = pending;
    }
    else
    {
        lbl_806E1E64->m_Next = pending;
        lbl_806E1E64 = pending;
    }
}

int TweakNode_8052BEB0::UnidentifiedVirtual0C()
{
    return 0;
}

TweakEntry_8052BF00* TweakNode_8052BEB0::UnidentifiedVirtual18()
{
    return 0;
}

static TweakValueIntImpl_804FD898 sStaticStringBytes("Static String Mem Used", "/Registry/Stats/Bytes", &lbl_8057C540[kTweakStringStatic]);
static TweakValueIntImpl_804FD898 sDynamicStringBytes("Dynamic String Mem Used", lbl_806E1E90, &lbl_8057C540[kTweakStringDynamic]);
static TweakValueIntImpl_804FD898 sValueStringBytes("String Value Mem Used", lbl_806E1E90, &lbl_8057C540[kTweakStringValue]);
static TweakValueIntImpl_804FD898 sFolderNameBytes("Folder Name Mem Used", lbl_806E1E90, &lbl_8057C540[kTweakStringFolder]);
static TweakValueIntImpl_804FD898 sStaticStringCount("Num static strings", "/Registry/Stats/Number", &lbl_8057C550[kTweakStringStatic]);
static TweakValueIntImpl_804FD898 sDynamicStringCount("Num dynamic strings", lbl_806E1E90, &lbl_8057C550[kTweakStringDynamic]);
static TweakValueIntImpl_804FD898 sValueStringCount("Num value strings", lbl_806E1E90, &lbl_8057C550[kTweakStringValue]);
static TweakValueIntImpl_804FD898 sFolderNameCount("Num folder names", lbl_806E1E90, &lbl_8057C550[kTweakStringFolder]);

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;

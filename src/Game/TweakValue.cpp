#include "Game/TweakValue.h"

#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlString.h"

extern "C"
{
    void fn_802C7480(const char* path, const char** name, char* group);
    void fn_802C7534(const char* left, const char* right, char* destination);
    int fn_802C75F4(const char* name);
    const char* fn_802C1EBC(const char* name, int mode);
    void* fn_802C278C(const char* name, int arg);
    const char* fn_802C2914(const char* name, int arg);
}

typedef struct TweakEntryValue
{
    /* 0x00 */ u32 _unk0;
    /* 0x04 */ u32 _unk4;
    /* 0x08 */ u32 _unk8;
    /* 0x0C */ TweakValueBase_8052BF70* value;
} TweakEntryValue;

extern bool lbl_806E1E42;

bool UnidentifiedTweakValueImplBase::fn_802C4F94(const char* path)
{
    const char* name;
    char group[0x100];
    fn_802C7480(path, &name, group);
    return fn_802C4FEC(name, 0.0f, group, false, 0.0f, 0.0f);
}

bool UnidentifiedTweakValueImplBase::fn_802C4FEC(const char* name, float value,
    const char* group, bool reload, float min, float max)
{
    if (reload)
    {
        if (fn_802C278C(name, 0) != 0)
        {
            const char* resolved;
            if (lbl_806E1E42)
            {
                resolved = fn_802C2914(name, 1);
            }
            else
            {
                resolved = fn_802C2914(name, 0);
            }
            return fn_802C4FEC(resolved, value, group, false, min, max);
        }
    }
    if (nlStrChr(name, '/') != 0)
    {
        const char* leaf;
        char path[0x100];
        char combined[0x100];
        fn_802C7480(name, &leaf, path);
        fn_802C7534(group, path, combined);
        return fn_802C4FEC(leaf, value, combined, false, min, max);
    }
    {
        TweakEntry_8052BF00* entry = fn_802C4504(fn_802C0E30(), group, 0);
        TweakEntryValue* found = (TweakEntryValue*)fn_802C5884(entry, name);
        lbl_806E1E90 = group;
        if (found == 0)
        {
            TweakValueBase_8052BF70* created;
            if (fn_802C75F4(name) != 0)
            {
                name = fn_802C1EBC(name, 5);
            }
            created = UnidentifiedVirtual34(name, entry);
            UnidentifiedVirtual38(created->UnidentifiedVirtual20());
            return false;
        }
        else
        {
            TweakValueBase_8052BF70* existing = found->value;
            UnidentifiedVirtual0C();
            existing->UnidentifiedVirtual0C();
            UnidentifiedVirtual38(existing->UnidentifiedVirtual20());
            return true;
        }
    }
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;

#include "Game/TweakValue.h"
#include "Game/TweakRegistry.h"

#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"

bool TweakBindingBase::Bind(const char* path)
{
    const char* name;
    char group[0x100];
    SplitTweakPath(path, &name, group);
    return Bind(name, 0.0f, group, false, 0.0f, 0.0f);
}

bool TweakBindingBase::Bind(const char* name, float value,
    const char* group, bool reload, float min, float max)
{
    if (reload)
    {
        if (NeedsTweakNameFormatting(name, 0) != 0)
        {
            const char* resolved;
            if (gTweakStatePushed)
            {
                resolved = FormatTweakName(name, 1);
            }
            else
            {
                resolved = FormatTweakName(name, 0);
            }
            return Bind(resolved, value, group, false, min, max);
        }
    }
    if (nlStrChr(name, '/') != 0)
    {
        const char* leaf;
        char path[0x100];
        char combined[0x100];
        SplitTweakPath(name, &leaf, path);
        JoinTweakPath(group, path, combined);
        return Bind(leaf, value, combined, false, min, max);
    }
    {
        TweakEntry* entry = FindOrCreateTweakPath(GetTweakRoot(), group, 0);
        TweakNode* found = FindTweakChild(entry, name);
        gLastTweakCategory = group;
        if (found == 0)
        {
            TweakValueBase* created;
            if (IsTweakNameOnStack(name) != 0)
            {
                name = InternTweakString(name, 5);
            }
            created = UnidentifiedVirtual34(name, entry);
            UnidentifiedVirtual38(created->UnidentifiedVirtual20());
            return false;
        }
        else
        {
            TweakValueBase* existing = found->m_Value;
            UnidentifiedVirtual0C();
            existing->UnidentifiedVirtual0C();
            UnidentifiedVirtual38(existing->UnidentifiedVirtual20());
            return true;
        }
    }
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;

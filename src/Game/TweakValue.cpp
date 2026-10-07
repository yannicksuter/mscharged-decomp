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
    return Bind(name, group, false, 0.0f, 0.0f, 0.0f);
}

bool TweakBindingBase::Bind(const char* name, const char* group,
    bool formatName, float value, float min, float max)
{
    if (formatName)
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
            return Bind(resolved, group, false, value, min, max);
        }
    }
    if (nlStrChr(name, '/') != 0)
    {
        const char* leaf;
        char path[0x100];
        char combined[0x100];
        SplitTweakPath(name, &leaf, path);
        JoinTweakPath(group, path, combined);
        return Bind(leaf, combined, false, value, min, max);
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
            created = CreateValue(name, entry);
            BindValueAddress(created->GetValueAddress());
            return false;
        }
        else
        {
            TweakValueBase* existing = found->m_Value;
            GetValueType();
            existing->GetValueType();
            BindValueAddress(existing->GetValueAddress());
            return true;
        }
    }
}


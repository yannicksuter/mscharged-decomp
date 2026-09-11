#include <revolution/os/OSThread.h>

#include "Game/TweakRegistry.h"

#include "NL/nlString.h"

void SplitTweakPath(const char* path, const char** name, char* dir)
{
    int separator = -1;
    int i = 0;
    nlStrNCpy(dir, path, nlStrLen(path) + 1);
    while (dir[i] != '\0')
    {
        if (dir[i] == '/')
        {
            separator = i;
        }
        ++i;
    }
    dir[separator] = '\0';
    *name = &path[separator + 1];
}

void JoinTweakPath(const char* a, const char* b, char* out)
{
    nlStrNCpy(out, a, 0x100);
    const char* joined = out;
    int length = nlStrLen(joined);
    if (joined[length - 1] != '/')
    {
        out[length] = '/';
        out[length + 1] = '\0';
    }
    if (b[0] == '/')
    {
        nlStrNCat(out, joined, b + 1, 0x100);
    }
    else
    {
        nlStrNCat(out, joined, b, 0x100);
    }
}

int IsTweakNameOnStack(const char* name)
{
    const char* stackBegin = (const char*)OSGetCurrentThread()->stackBegin;
    const char* stackEnd = (const char*)OSGetCurrentThread()->stackEnd;
    return stackEnd <= name && stackBegin >= name;
}

TweakValueBase::TweakValueBase()
    : mName(0)
    , mUnidentified009(false)
{
    mUnidentified008 = IsTweakRegistryInitialized();
}

TweakValueBase::~TweakValueBase()
{
    UnregisterTweakValue(this);
}

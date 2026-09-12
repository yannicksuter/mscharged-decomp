#include <revolution/os/OSThread.h>

#include "Game/TweakValueBase.h"
#include "Game/TweakRegistry.h"

#include "NL/nlString.h"

void SplitTweakPath(const char* path, const char** leafName, char* directory)
{
    int lastSlash = -1;
    int i = 0;
    nlStrNCpy(directory, path, nlStrLen(path) + 1);
    while (directory[i] != '\0')
    {
        if (directory[i] == '/')
        {
            lastSlash = i;
        }
        ++i;
    }
    directory[lastSlash] = '\0';
    *leafName = &path[lastSlash + 1];
}

void JoinTweakPath(const char* parentPath, const char* childPath, char* buffer)
{
    nlStrNCpy(buffer, parentPath, 0x100);
    const char* joined = buffer;
    int length = nlStrLen(joined);
    if (joined[length - 1] != '/')
    {
        buffer[length] = '/';
        buffer[length + 1] = '\0';
    }
    if (childPath[0] == '/')
    {
        nlStrNCat(buffer, joined, childPath + 1, 0x100);
    }
    else
    {
        nlStrNCat(buffer, joined, childPath, 0x100);
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
    , mFormatName(false)
{
    mCreatedAfterRegistryInit = IsTweakRegistryInitialized();
}

TweakValueBase::~TweakValueBase()
{
    UnregisterTweakValue(this);
}

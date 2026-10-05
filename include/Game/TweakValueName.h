#ifndef GAME_TWEAK_VALUE_NAME_H
#define GAME_TWEAK_VALUE_NAME_H

#include "Game/TweakValueBase.h"
#include "NL/nlMemory.h"

// Name-only value attached to folder entries.
// The overrides are declared in reverse of their emission order in TweakNode.
class TweakValueName : public TweakValueBase
{
public:
    TweakValueName(const char* name)
    {
        mName = name;
    }
    static void* operator new(unsigned long size) { return nlMalloc(size, 8, false); }
    virtual int GetValueType() { return 1; }
    virtual int UnidentifiedVirtual30() { return 1; }
    virtual int UnidentifiedVirtual34() { return 0; }
    virtual void FormatValue(char*, unsigned long) { }
    virtual void ParseValue(const char*) { }
    virtual void CopyValueFrom(TweakValueBase*) { }
    virtual int GetStorageKind() { return 3; }
    virtual void* GetValueAddress() { return 0; }
}; // size: 0x0C

#endif // GAME_TWEAK_VALUE_NAME_H

#ifndef GAME_TWEAK_VALUE_BASE_H
#define GAME_TWEAK_VALUE_BASE_H

#include "types.h"

extern const char* gLastTweakCategory;

void SplitTweakPath(const char* path, const char** leafName, char* directory);
void JoinTweakPath(const char* parentPath, const char* childPath, char* buffer);
int IsTweakNameOnStack(const char* name);

enum eTweakStorageKind
{
    TWEAK_STORAGE_OWNED = 1,
    TWEAK_STORAGE_BINDING = 2,
    TWEAK_STORAGE_FOLDER = 3,
};

enum eTweakValueType
{
    TWEAK_TYPE_FOLDER = 1,
    TWEAK_TYPE_BOOL = 2,
    TWEAK_TYPE_INT = 3,
    TWEAK_TYPE_UINT = 4,
    TWEAK_TYPE_FLOAT = 5,
    TWEAK_TYPE_STRING = 8,
};

class TweakValueBase
{
public:
    TweakValueBase();
    virtual ~TweakValueBase();
    // Value type discriminator shared by owned values and bindings.
    virtual int GetValueType() = 0;
    virtual int GetStorageKind() = 0;
    // These three slots have no established semantic names.
    virtual void GetFloatParameters(
        float* minimum, float* maximum, float* increment)
    {
        *minimum = 0.0f;
        *maximum = 0.0f;
        *increment = 0.0f;
    }
    virtual void ReservedValueHook()
    {
    }
    virtual void* ReservedValueQuery();
    virtual void* GetValueAddress() = 0;
    virtual void FormatValue(char* buffer, unsigned long size)
    {
        buffer[0] = '\0';
    }
    virtual void ParseValue(const char* value)
    {
    }
    virtual void CopyValueFrom(TweakValueBase*) = 0;

public:
    /* 0x04 */ const char* mName;
    // Construction-time registry state, consulted when deleting a node value.
    /* 0x08 */ u8 mCreatedAfterRegistryInit;
    // Request identifier-to-path formatting during pending registration.
    /* 0x09 */ bool mFormatName;
}; // total size: 0x0C (0x0A..0x0C tail padding, reused by derived classes)

#endif // GAME_TWEAK_VALUE_BASE_H

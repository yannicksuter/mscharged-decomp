#ifndef GAME_TWEAK_VALUE_BASE_H
#define GAME_TWEAK_VALUE_BASE_H

#include "types.h"

class TweakValueBase
{
public:
    TweakValueBase();
    virtual ~TweakValueBase();
    // Value type discriminator shared by owned values and bindings.
    virtual int GetValueType() = 0;
    // 1: owned value, 2: pointer binding, 3: folder name.
    virtual int GetStorageKind() = 0;
    // These three slots have no established semantic names.
    virtual void UnidentifiedVirtual14(
        float* minimum, float* maximum, float* increment)
    {
        *minimum = 0.0f;
        *maximum = 0.0f;
        *increment = 0.0f;
    }
    virtual void UnidentifiedVirtual18()
    {
    }
    virtual void* UnidentifiedVirtual1C()
    {
        return 0;
    }
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

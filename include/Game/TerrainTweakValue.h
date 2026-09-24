#ifndef GAME_TERRAIN_TWEAK_VALUE_H
#define GAME_TERRAIN_TWEAK_VALUE_H

#include "Game/TweakRegistry.h"

class TerrainTweakValueBase : public TweakValueBase
{
public:
    TerrainTweakValueBase(const char* name, const char* category, int value)
        : mValue(value)
    {
        mName = name;
        mFormatName = false;

        if (IsTweakRegistryInitialized() == 0)
        {
            void* entry = nlMalloc(0x18, 8, true);
            if (entry != 0)
            {
                QueueTweakValue((TweakPendingValue*)entry, this, category);
            }
        }
        else
        {
            TweakEntry* config = GetTweakRoot();
            TweakEntry* entry = FindOrCreateTweakPath(config, category, 0);
            if (entry != 0)
            {
                AddTweakValue(entry, this);
            }
        }

        gLastTweakCategory = category;
    }

    virtual void UnidentifiedVirtual14(
        float* minimum, float* maximum, float* increment);
    virtual void UnidentifiedVirtual18();
    virtual void CopyValueFrom(TweakValueBase*);
    virtual int GetStorageKind();
    virtual int GetValueType();
    virtual void* GetValueAddress();
    virtual void FormatValue(char* buffer, unsigned long size);
    virtual void ParseValue(const char*);

    static void operator delete(void* pointer)
    {
        gTweakValueAllocator->m_Pool1.Free(pointer);
    }

    /* 0x0C */ int mValue;
}; // total size: 0x10

class TerrainTweakValue : public TerrainTweakValueBase
{
public:
    virtual ~TerrainTweakValue() { }

    TerrainTweakValue(
        const char* name, const char* category, int value, char**)
        : TerrainTweakValueBase(name, category, value)
    {
        mFormatName = true;
    }

    virtual int GetValueType() { return 4; }
    virtual int GetStorageKind() { return 1; }
    virtual void* UnidentifiedVirtual1C() { return 0; }
    virtual void* GetValueAddress() { return &mValue; }
    virtual void CopyValueFrom(TweakValueBase* other)
    {
        if (other->GetValueType() == 4)
        {
            switch (other->GetStorageKind())
            {
            case 1:
                mValue = ((TerrainTweakValue*)other)->mValue;
                break;
            case 2:
                mValue = *((TweakIntBinding*)other)->m_pValue;
                break;
            }
        }

        if (other->GetValueType() == 8)
        {
            const char* value = 0;
            switch (other->GetStorageKind())
            {
            case 1:
                value = ((TweakValueString*)other)->m_Value;
                break;
            case 2:
                value = *(const char**)((TweakFloatBinding*)other)->m_pValue;
                break;
            }
            if (value != 0)
            {
                ParseValue(value);
            }
        }
    }
    virtual void FormatValue(char* buffer, unsigned long size)
    {
        nlSNPrintf(buffer, size, "%d", mValue);
    }
    virtual void ParseValue(const char*) { }
};

inline void TerrainTweakValueBase::UnidentifiedVirtual14(
    float* minimum, float* maximum, float* increment)
{
    *minimum = 0.0f;
    *maximum = 0.0f;
    *increment = 0.0f;
}

inline void TerrainTweakValueBase::UnidentifiedVirtual18()
{
}

#endif // GAME_TERRAIN_TWEAK_VALUE_H

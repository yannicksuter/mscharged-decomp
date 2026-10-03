#ifndef GAME_TWEAK_VALUE_FLOAT_H
#define GAME_TWEAK_VALUE_FLOAT_H

#include "Game/TweakValue.h"
#include "NL/nlMemory.h"
#include "NL/nlFormat.h"

class TweakValueFloat : public TweakValueBase
{
public:
    TweakValueFloat(const char* name, const char* category,
        float initialValue = 1.0f, bool formatName = true)
        : value(initialValue)
    {
        mName = name;
        mFormatName = formatName;

        if (IsTweakRegistryInitialized() == 0)
        {
            void* entry = nlMalloc(0x18, 8, true);
            if (entry != 0)
                QueueTweakValue((TweakPendingValue*)entry, this, category);
        }
        else
        {
            TweakEntry* config = GetTweakRoot();
            TweakEntry* entry = FindOrCreateTweakPath(config, category, 0);
            if (entry != 0)
                AddTweakValue(entry, this);
        }

        gLastTweakCategory = category;
    }
    TweakValueFloat(const char* name, float initialValue)
        : value(initialValue)
    {
        mName = name;
    }
    virtual ~TweakValueFloat()
    {
    }
    virtual void CopyValueFrom(
        TweakValueBase* other)
    {
        switch (other->GetStorageKind())
        {
        case 1:
            value = ((TweakValueFloat*)other)->value;
            break;
        case 2:
            value = *((TweakFloatBinding*)other)->m_pValue;
            break;
        }
    }

    virtual int GetStorageKind()
    {
        return 1;
    }
    virtual int GetValueType()
    {
        return 5;
    }
    virtual void* GetValueAddress()
    {
        return &value;
    }
    virtual void FormatValue(
        char* buffer, unsigned long size)
    {
        nlSNPrintf(buffer, size, "%.3f", value);
    }
    virtual void ParseValue(
        const char* string)
    {
        value = (float)atof(string);
    }
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

    operator float() const
    {
        return value;
    }

    static void operator delete(void* pointer)
    {
        gTweakValueAllocator->m_Pool1.Free(pointer);
    }

    /* 0x0C */ float value;
}; // size: 0x10

#include "Game/TweakBindingFloat.h"

#endif // GAME_TWEAK_VALUE_FLOAT_H

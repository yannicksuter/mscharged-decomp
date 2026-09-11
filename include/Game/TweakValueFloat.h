#ifndef GAME_TWEAK_VALUE_FLOAT_H
#define GAME_TWEAK_VALUE_FLOAT_H

#include "Game/TweakValue.h"
#include "NL/nlMemory.h"

class TweakValueFloat : public TweakValueBase
{
public:
    TweakValueFloat(const char* name, const char* category,
        float initialValue = 1.0f, bool unidentified = true)
        : value(initialValue)
    {
        mName = name;
        mUnidentified009 = unidentified;

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
    virtual ~TweakValueFloat();
    virtual int UnidentifiedVirtual0C();
    virtual int UnidentifiedVirtual10();
    virtual void UnidentifiedVirtual14(float*, float*, float*);
    virtual void UnidentifiedVirtual18();
    virtual void* UnidentifiedVirtual20();
    virtual void UnidentifiedVirtual24(char*, unsigned long);
    virtual void UnidentifiedVirtual28(const char*);
    virtual void UnidentifiedVirtual2C(TweakValueBase*);

    static void operator delete(void* pointer)
    {
        gTweakValueAllocator->m_Pool1.Free(pointer);
    }

    /* 0x0C */ float value;
}; // size: 0x10

#endif // GAME_TWEAK_VALUE_FLOAT_H

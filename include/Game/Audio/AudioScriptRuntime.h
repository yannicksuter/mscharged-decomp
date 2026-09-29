#ifndef GAME_AUDIO_AUDIO_SCRIPT_RUNTIME_H
#define GAME_AUDIO_AUDIO_SCRIPT_RUNTIME_H

#include "Game/InterpreterCore.h"
#include "Game/Audio/AudioEffect.h"
#include "NL/nlAVLTree.h"
#include "types.h"

// Selects effect bindings for sound instances from the audio bundle script.

class AudioEffectBase;
class AudioBindingNode;

// Callback used when an effect is first created for a binding. The middle
// field is left uninitialised by the construction retail emits.
inline bool NotifyNewEffectSoundStarted(u32 key, AudioEffectBase* effect)
{
    effect->OnSoundStarted((void*)key);
    return true;
}

struct AudioEffectSoundStartedVisitor
{
    AudioEffectSoundStartedVisitor(AudioEffectBase* effect)
        : mCallback(NotifyNewEffectSoundStarted)
        , mEffect(effect)
    {
    }

    bool operator()(const u32& key, bool*) const
    {
        return mCallback(key, mEffect);
    }

    /* 0x00 */ bool (*mCallback)(u32, AudioEffectBase*);
    /* 0x04 */ bool mUnidentified04;
    /* 0x08 */ AudioEffectBase* mEffect;
}; // size: 0x0C

// Bytecode interpreter embedded by the runtime.
class AudioScriptInterpreter : public InterpreterCore
{
public:
    AudioScriptInterpreter(unsigned int size)
        : InterpreterCore(size)
    {
    }

    virtual void DoFunctionCall(unsigned int index)
    {
        u32 value = m_SP[-1];
        ((u8*)m_SP)[-1] = value != 0;
        if (m_RunState == 3)
            m_SP[-1] = value;
    }
};

// Per-definition binding: the set of playing sound instances and the live
// effects, keyed by instance handle and effect id. The instance set stores a
// one-byte value that no reader uses.
class AudioEffectBinding
{
public:
    AudioEffectBinding()
        : mInstances(16, 16)
        , mEffects(16, 16)
    {
    }

    struct UpdateState
    {
        float mDeltaTime;
        struct Entry
        {
            u32 mKey;
            AudioEffectBase* mEffect;
        } mEntries[8];
        u32 mCount;
    };

    bool IsEmpty() const
    {
        return mInstances.m_Root == 0 && mEffects.m_Root == 0;
    }

    bool WalkEffects(const Function2<bool, const u32&, AudioEffectBase**>& callback)
    {
        return mEffects.Walk(callback);
    }

    void Unidentified8418(u32 key);
    void OnSoundStarted(u32 instance);
    void OnSoundStopped(u32 instance);
    void Update(float deltaTime);
    void Destroy();
    void ReleaseEffect(const u32& key, AudioEffectBase** effect);
    bool UpdateEffect(const u32& key, AudioEffectBase** effect,
        UpdateState* update);

    /* 0x00 */ nlAVLTreeSlotPool<u32, bool, DefaultKeyCompare<u32> > mInstances;
    /* 0x24 */ nlAVLTreeSlotPool<u32, AudioEffectBase*,
        DefaultKeyCompare<u32> > mEffects;
}; // size: 0x48

class AudioBindingNode
{
public:
    AudioEffectBinding* mBinding;
    AudioBindingNode* m_next;
    AudioBindingNode* m_prev;
};

struct AudioScriptEntry
{
    u32 mHash;
    void* mData;

    operator u32() const { return mHash; }
    bool operator==(const u32& key) const { return mHash == key; }
};

struct AudioScriptBindingList
{
    bool Contains(const u32& key) const;

    u16 mCount;
    u16 mUnidentified02;
    u32* mValues;
};

struct AudioScriptSelection
{
    u16 mCount;
    u16 mConditionCount;
    u32 mValues[1];
};

struct AudioScriptCondition
{
    u32 mKey;
    u16 mFunction;
    u16 mCount;
    u32 mArguments[1];
};

struct AudioScriptUpdateState
{
    u32 mKeys[8];
    float mDeltaTime;
    u32 mCount;
};

class AudioScriptRuntime
{
public:
    AudioScriptRuntime()
        : mBindings(16, 16)
        , mInstanceBindings(16, 16)
        , mContextValues(16, 16)
        , mInterpreter(100)
    {
    }

    void Shutdown();
    void DestroyBinding(const u32&, AudioEffectBinding*);
    bool LoadScriptData(void* data, unsigned int size);
    int SetEffectContext(u32 hash, int value);
    void OnSoundStarted(u32 hash, u32 instance);
    bool OnSoundStopped(u32 instance);
    void Update(float deltaTime);

    AudioEffectBinding* GetBinding(const u32& key)
    {
        return mBindings.UnidentifiedAddOrGet(key);
    }

    /* 0x00 */ AudioScriptEntry* mEntries;
    /* 0x04 */ u32 mEntryCount;
    /* 0x08 */ AudioScriptBindingList* mDefaultBindings;
    /* 0x0C */ nlAVLTreeSlotPool<u32, AudioEffectBinding,
        DefaultKeyCompare<u32> > mBindings;
    /* 0x30 */ nlAVLTreeSlotPool<u32, AudioBindingNode*,
        DefaultKeyCompare<u32> > mInstanceBindings;
    /* 0x54 */ nlAVLTreeSlotPool<u32, int,
        DefaultKeyCompare<u32> > mContextValues;
    /* 0x78 */ AudioScriptInterpreter mInterpreter;
}; // size: 0xA0

bool UpdateAudioScriptBinding(const u32& key, AudioEffectBinding* binding,
    AudioScriptUpdateState* update);

#endif // GAME_AUDIO_AUDIO_SCRIPT_RUNTIME_H

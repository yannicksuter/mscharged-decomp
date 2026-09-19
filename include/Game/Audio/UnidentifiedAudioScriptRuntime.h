#ifndef GAME_AUDIO_UNIDENTIFIED_AUDIO_SCRIPT_RUNTIME_H
#define GAME_AUDIO_UNIDENTIFIED_AUDIO_SCRIPT_RUNTIME_H

#include "Game/InterpreterCore.h"
#include "Game/Audio/AudioEffect.h"
#include "NL/nlAVLTree.h"
#include "types.h"

// State object AudioResourceRuntime allocates for the script-dispatch unit.
// The original runtime and packed-record type names are unidentified.

class AudioEffectBase;
class UnidentifiedAudioTransitionState;
class UnidentifiedAudioEffectSetState;

// Callback used when an effect is first created for a binding. The middle
// field is left uninitialised by the construction retail emits.
extern "C" inline bool fn_802F68F0(u32 key, AudioEffectBase* effect)
{
    effect->OnSoundStarted((void*)key);
    return true;
}

struct UnidentifiedAudioInstanceVisitor
{
    UnidentifiedAudioInstanceVisitor(AudioEffectBase* effect)
        : mUnidentified00(fn_802F68F0)
        , mEffect(effect)
    {
    }

    bool operator()(const u32& key, bool*) const
    {
        return mUnidentified00(key, mEffect);
    }

    /* 0x00 */ bool (*mUnidentified00)(u32, AudioEffectBase*);
    /* 0x04 */ bool mUnidentified04;
    /* 0x08 */ AudioEffectBase* mEffect;
}; // size: 0x0C

// Bytecode interpreter embedded by the runtime.
class UnidentifiedAudioInterpreter : public InterpreterCore
{
public:
    UnidentifiedAudioInterpreter(unsigned int size)
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

class UnidentifiedAudioTransitionState
{
public:
    AudioEffectBinding* mBinding;
    UnidentifiedAudioTransitionState* m_next;
    UnidentifiedAudioTransitionState* m_prev;
};

struct UnidentifiedAudioScriptEntry
{
    u32 mUnidentified00;
    void* mData;

    operator u32() const { return mUnidentified00; }
};

struct UnidentifiedAudioScriptList
{
    u16 mCount;
    u16 mUnidentified02;
    u32* mValues;
};

struct UnidentifiedAudioScriptSelection
{
    u16 mCount;
    u16 mConditionCount;
    u32 mValues[1];
};

struct UnidentifiedAudioScriptCondition
{
    u32 mKey;
    u16 mFunction;
    u16 mCount;
    u32 mArguments[1];
};

struct UnidentifiedAudioScriptUpdate
{
    u32 mKeys[8];
    float mDeltaTime;
    u32 mCount;
};

class UnidentifiedAudioScriptRuntime
{
public:
    UnidentifiedAudioScriptRuntime()
        : mBindings(16, 16)
        , mTransitions(16, 16)
        , mEffectSets(16, 16)
        , mInterpreter(100)
    {
    }

    void Unidentified6BC4();
    void Unidentified6DF8(const u32&, AudioEffectBinding*);
    bool Unidentified6E00(void* data, unsigned int size);
    int Unidentified6E98(u32 hash, UnidentifiedAudioEffectSetState* value);
    void Unidentified6F00(u32 hash, u32 instance);
    bool Unidentified77C8(u32 instance);
    void Unidentified78C0(float deltaTime);

    AudioEffectBinding* GetBinding(const u32& key)
    {
        return mBindings.UnidentifiedAddOrGet(key);
    }

    /* 0x00 */ UnidentifiedAudioScriptEntry* mUnidentified00;
    /* 0x04 */ u32 mUnidentified04;
    /* 0x08 */ UnidentifiedAudioScriptList* mUnidentified08;
    /* 0x0C */ nlAVLTreeSlotPool<u32, AudioEffectBinding,
        DefaultKeyCompare<u32> > mBindings;
    /* 0x30 */ nlAVLTreeSlotPool<u32, UnidentifiedAudioTransitionState*,
        DefaultKeyCompare<u32> > mTransitions;
    /* 0x54 */ nlAVLTreeSlotPool<u32, UnidentifiedAudioEffectSetState*,
        DefaultKeyCompare<u32> > mEffectSets;
    /* 0x78 */ UnidentifiedAudioInterpreter mInterpreter;
}; // size: 0xA0

#endif // GAME_AUDIO_UNIDENTIFIED_AUDIO_SCRIPT_RUNTIME_H

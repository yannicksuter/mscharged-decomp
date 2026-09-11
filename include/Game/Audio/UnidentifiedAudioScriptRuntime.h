#ifndef GAME_AUDIO_UNIDENTIFIED_AUDIO_SCRIPT_RUNTIME_H
#define GAME_AUDIO_UNIDENTIFIED_AUDIO_SCRIPT_RUNTIME_H

#include "Game/InterpreterCore.h"
#include "NL/nlAVLTree.h"
#include "types.h"

// State object AudioResourceRuntime allocates for itself and hands to the
// neighbouring script-dispatch unit. Its member functions live in that unit;
// only the layout, the constructor and the template members it forces are
// visible from AudioResourceRuntime.cpp. Every name below is an explicitly
// unidentified placeholder: the stripped DOL keeps the layout and the pool
// element sizes but not the original types.

class AudioEffectBase;
class UnidentifiedAudioEffectInstance;
class UnidentifiedAudioTransitionState;
class UnidentifiedAudioEffectSetState;

// Callback the runtime hands to the instance walk when an effect is created
// for a binding for the first time. Its call operator is defined by the
// script-dispatch unit; only the layout is visible here, and the middle field
// is left uninitialised by the construction retail emits.
extern "C" void fn_802F68F0();

struct UnidentifiedAudioInstanceVisitor
{
    UnidentifiedAudioInstanceVisitor(AudioEffectBase* effect)
        : mUnidentified00(fn_802F68F0)
        , mEffect(effect)
    {
    }

    bool operator()(const u32& key, UnidentifiedAudioEffectInstance** instance) const;

    /* 0x00 */ void (*mUnidentified00)();
    /* 0x04 */ bool mUnidentified04;
    /* 0x08 */ AudioEffectBase* mEffect;
}; // size: 0x0C

// Bytecode interpreter the runtime embeds. DoFunctionCall is defined by the
// script-dispatch unit (fn_802F6B90); the destructor is compiler generated.
class UnidentifiedAudioInterpreter : public InterpreterCore
{
public:
    UnidentifiedAudioInterpreter(unsigned int size)
        : InterpreterCore(size)
    {
    }

    virtual void DoFunctionCall(unsigned int index);
};

// Per-definition binding: the live effect instances and their parameter
// states, both keyed by the lower-cased name hash.
class UnidentifiedAudioEffectBinding
{
public:
    UnidentifiedAudioEffectBinding()
        : mInstances(16, 16)
        , mEffects(16, 16)
    {
    }

    /* 0x00 */ nlAVLTreeSlotPool<u32, UnidentifiedAudioEffectInstance*,
        DefaultKeyCompare<u32> > mInstances;
    /* 0x24 */ nlAVLTreeSlotPool<u32, AudioEffectBase*,
        DefaultKeyCompare<u32> > mEffects;
}; // size: 0x48

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

    /* 0x00 */ u32 mUnidentified00;
    /* 0x04 */ u32 mUnidentified04;
    /* 0x08 */ u32 mUnidentified08;
    /* 0x0C */ nlAVLTreeSlotPool<u32, UnidentifiedAudioEffectBinding,
        DefaultKeyCompare<u32> > mBindings;
    /* 0x30 */ nlAVLTreeSlotPool<u32, UnidentifiedAudioTransitionState*,
        DefaultKeyCompare<u32> > mTransitions;
    /* 0x54 */ nlAVLTreeSlotPool<u32, UnidentifiedAudioEffectSetState*,
        DefaultKeyCompare<u32> > mEffectSets;
    /* 0x78 */ UnidentifiedAudioInterpreter mInterpreter;
}; // size: 0xA0

#endif // GAME_AUDIO_UNIDENTIFIED_AUDIO_SCRIPT_RUNTIME_H

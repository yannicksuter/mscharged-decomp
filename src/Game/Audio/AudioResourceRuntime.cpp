#include "Game/Audio/AudioResourceRuntime.h"

#include "Game/Audio/AudioConfig.h"
#include "Game/Audio/AudioEffects.h"
#include "Game/Audio/UnidentifiedAudioScriptRuntime.h"
#include "Game/Audio/UnidentifiedSoundPools.h"
#include "NL/nlFunction.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "unclassified/tu_802BE64C.h"

AudioResourceRuntime* g_pAudioResourceRuntime;

inline UnidentifiedAudioPoolOwner::~UnidentifiedAudioPoolOwner()
{
    SlotPoolBase::BaseFreeBlocks(
        &UnidentifiedSoundPools<UnidentifiedSoundPoolTag>::sUnidentifiedPool14,
        sizeof(UnidentifiedSoundPoolEntry14));
    SlotPoolBase::BaseFreeBlocks(
        &UnidentifiedSoundPools<UnidentifiedSoundPoolTag>::sUnidentifiedPool40,
        sizeof(UnidentifiedSoundPoolEntry40));
}

// Configuration keys the runtime resolves by lower-cased name hash. The DOL
// keeps only the hashes; the names behind them are not recoverable.
#define AUDIO_DEFINITIONS_KEY 0xD2894EC5
#define AUDIO_EFFECT_KEY 0xFE7CE6FB


// Members of the script runtime, defined by the neighbouring script-dispatch
// unit.
extern "C"
{
    void fn_802F6BC4(UnidentifiedAudioScriptRuntime* script);
    void fn_802F6E00(UnidentifiedAudioScriptRuntime* script, void* data, unsigned int size);
    void fn_802F6E98(UnidentifiedAudioScriptRuntime* script, unsigned long hash, int index);
    void fn_802F6F00(UnidentifiedAudioScriptRuntime* script);
    void fn_802F77C8(UnidentifiedAudioScriptRuntime* script);
    void fn_802F78C0(UnidentifiedAudioScriptRuntime* script, float deltaTime);
}

AudioResourceRuntime::AudioResourceRuntime()
{
    mRoot = 0;
    mUnidentified14 = 1;
    mUnidentified14_1 = 0;
    m_ConfigRoot = 0;
    g_pAudioResourceRuntime = this;
    m_EffectFactory = GetAudioEffectFactory();
    m_Script = new UnidentifiedAudioScriptRuntime;
}

/**
 * Address/Size: 0x802F48FC | size: 0x8
 */
void AudioResourceRuntime::LoadScriptData(void* data, unsigned int size)
{
    fn_802F6E00(m_Script, data, size);
}

/**
 * Address/Size: 0x802F4904 | size: 0x54
 */
extern "C" void fn_802F4904(AudioResourceRuntime* runtime, float deltaTime)
{
    fn_802F78C0(runtime->m_Script, deltaTime);
    runtime->m_EffectFactory->Update(deltaTime);
}

/**
 * Address/Size: 0x802F4958 | size: 0x44
 */
extern "C" void fn_802F4958(AudioResourceRuntime* runtime)
{
    fn_802F6BC4(runtime->m_Script);
    runtime->m_EffectFactory->Shutdown();
}

/**
 * Address/Size: 0x802F499C | size: 0x8
 */
extern "C" void fn_802F499C(AudioResourceRuntime* runtime)
{
    fn_802F6F00(runtime->m_Script);
}

/**
 * Address/Size: 0x802F49A4 | size: 0x8
 */
extern "C" void fn_802F49A4(AudioResourceRuntime* runtime)
{
    fn_802F77C8(runtime->m_Script);
}

/**
 * Address/Size: 0x802F49AC | size: 0x14
 */
void SetAudioEffectContext(unsigned long* hash, int index)
{
    fn_802F6E98(g_pAudioResourceRuntime->m_Script, *hash, index);
}

/**
 * Address/Size: 0x802F49C0 | size: 0x4C4
 *
 * Starts one effect of a definition on a binding: resolves the effect id from
 * the configuration tree, creates the effect through the factory the first
 * time the binding asks for it, tells every instance already running on the
 * binding about the new effect, then pushes the parameter the caller wants.
 */
extern "C" bool fn_802F49C0(const u32* bindingKey, const u32* definitionKey,
    void* parameterData, bool immediate, float value)
{
    u32 key = *bindingKey;
    u32 definition = *definitionKey;
    UnidentifiedAudioEffectBinding* binding
        = g_pAudioResourceRuntime->m_Script->mBindings.UnidentifiedAddOrGet(key);

    u32 slot = AUDIO_EFFECT_KEY;
    u32 definitions = AUDIO_DEFINITIONS_KEY;
    u32 effectId = (u32)((UnidentifiedRegistryContainer*)
        ((UnidentifiedRegistryContainer*)((UnidentifiedRegistryContainer*)g_pAudioResourceRuntime->GetConfigRoot())
                ->Get(definitions)
                .mData)
            ->Get(definition)
            .mData)
        ->Get(slot)
        .mData;

    AudioEffectBase** found;
    AudioEffectBase* effect;
    if (binding->mEffects.FindGet(effectId, &found))
    {
        effect = *found;
    }
    else
    {
        effect = g_pAudioResourceRuntime->m_EffectFactory->CreateEffect(effectId);
        binding->mEffects.Add(effectId, effect);
        binding->mInstances.Walk(
            Function2<bool, const u32&, UnidentifiedAudioEffectInstance**>(
                UnidentifiedAudioInstanceVisitor(effect)));
    }

    AudioEffectParameter* parameter = 0;
    effect->CreateParameter(definition, parameterData, immediate, &parameter);
    parameter->m_State.m_Target.scalar = value;
    parameter->m_State.m_Current.scalar = 0.0f;
    effect->m_Parameters.AddEnd(parameter);
    return true;
}

/**
 * Address/Size: 0x802F4E84 | size: 0xDC4
 *
 * Applies the named transition set: walks every transition it lists, reads the
 * transition's "Time" and "Invert" properties and starts each effect of the
 * transition's "EffectSet" on the binding the entry names.
 */
extern "C" void fn_802F4E84(const unsigned long* hash, bool invert, void* owner)
{
    UnidentifiedRegistryContainer* transitions
        = (UnidentifiedRegistryContainer*)
            ((UnidentifiedRegistryContainer*)
                    g_pAudioResourceRuntime->GetConfigRoot())
                ->Get(nlStringLowerHash("Transitions"))
                .mData;
    UnidentifiedRegistryContainer* set
        = (UnidentifiedRegistryContainer*)transitions->Get(*hash).mData;
    if (set == 0)
    {
        return;
    }

    UnidentifiedRegistryValue time = set->Get(nlStringLowerHash("Time"));
    float duration = time.mType == 5 ? 0.0f : *(float*)&time.mData;

    UnidentifiedRegistryValue list = set->UnnamedList();
    UnidentifiedRegistryIterator_802BFE44 entries;
    ((UnidentifiedRegistryContainer*)list.mData)
        ->GetIterator(&entries, list.mType);
    for (; !entries.IsDone(); entries.Next())
    {
        UnidentifiedRegistryContainer* entry
            = (UnidentifiedRegistryContainer*)entries.GetValue().mData;
        u32 binding
            = (u32)entry->Get(nlStringLowerHash("EffectSet")).mData;
        bool inverted
            = invert != (entry->Get(nlStringLowerHash("Invert")).mData != 0);

        UnidentifiedRegistryValue effects = entry->UnnamedList();
        UnidentifiedRegistryIterator_802BFE44 effect;
        ((UnidentifiedRegistryContainer*)effects.mData)
            ->GetIterator(&effect, effects.mType);
        for (; !effect.IsDone(); effect.Next())
        {
            u32 definition = (u32)effect.GetValue().mData;
            fn_802F49C0(&binding, &definition, owner, inverted, duration);
        }
    }
}

/**
 * Address/Size: 0x802F6680 | size: 0x84
 */
AudioResourceRuntime::~AudioResourceRuntime()
{
}


/**
 * Address/Size: 0x802F6704 | size: 0x68
 */
UnidentifiedRegistryContainer* AudioResourceRuntime::AllocContainer()
{
    return (UnidentifiedRegistryContainer*)
        UnidentifiedSoundPools<UnidentifiedSoundPoolTag>::sUnidentifiedPool14
            .Allocate();
}

/**
 * Address/Size: 0x802F676C | size: 0x68
 */
UnidentifiedRegistryNode_802BE64C* AudioResourceRuntime::AllocNode()
{
    return (UnidentifiedRegistryNode_802BE64C*)
        UnidentifiedSoundPools<UnidentifiedSoundPoolTag>::sUnidentifiedPool40
            .Allocate();
}

/**
 * Address/Size: 0x802F67D4 | size: 0x10
 */
void* AudioResourceRuntime::UnidentifiedVirtual2C(unsigned int size)
{
    return nlMalloc(size, 8, true);
}

/**
 * Address/Size: 0x802F67E4 | size: 0x18
 */
void AudioResourceRuntime::UnidentifiedVirtual30(void* container)
{
    UnidentifiedSoundPools<UnidentifiedSoundPoolTag>::sUnidentifiedPool14.Free(
        (UnidentifiedSoundPoolEntry14*)container);
}

/**
 * Address/Size: 0x802F67FC | size: 0x18
 */
void AudioResourceRuntime::UnidentifiedVirtual34(void* node)
{
    UnidentifiedSoundPools<UnidentifiedSoundPoolTag>::sUnidentifiedPool40.Free(
        (UnidentifiedSoundPoolEntry40*)node);
}

/**
 * Address/Size: 0x802F6814 | size: 0x8
 */
void AudioResourceRuntime::FreeItem(void* data)
{
    nlFree(data);
}

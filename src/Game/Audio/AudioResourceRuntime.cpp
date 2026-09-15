#include "Game/Audio/AudioResourceRuntime.h"
#include "NL/nlFunction.inl"

#include "Game/Audio/AudioConfig.h"
#include "Game/Audio/AudioEffects.h"
#include "Game/Audio/UnidentifiedAudioScriptRuntime.h"
#include "Game/Audio/RegistryPools.h"
#include "NL/nlFunction.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/nlRegistry.h"

AudioResourceRuntime* g_pAudioResourceRuntime;

inline UnidentifiedAudioPoolOwner::~UnidentifiedAudioPoolOwner()
{
    SlotPoolBase::BaseFreeBlocks(
        &RegistryPoolTypes::sContainerPool,
        sizeof(ScopedRegistryContainer));
    SlotPoolBase::BaseFreeBlocks(
        &RegistryPoolTypes::sNodePool,
        sizeof(RegistryNode));
}

// Configuration keys the runtime resolves by lower-cased name hash. The DOL
// keeps only the hashes; the names behind them are not recoverable.
#define AUDIO_DEFINITIONS_KEY 0xD2894EC5
#define AUDIO_EFFECT_KEY 0xFE7CE6FB


AudioResourceRuntime::AudioResourceRuntime()
{
    mRoot = 0;
    mLoadFlag = 1;
    mReservedFlag = 0;
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
    m_Script->Unidentified6E00(data, size);
}

/**
 * Address/Size: 0x802F4904 | size: 0x54
 */
extern "C" void fn_802F4904(AudioResourceRuntime* runtime, float deltaTime)
{
    runtime->m_Script->Unidentified78C0(deltaTime);
    runtime->m_EffectFactory->Update(deltaTime);
}

/**
 * Address/Size: 0x802F4958 | size: 0x44
 */
extern "C" void fn_802F4958(AudioResourceRuntime* runtime)
{
    runtime->m_Script->Unidentified6BC4();
    runtime->m_EffectFactory->Shutdown();
}

/**
 * Address/Size: 0x802F499C | size: 0x8
 */
extern "C" void fn_802F499C(AudioResourceRuntime* runtime, u32 hash, u32 instance)
{
    runtime->m_Script->Unidentified6F00(hash, instance);
}

/**
 * Address/Size: 0x802F49A4 | size: 0x8
 */
extern "C" void fn_802F49A4(AudioResourceRuntime* runtime, u32 instance)
{
    runtime->m_Script->Unidentified77C8(instance);
}

/**
 * Address/Size: 0x802F49AC | size: 0x14
 */
void SetAudioEffectContext(unsigned long* hash, int index)
{
    g_pAudioResourceRuntime->m_Script->Unidentified6E98(*hash, (UnidentifiedAudioEffectSetState*)index);
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
    u32 effectId = (u32)((RegistryContainer*)
        ((RegistryContainer*)((RegistryContainer*)g_pAudioResourceRuntime->GetConfigRoot())
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
    RegistryContainer* transitions
        = (RegistryContainer*)
            ((RegistryContainer*)
                    g_pAudioResourceRuntime->GetConfigRoot())
                ->Get(nlStringLowerHash("Transitions"))
                .mData;
    RegistryContainer* set
        = (RegistryContainer*)transitions->Get(*hash).mData;
    if (set == 0)
    {
        return;
    }

    RegistryValue time = set->Get(nlStringLowerHash("Time"));
    float duration = time.mType == 5 ? 0.0f : *(float*)&time.mData;

    RegistryValue list = set->UnnamedList();
    RegistryIterator entries;
    ((RegistryContainer*)list.mData)
        ->GetIterator(&entries, list.mType);
    for (; !entries.IsDone(); entries.Next())
    {
        RegistryContainer* entry
            = (RegistryContainer*)entries.GetValue().mData;
        u32 binding
            = (u32)entry->Get(nlStringLowerHash("EffectSet")).mData;
        bool inverted
            = invert != (entry->Get(nlStringLowerHash("Invert")).mData != 0);

        RegistryValue effects = entry->UnnamedList();
        RegistryIterator effect;
        ((RegistryContainer*)effects.mData)
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
RegistryContainer* AudioResourceRuntime::AllocContainer()
{
    return RegistryAllocContainer();
}

/**
 * Address/Size: 0x802F676C | size: 0x68
 */
RegistryNode* AudioResourceRuntime::AllocNode()
{
    return RegistryAllocNode();
}

/**
 * Address/Size: 0x802F67D4 | size: 0x10
 */
void* AudioResourceRuntime::AllocItem(unsigned int size)
{
    return nlMalloc(size, 8, true);
}

/**
 * Address/Size: 0x802F67E4 | size: 0x18
 */
void AudioResourceRuntime::FreeContainer(void* container)
{
    RegistryFreeContainer(
        (ScopedRegistryContainer*)container);
}

/**
 * Address/Size: 0x802F67FC | size: 0x18
 */
void AudioResourceRuntime::FreeNode(void* node)
{
    RegistryFreeNode((RegistryNode*)node);
}

/**
 * Address/Size: 0x802F6814 | size: 0x8
 */
void AudioResourceRuntime::FreeItem(void* data)
{
    nlFree(data);
}

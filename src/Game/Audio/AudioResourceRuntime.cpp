#include "Game/Audio/AudioResourceRuntime.h"
#include "NL/nlFunction.inl"

#include "Game/Audio/AudioConfig.h"
#include "Game/Audio/AudioEffects.h"
#include "Game/Audio/AudioScriptRuntime.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Audio/AudioResourceRuntime.inl"
#include "NL/nlFunction.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/nlRegistry.h"

AudioResourceRuntime* g_pAudioResourceRuntime;

// Configuration keys the runtime resolves by lower-cased name hash. The DOL
// keeps only the hashes; the names behind them are not recoverable.
#define AUDIO_EFFECT_KEY 0xFE7BE6FB
#define AUDIO_EFFECT_SET_KEY 0xECBAFA4B

// Reads one property value of a definition node by property key.
static inline u32 UnidentifiedGetDefinitionValue(u32 definition, u32 key)
{
    return ConfigFindDefinition(definition)->Get(key).m_Words.m_Value;
}

static inline RegistryContainer* UnidentifiedGetEffectSet(u32 effectSet)
{
    u32 key = AUDIO_EFFECT_SET_KEY;
    AudioConfigValue effectSets
        = g_pAudioResourceRuntime->GetConfigRoot()->Get(key);
    AudioConfigValue value
        = ((AudioConfigNode*)effectSets.m_Words.m_Value)->Get(effectSet);
    return (RegistryContainer*)value.m_Words.m_Value;
}

static inline RegistryContainer* UnidentifiedGetTransitionSet(
    const u32& key, const u32& set)
{
    return (RegistryContainer*)((AudioConfigNode*)
        g_pAudioResourceRuntime->GetConfigRoot()->Get(key).m_Words.m_Value)
        ->Get(set).m_Words.m_Value;
}

static inline RegistryValue GetRegistryIteratorValue(
    RegistryIteratorBase* iterator)
{
    return iterator->GetValue();
}

static inline u32 GetRegistryIteratorHash(RegistryIteratorBase* iterator)
{
    return iterator->GetHash();
}

// Looks up the live effect a binding already runs for an effect id.
static inline bool UnidentifiedFindEffect(AudioEffectBinding* binding,
    const u32& effectId, AudioEffectBase** effect)
{
    AudioEffectBase** found;
    bool foundEffect = binding->mEffects.FindGet(effectId, &found);
    if (foundEffect)
    {
        *effect = *found;
    }
    return foundEffect;
}

// Starts one effect of a definition on a binding: resolves the effect id from
// the configuration tree, creates the effect through the factory the first
// time the binding asks for it, tells every instance already running on the
// binding about the new effect, then pushes the parameter the caller wants.
inline bool AudioEffectBinding::UnidentifiedStartEffect(u32 definition,
    void* parameterData, bool immediate, float value)
{
    u32 effectId = UnidentifiedGetDefinitionValue(definition, AUDIO_EFFECT_KEY);

    AudioEffectBase* effect;
    bool foundEffect = UnidentifiedFindEffect(this, effectId, &effect);
    if (!foundEffect)
    {
        effect = g_pAudioResourceRuntime->m_EffectFactory->CreateEffect(effectId);
        mEffects.Add(effectId, effect);
        mInstances.Walk(
            Function<bool(const u32&, bool*)>(
                AudioEffectSoundStartedVisitor(effect)));
    }

    AudioEffectParameter* parameter = 0;
    effect->CreateParameter(definition, parameterData, immediate, &parameter);
    return effect->AddParameter(parameter, value);
}

// Owner-bound form: the parameter follows its owner instead of a time.
inline bool AudioEffectBinding::UnidentifiedStartEffect(u32 definition,
    void* parameterData, bool immediate, void* owner)
{
    u32 effectId = UnidentifiedGetDefinitionValue(definition, AUDIO_EFFECT_KEY);

    AudioEffectBase* effect;
    bool foundEffect = UnidentifiedFindEffect(this, effectId, &effect);
    if (!foundEffect)
    {
        effect = g_pAudioResourceRuntime->m_EffectFactory->CreateEffect(effectId);
        mEffects.Add(effectId, effect);
        mInstances.Walk(
            Function<bool(const u32&, bool*)>(
                AudioEffectSoundStartedVisitor(effect)));
    }

    AudioEffectParameter* parameter = 0;
    effect->CreateParameter(definition, parameterData, immediate, &parameter);
    return effect->AddParameter(parameter, owner);
}

AudioResourceRuntime::AudioResourceRuntime()
{
    m_ConfigRoot = 0;
    g_pAudioResourceRuntime = this;
    m_EffectFactory = GetAudioEffectFactory();
    m_Script = new AudioScriptRuntime;
}

/**
 * Address/Size: 0x802F48FC | size: 0x8
 */
void AudioResourceRuntime::LoadScriptData(void* data, unsigned int size)
{
    m_Script->LoadScriptData(data, size);
}

/**
 * Address/Size: 0x802F4904 | size: 0x54
 */
extern "C" void fn_802F4904(AudioResourceRuntime* runtime, float deltaTime)
{
    runtime->m_Script->Update(deltaTime);
    runtime->m_EffectFactory->Update(deltaTime);
}

/**
 * Address/Size: 0x802F4958 | size: 0x44
 */
extern "C" void fn_802F4958(AudioResourceRuntime* runtime)
{
    runtime->m_Script->Shutdown();
    runtime->m_EffectFactory->Shutdown();
}

/**
 * Address/Size: 0x802F499C | size: 0x8
 */
void NotifyAudioSoundStarted(AudioResourceRuntime* runtime, u32 hash, u32 instance)
{
    runtime->m_Script->OnSoundStarted(hash, instance);
}

/**
 * Address/Size: 0x802F49A4 | size: 0x8
 */
void NotifyAudioSoundStopped(AudioResourceRuntime* runtime, u32 instance)
{
    runtime->m_Script->OnSoundStopped(instance);
}

/**
 * Address/Size: 0x802F49AC | size: 0x14
 */
void SetAudioEffectContext(unsigned long* hash, int index)
{
    g_pAudioResourceRuntime->m_Script->SetEffectContext(*hash, index);
}

// Starts the effect on the binding registered under a script key.
inline bool AudioScriptRuntime::UnidentifiedStartEffect(const u32& key,
    u32 definition, void* parameterData, bool immediate, float value)
{
    return GetBinding(key)->UnidentifiedStartEffect(
        definition, parameterData, immediate, value);
}

/**
 * Address/Size: 0x802F49C0 | size: 0x4C4
 */
extern "C" bool fn_802F49C0(const u32* bindingKey, const u32* definitionKey,
    void* parameterData, bool immediate, float value)
{
    u32 key = *bindingKey;
    u32 definition = *definitionKey;
    return g_pAudioResourceRuntime->m_Script->UnidentifiedStartEffect(
        key, definition, parameterData, immediate, value);
}

// Applies every parameter of an effect set to a binding.
inline void AudioEffectBinding::UnidentifiedApplyEffectSet(
    u32 effectSetKey, bool inverted, void* owner)
{
    RegistryContainer* effectSet = UnidentifiedGetEffectSet(effectSetKey);
    if (effectSet == 0)
    {
        return;
    }

    RegistryValue parameters = effectSet->NamedList();
    RegistryIterator parameterStorage;
    ((RegistryContainer*)parameters.mData)
        ->GetIterator(&parameterStorage, parameters.mType);
    for (; !((RegistryIteratorBase*)&parameterStorage)->IsDone();
         ((RegistryIteratorBase*)&parameterStorage)->Next())
    {
        RegistryValue value = GetRegistryIteratorValue(&parameterStorage);
        RegistryValue parameterData = value;
        void* context = &parameterData;
        UnidentifiedStartEffect(
            GetRegistryIteratorHash(&parameterStorage), context,
            inverted, owner);
    }
}

// Applies an effect set to the binding registered under a script key.
inline void AudioScriptRuntime::UnidentifiedApplyEffectSet(u32 key,
    u32 effectSetKey, bool inverted, void* owner)
{
    AudioEffectBinding* binding = GetBinding(key);
    binding->UnidentifiedApplyEffectSet(effectSetKey, inverted, owner);
}

// Runtime entry: applies an effect set to the binding under a script key.
inline void AudioResourceRuntime::UnidentifiedApplyEffectSet(u32 bindingKey,
    u32 effectSetKey, bool inverted, void* owner)
{
    m_Script->UnidentifiedApplyEffectSet(
        bindingKey, effectSetKey, inverted, owner);
}

// Applies every parameter of an effect set to a binding.
inline void AudioEffectBinding::UnidentifiedApplyEffectSet(
    u32 effectSetKey, bool inverted, float duration)
{
    RegistryContainer* effectSet = UnidentifiedGetEffectSet(effectSetKey);
    if (effectSet == 0)
    {
        return;
    }

    RegistryValue parameters = effectSet->NamedList();
    RegistryIterator parameterStorage;
    ((RegistryContainer*)parameters.mData)
        ->GetIterator(&parameterStorage, parameters.mType);
    for (; !((RegistryIteratorBase*)&parameterStorage)->IsDone();
         ((RegistryIteratorBase*)&parameterStorage)->Next())
    {
        RegistryValue value = GetRegistryIteratorValue(&parameterStorage);
        RegistryValue parameterData = value;
        void* context = &parameterData;
        UnidentifiedStartEffect(
            GetRegistryIteratorHash(&parameterStorage), context,
            inverted, duration);
    }
}

// Applies an effect set to the binding registered under a script key.
inline void AudioScriptRuntime::UnidentifiedApplyEffectSet(u32 key,
    u32 effectSetKey, bool inverted, float duration)
{
    AudioEffectBinding* binding = GetBinding(key);
    binding->UnidentifiedApplyEffectSet(effectSetKey, inverted, duration);
}

// Runtime entry: applies an effect set to the binding under a script key.
inline void AudioResourceRuntime::UnidentifiedApplyEffectSet(u32 bindingKey,
    u32 effectSetKey, bool inverted, float duration)
{
    m_Script->UnidentifiedApplyEffectSet(
        bindingKey, effectSetKey, inverted, duration);
}

/**
 * Address/Size: 0x802F4E84 | size: 0xDC4
 *
 * Applies the named transition set: walks every transition it lists, reads the
 * transition's "Time" and "Invert" properties and starts each effect of the
 * transition's "EffectSet" on the binding the entry names.
 */
extern "C" bool fn_802F4E84(const u32* hash, bool invert, void* owner)
{
    u32 transitionKey = nlStringLowerHash("Transitions");
    RegistryContainer* set
        = UnidentifiedGetTransitionSet(transitionKey, *hash);
    if (set == 0)
    {
        return false;
    }

    RegistryValue time = set->Get(nlStringLowerHash("Time"));
    float duration = time.mType == 5 ? 0.0f : *(float*)&time.mData;

    RegistryValue list = set->UnnamedList();
    RegistryIterator entryStorage;
    RegistryIteratorBase* entries = &entryStorage;
    ((RegistryContainer*)list.mData)
        ->GetIterator(entries, list.mType);
    for (; !entries->IsDone(); entries->Next())
    {
        RegistryValue entryValue = GetRegistryIteratorValue(entries);
        RegistryContainer* entry = (RegistryContainer*)entryValue.mData;
        u32 effectSet
            = (u32)entry->Get(nlStringLowerHash("EffectSet")).mData;
        bool inverted
            = invert ^ (entry->Get(nlStringLowerHash("Invert")).mData != 0);

        RegistryValue bindings = entry->UnnamedList();
        RegistryIterator bindingStorage;
        RegistryIteratorBase* binding = &bindingStorage;
        ((RegistryContainer*)bindings.mData)
            ->GetIterator(binding, bindings.mType);
        for (; !binding->IsDone(); binding->Next())
        {
            RegistryValue bindingValue = GetRegistryIteratorValue(binding);
            u32 bindingKey = (u32)bindingValue.mData;
            if (owner != 0)
            {
                g_pAudioResourceRuntime->UnidentifiedApplyEffectSet(
                    bindingKey, effectSet, inverted, owner);
            }
            else
            {
                g_pAudioResourceRuntime->UnidentifiedApplyEffectSet(
                    bindingKey, effectSet, inverted, duration);
            }
        }
    }
    return true;
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

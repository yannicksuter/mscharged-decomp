#include "Game/Audio/UnidentifiedAudioScriptRuntime.h"
#include "NL/nlFunction.inl"

#include "Game/Audio/AudioEffects.h"
#include "Game/Audio/AudioResourceRuntime.h"
#include "NL/nlBind_impl.h"

bool NotifyEffectSoundStarted(const u32&, AudioEffectBase**, u32);
bool NotifyEffectSoundStopped(const u32&, AudioEffectBase**, u32);
bool ApplyEffectToSound(const u32&, bool*, AudioEffectBase*);

void AudioEffectBinding::Unidentified8418(u32 key)
{
}

void AudioEffectBinding::OnSoundStarted(u32 instance)
{
    mInstances.Add(instance, false);
    mEffects.Walk(Function2<bool, const u32&, AudioEffectBase**>(
        Bind<bool>(NotifyEffectSoundStarted, Placeholder<0>(), Placeholder<1>(), instance)));
}

bool NotifyEffectSoundStarted(const u32&, AudioEffectBase** effect, u32 instance)
{
    (*effect)->OnSoundStarted((void*)instance);
    return true;
}

void AudioEffectBinding::OnSoundStopped(u32 instance)
{
    mInstances.Remove(instance);
    mEffects.Walk(Function2<bool, const u32&, AudioEffectBase**>(
        Bind<bool>(NotifyEffectSoundStopped, Placeholder<0>(), Placeholder<1>(), instance)));
}

bool NotifyEffectSoundStopped(const u32&, AudioEffectBase** effect, u32 instance)
{
    (*effect)->OnSoundStopped((void*)instance);
    return true;
}

void AudioEffectBinding::Update(float deltaTime)
{
    UpdateState update;
    update.mDeltaTime = deltaTime;
    update.mCount = 0;
    mEffects.Walk(Function2<bool, const u32&, AudioEffectBase**>(
        Bind<bool>(MemFun(&AudioEffectBinding::UpdateEffect),
            this, Placeholder<0>(), Placeholder<1>(), &update)));
    for (u32 i = 0; i < update.mCount; ++i)
    {
        g_pAudioResourceRuntime->m_EffectFactory->ReleaseEffect(update.mEntries[i].mEffect);
        mEffects.Remove(update.mEntries[i].mKey);
    }
}

void AudioEffectBinding::Destroy()
{
    mInstances.Clear();
    mInstances.GetAllocator()->FreeBlocks();
    mEffects.Walk(this, &AudioEffectBinding::ReleaseEffect);
    mEffects.Clear();
    mEffects.GetAllocator()->FreeBlocks();
}

void AudioEffectBinding::ReleaseEffect(const u32&, AudioEffectBase** effect)
{
    g_pAudioResourceRuntime->m_EffectFactory->ReleaseEffect(*effect);
}

bool AudioEffectBinding::UpdateEffect(const u32& key,
    AudioEffectBase** effect, UpdateState* update)
{
    AudioEffectBase* value = *effect;
    value->Update(update->mDeltaTime);
    if (value->m_Enabled)
    {
        update->mEntries[update->mCount].mKey = key;
        update->mEntries[update->mCount].mEffect = value;
        ++update->mCount;
    }
    else
    {
        mInstances.Walk(Function2<bool, const u32&, bool*>(
            Bind<bool>(ApplyEffectToSound, Placeholder<0>(), Placeholder<1>(), value)));
    }
    return true;
}

bool ApplyEffectToSound(const u32& key, bool*, AudioEffectBase* effect)
{
    effect->ApplyToSound((void*)key);
    return true;
}

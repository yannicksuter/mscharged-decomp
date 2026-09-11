#include "Game/Audio/UnidentifiedAudioScriptRuntime.h"

#include "Game/Audio/AudioEffects.h"
#include "Game/Audio/AudioResourceRuntime.h"
#include "NL/nlBind_impl.h"

extern "C"
{
    bool fn_802F86E4(const u32&, AudioEffectBase**, u32);
    bool fn_802F89F4(const u32&, AudioEffectBase**, u32);
    bool fn_802F92B4(const u32&, UnidentifiedAudioEffectInstance**, AudioEffectBase*);
}

void UnidentifiedAudioEffectBinding::Unidentified8418(u32 key)
{
}

void UnidentifiedAudioEffectBinding::Unidentified841C(u32 instance)
{
    mInstances.Add(instance, (UnidentifiedAudioEffectInstance*)0);
    mEffects.Walk(Function2<bool, const u32&, AudioEffectBase**>(
        Bind<bool>(fn_802F86E4, Placeholder<0>(), Placeholder<1>(), instance)));
}

extern "C" bool fn_802F86E4(const u32&, AudioEffectBase** effect, u32 instance)
{
    (*effect)->OnSoundStarted((void*)instance);
    return true;
}

void UnidentifiedAudioEffectBinding::Unidentified8720(u32 instance)
{
    mInstances.Remove(instance);
    mEffects.Walk(Function2<bool, const u32&, AudioEffectBase**>(
        Bind<bool>(fn_802F89F4, Placeholder<0>(), Placeholder<1>(), instance)));
}

extern "C" bool fn_802F89F4(const u32&, AudioEffectBase** effect, u32 instance)
{
    (*effect)->OnSoundStopped((void*)instance);
    return true;
}

void UnidentifiedAudioEffectBinding::Unidentified8A30(float deltaTime)
{
    UnidentifiedUpdate update;
    update.mDeltaTime = deltaTime;
    update.mCount = 0;
    mEffects.Walk(Function2<bool, const u32&, AudioEffectBase**>(
        Bind<bool>(MemFun(&UnidentifiedAudioEffectBinding::Unidentified8F98),
            this, Placeholder<0>(), Placeholder<1>(), &update)));
    for (u32 i = 0; i < update.mCount; ++i)
    {
        g_pAudioResourceRuntime->m_EffectFactory->ReleaseEffect(update.mEntries[i].mEffect);
        mEffects.Remove(update.mEntries[i].mKey);
    }
}

void UnidentifiedAudioEffectBinding::Unidentified8DA0()
{
    mInstances.Clear();
    mInstances.GetAllocator()->FreeBlocks();
    mEffects.Walk(this, &UnidentifiedAudioEffectBinding::Unidentified8F7C);
    mEffects.Clear();
    mEffects.GetAllocator()->FreeBlocks();
}

void UnidentifiedAudioEffectBinding::Unidentified8F7C(const u32&, AudioEffectBase** effect)
{
    g_pAudioResourceRuntime->m_EffectFactory->ReleaseEffect(*effect);
}

bool UnidentifiedAudioEffectBinding::Unidentified8F98(const u32& key,
    AudioEffectBase** effect, UnidentifiedUpdate* update)
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
        mInstances.Walk(Function2<bool, const u32&, UnidentifiedAudioEffectInstance**>(
            Bind<bool>(fn_802F92B4, Placeholder<0>(), Placeholder<1>(), value)));
    }
    return true;
}

extern "C" bool fn_802F92B4(const u32& key, UnidentifiedAudioEffectInstance**,
    AudioEffectBase* effect)
{
    effect->ApplyToSound((void*)key);
    return true;
}

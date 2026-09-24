#include "Game/Audio/UnidentifiedAudioScriptRuntime.h"
#include "NL/nlFunction.inl"

#include "NL/nlAlgorithm.h"
#include "NL/nlBind.h"
#include "NL/nlDLRing.h"

static SlotPool<UnidentifiedAudioTransitionState> sUnidentifiedTransitions(16, 16);

void UnidentifiedAudioScriptRuntime::Unidentified6BC4()
{
    mBindings.Walk(this, &UnidentifiedAudioScriptRuntime::Unidentified6DF8);
    mBindings.Clear();
    mBindings.GetAllocator()->FreeBlocks();
    mTransitions.Clear();
    mTransitions.GetAllocator()->FreeBlocks();
    mEffectSets.Clear();
    mEffectSets.GetAllocator()->FreeBlocks();
    sUnidentifiedTransitions.FreeBlocks();
}

void UnidentifiedAudioScriptRuntime::Unidentified6DF8(
    const u32&, AudioEffectBinding* binding)
{
    binding->Destroy();
}

bool UnidentifiedAudioScriptRuntime::Unidentified6E00(void* data, unsigned int size)
{
    u32* header = (u32*)data;
    mUnidentified04 = header[2];
    mUnidentified00 = (UnidentifiedAudioScriptEntry*)(header + 4);
    for (u32 i = 0; i < mUnidentified04; ++i)
        mUnidentified00[i].mData = (u8*)mUnidentified00[i].mData + (u32)data;
    mUnidentified08 = (UnidentifiedAudioScriptList*)(mUnidentified00 + mUnidentified04);
    mUnidentified08->mValues = (u32*)((u8*)mUnidentified08->mValues + (u32)data);
    if (header[3] != 0)
        mInterpreter.LoadByteCode((u8*)data + header[3]);
    return true;
}

int UnidentifiedAudioScriptRuntime::Unidentified6E98(u32 hash, int value)
{
    int* previous = mEffectSets.Add(hash, value);
    if (previous != 0)
        *previous = value;
    return 0;
}

static inline void UnidentifiedAddBinding(UnidentifiedAudioScriptRuntime* script,
    u32 key, u32 instance)
{
    bool added;
    AudioEffectBinding* binding = script->mBindings.UnidentifiedAddOrGet(key, added);
    if (added)
        binding->Unidentified8418(key);
    binding->OnSoundStarted(instance);

    UnidentifiedAudioTransitionState* entry = 0;
    sUnidentifiedTransitions.Allocate(entry);
    entry->mBinding = binding;
    UnidentifiedAudioTransitionState** head = script->mTransitions.UnidentifiedAddOrGet(instance, added);
    if (added)
        *head = 0;
    nlDLRingAddEnd(head, entry);
}

inline bool UnidentifiedAudioScriptList::UnidentifiedContains(const u32& key) const
{
    bool found = false;
    if (mCount != 0)
        found = nlBSearch<u32, u32>(key, mValues, mCount) != 0;
    return found;
}

static inline bool UnidentifiedListAbsent(const u32& key, u32* values, int count)
{
    bool absent = true;
    if (count != 0)
        absent = nlBSearch<u32, u32>(key, values, count) == 0;
    return absent;
}

void UnidentifiedAudioScriptRuntime::Unidentified6F00(u32 hash, u32 instance)
{
    UnidentifiedAddBinding(this, 0x8CE35E27, instance);
    UnidentifiedAudioScriptEntry* entry =
        nlBSearch<UnidentifiedAudioScriptEntry, u32>(hash, mUnidentified00, mUnidentified04);
    if (entry == 0)
    {
        for (u32 i = 0; i < mUnidentified08->mCount; ++i)
            UnidentifiedAddBinding(this, mUnidentified08->mValues[i], instance);
        return;
    }

    UnidentifiedAudioScriptSelection* selection = (UnidentifiedAudioScriptSelection*)entry->mData;
    for (u32 i = 0; i < selection->mCount; ++i)
    {
        if (!mUnidentified08->UnidentifiedContains(selection->mValues[i]))
            UnidentifiedAddBinding(this, selection->mValues[i], instance);
    }

    u32 keys[50];
    UnidentifiedAudioScriptCondition* condition =
        (UnidentifiedAudioScriptCondition*)(selection->mValues + selection->mCount);
    for (u32 i = 0; i < selection->mConditionCount; ++i)
    {
        keys[i] = condition->mKey;
        if (condition->mFunction == 0xFFFF)
        {
            int* found;
            int value = 0;
            if (mEffectSets.FindGet(condition->mArguments[0], &found))
                value = *found;
            if (value != 0 && !mUnidentified08->UnidentifiedContains(condition->mKey))
                UnidentifiedAddBinding(this, condition->mKey, instance);
        }
        else
        {
            FunctionEntryPoint* function = mInterpreter.GetFunctionEntryPoint(condition->mFunction);
            u32 values[4];
            for (int j = 0; j < condition->mCount; ++j)
            {
                int* found;
                u32 value = 0;
                if (mEffectSets.FindGet(condition->mArguments[j], &found))
                    value = (u32)*found;
                values[j] = value;
            }
            mInterpreter.ExecuteFunction(function, condition->mCount, values);
            if (*mInterpreter.m_SP != 0
                && !mUnidentified08->UnidentifiedContains(condition->mKey))
                UnidentifiedAddBinding(this, condition->mKey, instance);
        }
        condition = (UnidentifiedAudioScriptCondition*)(condition->mArguments + condition->mCount);
    }

    nlQSort(keys, selection->mConditionCount, nlDefaultQSortComparer<u32>);
    for (u32 i = 0; i < mUnidentified08->mCount; ++i)
    {
        u32* key = &mUnidentified08->mValues[i];
        if (UnidentifiedListAbsent(*key, selection->mValues, selection->mCount)
            && UnidentifiedListAbsent(*key, keys, selection->mConditionCount))
            UnidentifiedAddBinding(this, *key, instance);
    }
}

bool UnidentifiedAudioScriptRuntime::Unidentified77C8(u32 instance)
{
    UnidentifiedAudioTransitionState** found;
    if (!mTransitions.FindGet(instance, &found))
        return false;

    UnidentifiedAudioTransitionState* head = *found;
    UnidentifiedAudioTransitionState* current = head;
    for (;;)
    {
        current->mBinding->OnSoundStopped(instance);
        UnidentifiedAudioTransitionState* next = current->m_next;
        sUnidentifiedTransitions.Free(current);
        if (next == head)
            break;
        current = next;
    }
    mTransitions.Remove(instance);
    return true;
}

extern "C" bool fn_802F7CFC(const u32& key, AudioEffectBinding* binding,
    UnidentifiedAudioScriptUpdate* update);

void UnidentifiedAudioScriptRuntime::Unidentified78C0(float deltaTime)
{
    UnidentifiedAudioScriptUpdate update;
    update.mDeltaTime = deltaTime;
    update.mCount = 0;
    mBindings.Walk(Function2<bool, const u32&, AudioEffectBinding*>(
        Bind<bool>(fn_802F7CFC, Placeholder<0>(), Placeholder<1>(), &update)));
    for (u32 i = 0; i < update.mCount; ++i)
        mBindings.Remove(update.mKeys[i]);
}

extern "C" bool fn_802F7CFC(const u32& key, AudioEffectBinding* binding,
    UnidentifiedAudioScriptUpdate* update)
{
    binding->Update(update->mDeltaTime);
    if (binding->IsEmpty() && update->mCount < 8)
        update->mKeys[update->mCount++] = key;
    return true;
}

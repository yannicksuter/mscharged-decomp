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
    const u32&, UnidentifiedAudioEffectBinding* binding)
{
    binding->Unidentified8DA0();
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

int UnidentifiedAudioScriptRuntime::Unidentified6E98(u32 hash, UnidentifiedAudioEffectSetState* value)
{
    UnidentifiedAudioEffectSetState** previous = mEffectSets.Add(hash, value);
    if (previous != 0)
        *previous = value;
    return 0;
}

static inline void UnidentifiedAddBinding(UnidentifiedAudioScriptRuntime* script,
    u32 key, u32 instance)
{
    bool added;
    UnidentifiedAudioEffectBinding* binding = script->mBindings.UnidentifiedAddOrGet(key, added);
    if (added)
        binding->Unidentified8418(key);
    binding->Unidentified841C(instance);

    UnidentifiedAudioTransitionState* entry = 0;
    sUnidentifiedTransitions.Allocate(entry);
    entry->mBinding = binding;
    UnidentifiedAudioTransitionState** head = script->mTransitions.UnidentifiedAddOrGet(instance, added);
    if (added)
        *head = 0;
    nlDLRingAddEnd(head, entry);
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
        if (mUnidentified08->mCount == 0
            || nlBSearch<u32, u32>(selection->mValues[i],
                mUnidentified08->mValues, mUnidentified08->mCount) == 0)
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
            UnidentifiedAudioEffectSetState** found;
            UnidentifiedAudioEffectSetState* value = 0;
            if (mEffectSets.FindGet(condition->mArguments[0], &found))
                value = *found;
            if (value != 0
                && (mUnidentified08->mCount == 0
                    || nlBSearch<u32, u32>(condition->mKey,
                        mUnidentified08->mValues, mUnidentified08->mCount) == 0))
                UnidentifiedAddBinding(this, condition->mKey, instance);
        }
        else
        {
            FunctionEntryPoint* function = mInterpreter.GetFunctionEntryPoint(condition->mFunction);
            u32 values[4];
            for (int j = 0; j < condition->mCount; ++j)
            {
                UnidentifiedAudioEffectSetState** found;
                values[j] = mEffectSets.FindGet(condition->mArguments[j], &found)
                    ? (u32)*found : 0;
            }
            mInterpreter.ExecuteFunction(function, condition->mCount, values);
            if (*mInterpreter.m_SP != 0
                && (mUnidentified08->mCount == 0
                    || nlBSearch<u32, u32>(condition->mKey,
                        mUnidentified08->mValues, mUnidentified08->mCount) == 0))
                UnidentifiedAddBinding(this, condition->mKey, instance);
        }
        condition = (UnidentifiedAudioScriptCondition*)(condition->mArguments + condition->mCount);
    }

    nlQSort(keys, selection->mConditionCount, nlDefaultQSortComparer<u32>);
    for (u32 i = 0; i < mUnidentified08->mCount; ++i)
    {
        u32* key = &mUnidentified08->mValues[i];
        if ((selection->mCount == 0
                || nlBSearch<u32, u32>(*key, selection->mValues, selection->mCount) == 0)
            && (selection->mConditionCount == 0
                || nlBSearch<u32, u32>(*key, keys, selection->mConditionCount) == 0))
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
        current->mBinding->Unidentified8720(instance);
        UnidentifiedAudioTransitionState* next = current->m_next;
        sUnidentifiedTransitions.Free(current);
        if (next == head)
            break;
        current = next;
    }
    mTransitions.Remove(instance);
    return true;
}

extern "C" bool fn_802F7CFC(const u32& key, UnidentifiedAudioEffectBinding* binding,
    UnidentifiedAudioScriptUpdate* update);

void UnidentifiedAudioScriptRuntime::Unidentified78C0(float deltaTime)
{
    UnidentifiedAudioScriptUpdate update;
    update.mDeltaTime = deltaTime;
    update.mCount = 0;
    mBindings.Walk(Function2<bool, const u32&, UnidentifiedAudioEffectBinding*>(
        Bind<bool>(fn_802F7CFC, Placeholder<0>(), Placeholder<1>(), &update)));
    for (u32 i = 0; i < update.mCount; ++i)
        mBindings.Remove(update.mKeys[i]);
}

extern "C" bool fn_802F7CFC(const u32& key, UnidentifiedAudioEffectBinding* binding,
    UnidentifiedAudioScriptUpdate* update)
{
    binding->Unidentified8A30(update->mDeltaTime);
    if (binding->IsEmpty() && update->mCount < 8)
        update->mKeys[update->mCount++] = key;
    return true;
}

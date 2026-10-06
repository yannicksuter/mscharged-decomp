#include "NL/nlDLListContainer.inl"
#include "Game/Audio/AudioScriptRuntime.h"
#include "NL/nlFunction.inl"

#include "NL/nlAlgorithm.h"
#include "NL/nlBind.h"
#include "NL/nlDLRing.h"

static SlotPool<AudioBindingNode> sBindingNodes(16, 16);

void AudioScriptRuntime::Shutdown()
{
    mBindings.Walk(this, &AudioScriptRuntime::DestroyBinding);
    mBindings.Clear();
    mBindings.GetAllocator()->FreeBlocks();
    mInstanceBindings.Clear();
    mInstanceBindings.GetAllocator()->FreeBlocks();
    mContextValues.Clear();
    mContextValues.GetAllocator()->FreeBlocks();
    sBindingNodes.FreeBlocks();
}

void AudioScriptRuntime::DestroyBinding(
    const u32&, AudioEffectBinding* binding)
{
    binding->Destroy();
}

bool AudioScriptRuntime::LoadScriptData(void* data, unsigned int size)
{
    u32* header = (u32*)data;
    mEntryCount = header[2];
    mEntries = (AudioScriptEntry*)(header + 4);
    for (u32 i = 0; i < mEntryCount; ++i)
        mEntries[i].mData = (u8*)mEntries[i].mData + (u32)data;
    mDefaultBindings = (AudioScriptBindingList*)(mEntries + mEntryCount);
    mDefaultBindings->mValues = (u32*)((u8*)mDefaultBindings->mValues + (u32)data);
    if (header[3] != 0)
        mInterpreter.LoadByteCode((u8*)data + header[3]);
    return true;
}

int AudioScriptRuntime::SetEffectContext(u32 hash, int value)
{
    int* previous = mContextValues.Add(hash, value);
    if (previous != 0)
        *previous = value;
    return 0;
}

static inline void AddInstanceBinding(AudioScriptRuntime* script,
    const u32& instance, AudioEffectBinding* binding)
{
    AudioBindingNode* entry = 0;
    sBindingNodes.Allocate(entry);
    entry->mBinding = binding;
    bool added;
    AudioBindingNode** head = script->mInstanceBindings.UnidentifiedAddOrGet(instance, added);
    if (added)
        *head = 0;
    nlDLRingAddEnd(head, entry);
}

static inline void AddSoundBinding(AudioScriptRuntime* script,
    u32 instance, u32 key)
{
    bool added;
    AudioEffectBinding* binding = script->mBindings.UnidentifiedAddOrGet(key, added);
    if (added)
        binding->Unidentified8418(key);
    binding->OnSoundStarted(instance);

    AddInstanceBinding(script, instance, binding);
}

inline bool AudioScriptBindingList::Contains(const u32& key) const
{
    bool found = false;
    if (mCount != 0)
        found = nlBSearch<u32, u32>(key, mValues, mCount) != 0;
    return found;
}

static inline bool GetContextValue(AudioScriptRuntime* script,
    const u32& key, int& value)
{
    int* found;
    bool hasValue = script->mContextValues.FindGet(key, &found);
    if (hasValue)
        value = *found;
    return hasValue;
}

void AudioScriptRuntime::OnSoundStarted(u32 hash, u32 instance)
{
    AddSoundBinding(this, instance, 0x8CE35E27);
    AudioScriptEntry* entry =
        nlBSearch<AudioScriptEntry, u32>(hash, mEntries, mEntryCount);
    if (entry == 0)
    {
        for (u32 i = 0; i < mDefaultBindings->mCount; ++i)
        {
            u32 key = mDefaultBindings->mValues[i];
            AddSoundBinding(this, instance, key);
        }
        return;
    }

    AudioScriptSelection* selection = (AudioScriptSelection*)entry->mData;
    for (u32 i = 0; i < selection->mCount; ++i)
    {
        if (!mDefaultBindings->Contains(selection->mValues[i]))
        {
            u32 key = selection->mValues[i];
            AddSoundBinding(this, instance, key);
        }
    }

    u32 keys[50];
    AudioScriptCondition* condition =
        (AudioScriptCondition*)(selection->mValues + selection->mCount);
    for (u32 i = 0; i < selection->mConditionCount; ++i)
    {
        keys[i] = condition->mKey;
        if (condition->mFunction == 0xFFFF)
        {
            int value = 0;
            GetContextValue(this, condition->mArguments[0], value);
            if (value != 0 && !mDefaultBindings->Contains(condition->mKey))
                AddSoundBinding(this, instance, condition->mKey);
        }
        else
        {
            FunctionEntryPoint* function = mInterpreter.GetFunctionEntryPoint(condition->mFunction);
            u32 values[4];
            for (int j = 0; j < condition->mCount; ++j)
            {
                int value;
                bool found = GetContextValue(this, condition->mArguments[j], value);
                values[j] = found ? value : 0;
            }
            mInterpreter.ExecuteFunction(function, condition->mCount, values);
            if (*mInterpreter.m_SP != 0 && !mDefaultBindings->Contains(condition->mKey))
                AddSoundBinding(this, instance, condition->mKey);
        }
        condition = (AudioScriptCondition*)(condition->mArguments + condition->mCount);
    }

    nlQSort(keys, selection->mConditionCount, nlDefaultQSortComparer<u32>);
    for (u32 i = 0; i < mDefaultBindings->mCount; ++i)
    {
        u32* key = &mDefaultBindings->mValues[i];
        bool absent = true;
        if (selection->mCount != 0)
            absent = nlBSearch<u32, u32>(*key, selection->mValues, selection->mCount) == 0;
        if (absent)
        {
            bool conditionAbsent = true;
            if (selection->mConditionCount != 0)
                conditionAbsent = nlBSearch<u32, u32>(*key, keys, selection->mConditionCount) == 0;
            if (conditionAbsent)
            {
                u32 bindingKey = *key;
                AddSoundBinding(this, instance, bindingKey);
            }
        }
    }
}

bool AudioScriptRuntime::OnSoundStopped(u32 instance)
{
    AudioBindingNode** found;
    bool exists = mInstanceBindings.FindGet(instance, &found);
    AudioBindingNode* head;
    if (exists)
        head = *found;
    if (exists)
    {
        AudioBindingNode* current = head;
        for (;;)
        {
            current->mBinding->OnSoundStopped(instance);
            AudioBindingNode* next = current->m_next;
            sBindingNodes.Free(current);
            if (next == head)
                break;
            current = next;
        }
        mInstanceBindings.Remove(instance);
        return true;
    }
    return false;
}

void AudioScriptRuntime::Update(float deltaTime)
{
    AudioScriptUpdateState update;
    update.mDeltaTime = deltaTime;
    update.mCount = 0;
    mBindings.Walk(Function<bool(const u32&, AudioEffectBinding*)>(
        Bind<bool>(UpdateAudioScriptBinding, Placeholder<0>(), Placeholder<1>(), &update)));
    for (u32 i = 0; i < update.mCount; ++i)
        mBindings.Remove(update.mKeys[i]);
}

bool UpdateAudioScriptBinding(const u32& key, AudioEffectBinding* binding,
    AudioScriptUpdateState* update)
{
    binding->Update(update->mDeltaTime);
    if (binding->IsEmpty() && update->mCount < 8)
        update->mKeys[update->mCount++] = key;
    return true;
}

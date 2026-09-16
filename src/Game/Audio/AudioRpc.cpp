#include "NL/nlDebugString.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioGlobals.h"
#include "Game/Audio/AudioRpc.h"
#include "Game/Audio/AudioSlider.h"
#include "Game/Audio/RegistryPools.h"
#include "NL/nlChunk.h"
#include "Game/Sys/debug.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlSlotPool.h"
#include "types.h"

struct AudioSystemRpcView
{
    u8 pad_00[0xCC];
    AudioBundleManager* bundleManager;
};

char sAudioRpcValueFormat[] = "RpcController: %s %f\n";

static inline AudioSliderTable* GetAudioSliderTable()
{
    return (AudioSliderTable*)((AudioSystemRpcView*)g_pAudioSystem)
        ->bundleManager->GetSliderTable();
}

static inline float EvaluateAudioRpcCurve(
    AudioRpcRuntimeNode* node, float input)
{
    AudioRpcCurvePoint* point = node->definition->points;
    u32 pointCount;
    if (input < point->input)
    {
        return point->output;
    }

    pointCount = node->definition->pointCount;
    for (u32 i = 1; i < pointCount; ++i)
    {
        AudioRpcCurvePoint* next = point + 1;
        if (input < next->input)
        {
            float amount = (input - point->input)
                / (next->input - point->input);
            return amount * (next->output - point->output)
                + point->output;
        }
        ++point;
    }
    return point->output;
}

static inline void UpdateAudioRpcRuntimeNode(AudioRpcRuntimeNode* node)
{
    XSoundHandle* localOwner = node->owner != 0
                                ? node->owner->soundHandle
                                : 0;
    AudioSlider* slider = GetAudioSlider(
        GetAudioSliderTable(), node->definition->sliderIndex, localOwner);
    node->valid = slider->valid;
    if (!node->valid)
        return;

    node->value = EvaluateAudioRpcCurve(node, slider->value);
    node->valid = true;
    tDebugPrintManager::Print(DC_SOUND, sAudioRpcValueFormat, nlLookupDebugString(g_pDebugStringTable, (unsigned long)slider->definition->name), node->value);
}

extern "C" AudioRpcController* ParseAudioRpcController(nlChunk* outer)
{
    nlChunk* header = (nlChunk*)outer->GetData();
    AudioRpcController* controller = (AudioRpcController*)header->GetData();
    u32 groupOffset;
    u32 definitionOffset;
    u32 groupIndex;
    AudioRpcGroup* group;
    u32 definitionIndex;
    AudioRpcDefinition* definitions;
    AudioRpcDefinition* definition;
    nlChunk* cursor;

    cursor = header->GetNextChunk();
    controller->groups = (AudioRpcGroup*)cursor->GetData();
    groupIndex = 0;
    groupOffset = 0;
    while (groupIndex < controller->groupCount)
    {
        group =
            (AudioRpcGroup*)((u8*)controller->groups + groupOffset);
        cursor = cursor->GetNextChunk();
        definitions = (AudioRpcDefinition*)cursor->GetData();
        group->definitions = definitions;
        group->staticDefinitions = definitions;
        group->dynamicDefinitions = definitions + group->staticDefinitionCount;

        definitionIndex = 0;
        definitionOffset = 0;
        while (definitionIndex < group->definitionCount)
        {
            definition =
                (AudioRpcDefinition*)((u8*)group->definitions
                    + definitionOffset);
            cursor = cursor->GetNextChunk();
            definition->points = (AudioRpcCurvePoint*)cursor->GetData();
            definitionOffset += sizeof(AudioRpcDefinition);
            ++definitionIndex;
        }
        groupOffset += sizeof(AudioRpcGroup);
        ++groupIndex;
    }

    controller->runtimeNodes = (AudioRpcRuntimeNode**)nlMalloc(
        controller->runtimeCount * sizeof(AudioRpcRuntimeNode*),
        8,
        false);
    controller->dynamicNodes =
        new (8, false) AudioRpcList(sAudioRpcListEntryPool);
    InitializeAudioRpcController(controller);
    return controller;
}

extern "C" void InitializeAudioRpcController(AudioRpcController* controller)
{
    u32 groupOffset;
    u32 runtimeOffset;
    u32 definitionOffset;
    u32 groupIndex;
    AudioRpcGroup* group;
    u32 definitionIndex;
    AudioRpcDefinition* definition;
    AudioRpcRuntimeNode* node;

    groupIndex = 0;
    groupOffset = 0;
    runtimeOffset = 0;
    while (groupIndex < controller->groupCount)
    {
        group =
            (AudioRpcGroup*)((u8*)controller->groups + groupOffset);
        definitionIndex = 0;
        definitionOffset = 0;
        while (definitionIndex < group->definitionCount)
        {
            definition =
                (AudioRpcDefinition*)((u8*)group->definitions
                    + definitionOffset);
            if (definition->enabled != 0)
            {
                node = 0;
                sAudioRpcRuntimeNodePool.AllocateForReturn(node);
                if (node != 0)
                {
                    node->definition = 0;
                    node->value = 0.0f;
                    node->owner = 0;
                }
                node->definition = definition;
                definition->runtimeNode = node;
                UpdateAudioRpcRuntimeNode(node);
                *(AudioRpcRuntimeNode**)(
                    (u8*)controller->runtimeNodes + runtimeOffset) = node;
                runtimeOffset += sizeof(AudioRpcRuntimeNode*);
            }
            definitionOffset += sizeof(AudioRpcDefinition);
            ++definitionIndex;
        }
        groupOffset += sizeof(AudioRpcGroup);
        ++groupIndex;
    }
}

extern "C" void UpdateAudioRpcController(
    AudioRpcController* controller, float)
{
    for (u32 i = 0; i < controller->runtimeCount; i++)
        UpdateAudioRpcRuntimeNode(controller->runtimeNodes[i]);

    nlDLListIterator<AudioRpcRuntimeNode*> iterator;
    iterator = controller->dynamicNodes->Begin();
    while (iterator.hasNext())
    {
        UpdateAudioRpcRuntimeNode(*iterator);
        iterator.Step();
    }
}

extern "C" AudioRpcRuntimeNode* AddAudioRpcRuntimeNode(
    AudioRpcController* controller,
    AudioRpcDefinition* definition, AudioRpcOwner* owner)
{
    AudioRpcRuntimeNode* node = 0;
    sAudioRpcRuntimeNodePool.Allocate(node);
    if (node != 0)
    {
        node->definition = 0;
        node->value = 0.0f;
        node->owner = 0;
    }
    node->definition = definition;
    node->owner = owner;
    controller->dynamicNodes->AddEnd(node);
    return node;
}

extern "C" void RemoveAudioRpcRuntimeNodes(
    AudioRpcController* controller, AudioRpcOwner* owner)
{
    nlDLListIterator<AudioRpcRuntimeNode*> iterator =
        controller->dynamicNodes->Begin();
    while (iterator.hasNext())
    {
        if (owner == (*iterator)->owner)
        {
            AudioRpcRuntimeNode* node = *iterator;
            if (node != 0)
            {
                sAudioRpcRuntimeNodePool.Free(node);
            }
            controller->dynamicNodes->Remove(&iterator);
        }
        iterator.Step();
    }
}

SlotPool<AudioRpcRuntimeNode> sAudioRpcRuntimeNodePool(32, 16);
SlotPool<AudioRpcListEntry> sAudioRpcListEntryPool(16, 16);

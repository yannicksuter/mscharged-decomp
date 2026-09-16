#include "NL/nlDebugString.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioSystem.h"
#include "Game/Audio/RegistryPools.h"
#include "NL/nlChunk.h"
#include "Game/Sys/debug.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlSlotPool.h"
#include "types.h"
#include "Game/Audio/RpcList_802EFB70.h"

struct RpcCurvePoint_802EFB70
{
    float input;
    float output;
};

struct RpcDefinition_802EFB70
{
    u32 field_00;
    u32 field_04;
    u32 sliderIndex;
    u32 field_0C;
    u32 enabled;
    u32 field_14;
    u32 pointCount;
    RpcCurvePoint_802EFB70* points;
    void* runtimeNode;
};

struct RpcGroup_802EFB70
{
    u32 field_00;
    u32 definitionCount;
    RpcDefinition_802EFB70* definitions;
    u32 field_0C;
    RpcDefinition_802EFB70* definitionsCopy;
    u32 field_14;
    void* field_18;
};

struct RpcRuntimeNode_802EFB70
{
    RpcDefinition_802EFB70* definition;
    float value;
    u8 valid;
    u8 pad_09[3];
    s32* localIndex;
};

struct RpcController_802EFB70
{
    u32 groupCount;
    RpcGroup_802EFB70* groups;
    u32 runtimeCount;
    RpcRuntimeNode_802EFB70** runtimeNodes;
    RpcList_802EFB70* dynamicNodes;
};

struct SliderState_802EFB70
{
    void** vtable;
    float value;
    u8 valid;
    u8 pad_09[0x1B];
    struct
    {
        u32 field_00;
        const char* name;
    }* definition;
};

extern char lbl_8052F668[0x16];
SlotPool<RpcRuntimeNode_802EFB70> lbl_8057F9E8(32, 16);
SlotPool<RpcListEntry_802EFB70> lbl_8057FA10(16, 16);

extern "C" SliderState_802EFB70* fn_802EED38(
    void* sliderTable, u32 sliderIndex, void* localOwner);

extern "C" void fn_802EFEFC(RpcController_802EFB70* controller);

static inline void* GetSliderTable_802EFB70()
{
    return g_pAudioSystem->GetBundleManager()->GetSliderTable();
}

static inline RpcRuntimeNode_802EFB70* AllocateRuntimeNode_802EFB70()
{
    RpcRuntimeNode_802EFB70* node = 0;
    if (lbl_8057F9E8.m_FreeList == 0)
        SlotPoolBase::BaseAddNewBlock(
            &lbl_8057F9E8, sizeof(RpcRuntimeNode_802EFB70));
    if (lbl_8057F9E8.m_FreeList != 0)
    {
        node = (RpcRuntimeNode_802EFB70*)lbl_8057F9E8.m_FreeList;
        lbl_8057F9E8.m_FreeList = lbl_8057F9E8.m_FreeList->next;
    }
    if (node != 0)
    {
        node->definition = 0;
        node->value = 0.0f;
        node->localIndex = 0;
    }
    return node;
}

static inline RpcListEntry_802EFB70* AllocateListEntry_802EFB70(
    RpcList_802EFB70* list)
{
    RpcListEntry_802EFB70* entry = 0;
    if (list->m_Allocator.m_FreeList == 0)
        SlotPoolBase::BaseAddNewBlock(
            &list->m_Allocator, sizeof(RpcListEntry_802EFB70));
    if (list->m_Allocator.m_FreeList != 0)
    {
        entry = (RpcListEntry_802EFB70*)list->m_Allocator.m_FreeList;
        list->m_Allocator.m_FreeList = list->m_Allocator.m_FreeList->next;
    }
    if (entry != 0)
    {
        entry->m_next = 0;
        entry->m_prev = 0;
    }
    return entry;
}

static inline float EvaluateCurve_802EFB70(
    RpcDefinition_802EFB70* definition, float input)
{
    RpcCurvePoint_802EFB70* point = definition->points;
    if (input < point->input)
        return point->output;

    for (u32 i = 1; i < definition->pointCount; i++, point++)
    {
        if (input < point[1].input)
        {
            float fraction = (input - point->input)
                           / (point[1].input - point->input);
            return fraction * (point[1].output - point->output)
                 + point->output;
        }
    }
    return point->output;
}

static inline void UpdateRuntimeNode_802EFB70(RpcRuntimeNode_802EFB70* node)
{
    void* localOwner = node->localIndex != 0
                         ? (void*)*node->localIndex
                         : 0;
    SliderState_802EFB70* slider = fn_802EED38(
        GetSliderTable_802EFB70(), node->definition->sliderIndex, localOwner);
    node->valid = slider->valid;
    if (!node->valid)
        return;

    node->value = EvaluateCurve_802EFB70(
        node->definition, slider->value);
    node->valid = true;
    tDebugPrintManager::Print(DC_SOUND, lbl_8052F668, nlLookupDebugString(g_pDebugStringTable, (unsigned long)slider->definition->name), node->value);
}

extern "C" RpcController_802EFB70* fn_802EFB70(nlChunk* outer)
{
    nlChunk* header = (nlChunk*)outer->GetData();
    RpcController_802EFB70* controller = (RpcController_802EFB70*)header->GetData();

    nlChunk* cursor = header->GetNextChunk();
    controller->groups = (RpcGroup_802EFB70*)cursor->GetData();
    for (u32 groupIndex = 0; groupIndex < controller->groupCount;
        groupIndex++)
    {
        RpcGroup_802EFB70* group = controller->groups + groupIndex;
        cursor = cursor->GetNextChunk();
        group->definitions = (RpcDefinition_802EFB70*)cursor->GetData();
        group->definitionsCopy = group->definitions;
        group->field_18 = group->definitions + group->field_0C;

        for (u32 definitionIndex = 0;
            definitionIndex < group->definitionCount;
            definitionIndex++)
        {
            cursor = cursor->GetNextChunk();
            group->definitions[definitionIndex].points = (RpcCurvePoint_802EFB70*)cursor->GetData();
        }
    }

    controller->runtimeNodes = (RpcRuntimeNode_802EFB70**)nlMalloc(
        controller->runtimeCount * sizeof(RpcRuntimeNode_802EFB70*),
        8,
        false);
    controller->dynamicNodes = new (8, false) RpcList_802EFB70(lbl_8057FA10);
    fn_802EFEFC(controller);
    return controller;
}

extern "C" void fn_802EFEFC(RpcController_802EFB70* controller)
{
    u32 runtimeIndex = 0;
    for (u32 groupIndex = 0; groupIndex < controller->groupCount;
        groupIndex++)
    {
        RpcGroup_802EFB70* group = controller->groups + groupIndex;
        for (u32 definitionIndex = 0;
            definitionIndex < group->definitionCount;
            definitionIndex++)
        {
            RpcDefinition_802EFB70* definition = group->definitions + definitionIndex;
            if (definition->enabled == 0)
                continue;

            RpcRuntimeNode_802EFB70* node = AllocateRuntimeNode_802EFB70();
            node->definition = definition;
            definition->runtimeNode = node;
            UpdateRuntimeNode_802EFB70(node);
            controller->runtimeNodes[runtimeIndex++] = node;
        }
    }
}

extern "C" void fn_802F00F0(
    RpcController_802EFB70* controller, float)
{
    for (u32 i = 0; i < controller->runtimeCount; i++)
        UpdateRuntimeNode_802EFB70(controller->runtimeNodes[i]);

    nlDLListIterator<RpcRuntimeNode_802EFB70*> iterator =
        controller->dynamicNodes->Begin();
    while (iterator.hasNext())
    {
        UpdateRuntimeNode_802EFB70(*iterator);
        iterator.Step();
    }
}

extern "C" RpcRuntimeNode_802EFB70* fn_802F0394(
    RpcController_802EFB70* controller,
    RpcDefinition_802EFB70* definition, u32 localIndex)
{
    RpcRuntimeNode_802EFB70* node = AllocateRuntimeNode_802EFB70();
    node->definition = definition;
    node->localIndex = (s32*)localIndex;

    RpcList_802EFB70* list = controller->dynamicNodes;
    RpcListEntry_802EFB70* entry = AllocateListEntry_802EFB70(list);
    entry->entry = node;
    if (list->m_Head == 0)
    {
        list->m_Head = entry;
        entry->m_next = entry;
        entry->m_prev = entry;
    }
    else
    {
        RpcListEntry_802EFB70* head = list->m_Head;
        head->m_next->m_prev = entry;
        entry->m_next = head->m_next;
        entry->m_prev = head;
        head->m_next = entry;
    }
    list->m_Head = entry;
    return node;
}

extern "C" void fn_802F04D4(
    RpcController_802EFB70* controller, u32 localIndex)
{
    RpcList_802EFB70* list = controller->dynamicNodes;
    RpcListEntry_802EFB70* start = list->m_Head;
    RpcListEntry_802EFB70* entry = start;
    while (entry != 0)
    {
        RpcListEntry_802EFB70* next = entry->m_next;
        if ((u32)entry->entry->localIndex == localIndex)
        {
            RpcRuntimeNode_802EFB70* node = entry->entry;
            node->definition = (RpcDefinition_802EFB70*)lbl_8057F9E8.m_FreeList;
            lbl_8057F9E8.m_FreeList = (SlotPoolEntry*)node;

            if (entry->m_next == entry)
                list->m_Head = 0;
            else
            {
                entry->m_prev->m_next = entry->m_next;
                entry->m_next->m_prev = entry->m_prev;
                if (list->m_Head == entry)
                    list->m_Head = entry->m_prev;
            }
            list->m_Allocator.DeleteEntry(entry);
        }

        if (next == start || list->m_Head == 0)
            entry = 0;
        else
            entry = next;
    }
}

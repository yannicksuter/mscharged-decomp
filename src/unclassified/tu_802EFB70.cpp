#include "Game/SAnim.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlSlotPool.h"
#include "types.h"

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

struct RpcListEntry_802EFB70
{
    RpcListEntry_802EFB70* next;
    RpcListEntry_802EFB70* previous;
    RpcRuntimeNode_802EFB70* node;
};

struct RpcList_802EFB70
{
    SlotPoolBase* pool;
    RpcListEntry_802EFB70* head;
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
extern SlotPoolBase lbl_8057F9E8;
extern SlotPoolBase lbl_8057FA10;
extern int lbl_806E1DC8;
extern void* lbl_806E201C;

extern "C" void fn_8004F594(int category, const char* format, ...);
extern "C" const char* fn_802B9568(int table, ...);
extern "C" SliderState_802EFB70* fn_802EED38(
    void* sliderTable, u32 sliderIndex, void* localOwner);

extern "C" void fn_802EFEFC(RpcController_802EFB70* controller);

static inline void* GetSliderTable_802EFB70()
{
    void* resources = *(void**)((u8*)lbl_806E201C + 0xCC);
    return *(void**)((u8*)resources + 0xC);
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
    if (list->pool->m_FreeList == 0)
        SlotPoolBase::BaseAddNewBlock(
            list->pool, sizeof(RpcListEntry_802EFB70));
    if (list->pool->m_FreeList != 0)
    {
        entry = (RpcListEntry_802EFB70*)list->pool->m_FreeList;
        list->pool->m_FreeList = list->pool->m_FreeList->next;
    }
    if (entry != 0)
    {
        entry->next = 0;
        entry->previous = 0;
        entry->node = 0;
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
            return ((input - point->input)
                       / (point[1].input - point->input))
                     * (point[1].output - point->output)
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
    fn_8004F594(10, lbl_8052F668, fn_802B9568(lbl_806E1DC8, slider->definition->name), node->value);
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
    controller->dynamicNodes = (RpcList_802EFB70*)nlMalloc(sizeof(RpcList_802EFB70), 8, false);
    if (controller->dynamicNodes != 0)
    {
        controller->dynamicNodes->pool = &lbl_8057FA10;
        controller->dynamicNodes->head = 0;
    }
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

    RpcListEntry_802EFB70* start = controller->dynamicNodes->head;
    RpcListEntry_802EFB70* entry = start;
    while (entry != 0)
    {
        UpdateRuntimeNode_802EFB70(entry->node);
        entry = entry->next;
        if (entry == start)
            entry = 0;
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
    entry->node = node;
    if (list->head == 0)
    {
        list->head = entry;
        entry->next = entry;
        entry->previous = entry;
    }
    else
    {
        RpcListEntry_802EFB70* tail = list->head->previous;
        tail->next = entry;
        entry->next = list->head;
        entry->previous = tail;
        list->head->previous = entry;
    }
    list->head = entry;
    return node;
}

extern "C" void fn_802F04D4(
    RpcController_802EFB70* controller, u32 localIndex)
{
    RpcList_802EFB70* list = controller->dynamicNodes;
    RpcListEntry_802EFB70* start = list->head;
    RpcListEntry_802EFB70* entry = start;
    while (entry != 0)
    {
        RpcListEntry_802EFB70* next = entry->next;
        if ((u32)entry->node->localIndex == localIndex)
        {
            RpcRuntimeNode_802EFB70* node = entry->node;
            node->definition = (RpcDefinition_802EFB70*)lbl_8057F9E8.m_FreeList;
            lbl_8057F9E8.m_FreeList = (SlotPoolEntry*)node;

            if (entry->next == entry)
                list->head = 0;
            else
            {
                entry->previous->next = entry->next;
                entry->next->previous = entry->previous;
                if (list->head == entry)
                    list->head = entry->previous;
            }
            entry->next = (RpcListEntry_802EFB70*)list->pool->m_FreeList;
            list->pool->m_FreeList = (SlotPoolEntry*)entry;
        }

        if (next == start || list->head == 0)
            entry = 0;
        else
            entry = next;
    }
}

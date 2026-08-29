#include "Game/SAnim.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"
#include "types.h"

struct ChainNode_802F076C
{
    ChainNode_802F076C* next;
};

struct ChainOwner_802F076C
{
    u8 pad_00[0xC];
    ChainNode_802F076C* head;
};

struct ResourceEntry_802F0860
{
    void* value;
    float field_04;
    float weight;
    u32 useCount;
    u8 eligible;
    u8 pad_11[3];
};

struct ResourceGroupA_802F0860
{
    u32 field_00;
    u32 entryCount;
    ResourceEntry_802F0860* entries;
    u8 pad_0C[0x1C];
};

struct ResourceGroupB_802F0860
{
    u32 field_00;
    u32 field_04;
    void* field_08;
    u32 field_0C;
    u32 indexCount;
    u32* indices;
    u32 referenceCount;
    u32* references;
    u32 flattenedCount;
    u32 secondaryCount;
    void** flattenedReferences;
};

struct ResourceReference_802F0860
{
    u32 kind;
    void* value;
};

struct ResourceGroupC_802F0860
{
    u32 field_00;
    u32 referenceCount;
    ResourceReference_802F0860* references;
};

struct ResourceGroupD_802F0860
{
    u8 pad_00[0x10];
    void* data;
    u8 pad_14[0x1C];
};

struct ResourceBundle_802F0860
{
    u32 field_00;
    u32 field_04;
    u32 groupACount;
    ResourceGroupA_802F0860* groupsA;
    u32 groupBCount;
    ResourceGroupB_802F0860* groupsB;
    u32 groupCCount;
    ResourceGroupC_802F0860* groupsC;
    u32 groupDCount;
    ResourceGroupD_802F0860* groupsD;
    u32 field_28;
    void* field_2C;
    u32 field_30;
    void* field_34;
};

struct WeightedSelector_802F11A0
{
    u32 field_00;
    u32 entryCount;
    ResourceEntry_802F0860* entries;
    u32 field_0C;
    s32 mode;
    u32 field_14;
    u32 selectedIndex;
};

struct RpcDefinition_802F1548
{
    u8 pad_00[0x10];
    u32 enabled;
    u8 pad_14[0xC];
    void* runtimeNode;
};

struct RpcGroup_802F1548
{
    u8 pad_00[0xC];
    u32 definitionCount;
    RpcDefinition_802F1548* definitions;
    u32 secondaryCount;
    u8 pad_18[4];
};

struct RpcController_802F1548
{
    u32 groupCount;
    RpcGroup_802F1548* groups;
};

struct AudioResources_802F1548
{
    u8 pad_00[0x50];
    RpcController_802F1548* rpcController;
};

struct AudioSystem_802F1548
{
    u8 pad_00[0xCC];
    AudioResources_802F1548* resources;
};

extern AudioSystem_802F1548* lbl_806E201C;

extern "C" void fn_802F1548(ResourceGroupB_802F0860* group);

extern "C" void fn_802F076C(
    ChainOwner_802F076C** owner, ChainNode_802F076C* node)
{
    ChainOwner_802F076C* chain = *owner;
    node->next = chain->head;
    chain->head = node;
}

extern "C" SlotPoolBase* fn_802F0780(
    SlotPoolBase* pool, int destroy)
{
    if (pool != 0)
    {
        fn_802B467C(pool);
        SlotPoolBase::BaseFreeBlocks(pool, 0x10);
        pool->~SlotPoolBase();
        if (destroy > 0)
            operator delete(pool);
    }
    return pool;
}

extern "C" SlotPoolBase* fn_802F07F0(
    SlotPoolBase* pool, int destroy)
{
    if (pool != 0)
    {
        fn_802B467C(pool);
        SlotPoolBase::BaseFreeBlocks(pool, 0xC);
        pool->~SlotPoolBase();
        if (destroy > 0)
            operator delete(pool);
    }
    return pool;
}

extern "C" ResourceBundle_802F0860* fn_802F0860(nlChunk* outer)
{
    nlChunk* header = (nlChunk*)outer->GetData();
    ResourceBundle_802F0860* bundle = (ResourceBundle_802F0860*)header->GetData();

    ResourceGroupB_802F0860* oldGroupsB = bundle->groupsB;
    ResourceGroupC_802F0860* oldGroupsC = bundle->groupsC;
    ResourceGroupD_802F0860* oldGroupsD = bundle->groupsD;
    void* oldField2C = bundle->field_2C;
    void* oldField34 = bundle->field_34;

    nlChunk* cursor = header->GetNextChunk();
    bundle->groupsA = (ResourceGroupA_802F0860*)cursor->GetData();
    cursor = cursor->GetNextChunk();
    bundle->groupsB = (ResourceGroupB_802F0860*)cursor->GetData();
    cursor = cursor->GetNextChunk();
    bundle->groupsC = (ResourceGroupC_802F0860*)cursor->GetData();
    cursor = cursor->GetNextChunk();
    bundle->groupsD = (ResourceGroupD_802F0860*)cursor->GetData();
    cursor = cursor->GetNextChunk();
    bundle->field_2C = cursor->GetData();
    cursor = cursor->GetNextChunk();
    bundle->field_34 = cursor->GetData();

    for (u32 groupIndex = 0; groupIndex < bundle->groupACount;
        groupIndex++)
    {
        ResourceGroupA_802F0860* group = bundle->groupsA + groupIndex;
        cursor = cursor->GetNextChunk();
        group->entries = (ResourceEntry_802F0860*)cursor->GetData();
        for (u32 entryIndex = 0; entryIndex < group->entryCount;
            entryIndex++)
        {
            ResourceEntry_802F0860* entry = group->entries + entryIndex;
            entry->value = (u8*)entry->value
                         + ((u8*)bundle->groupsB - (u8*)oldGroupsB);
        }
    }

    for (u32 groupIndex = 0; groupIndex < bundle->groupBCount;
        groupIndex++)
    {
        ResourceGroupB_802F0860* group = bundle->groupsB + groupIndex;
        cursor = cursor->GetNextChunk();
        group->indices = (u32*)cursor->GetData();
        for (u32 i = 0; i < group->indexCount; i++)
        {
            group->indices[i] += (u8*)bundle->groupsC - (u8*)oldGroupsC;
        }
        cursor = cursor->GetNextChunk();
        group->references = (u32*)cursor->GetData();
    }

    for (u32 groupIndex = 0; groupIndex < bundle->groupCCount;
        groupIndex++)
    {
        ResourceGroupC_802F0860* group = bundle->groupsC + groupIndex;
        cursor = cursor->GetNextChunk();
        group->references = (ResourceReference_802F0860*)cursor->GetData();
        for (u32 referenceIndex = 0;
            referenceIndex < group->referenceCount;
            referenceIndex++)
        {
            ResourceReference_802F0860* reference = group->references + referenceIndex;
            if (reference->kind == 1)
                reference->value = (u8*)reference->value
                                 + ((u8*)bundle->groupsD - (u8*)oldGroupsD);
            else if (reference->kind == 3)
                reference->value = (u8*)reference->value
                                 + ((u8*)bundle->field_2C - (u8*)oldField2C);
            else if (reference->kind == 2)
                reference->value = (u8*)reference->value
                                 + ((u8*)bundle->field_34 - (u8*)oldField34);
        }
    }

    for (u32 groupIndex = 0; groupIndex < bundle->groupDCount;
        groupIndex++)
    {
        cursor = cursor->GetNextChunk();
        bundle->groupsD[groupIndex].data = cursor->GetData();
    }

    for (u32 groupIndex = 0; groupIndex < bundle->groupBCount;
        groupIndex++)
        fn_802F1548(bundle->groupsB + groupIndex);
    return bundle;
}

extern "C" void* fn_802F11A0(WeightedSelector_802F11A0* selector)
{
    ResourceEntry_802F0860* selected = 0;
    if (selector->mode == -1 || selector->entryCount == 0)
        return 0;

    if (selector->entryCount == 1)
    {
        selected = selector->entries;
    }
    else if (selector->mode == 0
             || (selector->mode == 1 && selector->selectedIndex != 0xFFFFFFFF))
    {
        if (selector->selectedIndex == 0xFFFFFFFF)
            selected = selector->entries;
        else
        {
            u32 next = selector->selectedIndex + 1;
            if (next >= selector->entryCount)
                next = 0;
            selected = selector->entries + next;
        }
    }
    else
    {
        float totalWeight = 0.0f;
        if (selector->mode == 3)
        {
            for (u32 i = 0; i < selector->entryCount; i++)
            {
                ResourceEntry_802F0860* entry = selector->entries + i;
                entry->eligible = i != selector->selectedIndex;
                if (entry->eligible)
                    totalWeight += entry->weight;
            }
        }
        else if (selector->mode == 1 || selector->mode == 2)
        {
            for (u32 i = 0; i < selector->entryCount; i++)
            {
                selector->entries[i].eligible = true;
                totalWeight += selector->entries[i].weight;
            }
        }
        else if (selector->mode == 4)
        {
            if (selector->selectedIndex != 0xFFFFFFFF)
            {
                u32 selectedUses = selector->entries[selector->selectedIndex].useCount;
                for (u32 i = 0; i < selector->entryCount; i++)
                {
                    ResourceEntry_802F0860* entry = selector->entries + i;
                    entry->eligible = entry->useCount < selectedUses;
                    if (entry->eligible)
                        totalWeight += entry->weight;
                }
            }
            if (totalWeight == 0.0f)
            {
                for (u32 i = 0; i < selector->entryCount; i++)
                {
                    selector->entries[i].eligible = true;
                    totalWeight += selector->entries[i].weight;
                }
            }
        }

        float choice = (float)nlRandom((u32)(s32)totalWeight, &nlDefaultSeed);
        for (u32 i = 0; i < selector->entryCount; i++)
        {
            ResourceEntry_802F0860* entry = selector->entries + i;
            if (!entry->eligible)
                continue;
            if (choice <= entry->weight)
            {
                selected = entry;
                break;
            }
            choice -= entry->weight;
        }
    }

    if (selected == 0)
    {
        selector->selectedIndex = 0xFFFFFFFF;
        return 0;
    }
    selector->selectedIndex = selected - selector->entries;
    selected->useCount++;
    return selected->value;
}

extern "C" void* fn_802F1460(
    WeightedSelector_802F11A0* selector, float value)
{
    if (selector->entryCount == 0)
        return 0;
    if (selector->entryCount == 1)
        return selector->entries[0].value;

    ResourceEntry_802F0860* selected = 0;
    u32 randomValue = 0;
    for (u32 i = 0; i < selector->entryCount; i++)
    {
        ResourceEntry_802F0860* entry = selector->entries + i;
        if (value >= entry->field_04 && value <= entry->weight)
        {
            u32 candidate = nlRandom(100, &nlDefaultSeed);
            if (selected == 0 || candidate >= randomValue)
            {
                randomValue = candidate;
                selected = entry;
            }
        }
    }
    return selected != 0 ? selected->value : 0;
}

extern "C" void fn_802F1548(ResourceGroupB_802F0860* group)
{
    group->secondaryCount = 0;
    group->flattenedCount = 0;
    RpcController_802F1548* controller = lbl_806E201C->resources->rpcController;

    for (u32 i = 0; i < group->referenceCount; i++)
    {
        RpcGroup_802F1548* rpcGroup = controller->groups + group->references[i];
        group->secondaryCount += rpcGroup->definitionCount;
        group->flattenedCount += rpcGroup->secondaryCount;
    }

    if (group->secondaryCount == 0)
        return;

    group->flattenedReferences = (void**)nlMalloc(
        group->secondaryCount * sizeof(void*), 8, false);
    u32 outputIndex = 0;
    for (u32 i = 0; i < group->referenceCount; i++)
    {
        RpcGroup_802F1548* rpcGroup = controller->groups + group->references[i];
        for (u32 definitionIndex = 0;
            definitionIndex < rpcGroup->definitionCount;
            definitionIndex++)
        {
            RpcDefinition_802F1548* definition = rpcGroup->definitions + definitionIndex;
            if (definition->enabled)
                group->flattenedReferences[outputIndex++] = definition->runtimeNode;
        }
    }
}

#include "Game/SAnim.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlSlotPool.h"
#include "types.h"

#include <NMWException.h>

struct SliderDefinition_802EE964
{
    u32 field_00;
    const char* name;
    float initialValue;
    float minimumValue;
    float maximumValue;
    u32 field_14;
    u32 kind;
    u32 index;
};

struct SliderState_802EE964
{
    void** vtable;
    float value;
    u8 reportValue;
    u8 pad_09[3];
    float targetValue;
    float elapsed;
    float duration;
    float minimumValue;
    float maximumValue;
    u8 enabled;
    u8 pad_21[3];
    SliderDefinition_802EE964* definition;
};

struct LocalSliderSet_802EE964
{
    u32 field_00;
    SliderState_802EE964* sliders;
    void* owner;
};

struct SliderOwnerValue_802EE964
{
    u8 pad_00[0x1C];
    u32 value;
};

struct SliderOwner_802EE964
{
    u8 pad_00[0x20];
    LocalSliderSet_802EE964* localSet;
    u8 pad_24[8];
    SliderOwnerValue_802EE964* valueSource;
};

struct SliderTable_802EE964
{
    u32 field_00;
    SliderDefinition_802EE964* globalDefinitions;
    u32 globalCount;
    SliderDefinition_802EE964* globalDefinitionsCopy;
    u32 localCount;
    SliderDefinition_802EE964* localDefinitions;
    u32* localToGlobal;
    SliderState_802EE964* globalSliders;
    LocalSliderSet_802EE964* localSets;
};

struct LocalSliderDefinition_802EF218
{
    u8 pad_00[0xC];
    s32* field_0C;
    s32* field_10;
    s32* field_14;
};

struct LocalSliderTable_802EF218
{
    u32 count;
    LocalSliderDefinition_802EF218* definitions;
    SliderState_802EE964* sliders;
};

struct SliderBinding_802EF6B0
{
    u8 pad_00[0x18];
    void* owner;
};

struct SliderBindingTable_802EF6B0
{
    u8 pad_00[0x10];
    u32* count;
    SliderBinding_802EF6B0* bindings;
};

struct AudioSystem_802EE964
{
    void** vtable;
    u8 pad_04[0xC8];
    struct
    {
        u8 pad_00[0xC];
        SliderTable_802EE964* sliders;
    }* resources;
};

struct AudioEngine_802EE964
{
    void** vtable;
};

struct SliderManager_802EFA14
{
    void** vtable;
    AudioEngine_802EE964* engine;
    u8 pad_08[4];
    SliderTable_802EE964* globalTable;
    LocalSliderTable_802EF218* localTable;
    u8 pad_14[4];
    u8 controller[0x38];
    void* rpcController;
};

extern void* lbl_8052F3C8[4];
extern char lbl_8052F5D0[0x11];
extern char lbl_8052F5E8[0x18];
extern char lbl_8052F600[0x17];
extern void* lbl_8052F618[4];
extern void* lbl_8052F628[4];

extern SlotPoolBase lbl_8057F9E8;
extern SlotPoolBase lbl_8057FA10;
extern SlotPoolBase lbl_8057FA68;
extern SlotPoolBase lbl_8057FAA8;
extern SlotPoolBase lbl_8057FAE8;
extern SlotPoolBase lbl_8057FB10;
extern SlotPoolBase lbl_8057FB38;
extern SlotPoolBase lbl_8057FB78;

extern int lbl_806E1DC8;
extern AudioSystem_802EE964* lbl_806E201C;

extern "C" void fn_8004F594(int category, const char* format, ...);
extern "C" const char* fn_802B9568(int table, ...);
extern "C" bool fn_802EDB00();
extern "C" void fn_802EDC34();
extern "C" void* fn_802EFB70(nlChunk* chunk);
extern "C" void fn_802F00F0(void* controller, float dt);
extern "C" void fn_802F4904(void* controller, float dt);
extern "C" void fn_802F4958(void* controller);
extern "C" void fn_8035C11C(void* engine, float dt);
extern "C" void fn_8035CA84();

extern "C" void fn_802EF130(SliderState_802EE964* slider);
extern "C" void fn_802EF17C(LocalSliderSet_802EE964* set);
extern "C" LocalSliderSet_802EE964* fn_802EF198(
    LocalSliderSet_802EE964* set, int destroy);
extern "C" SliderState_802EE964* fn_802EF1D8(
    SliderState_802EE964* slider, int destroy);
extern "C" void fn_802EF5D0(SliderState_802EE964* slider);
extern "C" SliderState_802EE964* fn_802EF670(
    SliderState_802EE964* slider, int destroy);

static inline void SetSliderValue_802EE964(
    SliderState_802EE964* slider, float value, float duration)
{
    typedef void (*SetValueFunc)(SliderState_802EE964*, float, float);
    ((SetValueFunc)slider->vtable[3])(slider, value, duration);
}

extern "C" SliderTable_802EE964* fn_802EE964(nlChunk* outer)
{
    nlChunk* header = (nlChunk*)outer->GetData();
    SliderTable_802EE964* table = (SliderTable_802EE964*)header->GetData();

    nlChunk* definitions = header->GetNextChunk();
    table->globalDefinitions = (SliderDefinition_802EE964*)definitions->GetData();
    table->globalDefinitionsCopy = table->globalDefinitions;
    table->localDefinitions = table->globalDefinitions + table->globalCount;

    nlChunk* indices = definitions->GetNextChunk();
    table->localToGlobal = (u32*)indices->GetData();

    table->globalSliders = (SliderState_802EE964*)__construct_new_array(
        nlMalloc((table->globalCount * sizeof(SliderState_802EE964)) + 0x10,
            8,
            false),
        (ctor_dtor_ptr)fn_802EF130,
        (ctor_dtor_ptr)fn_802EF1D8,
        sizeof(SliderState_802EE964),
        table->globalCount);

    for (u32 i = 0; i < table->globalCount; i++)
    {
        SliderDefinition_802EE964* definition = table->globalDefinitions + i;
        SliderState_802EE964* slider = table->globalSliders + i;
        slider->definition = definition;
        slider->targetValue = definition->initialValue;
        slider->value = definition->initialValue;
        slider->minimumValue = definition->minimumValue;
        slider->maximumValue = definition->maximumValue;
        slider->reportValue = true;
    }

    table->localSets = (LocalSliderSet_802EE964*)__construct_new_array(
        nlMalloc((0x50 * sizeof(LocalSliderSet_802EE964)) + 0x10, 8, false),
        (ctor_dtor_ptr)fn_802EF17C,
        (ctor_dtor_ptr)fn_802EF198,
        sizeof(LocalSliderSet_802EE964),
        0x50);

    for (u32 setIndex = 0; setIndex < 0x50; setIndex++)
    {
        LocalSliderSet_802EE964* set = table->localSets + setIndex;
        set->sliders = (SliderState_802EE964*)__construct_new_array(
            nlMalloc((table->localCount * sizeof(SliderState_802EE964)) + 0x10,
                8,
                false),
            (ctor_dtor_ptr)fn_802EF130,
            (ctor_dtor_ptr)fn_802EF1D8,
            sizeof(SliderState_802EE964),
            table->localCount);

        for (u32 sliderIndex = 0; sliderIndex < table->localCount;
            sliderIndex++)
        {
            SliderDefinition_802EE964* definition = table->localDefinitions + sliderIndex;
            SliderState_802EE964* slider = set->sliders + sliderIndex;
            slider->definition = definition;
            slider->targetValue = definition->initialValue;
            slider->value = definition->initialValue;
            slider->minimumValue = definition->minimumValue;
            slider->maximumValue = definition->maximumValue;
            slider->reportValue = true;
        }
    }
    return table;
}

extern "C" SliderState_802EE964* fn_802EED38(
    SliderTable_802EE964* table, u32 index, SliderOwner_802EE964* owner)
{
    SliderDefinition_802EE964* definition = table->globalDefinitions + table->localToGlobal[index];
    if (definition->kind == 2)
        return table->globalSliders + definition->index;
    return owner->localSet->sliders + definition->index;
}

extern "C" LocalSliderSet_802EE964* fn_802EED88(
    SliderTable_802EE964* table, SliderOwner_802EE964* owner)
{
    u32 setIndex = 0;
    while (setIndex < 0x50 && table->localSets[setIndex].owner != 0)
        setIndex++;
    if (setIndex >= 0x50)
        fn_8035CA84();

    LocalSliderSet_802EE964* set = table->localSets + setIndex;
    set->owner = owner;
    for (u32 i = 0; i < table->localCount; i++)
    {
        SliderState_802EE964* slider = set->sliders + i;
        float value = slider->definition->initialValue;
        if (value < slider->minimumValue)
            value = slider->minimumValue;
        else if (value > slider->maximumValue)
            value = slider->maximumValue;
        slider->targetValue = value;
        slider->elapsed = 0.0f;
        SetSliderValue_802EE964(slider, 0.0f, 1.0f);
    }

    SliderDefinition_802EE964* definition = table->globalDefinitions + table->localToGlobal[0];
    SliderState_802EE964* slider = table->globalSliders + definition->index;
    fn_8004F594(10, lbl_8052F5D0, fn_802B9568(lbl_806E1DC8, slider->definition->name, set->sliders));
    float value = (float)owner->valueSource->value;
    if (value < slider->minimumValue)
        value = slider->minimumValue;
    else if (value > slider->maximumValue)
        value = slider->maximumValue;
    slider->targetValue = value;
    slider->elapsed = 0.0f;
    SetSliderValue_802EE964(slider, 0.0f, 1.0f);
    return set;
}

extern "C" void fn_802EEFF0(SliderTable_802EE964* table, float dt)
{
    for (u32 i = 0; i < table->globalCount; i++)
    {
        SliderState_802EE964* slider = table->globalSliders + i;
        SetSliderValue_802EE964(slider, dt, 1.0f);
        if (slider->reportValue)
            fn_8004F594(10, lbl_8052F5E8, slider->value);
    }

    for (u32 setIndex = 0; setIndex < 0x50; setIndex++)
    {
        LocalSliderSet_802EE964* set = table->localSets + setIndex;
        if (set->owner == 0)
            continue;
        for (u32 sliderIndex = 0; sliderIndex < table->localCount;
            sliderIndex++)
        {
            SliderState_802EE964* slider = set->sliders + sliderIndex;
            SetSliderValue_802EE964(slider, dt, 1.0f);
            if (slider->reportValue)
                fn_8004F594(10, lbl_8052F600, slider->value);
        }
    }
}

extern "C" void fn_802EF130(SliderState_802EE964* slider)
{
    slider->value = 0.0f;
    slider->elapsed = -1.0f;
    slider->duration = 1.0f;
    slider->enabled = true;
    slider->vtable = lbl_8052F618;
    slider->definition = 0;
    slider->targetValue = 0.0f;
    slider->value = 0.0f;
    slider->minimumValue = 0.0f;
    slider->maximumValue = 0.0f;
    slider->reportValue = true;
}

extern "C" void fn_802EF17C(LocalSliderSet_802EE964* set)
{
    set->sliders = 0;
    set->field_00 = 0xFFFF;
    set->owner = 0;
}

extern "C" LocalSliderSet_802EE964* fn_802EF198(
    LocalSliderSet_802EE964* set, int destroy)
{
    if (set != 0 && destroy > 0)
        operator delete(set);
    return set;
}

extern "C" SliderState_802EE964* fn_802EF1D8(
    SliderState_802EE964* slider, int destroy)
{
    if (slider != 0 && destroy > 0)
        operator delete(slider);
    return slider;
}

extern "C" LocalSliderTable_802EF218* fn_802EF218(nlChunk* outer)
{
    nlChunk* header = (nlChunk*)outer->GetData();
    LocalSliderTable_802EF218* table = (LocalSliderTable_802EF218*)header->GetData();
    LocalSliderDefinition_802EF218* oldDefinitions = table->definitions;

    nlChunk* definitions = header->GetNextChunk();
    table->definitions = (LocalSliderDefinition_802EF218*)definitions->GetData();
    s32 delta = (u8*)oldDefinitions - (u8*)table->definitions;
    for (u32 i = 0; i < table->count; i++)
    {
        LocalSliderDefinition_802EF218* definition = table->definitions + i;
        if (definition->field_0C != 0)
            definition->field_0C = (s32*)((u8*)definition->field_0C - delta);
        if (definition->field_10 != 0)
            definition->field_10 = (s32*)((u8*)definition->field_10 - delta);
        if (definition->field_14 != 0)
            definition->field_14 = (s32*)((u8*)definition->field_14 - delta);
    }

    table->sliders = (SliderState_802EE964*)__construct_new_array(
        nlMalloc((table->count * sizeof(SliderState_802EE964)) + 0x10,
            8,
            false),
        (ctor_dtor_ptr)fn_802EF5D0,
        (ctor_dtor_ptr)fn_802EF670,
        sizeof(SliderState_802EE964),
        table->count);

    for (u32 i = 0; i < table->count; i++)
    {
        SliderState_802EE964* slider = table->sliders + i;
        LocalSliderDefinition_802EF218* definition = table->definitions + i;
        slider->definition = (SliderDefinition_802EE964*)definition;
        float value = *(float*)((u8*)definition + 8);
        if (value < slider->minimumValue)
            value = slider->minimumValue;
        else if (value > slider->maximumValue)
            value = slider->maximumValue;
        slider->targetValue = value;
        slider->elapsed = 0.0f;
    }
    return table;
}

extern "C" void fn_802EF514(LocalSliderTable_802EF218* table, float dt)
{
    for (u32 i = 0; i < table->count; i++)
    {
        LocalSliderDefinition_802EF218* definition = table->definitions + i;
        float multiplier = definition->field_0C != 0
                             ? table->sliders[*definition->field_0C].value
                             : 1.0f;
        SetSliderValue_802EE964(table->sliders + i, dt, multiplier);
    }
}

extern "C" void fn_802EF5D0(SliderState_802EE964* slider)
{
    slider->value = 0.0f;
    slider->reportValue = true;
    slider->vtable = lbl_8052F3C8;
    slider->elapsed = -1.0f;
    slider->duration = 1.0f;
    slider->enabled = true;
    slider->vtable = lbl_8052F628;
    slider->definition = 0;
    slider->targetValue = 0.0f;
    slider->value = 0.0f;
    slider->minimumValue = -96.0f;
    slider->maximumValue = 6.0f;
    slider->reportValue = true;
}

extern "C" float fn_802EF634(SliderState_802EE964* slider)
{
    float value = slider->duration + slider->value;
    if (value < -96.0f)
        value = -96.0f;
    if (value > 6.0f)
        value = 6.0f;
    return value;
}

extern "C" SliderState_802EE964* fn_802EF670(
    SliderState_802EE964* slider, int destroy)
{
    if (slider != 0 && destroy > 0)
        operator delete(slider);
    return slider;
}

extern "C" void fn_802EF6B0(
    SliderBindingTable_802EF6B0* table, nlChunk* chunk)
{
    if (chunk->GetID() != 0x80023200)
        return;

    nlChunk* header = (nlChunk*)chunk->GetData();
    table->count = (u32*)header->GetData();
    nlChunk* definitions = header->GetNextChunk();
    table->bindings = (SliderBinding_802EF6B0*)definitions->GetData();
    for (u32 i = 0; i < *table->count; i++)
        table->bindings[i].owner = table;
}

extern "C" u8 fn_802EF870(u8* state)
{
    if (fn_802EDB00())
        state[8] = true;
    return state[8];
}

extern "C" void fn_802EF8B0(SliderManager_802EFA14* manager)
{
    fn_802B467C(&lbl_8057FA68);
    SlotPoolBase::BaseFreeBlocks(&lbl_8057FA68, 0x40);
    fn_802B467C(&lbl_8057FAA8);
    SlotPoolBase::BaseFreeBlocks(&lbl_8057FAA8, 0x7C);
    fn_802B467C(&lbl_8057FB78);
    SlotPoolBase::BaseFreeBlocks(&lbl_8057FB78, 0x1C);
    fn_802B467C(&lbl_8057FAE8);
    SlotPoolBase::BaseFreeBlocks(&lbl_8057FAE8, 0x34);
    fn_802B467C(&lbl_8057FB10);
    SlotPoolBase::BaseFreeBlocks(&lbl_8057FB10, 0x18);
    fn_802B467C(&lbl_8057FB38);
    SlotPoolBase::BaseFreeBlocks(&lbl_8057FB38, 0x18);
    fn_802B467C(&lbl_8057FA10);
    SlotPoolBase::BaseFreeBlocks(&lbl_8057FA10, 0xC);
    fn_802B467C(&lbl_8057F9E8);
    SlotPoolBase::BaseFreeBlocks(&lbl_8057F9E8, 0x10);
    fn_802F4958(manager->controller);

    typedef void (*ShutdownFunc)(AudioEngine_802EE964*);
    ((ShutdownFunc)manager->engine->vtable[4])(manager->engine);
}

extern "C" void fn_802EF9BC(
    SliderManager_802EFA14* manager, nlChunk* chunk)
{
    if (chunk->GetID() != 0x80023600)
    {
        fn_802EDC34();
        return;
    }
    manager->rpcController = fn_802EFB70(chunk);
}

extern "C" void fn_802EFA14(
    SliderManager_802EFA14* manager, float dt)
{
    typedef bool (*ReadyFunc)(SliderManager_802EFA14*);
    if (((ReadyFunc)manager->vtable[2])(manager))
    {
        fn_802EEFF0(manager->globalTable, dt);
        fn_802F00F0(manager->rpcController, dt);
        fn_802F4904(manager->controller, dt);
        fn_802EF514(manager->localTable, dt);
    }
    fn_8035C11C(manager->engine, dt);
}

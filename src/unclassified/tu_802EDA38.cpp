#include "Game/SAnim.h"
#include "NL/MemAlloc.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "types.h"

struct CompletionOwner_802EDA38
{
    u8 pad_00[8];
    u8 finished;
    u8 pad_09[0xF];
    void (*callback)(CompletionOwner_802EDA38*, void*);
    void* callbackContext;
};

struct Base_802EDA38
{
    void** vtable;
    CompletionOwner_802EDA38* owner;
    u32 field_08;
    MemoryAllocator* allocator;
};

struct Embedded_802EDA7C
{
    void** vtable;
    u8 pad_04[0x1C];
    u32 selected;
    u8 pad_24[8];
};

struct Engine_802EDA7C
{
    void** vtable;
};

struct State_802EDA7C
{
    void** vtable;
    Engine_802EDA7C* engine;
    u8 field_08;
    u8 pad_09[3];
    void* field_0C;
    void* field_10;
    void* field_14;
    Embedded_802EDA7C embedded;
    u8 field_44;
    u8 pad_45[3];
    nlChunk* fileData;
};

struct AudioSystem_802EDA7C
{
    void** vtable;
};

extern void* lbl_8052F528[];
extern void* lbl_8052F544[];
extern char lbl_8052F538[];
extern char lbl_806DF4D0[5];
extern AudioSystem_802EDA7C* lbl_806E201C;

extern "C" void* fn_802EB644(nlChunk* chunk);
extern "C" void fn_802EBADC();
extern "C" void* fn_802EE964(nlChunk* chunk);
extern "C" void* fn_802EF218(nlChunk* chunk);
extern "C" void fn_802F479C(Embedded_802EDA7C*, void*);
extern "C" void fn_802F48FC(Embedded_802EDA7C*, void*, u32);
extern "C" Engine_802EDA7C* fn_8035B8E8(Engine_802EDA7C*);

extern "C" Base_802EDA38* fn_802EDA38(
    Base_802EDA38* state, CompletionOwner_802EDA38* owner)
{
    state->owner = owner;
    state->vtable = lbl_8052F528;
    state->allocator = CurrentAllocator;
    return state;
}

extern "C" void fn_802EDA54(Base_802EDA38* state)
{
    CompletionOwner_802EDA38* owner = state->owner;
    owner->finished = true;
    if (owner->callback != 0)
        owner->callback(owner, owner->callbackContext);
}

extern "C" State_802EDA7C* fn_802EDA7C(State_802EDA7C* state)
{
    state->engine = 0;
    state->vtable = lbl_8052F544;
    state->field_08 = false;
    state->field_0C = 0;
    state->field_10 = 0;
    state->field_14 = 0;
    fn_802F479C(&state->embedded, lbl_8052F544);
    state->field_44 = false;

    Engine_802EDA7C* engine = (Engine_802EDA7C*)nlMalloc(0x728, 8, false);
    if (engine != 0)
        engine = fn_8035B8E8(engine);
    state->engine = engine;
    return state;
}

extern "C" void fn_802EDB00(State_802EDA7C* state)
{
    typedef void (*EngineMethod)(Engine_802EDA7C*);
    ((EngineMethod)state->engine->vtable[3])(state->engine);
}

extern "C" void fn_802EDB14(
    void* data, unsigned long, State_802EDA7C* state)
{
    nlChunk* outer = (nlChunk*)data;
    state->fileData = outer;
    nlChunk* chunk = outer->GetFirstChunk();
    nlChunk* end = outer->GetNextChunk();
    while (chunk != end)
    {
        typedef void (*ProcessChunkFunc)(State_802EDA7C*, nlChunk*);
        ((ProcessChunkFunc)state->vtable[9])(state, chunk);
        chunk = chunk->GetNextChunk();
    }
    typedef void (*FinishFunc)(State_802EDA7C*);
    ((FinishFunc)state->vtable[10])(state);
}

extern "C" void fn_802EDC34(
    State_802EDA7C* state, nlChunk* chunk)
{
    switch (chunk->GetID())
    {
    case 0x80023100:
        state->field_0C = fn_802EE964(chunk);
        break;
    case 0x80023400:
        state->field_10 = fn_802EF218(chunk);
        break;
    case 0x80023500:
        state->field_14 = fn_802EB644(chunk);
        fn_802EBADC();
        break;
    case 0x1200:
    {
        typedef bool (*LoadFunc)(
            Embedded_802EDA7C*, void*, u32, bool);
        LoadFunc load = (LoadFunc)state->embedded.vtable[3];
        if (load(&state->embedded, chunk->GetData(), (u8*)chunk->GetNextChunk() - (u8*)chunk->GetData(), false))
        {
            typedef u32 (*SelectFunc)(Embedded_802EDA7C*);
            state->embedded.selected = ((SelectFunc)state->embedded.vtable[5])(&state->embedded);
        }
        break;
    }
    case 0x23704:
        fn_802F48FC(&state->embedded, chunk->GetData(), (u8*)chunk->GetNextChunk() - (u8*)chunk->GetData());
        break;
    default:
        break;
    }
}

extern "C" void fn_802EDEE4(
    State_802EDA7C* state, const char* directory)
{
    char path[0x80];
    nlSNPrintf(path, sizeof(path), lbl_806DF4D0, directory, lbl_8052F538);

    typedef bool (*IsAsyncFunc)(AudioSystem_802EDA7C*);
    if (((IsAsyncFunc)lbl_806E201C->vtable[2])(lbl_806E201C))
    {
        nlLoadEntireFileAsync(path, (LoadAsyncCallback)fn_802EDB14, state, 0x20, AllocateStart, 0, 0, 0);
        return;
    }

    unsigned long size;
    nlChunk* outer = (nlChunk*)nlLoadEntireFile(path, &size, 0x20, AllocateStart, 0, 0, 0);
    state->fileData = outer;
    nlChunk* chunk = outer->GetFirstChunk();
    nlChunk* end = outer->GetNextChunk();
    while (chunk != end)
    {
        typedef void (*ProcessChunkFunc)(State_802EDA7C*, nlChunk*);
        ((ProcessChunkFunc)state->vtable[9])(state, chunk);
        chunk = chunk->GetNextChunk();
    }
    typedef void (*FinishFunc)(State_802EDA7C*);
    ((FinishFunc)state->vtable[10])(state);
}

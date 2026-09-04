#include "unclassified/tu_801A4188.h"

#include "Game/Inventory.h"
#include "Game/Render/tu_802DCDB4.h"
#include "Game/SHierarchy.h"
#include "NL/MemAlloc.h"
#include "NL/gl/glMemory.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"

#include <string.h>

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

extern "C"
{
    bool fn_802B3E94(const char* path, LoadAsyncCallback callback,
        void* userData, unsigned int alignment, int allocType,
        unsigned int chunkSize, void* readBuffer0, void* readBuffer1,
        void* param, unsigned long value, MemoryAllocator* allocator);
    unsigned int* fn_802C81FC(void* data, unsigned long size,
        unsigned long* numModels, void* context);
    bool fn_802C8200(const char* path, LoadAsyncCallback callback,
        void* userData, void* context);
    bool fn_802C8204(const char* path, LoadAsyncCallback callback,
        void* userData, void* context);
    unsigned int* fn_802C8208(
        const char* path, unsigned long* numModels, void* context);
    bool fn_802C820C(const char* path, void* context);
    bool fn_802CDD78(void* data, unsigned long size,
        void* context, int value);
}

struct UnidentifiedChunkTree_802DC800
{
    /* 0x00 */ u8 mUnidentified000[0x18];
    /* 0x18 */ int mUnidentified018;
    /* 0x1C */ unsigned long mUnidentified01C;
    /* 0x20 */ void* mUnidentified020;
    /* 0x24 */ UnidentifiedChunkTree_802DC800* mUnidentified024[2];
    /* 0x2C */ u8 mUnidentified02C[1];
};

typedef void (*UnidentifiedChunkCallback_802DCC44)(
    UnidentifiedChunkTree_802DC800*);

static bool lbl_806E1F80;
static nlChunk* lbl_806E1F88;
static char lbl_806DF448[] = ".zlib";

extern "C" void fn_802DC7D0(
    void* data, unsigned long size, void* userData);
extern "C" void fn_802DC7DC(
    void* data, unsigned long size, void* userData);
extern "C" void fn_802DC7E8(
    void* data, unsigned long size, void* userData);
extern "C" void fn_802DC7F4(
    void* data, unsigned long size, void* userData);
extern "C" UnidentifiedChunkTree_802DC800* fn_802DC800(nlChunk* chunk);

CrowdModelCollection_801A4188::CrowdModelCollection_801A4188()
{
    mUnidentified000 = false;
    mUnidentified001 = false;
    mUnidentified002 = false;
    mUnidentified003 = false;
    mUnidentified004 = 0;
    mUnidentified008 = 0;
    mUnidentified00C = 0;
    mUnidentified010 = 0;
    mUnidentified014 = 0;
    mUnidentified018 = 0;
    mUnidentified020 = 0;
    mUnidentified028 = 0;
    mUnidentified038 = 0;
    models = 0;
}

CrowdModelCollection_801A4188::~CrowdModelCollection_801A4188()
{
    fn_802DB9C4(this);
}

extern "C" void fn_802DB9C4(
    CrowdModelCollection_801A4188* collection)
{
    if (collection->models != 0)
    {
        delete[] collection->models;
        collection->models = 0;
    }

    for (int i = 0; i < collection->mUnidentified00C; ++i)
    {
        if (collection->mUnidentified04C[i] != 0)
        {
            delete collection->mUnidentified04C[i];
        }
    }

    if (collection->mUnidentified04C != 0)
    {
        delete[] collection->mUnidentified04C;
        collection->mUnidentified04C = 0;
    }

    if (collection->mUnidentified008 != 0)
    {
        delete collection->mUnidentified008;
    }

    collection->mUnidentified008 = 0;
    collection->mUnidentified014 = 0;
    collection->mUnidentified002 = false;
    collection->mUnidentified003 = false;
    collection->mUnidentified001 = false;
    collection->mUnidentified000 = false;
}

extern "C" void fn_802DBDA0(CrowdModelCollection_801A4188* collection,
    CrowdCharacterDefinition_801A4188* definitions, int count)
{
    collection->mUnidentified010 = definitions;
    collection->mUnidentified00C = count;
    collection->models = new (8, false) ImpostorModel_802DAEE0*[count];
    collection->mUnidentified04C = new (8, false) cInventory<cSAnim>*[count];

    for (int i = 0; i < count; ++i)
    {
        collection->models[i] = 0;
        collection->mUnidentified04C[i] = 0;
    }
}

extern "C" bool fn_802DBF5C(
    CrowdModelCollection_801A4188* collection)
{
    return collection->mUnidentified014 < collection->mUnidentified00C;
}

extern "C" void fn_802DBF7C(
    CrowdModelCollection_801A4188* collection)
{
    collection->mUnidentified000 = false;
    collection->mUnidentified001 = false;
    collection->mUnidentified002 = false;
    collection->mUnidentified003 = false;

    if (collection->mUnidentified008 == 0)
    {
        collection->mUnidentified008
            = new (8, false) cInventory<cSHierarchy>;
    }

    CrowdCharacterDefinition_801A4188& definition
        = collection->mUnidentified010[collection->mUnidentified014];

    collection->mUnidentified018 = 0;
    collection->mUnidentified01C = 0;
    if (lbl_806E1F80)
    {
        fn_802C820C(definition.mUnidentified10, fn_802CC094());
        collection->mUnidentified018 = (void*)-1;
    }
    else
    {
        fn_802C8204(definition.mUnidentified10, fn_802DC7D0,
            collection, fn_802CC094());
    }

    collection->mUnidentified030 = 0;
    collection->mUnidentified034 = 0;
    collection->mUnidentified040 = 0;
    if (lbl_806E1F80)
    {
        unsigned long numModels;
        unsigned int* models = fn_802C8208(
            definition.mUnidentified14, &numModels, fn_802CC094());
        collection->mUnidentified040 = *models;
    }
    else
    {
        fn_802C8200(definition.mUnidentified14, fn_802DC7DC,
            collection, fn_802CC094());
    }

    collection->mUnidentified038 = 0;
    collection->mUnidentified03C = 0;
    CurrentAllocator = &StandardAllocator;
    AllocatorStack[AllocatorStackDepth++] = &StandardAllocator;
    if (lbl_806E1F80)
    {
        collection->mUnidentified038 = nlLoadEntireFile(
            definition.mUnidentified08, &collection->mUnidentified03C,
            0x20, AllocateStart, 0, 0, 0);
    }
    else
    {
        nlLoadEntireFileAsync(definition.mUnidentified08, fn_802DC7E8,
            collection, 0x20, AllocateStart, 0, 0, 0);
    }

    collection->mUnidentified028 = 0;
    collection->mUnidentified02C = 0;
    CurrentAllocator = &StandardAllocator;
    AllocatorStack[AllocatorStackDepth++] = &StandardAllocator;
    collection->mUnidentified004 = 0;

    const char* path = definition.mUnidentified04;
    if (lbl_806E1F80)
    {
        if (strstr(path, lbl_806DF448) == 0)
        {
            collection->mUnidentified028 = nlLoadEntireFile(path,
                &collection->mUnidentified02C, 0x20, AllocateStart,
                0, 0, 0);
        }
    }
    else if (strstr(path, lbl_806DF448) != 0)
    {
        fn_802B3E94(path, fn_802DC7F4, collection, 0x20,
            AllocateStart, 0x10000, 0, 0, 0, 0, 0);
    }
    else
    {
        nlLoadEntireFileAsync(path, fn_802DC7F4, collection,
            0x20, AllocateStart, 0, 0, 0);
    }
}

extern "C" bool fn_802DC2A4(
    CrowdModelCollection_801A4188* collection)
{
    if (!collection->mUnidentified000)
    {
        if (collection->mUnidentified018 == (void*)-1)
        {
            collection->mUnidentified000 = true;
        }
        else if (collection->mUnidentified018 == 0)
        {
            collection->mUnidentified000 = false;
        }
        else
        {
            fn_802CDD78(collection->mUnidentified018,
                collection->mUnidentified01C, fn_802CC094(), 0);
            nlFree(collection->mUnidentified018);
            collection->mUnidentified018 = 0;
            collection->mUnidentified000 = true;
        }
    }

    if (!collection->mUnidentified001)
    {
        if (collection->mUnidentified040 != 0)
        {
            collection->mUnidentified001 = true;
        }
        else if (collection->mUnidentified030 == 0)
        {
            collection->mUnidentified001 = false;
        }
        else
        {
            unsigned long numModels = 0;
            unsigned int* models = fn_802C81FC(collection->mUnidentified030,
                collection->mUnidentified034, &numModels, fn_802CC094());
            nlFree(collection->mUnidentified030);
            collection->mUnidentified030 = 0;
            collection->mUnidentified040 = *models;
            collection->mUnidentified001 = true;
        }
    }

    if (!collection->mUnidentified002)
    {
        if (collection->mUnidentified038 == 0)
        {
            collection->mUnidentified002 = false;
        }
        else
        {
            collection->mUnidentified008->AddFile(
                (char*)collection->mUnidentified038,
                collection->mUnidentified03C);

            --AllocatorStackDepth;
            AllocatorStack[AllocatorStackDepth] = 0;
            CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
            collection->mUnidentified002 = true;
        }
    }

    if (!collection->mUnidentified003)
    {
        if (collection->mUnidentified028 == 0)
        {
            collection->mUnidentified003 = false;
        }
        else
        {
            collection->mUnidentified004
                = new (8, false) cInventory<cSAnim>;
            collection->mUnidentified004->AddFile(
                (char*)collection->mUnidentified028,
                collection->mUnidentified02C);
            collection->mUnidentified04C[collection->mUnidentified014]
                = collection->mUnidentified004;

            --AllocatorStackDepth;
            AllocatorStack[AllocatorStackDepth] = 0;
            CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
            collection->mUnidentified003 = true;
        }
    }

    return collection->mUnidentified000
        && collection->mUnidentified001
        && collection->mUnidentified002
        && collection->mUnidentified003;
}

extern "C" void fn_802DC6F8(
    CrowdModelCollection_801A4188* collection)
{
    ImpostorModel_802DAEE0* model
        = new (8, false) ImpostorModel_802DAEE0(
            *collection->mUnidentified008->Find(
                const_cast<char*>(collection
                        ->mUnidentified010[collection->mUnidentified014]
                        .mUnidentified0C)),
            collection->mUnidentified040, collection->mUnidentified004,
            fn_802CC094());
    collection->models[collection->mUnidentified014] = model;
    ++collection->mUnidentified014;
}

extern "C" void fn_802DC7D0(
    void* data, unsigned long size, void* userData)
{
    CrowdModelCollection_801A4188* collection
        = (CrowdModelCollection_801A4188*)userData;
    collection->mUnidentified018 = data;
    collection->mUnidentified01C = size;
}

extern "C" void fn_802DC7DC(
    void* data, unsigned long size, void* userData)
{
    CrowdModelCollection_801A4188* collection
        = (CrowdModelCollection_801A4188*)userData;
    collection->mUnidentified030 = data;
    collection->mUnidentified034 = size;
}

extern "C" void fn_802DC7E8(
    void* data, unsigned long size, void* userData)
{
    CrowdModelCollection_801A4188* collection
        = (CrowdModelCollection_801A4188*)userData;
    collection->mUnidentified038 = data;
    collection->mUnidentified03C = size;
}

extern "C" void fn_802DC7F4(
    void* data, unsigned long size, void* userData)
{
    CrowdModelCollection_801A4188* collection
        = (CrowdModelCollection_801A4188*)userData;
    collection->mUnidentified028 = data;
    collection->mUnidentified02C = size;
}

extern "C" UnidentifiedChunkTree_802DC800* fn_802DC800(nlChunk* chunk)
{
    lbl_806E1F88 = chunk;
    UnidentifiedChunkTree_802DC800* node
        = (UnidentifiedChunkTree_802DC800*)chunk->GetData();

    if (node->mUnidentified01C != 0)
    {
        node->mUnidentified020 = node->mUnidentified02C;
    }

    for (int i = 0; i < 2; ++i)
    {
        if (node->mUnidentified024[i] != 0)
        {
            lbl_806E1F88 = lbl_806E1F88->GetNextChunk();
            node->mUnidentified024[i] = fn_802DC800(lbl_806E1F88);
        }
    }
    return node;
}

extern "C" UnidentifiedChunkTree_802DC800* fn_802DC9EC(nlChunk* chunk)
{
    return fn_802DC800((nlChunk*)chunk->GetData());
}

extern "C" void fn_802DCC44(UnidentifiedChunkTree_802DC800* node,
    void* unidentified, UnidentifiedChunkCallback_802DCC44 callback,
    unsigned long value, int state)
{
    FrustumResult_802DD05C result;
    if (state == 0)
    {
        result = FRUSTUM_OUTSIDE;
    }
    else if (state == 1)
    {
        result = FRUSTUM_INSIDE;
    }
    else
    {
        result = fn_802DD05C((const nlVector4*)unidentified,
            (const nlVector3*)node,
            (const nlVector3*)&node->mUnidentified000[0xC], &value);
    }

    if (result)
    {
        node->mUnidentified018 = 1;
        if (callback != 0)
        {
            callback(node);
        }
    }
    else
    {
        node->mUnidentified018 = 0;
    }

    for (int i = 0; i < 2; ++i)
    {
        if (node->mUnidentified024[i] != 0)
        {
            fn_802DCC44(node->mUnidentified024[i], unidentified,
                callback, value, result);
        }
    }
}

extern "C" void fn_802DCD18(UnidentifiedChunkTree_802DC800* node,
    void* unidentified, UnidentifiedChunkCallback_802DCC44 callback)
{
    fn_802DCC44(node, unidentified, callback, 0, 2);
}

#include "unclassified/tu_806E1908.h"

#include "NL/MemAlloc.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glState.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"

#include <RVLFaceLib/RFL_Database.h>
#include <RVLFaceLib/RFL_Icon.h>
#include <RVLFaceLib/RFL_System.h>
#include <revolution/arc.h>

TU806E1908* lbl_806E1908;

extern "C" const char* fn_804CF018();
extern "C" void fn_802CDF14(
    unsigned long texture, PlatTexture* platformTexture,
    ResourceInterface_802CC094* resource);

extern "C" void fn_8026EF60(void* buffer, unsigned long size, void* userData)
{
    TU806E1908* object = lbl_806E1908;
    bool found = false;
    void* resource;
    unsigned long resourceSize;
    ARCHandle handle;
    ARCFileInfo file;

    object->mUnidentified0C = buffer;
    object->mUnidentified08 = VirtualAllocator.Allocate(RFLGetWorkSize(TRUE), 32, false);

    if (ARCInitHandle(buffer, &handle))
    {
        if (ARCOpen(&handle, fn_804CF018(), &file))
        {
            resource = ARCGetStartAddrInMem(&file);
            resourceSize = ARCGetLength(&file);
            found = true;
            ARCClose(&file);
        }
    }

    if (found)
    {
        object->mUnidentified05 = RFLInitRes(
            object->mUnidentified08, resource, resourceSize, TRUE) == RFLErrcode_Success;
    }
    object->mUnidentified04 = true;
}

TU806E1908::TU806E1908()
    : mUnidentified04(false)
    , mUnidentified05(false)
{
    for (int i = 0; i < 10; ++i)
    {
        char name[12];
        nlSNPrintf(name, sizeof(name), "mii icon %d", i);
        mUnidentified3C[i] = glGetTexture(name);

        ResourceInterface_802CC094* resource = fn_802CC094();
        mUnidentified64[i] = glx_CreatePlatTexture((MemoryAllocator*)resource);
        fn_802CDF14(mUnidentified3C[i], mUnidentified64[i], resource);

        mUnidentified14[i] = nlMalloc(0x8000, 32, false);
        mUnidentified64[i]->CreateWithMemory(
            128, 128, GXTex_RGB5A3, 1, mUnidentified14[i]);
        mUnidentified64[i]->Prepare();
    }

    mUnidentified10 = VirtualAllocator.Allocate(RFLGetMiddleDBBufferSize(1), 32, false);
    RFLInitMiddleDB(&mUnidentified8C, RFLMiddleDBType_UserSet, mUnidentified10, 1);
}

TU806E1908::~TU806E1908()
{
    RFLExit();
    for (int i = 0; i < 10; ++i)
    {
        ::operator delete[](mUnidentified14[i]);
        mUnidentified14[i] = 0;
    }

    VirtualAllocator.Free(mUnidentified08);
    VirtualAllocator.Free(mUnidentified0C);
    VirtualAllocator.Free(mUnidentified10);
}

extern "C" void fn_8026F21C(TU806E1908* object)
{
    char filename[32];
    nlSNPrintf(filename, sizeof(filename), "%s", "/RFLRes01.arc");
    nlLoadEntireFileAsync(filename, fn_8026EF60, 0, 32, AllocateStart, 0, 0, &VirtualAllocator);
}

extern "C" bool fn_8026F280(TU806E1908* object, int index, int slot, RFLExpression value)
{
    if (index < 0)
    {
        return false;
    }

    gxInit();
    if (RFLIsAvailableOfficialData(index))
    {
        GXColor bgColor = { 0, 0, 0, 0 };
        RFLIconSetting setting;
        setting.bgType = RFLIconBG_Direct;
        setting.bgColor = bgColor;
        setting.drawXluOnly = FALSE;
        setting.width = 128;
        setting.height = 128;

        if (RFLMakeIcon(object->mUnidentified14[slot], RFLDataSource_Official,
                0, index, value, &setting) == RFLErrcode_Success)
        {
            gxInit();
            return true;
        }
    }
    gxInit();
    return false;
}

extern "C" bool fn_8026F358(TU806E1908* object, const RFLStoreData* data, int slot, RFLExpression value)
{
    RFLUpdateMiddleDBAsync(&object->mUnidentified8C);
    if (RFLAddMiddleDBStoreData(&object->mUnidentified8C, data) == RFLErrcode_Success)
    {
        gxInit();
        GXColor bgColor = { 0, 0, 0, 0 };
        RFLIconSetting setting;
        setting.bgType = RFLIconBG_Direct;
        setting.bgColor = bgColor;
        setting.drawXluOnly = FALSE;
        setting.width = 128;
        setting.height = 128;

        if (RFLMakeIcon(object->mUnidentified14[slot], RFLDataSource_Middle,
                &object->mUnidentified8C, 0, value, &setting) == RFLErrcode_Success)
        {
            gxInit();
            return true;
        }
        gxInit();
    }
    return false;
}

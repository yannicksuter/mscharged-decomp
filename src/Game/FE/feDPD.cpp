#include "NL/plat/PlatPadManager.h"
#include "NL/plat/WiiRemotePad.h"
#include "Game/Render/RLViewLayers.h"
#include "NL/plat/WiiFreestylePad.h"

#include "Game/FE/feDPD.h"

#include "Game/FE/feInput.h"
#include "Game/PadActions.h"
#include "Game/FE/fePointerManager.h"
#include "Game/TweakRegistry.h"
#include "NL/gl/glStruct.h"
#include "NL/globalpad.h"
#include "NL/platpad.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/plat/DPDData.h"
#include "decomp.h"
#include "Game/Render/RLViewLayers.h"

TLComponentInstance* gFEPointerInstances[4];
nlVector2 gFEPointerPositions[4];
bool gFEPointerEnabled[4];

FEDPDTask::FEDPDTask()
{
    if (g_pFEPointerManager == 0)
    {
        g_pFEPointerManager = new (nlMalloc(sizeof(FEPointerManager), 8, false))
            FEPointerManager;
    }

    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i] = 0;
        gFEPointerPositions[i].x = 0.0f;
        gFEPointerPositions[i].y = 0.0f;
        gFEPointerEnabled[i] = false;
    }
    g_pFEPointerManager->mUnidentified054 = 30;
}

FEDPDTask::~FEDPDTask()
{
    if (g_pFEPointerManager != 0)
    {
        delete g_pFEPointerManager;
        g_pFEPointerManager = 0;
    }
}

nlVector2 GetPointerPosition(int pad, u8* valid)
{
    u16 angle;
    return GetPointerPosition(pad, &angle, valid);
}

nlVector2 GetPointerPosition(int pad, u16* angle, u8* valid)
{
    cGlobalPad* globalPad = g_pPadManager->GetPad(pad);
    PadBackend* device = globalPad->mBackend;
    nlVector2 position;
    nlVec2Set(position, 0.0f, 0.0f);

    gl_ScreenInfo* screenInfo = glGetScreenInfo();
    int width = IsWidescreen() ? 854 : screenInfo->ScreenWidth;
    int height = screenInfo->ScreenHeight;

    if (g_bEnableGamecubePadMonkey)
    {
        char path[32];
        nlSNPrintf(path, sizeof(path), "user/monkey_%d_offset", pad);
        float offset = GetTweakFloat(path, 50.0f);
        float randomX = nlRandomf(-offset, offset, &nlDefaultSeed);
        float randomY = nlRandomf(-offset, offset, &nlDefaultSeed);
        position.x = randomX + gFEPointerPositions[pad].x;
        position.y = randomY + gFEPointerPositions[pad].y;

        gFEPointerPositions[pad].x = CLAMP(
            (float)((-width / 2) + 20), (float)((width / 2) - 20), position.x);
        gFEPointerPositions[pad].y = CLAMP(
            (float)((-height / 2) + 5), (float)((height / 2) - 5), position.y);
        *angle = 0;
        *valid = true;
        return position;
    }

    DPDData* data = 0;
    if (device->GetClassID() == gWiiFreestylePadClassID)
    {
        data = &static_cast<WiiFreestylePad*>(globalPad->mBackend)->mDPDData;
    }
    else if (!gDisableRemotePointer
             && device->GetClassID() == gWiiRemotePadClassID)
    {
        data = &static_cast<WiiRemotePad*>(globalPad->mBackend)->mDPDData;
    }

    if (data != 0 && g_pFEInput->IsConnected((eFEINPUT_PAD)pad))
    {
        if (data->mValidFlag > 0)
        {
            data->GetPosition(&position, angle);
            position.x = -1.0f * (position.x * (float)width * 0.5f);
            position.y = position.y * (float)height * 0.5f;
            *valid = true;

            gFEPointerPositions[pad].x = CLAMP(
                (float)((-width / 2) + 20),
                (float)((width / 2) - 20),
                position.x);
            gFEPointerPositions[pad].y = CLAMP(
                (float)((-height / 2) + 5),
                (float)((height / 2) - 5),
                position.y);
        }
        else
        {
            *angle = 0;
            *valid = true;
        }
    }
    else
    {
        *valid = false;
        nlVec2Set(position, -999.0f, -999.0f);
        return position;
    }

    return gFEPointerPositions[pad];
}

void FEDPDTask::Run(float)
{
    for (int i = 0; i < 4; ++i)
    {
        if (g_pPadManager->GetPad(i) != 0
            && !g_pPlatPadManager->IsDPDEnabled(i))
        {
            g_pPlatPadManager->SetDPDEnabled(i, true);
        }
    }
}

void SetPointerColour(int, nlColour)
{
}

bool IsFreeStylePad(int index)
{
    cGlobalPad* globalPad = g_pPadManager->GetPad(index);
    PadBackend* device = globalPad->mBackend;
    int type = device->GetClassID();
    return type == gWiiFreestylePadClassID;
}

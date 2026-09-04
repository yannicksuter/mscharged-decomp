#include <revolution/pad.h>

#include "Game/Task/FrontEndTask.h"

#include "types.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/BasicStadium.h"
#include "Game/FE/feHelpFuncs.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feManager.h"
#include "Game/FE/feModelManager.h"
#include "Game/FE/feResourceManager.h"
#include "Game/FE/feSceneManager.h"
#include "Game/GL/GLInventory.h"
#include "Game/GameInfo.h"
#include "Game/GameObjectLighting.h"
#include "Game/HBMManager_8024795C.h"
#include "Game/Render/Presentation.h"
#include "Game/TweakRegistry.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/main.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/globalpad.h"
#include "NL/glx/glxSend.h"
#include "NL/nlConfig.h"
#include "NL/nlTask.h"
#include "unclassified/tu_802492E4.h"

#include <math.h>

extern "C"
{
    void* fn_80271960();
    void fn_80271A64(void*, float);
    void fn_80271AEC(void*);
    void fn_80276FB8(float);
    bool fn_80277238();
    cGlobalPad* fn_802C082C(void* padManager, int padIndex);
}

extern void* lbl_806E1E28;
extern BaseGameSceneManager* lbl_806E1838;

float g_fE3DebugTime;
float g_fUpTime;
float g_fIdleGameTime;
bool g_bE3IdleReset;

void SetE3DebugTime(float value)
{
    g_fE3DebugTime = value;
}

static inline bool IsHBMActive()
{
    return gpHBMManager != 0 && gpHBMManager->mActive;
}

static void DrawFrontEndElements(float fDeltaT)
{
    UpdateGameObjectLighting();

    if ((nlTaskManager::m_pInstance->mCurrentState & 0x1b)
        && !IsHBMActive())
    {
        FrontEnd::Update(fDeltaT);
    }

    if (FEResourceManager::Instance() != 0)
    {
        FEResourceManager::Instance()->Run(fDeltaT);

        if (!IsHBMActive())
        {
            if (nlTaskManager::m_pInstance->mCurrentState & 0x1)
            {
                float realTimeDelta = nlTaskManager::m_pInstance->mRealTimeDelta;
                float timeDilation = nlTaskManager::m_pInstance->mTimeDilation;
                FESceneManager::Instance()->Update(realTimeDelta * timeDilation);
            }
            else
            {
                FESceneManager::Instance()->Update(fDeltaT);
            }
        }

        if (!IsHBMActive())
        {
            FESceneManager::Instance()->RenderActiveScenes();
        }

        if (!FESceneManager::Instance()->AreAllScenesValid())
        {
            glDiscardFrame(1);
        }
    }

    if (FEModelManager::Instance() != 0
        && !IsHBMActive())
    {
        FEModelManager::Instance()->Update(fDeltaT);
        FEModelManager::Instance()->Render();
    }

    if (nlTaskManager::m_pInstance->mCurrentState == 4 && fn_80277238()
        && !IsHBMActive())
    {
        glx_Fog(true);
        fn_802CC094()->m_inventory->Update(fDeltaT);
        fn_80276FB8(fDeltaT);
        RenderWorldNPCs();
        UpdateHighRange();
    }

    fn_80271A64(fn_80271960(), fDeltaT);

    if (!(gpHBMManager != 0 && gpHBMManager->mActive && gpHBMManager->mReady))
    {
        fn_80271AEC(fn_80271960());
    }
}

FrontEndTask::FrontEndTask()
{
    softResetTime[0] = 0.0f;
    softResetTime[1] = 0.0f;
    softResetTime[2] = 0.0f;
    softResetTime[3] = 0.0f;
    mTimeDilated = false;
}

void FrontEndTask::Run(float dt)
{
    g_fUpTime += dt;
    g_pFEInput->Update(dt);
    HandleE3IdleReset(dt);
    HandleSoftReset(dt);

    if (gpHBMManager != 0 && gpHBMManager->mReady)
    {
        if (!gpHBMManager->mActive
            && g_pFEInput->PlatJustPressed(FE_ALL_PADS, 0x2e, true, 0))
        {
            gpHBMManager->fn_80248008();
        }
        if (gpHBMManager->mActive)
        {
            gpHBMManager->fn_802480EC();
        }
    }

    fn_801FEEAC()->Update(dt);
    DrawFrontEndElements(dt);

    if (nlTaskManager::m_pInstance->mCurrentState != 4)
    {
        if (GetConfigBool(Config::Global(), "domemsnapshot", false))
        {
            TakeGameMemSnapshot::Update(dt);
        }
    }
    else if (GameInfoManager::Instance()->IsInMode3())
    {
        if (!fn_8024A34C())
        {
            fn_8024A178();
        }
        if (fn_8024A354())
        {
            fn_8024A290(dt);
        }
    }
}

void FrontEndTask::HandleE3IdleReset(float fDeltaT)
{
    if (!g_e3_Build || fn_802C2C84("/user/dosoak", false))
    {
        return;
    }

    if (g_e3_Build && !fn_802C2C84("/user/e3_idle_reset", false))
    {
        return;
    }

    if (GameInfoManager::Instance()->IsInMode2()
        && (nlTaskManager::m_pInstance->mCurrentState & 0x2))
    {
        g_fIdleGameTime = 0.0f;
        return;
    }

    bool padsidle = true;

    for (int i = 0; i < 4; i++)
    {
        cGlobalPad* pad = fn_802C082C(lbl_806E1E28, i);
        if (!pad->IsConnected())
        {
            continue;
        }

        if (pad->IsPressed(0x2f, true) || pad->IsPressed(0x1e, true)
            || pad->IsPressed(0x1f, true) || pad->IsPressed(0x14, true)
            || pad->IsPressed(0x15, true))
        {
            padsidle = false;
            break;
        }

        if (((float)fabs(pad->AnalogLeftX()) >= 0.5f)
            || ((float)fabs(pad->AnalogLeftY()) >= 0.5f)
            || ((float)fabs(pad->AnalogRightX()) >= 0.5f)
            || ((float)fabs(pad->AnalogRightY()) >= 0.5f))
        {
            padsidle = false;
            break;
        }

        if (pad->IsPressed(0xd, true) || pad->IsPressed(0xe, true)
            || pad->IsPressed(0xb, true) || pad->IsPressed(0xc, true))
        {
            padsidle = false;
            break;
        }
    }

    if (padsidle)
    {
        if (nlTaskManager::m_pInstance->mCurrentState & 0x7)
        {
            g_fIdleGameTime += fDeltaT;

            if (g_fIdleGameTime >= 60.0f)
            {
                if (nlTaskManager::m_pInstance->mCurrentState == 4)
                {
                    if (!lbl_806E1838->IsOnStack(SCENE_TITLE))
                    {
                        lbl_806E1838->fn_801C5FB8((SceneList)0x19);
                        FESceneManager::Instance()->ForceImmediateStackProcessing();
                        lbl_806E1838->Push(SCENE_TITLE, SCREEN_NOTHING, false);
                        fn_801FEEAC()->Call("StartTitleScreenSequence");
                    }
                }
                else
                {
                    g_bE3IdleReset = true;
                    FrontEnd::ReturnToFE();
                }

                g_fIdleGameTime = 0.0f;
            }
        }
    }
    else
    {
        g_bE3IdleReset = false;
        g_fIdleGameTime = 0.0f;
    }
}

void FrontEndTask::HandleSoftReset(float fDeltaT)
{
    PADStatus pads[4];

    for (int i = 0; i < 4; i++)
    {
        cGlobalPad* pad = fn_802C082C(lbl_806E1E28, i);
        if (!pad->IsConnected())
        {
            continue;
        }

        if (pad->IsPressed(0x2c, true) && pad->IsPressed(0x2d, true)
            && pad->IsPressed(0x2e, true))
        {
            softResetTime[i] += fDeltaT;
            if (softResetTime[i] < 3.0f)
            {
                continue;
            }

            u32 chan;
            softResetTime[i] = 0.0f;
            if (i == 0)
            {
                chan = PAD_CHAN0_BIT;
            }
            else if (i == 1)
            {
                chan = PAD_CHAN1_BIT;
            }
            else if (i == 2)
            {
                chan = PAD_CHAN2_BIT;
            }
            else
            {
                chan = PAD_CHAN3_BIT;
            }

            bool bReset = PADReset(chan);
            if (!bReset)
            {
                do
                {
                    PADReset(chan);
                    PADRead(pads);
                    // Retail indexes the status array with the channel
                    // mask instead of the channel number.
                } while (pads[chan].err == PAD_ERR_NO_CONTROLLER);
            }
        }
        else
        {
            softResetTime[i] = 0.0f;
        }
    }
}

// The single out-of-line body for FEResourceManager::Run lives here: retail
// places it inside this unit's text, right after HandleSoftReset, and
// feResourceManager.o carries no copy of it.
void FEResourceManager::Run(float dt)
{
    Update(dt);
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;

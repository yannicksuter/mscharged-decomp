#include <revolution/pad.h>
#include "Game/Render/StadiumLoading.h"

#include "Game/Task/FrontEndTask.h"

#include "types.h"

#include "Game/GameSceneManager.h"
#include "Game/BasicStadium.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feManager.h"
#include "Game/FE/feModelManager.h"
#include "Game/FE/feResourceManager.h"
#include "Game/FE/feSceneManager.h"
#include "Game/GL/GLInventory.h"
#include "Game/GameInfo.h"
#include "Game/GameObjectLighting.h"
#include "Game/HBMManager.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/Render/HomeButtonFade.h"
#include "Game/TweakRegistry.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/main.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/globalpad.h"
#include "NL/glx/glxSend.h"
#include "NL/nlConfig.h"
#include "NL/nlTask.h"
#include "Game/SH/SHHallOfFame.h"

#include <math.h>

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

    if (nlTaskManager::m_pInstance->mCurrentState == 4 && IsStadiumWorldLoaded()
        && !IsHBMActive())
    {
        glx_Fog(true);
        glGetCurrentResourcePool()->m_inventory->Update(fDeltaT);
        UpdateStadium(fDeltaT);
        RenderWorldNPCs();
        UpdateHighRange();
    }

    HomeButtonFade::Instance()->Update(fDeltaT);

    if (!(gpHBMManager != 0 && gpHBMManager->mActive && gpHBMManager->mReady))
    {
        HomeButtonFade::Instance()->Render();
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
            gpHBMManager->Show();
        }
        if (gpHBMManager->mActive)
        {
            gpHBMManager->Update();
        }
    }

    FrontEndPresentation::GetInstance()->Update(dt);
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
        if (!IsHallOfFameImagePreloadStarted())
        {
            PreloadHallOfFameImages();
        }
        if (IsHallOfFameImagePreloadPending())
        {
            UpdateHallOfFameImagePreload(dt);
        }
    }
}

void FrontEndTask::HandleE3IdleReset(float fDeltaT)
{
    if (!g_e3_Build || GetTweakBool("/user/dosoak", false))
    {
        return;
    }

    if (g_e3_Build && !GetTweakBool("/user/e3_idle_reset", false))
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
        cGlobalPad* pad = g_pPadManager->GetPad(i);
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
                    if (!GameSceneManager::Instance()->IsOnStack(SCENE_TITLE))
                    {
                        GameSceneManager::Instance()->PopToScene((SceneList)0x19);
                        FESceneManager::Instance()->ForceImmediateStackProcessing();
                        GameSceneManager::Instance()->Push(SCENE_TITLE, SCREEN_NOTHING, false);
                        FrontEndPresentation::GetInstance()->Call("StartTitleScreenSequence");
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
        cGlobalPad* pad = g_pPadManager->GetPad(i);
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


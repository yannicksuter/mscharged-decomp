#include "Game/Render/Wiper.h"

#include "Game/NisPlayer.h"
#include "Game/Effects/EmissionController.h"
#include "Game/FE/feManager.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Render/RLView.h"
#include "NL/gl/gl.h"
#include "NL/nlConfig.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include "NL/gl/glMemory.h"
#include "string.h"
#include "NL/nlstring_tmpl.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Sys/audio.h"

class MemoryAllocator;

namespace
{
static WiperCallback wiperCallback;
}

void WiperCallback::TransitionFinished()
{
    mTransitionActive = false;
}

void WiperCallback::TransitionProgressed(float fDeltaT)
{
}

void Wiper::Reset()
{
    wiperCallback.mTransitionActive = false;
    ScreenTransitionManager::Instance()->m_SelectedTransition = 0;
}

void Wiper::Initialize()
{
    GLView* view = GetLayerView(eCLV_Transitions);
    ScreenTransitionManager::Instance()->m_eView = view;
    glLoadTextureBundle("art/transitions/transitions.rlt",
        glGetCurrentResourcePool());

    unsigned long fileSize = 0;
    char* loadedData = (char*)fxLoadEntireFileHigh("art/transitions/transitions.fx", &fileSize);
    ScreenTransitionManager::Instance()->AddTransitions(loadedData, fileSize);
    nlFree(loadedData);
}

bool Wiper::WipeInProgress() const
{
    return wiperCallback.mTransitionActive;
}

bool Wiper::CutHasOccured() const
{
    return ScreenTransitionManager::Instance()->m_Cut;
}

Wiper& Wiper::Instance()
{
    static Wiper instance;
    return instance;
}

void Wiper::DoWipe(const char* wipe)
{
    if (!wiperCallback.mTransitionActive)
    {
        if (g_ForceDoubleBallTransition)
        {
            wipe = "double_ball";
        }

        wiperCallback.mTransitionActive = true;

        if (nlStrICmp<char>(wipe, "out") == 0 || nlStrICmp<char>(wipe, "in") == 0)
        {
            PlaySound(10, 0xE7013118, 0, 0);
        }

        if (strcmp(wipe, "cut") == 0)
        {
            wiperCallback.mTransitionActive = false;
            wiperCallback.Cut();
            return;
        }

        ScreenTransitionManager::Instance()->m_pCallback = &wiperCallback;
        if (!g_ForceDoubleBallTransition && ScreenTransitionManager::Instance()->m_SelectedTransition != 0)
        {
            ScreenTransitionManager::Instance()->EnableSelectedTransition();
            return;
        }

        ScreenTransitionManager::Instance()->EnableRandomTransition(wipe);
        g_ForceDoubleBallTransition = false;
    }
}

void Wiper::Run(float dt)
{
    if (!FrontEnd::m_bGameOver)
    {
        bool b = GetFixedUpdateTask()->mfFrameLockTime > 0.0f;
        if (!b && nlTaskManager::m_pInstance->mCurrentState == 1)
        {
            dt = 0.0f;
        }
    }

    dt = dt * GetConfigFloat(Config::Global(), "transitions/speed", 1.0f);
    ScreenTransitionManager::Instance()->Update(dt);
}

void Wiper::Render()
{
    GetLayerView(eCLV_Transitions3D)->m_ClearDepth = wiperCallback.mTransitionActive;
    ScreenTransitionManager::Instance()->Render();
}

void ScreenTransitionCallback::fn_80188764()
{
}

void ScreenTransitionCallback::Cut()
{
}

void ScreenTransitionCallback::SequenceSwitch()
{
}

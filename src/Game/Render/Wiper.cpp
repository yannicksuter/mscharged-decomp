#include "Game/Render/Wiper.h"

#include "Game/Effects/EmissionController.h"
#include "Game/FE/feManager.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Render/RLView.h"
#include "NL/gl/gl.h"
#include "NL/nlConfig.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include "string.h"

class MemoryAllocator;

extern "C" bool fn_800EBBFC(int, unsigned long, const void*, void*);
extern "C" bool fn_802C820C(const char*, MemoryAllocator*);
extern "C" MemoryAllocator* fn_802CC094();

extern bool g_ForceDoubleBallTransition;

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
    fn_802C820C("art/transitions/transitions.rlt", fn_802CC094());

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
            fn_800EBBFC(10, 0xE7013118, 0, 0);
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
    if (!FrontEnd::m_bGameOver && GetFixedUpdateTask()->mfFrameLockTime <= 0.0f
        && nlTaskManager::m_pInstance->mCurrentState == 1)
    {
        dt = 0.0f;
    }

    dt = dt * GetConfigFloat(Config::Global(), "transitions/speed", 1.0f);
    ScreenTransitionManager::Instance()->Update(dt);
}

void Wiper::Render()
{
    GetLayerView(eCLV_Transitions3D)->m_ClearColour = wiperCallback.mTransitionActive;
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

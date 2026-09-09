#include "Game/FE/feCaptainComponent.h"
#include "Game/FE/feScrollText.h"

#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/feBackButton.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/OverlayHandlerInGameText.h"
#include "Game/OverlayHandlerGoal.h"
#include "Game/OverlayHandlerHUD.h"
#include "unclassified/tu_801F6A24.h"
#include "unclassified/tu_801AE530.h"
#include "Game/FE/feInput.h"
#include "Game/GameInfo.h"
#include "Game/DB/GameProgress.h"
#include "NL/nlMath.h"
#include "NL/nlFunctionMemory.h"

TU801DA134Component::TU801DA134Component()
    : mComponent(0)
    , mUnidentified08(0)
    , mUnidentified14(-1)
{
}

TU801DA134Component::~TU801DA134Component()
{
}

void TU801DA134Component::fn_801DA88C()
{
    if (mComponent != 0)
    {
        mComponent->SetActiveSlide("in", false, false);
        mComponent->m_bVisible = true;
    }

    if (mUnidentified08 != 0)
    {
        mUnidentified08->m_bVisible = true;
    }

    mUnidentified24 = 1;
    fn_801DC824(1, 1, 1);
    mUnidentified08->SetActiveSlide("Slide1", true, false);
}

int TU801DA134Component::fn_801DCD74(int index)
{
    return mSidekicks[index];
}

void TU801DA134Component::fn_801DCD84(int value)
{
    mUnidentified14 = value;
}

void TU801DA134Component::fn_801DCD8C(int index, int value)
{
    mSidekicks[index] = value;
}

void FEScrollText::SetScrollMode(int mode)
{
    m_scrollMode = mode;
}

void FEScrollText::SetScrollDirection(int direction)
{
    m_scrollDirection = direction;
}

void FEScrollText::SetEndBehavior(int behavior)
{
    m_endBehavior = behavior;
}

void FEBackButton::SetBackScene(int value)
{
    mBackScene = value;
}

extern "C" void fn_801E258C()
{
}

extern "C" void fn_801E68DC(void* p)
{
    ((char*)p)[33] = 0;
    *(int*)((char*)p + 188) = 3;
}

extern "C" void fn_801E2A14(void* p)
{
    *(int*)((char*)p + 276) = -1;
    *(int*)((char*)p + 280) = -1;
    *(int*)((char*)p + 284) = -1;
}

extern "C" void fn_801DE570(void* p, unsigned char v)
{
    void* q = *(void**)((char*)p + 116);
    if (q == 0)
    {
        return;
    }
    *(unsigned char*)((char*)q + 142) = v;
}

extern "C" void* fn_801E2DEC(void* p, int flag)
{
    if ((p != 0) && (flag > 0))
    {
        FreeFunctionMemory(p, 20);
    }
    return p;
}

extern "C" void* fn_801E2DA8(void* p, int flag)
{
    if ((p != 0) && (flag > 0))
    {
        FreeFunctionMemory(p, 24);
    }
    return p;
}

extern "C" void* fn_801E2D64(void* p, int flag)
{
    if ((p != 0) && (flag > 0))
    {
        FreeFunctionMemory(p, 24);
    }
    return p;
}

extern "C" void* fn_801E2D20(void* p, int flag)
{
    if ((p != 0) && (flag > 0))
    {
        FreeFunctionMemory(p, 24);
    }
    return p;
}

extern "C" void* fn_801E6DF4(void* p, int flag)
{
    if ((p != 0) && (flag > 0))
    {
        FreeFunctionMemory(p, 20);
    }
    return p;
}

extern "C" void* fn_801E6E38(void* p, int flag)
{
    if ((p != 0) && (flag > 0))
    {
        FreeFunctionMemory(p, 24);
    }
    return p;
}

extern "C" void* fn_801E1258(void* p, int flag)
{
    if ((p != 0) && (flag > 0))
    {
        FreeFunctionMemory(p, 32);
    }
    return p;
}

extern "C" void fn_801E2230(void* p, int arg)
{
    void* q = *(void**)((char*)p + 264);
    if (q == 0)
    {
        return;
    }
    ((InGameTextOverlay*)q)->SetSlide((OverlaySlideName)arg);
}

extern "C" void fn_801E222C(BaseGameSceneManager* mgr)
{
    mgr->BaseGameSceneManager::Pop();
}

extern "C" void fn_801E2564(BaseGameSceneManager* mgr)
{
    ((GoalOverlay*)mgr->GetScene((SceneList)95))->Restart();
}

extern "C" void fn_801E21E0(BaseGameSceneManager* mgr, SceneList scene, ScreenMovement movement, bool popfirst)
{
    BaseSceneHandler* h = mgr->BaseGameSceneManager::Push(scene, movement, popfirst);
    if ((h != 0) && (scene == 90))
    {
        *(BaseSceneHandler**)((char*)mgr + 264) = h;
    }
}

extern "C" void fn_801E0AD0(void* p)
{
    ((TLComponentInstance*)(*(void**)((char*)p + 116)))->SetActiveSlide("in", true, false);
    void* q = *(void**)((char*)p + 116);
    if (q != 0)
    {
        *(unsigned char*)((char*)q + 142) = 1;
    }
}

extern "C" bool fn_801E45C0()
{
    bool b = g_pFEInput->JustPressed(FE_ALL_PADS, 31, true, 0) || g_pFEInput->JustPressed(FE_ALL_PADS, 30, true, 0);
    return b;
}

extern "C" void __dt__20BaseGameSceneManagerFv(void* p, int flag);
extern "C" void __dl__FPv(void* p);

extern "C" void* fn_801E14BC(void* p, int flag)
{
    if (p != 0)
    {
        __dt__20BaseGameSceneManagerFv(p, 0);
        if (flag > 0)
        {
            __dl__FPv(p);
        }
    }
    return p;
}

extern "C" void fn_801E2988(void* unused, void* s)
{
    if (*(signed char*)((char*)s + 5) == 0)
    {
        return;
    }
    ((HUDOverlay*)g_pOverlayManager->GetScene(OVERLAY_HUD))->UpdateScore();
}

extern "C" void fn_801E28A8(void* unused, void* s)
{
    void* saved = s;
    TU801F6A24Overlay* ov = (TU801F6A24Overlay*)g_pOverlayManager->GetScene((SceneList)100);
    float f = *(volatile float*)((char*)saved + 4);
    ov->fn_801F6D94(f);
}

extern "C" void fn_801E28E4(void* unused, void* s)
{
    void* saved = s;
    TU801F6A24Overlay* ov = (TU801F6A24Overlay*)g_pOverlayManager->GetScene((SceneList)100);
    float f = *(volatile float*)((char*)saved + 4);
    ov->fn_801F6E18(f);
}

extern "C" void fn_801E29C0(void* unused, void* s)
{
    void* saved = s;
    TU801F6A24Overlay* ov = (TU801F6A24Overlay*)g_pOverlayManager->GetScene((SceneList)100);
    volatile u32* src = (volatile u32*)saved;
    u32 a2;
    u32 a1;
    u32 a0;
    a0 = src[0];
    a1 = src[1];
    a2 = src[2];
    nlVector3 v;
    v.as_u32[0] = a0;
    v.as_u32[1] = a1;
    v.as_u32[2] = a2;
    ov->fn_801F6E8C(v);
}

extern "C" void fn_801DCCEC(TU801DA134Component* comp)
{
    int team;
    if (nlSingleton<GameInfoManager>::Instance()->IsInMode3())
    {
        team = g_pCupManager->unknown_0x8A28;
    }
    else
    {
        team = nlSingleton<GameInfoManager>::Instance()->GetTeam((short)comp->mSide);
    }
    GameRules* rules = &nlSingleton<GameInfoManager>::Instance()->mRulesTable[team];
    comp->mSidekicks[0] = rules->unknown_0x0;
    comp->mSidekicks[1] = rules->unknown_0x4;
    comp->mSidekicks[2] = rules->unknown_0x8;
}

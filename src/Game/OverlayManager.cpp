#include "Game/OverlayManager.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/BaseSceneHandler.inl"
#include "Game/EventDataTypes.h"
#include "Game/EventRegistry.h"
#include "Game/GameInfo.h"
#include "Game/GameSceneManager.h"
#include "Game/OverlayHandlerGoal.h"
#include "Game/OverlayHandlerHUD.h"
#include "Game/OverlayHandlerInGameText.h"
#include "Game/FE/Overlay/OverlayHandlerMegaStrikeMeter.h"
#include "Game/Render/NumberDisplay.h"
#include "Game/FE/feSceneManager.h"
#include "Game/TweakQuery.h"
#include "Game/Render/Presentation.h"
#include "Game/SH/SHStrikerTimesBase.h"
#include "Game/main.h"
#include "NL/nlBindMember.h"
#include "NL/nlBind_impl.h"
#include "NL/nlFunction.inl"
#include "NL/nlFunctionMemory.h"

BaseGameSceneManager* g_pOverlayManager;
bool lbl_806E1864;

OverlayManager::OverlayManager()
{
    mInGameTextOverlay = 0;
    mIsHUDSlideIn = false;
    mDoHUDSlideIn = false;
    mUnidentified10E = 0;
    mIsDemoSlideVisible = false;
    mHUDDelay = 0.0f;
    mUnidentified120 = 0;
    mStrikerTimesStoryVariant = -1;
    mStrikerTimesHeadlineVariant = -1;
    mStrikerTimesImageVariant = -1;
}

OverlayManager::~OverlayManager()
{
}

void OverlayManager::fn_801E1514()
{
    UnidentifiedFindEvent<UnidentifiedEventNoData>("GetReadyForKickoff", -1)->Add(Function<FnVoidVoid>(BindMember(this, &OverlayManager::fn_801E258C)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("Kickoff", -1)->Add(Function<FnVoidVoid>(BindMember(this, &OverlayManager::fn_801E2590)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("GameOver", -1)->Add(Function<FnVoidVoid>(BindMember(this, &OverlayManager::fn_801E2608)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData_8006701C>("MegaStrikeMeterStart", -1)->Add(Function<UnidentifiedEventData_8006701C*>(BindMember(this, &OverlayManager::fn_801E2784)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("MegaStrikeMeterEnd", -1)->Add(Function<FnVoidVoid>(BindMember(this, &OverlayManager::fn_801E281C)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData_8006701C>("MegaStrikeMeterFirst", -1)->Add(Function<UnidentifiedEventData_8006701C*>(BindMember(this, &OverlayManager::fn_801E28A8)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData_8006701C>("MegaStrikeMeterSecond", -1)->Add(Function<UnidentifiedEventData_8006701C*>(BindMember(this, &OverlayManager::fn_801E28E4)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("MegastrikeStart", -1)->Add(Function<FnVoidVoid>(BindMember(this, &OverlayManager::fn_801E2920)), 0, -1);
    UnidentifiedFindEvent<MegaStrikeEndData>("MegastrikeEnd", -1)->Add(Function<MegaStrikeEndData*>(BindMember(this, &OverlayManager::fn_801E2988)), 0, -1);
    UnidentifiedFindEvent<GoalScoredData>("GoalScored", -1)->Add(Function<GoalScoredData*>(BindMember(this, &OverlayManager::fn_801E2A28)), 0, -1);
}

BaseSceneHandler* OverlayManager::Push(SceneList scene, ScreenMovement movement, bool popfirst)
{
    BaseSceneHandler* h = BaseGameSceneManager::Push(scene, movement, popfirst);
    if ((h != 0) && (scene == 90))
    {
        mInGameTextOverlay = static_cast<InGameTextOverlay*>(h);
    }
    return h;
}

void OverlayManager::Pop()
{
    BaseGameSceneManager::Pop();
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

inline void OverlayManager::SlideHUDOut()
{
    mHUDDelay = 0.0f;
    if (mIsHUDSlideIn == true)
    {
        static_cast<HUDOverlay*>(GetScene((SceneList)89))->SetSlideOut();
        mIsHUDSlideIn = false;
        gpNumberDisplay->mVisible = false;
    }
}

void OverlayManager::Update(float deltaTime)
{
    if (mHUDDelay > 0.0f && nlTaskManager::m_pInstance->mCurrentState == 2)
    {
        mHUDDelay -= deltaTime;
        if (mHUDDelay <= 0.0f)
        {
            mHUDDelay = 0.0f;
            if (mDoHUDSlideIn)
            {
                fn_801E2498(0.0f);
            }
            else
            {
                SlideHUDOut();
            }
        }
    }
}

void OverlayManager::SetVisible(SceneList scene, bool visibility, bool overrideStateSettings)
{
    if (nlSingleton<GameInfoManager>::Instance()->mCurrentMode == 2 && scene != OVERLAY_HUD)
    {
        return;
    }

    const char* fileName = GetFileName(scene);
    unsigned long hashID = nlStringLowerHash(fileName);
    BaseOverlayHandler* handler = static_cast<BaseOverlayHandler*>(nlSingleton<FESceneManager>::Instance()->GetSceneHandler(hashID));
    if (handler != 0)
    {
        u32 state = nlTaskManager::m_pInstance->mCurrentState;
        if (overrideStateSettings || (handler->mVisibilityMask & state))
        {
            handler->SetVisible(visibility);
        }
    }
}

void OverlayManager::HandleStateTransition(u32 from, u32 to)
{
    if (to == 0x02000000)
    {
        return;
    }
    if (from == 0x02000000)
    {
        return;
    }

    for (u32 i = 0; i < mCurrentStackDepth; i++)
    {
        SceneList sceneType = m_sceneStack[i];
        if (sceneType <= 88)
        {
            continue;
        }

        BaseSceneHandler* handler = mBaseSceneHandlerStack[i];
        if (handler == 0)
        {
            continue;
        }

        if (sceneType == 90 && mUnidentified10E)
        {
            continue;
        }

        BaseOverlayHandler* overlayHandler = static_cast<BaseOverlayHandler*>(handler);
        if ((overlayHandler->mVisibilityMask & to) != 0)
        {
            if (!overlayHandler->mWasLastVisible)
            {
                continue;
            }
            overlayHandler->SetVisible(true);
        }
        else
        {
            overlayHandler->mWasLastVisible = overlayHandler->mVisible;
            overlayHandler->SetVisible(false);
        }
    }
}

void OverlayManager::fn_801E2498(float delay)
{
    mHUDDelay = delay;
    if (0.0f != delay)
    {
        mDoHUDSlideIn = true;
    }
    else if (!mIsHUDSlideIn)
    {
        static_cast<HUDOverlay*>(GetScene((SceneList)89))->SetSlideIn();
        mIsHUDSlideIn = true;
        gpNumberDisplay->mVisible = true;
    }
}

void OverlayManager::ShowDemoSlide()
{
    if (!mIsDemoSlideVisible)
    {
        BaseSceneHandler* scene = GetScene((SceneList)96);
        if (scene != 0)
        {
            scene->SetVisible(true);
            mIsDemoSlideVisible = true;
        }
    }
}

extern "C" void fn_801E2564(BaseGameSceneManager* mgr)
{
    ((GoalOverlay*)mgr->GetScene((SceneList)95))->Restart();
}

void OverlayManager::fn_801E258C()
{
}

void OverlayManager::fn_801E2590()
{
    static_cast<OverlayManager*>(g_pOverlayManager)->SetVisible((SceneList)90, false, false);
}

void OverlayManager::fn_801E2608()
{
    if (GetTweakBool("/user/dosoak", false)
        || (g_e3_Build && GameInfoManager::Instance()->IsInMode2()))
    {
        nlTaskManager::SetNextState(0x400000);
        return;
    }

    if (GameInfoManager::Instance()->mCurrentMode == GameInfoManager::GM_FRIENDLY)
    {
        SHStrikerTimesBase* scene = static_cast<SHStrikerTimesBase*>(g_pOverlayManager->Push((SceneList)91, SCREEN_NOTHING, false));
        scene->SetDisplayMode(10);
    }
    else if (GameInfoManager::Instance()->IsInMode4())
    {
        SHStrikerTimesBase* scene = static_cast<SHStrikerTimesBase*>(g_pOverlayManager->Push((SceneList)77, SCREEN_NOTHING, false));
        if (scene != 0)
        {
            scene->SetDisplayMode(9);
        }
    }
    else
    {
        SHStrikerTimesBase* scene = static_cast<SHStrikerTimesBase*>(g_pOverlayManager->Push((SceneList)91, SCREEN_NOTHING, false));
        scene->SetDisplayMode(11);
    }

    static_cast<OverlayManager*>(g_pOverlayManager)->SlideHUDOut();
    GetPresentation()->StopOverlay();
    static_cast<HUDOverlay*>(g_pOverlayManager->GetScene((SceneList)89))->ResetScores();
}

void OverlayManager::fn_801E2784(UnidentifiedEventData_8006701C* eventData)
{
    static_cast<OverlayManager*>(g_pOverlayManager)->SlideHUDOut();
    MegaStrikeMeterOverlay* overlay = static_cast<MegaStrikeMeterOverlay*>(g_pOverlayManager->GetScene((SceneList)100));
    overlay->SetVisible(true);
    overlay->Start(eventData->pFielder);
}

void OverlayManager::fn_801E281C()
{
    MegaStrikeMeterOverlay* overlay = static_cast<MegaStrikeMeterOverlay*>(g_pOverlayManager->GetScene((SceneList)100));
    overlay->SetVisible(false);
    if (!overlay->mMegaStrikeStarted)
    {
        static_cast<OverlayManager*>(g_pOverlayManager)->fn_801E2498(0.0f);
    }
}

void OverlayManager::fn_801E28A8(UnidentifiedEventData_8006701C* eventData)
{
    MegaStrikeMeterOverlay* overlay = static_cast<MegaStrikeMeterOverlay*>(g_pOverlayManager->GetScene((SceneList)100));
    overlay->SetFirstResult(eventData->fMeterValue);
}

void OverlayManager::fn_801E28E4(UnidentifiedEventData_8006701C* eventData)
{
    MegaStrikeMeterOverlay* overlay = static_cast<MegaStrikeMeterOverlay*>(g_pOverlayManager->GetScene((SceneList)100));
    overlay->SetSecondResult(eventData->fMeterValue);
}

void OverlayManager::fn_801E2920()
{
    lbl_806E1864 = false;
    static_cast<OverlayManager*>(g_pOverlayManager)->SlideHUDOut();
}

void OverlayManager::fn_801E2988(MegaStrikeEndData* eventData)
{
    if (eventData->goals == 0)
    {
        return;
    }
    static_cast<HUDOverlay*>(g_pOverlayManager->GetScene(OVERLAY_HUD))->UpdateScore();
}

void OverlayManager::fn_801E29C0(nlVector3 position)
{
    MegaStrikeMeterOverlay* ov = static_cast<MegaStrikeMeterOverlay*>(g_pOverlayManager->GetScene((SceneList)100));
    ov->SetPosition(position);
}

extern "C" void fn_801E2A14(void* p)
{
    *(int*)((char*)p + 276) = -1;
    *(int*)((char*)p + 280) = -1;
    *(int*)((char*)p + 284) = -1;
}

void OverlayManager::fn_801E2A28(GoalScoredData* eventData)
{
    lbl_806E1864 = true;
    if (eventData->uGoalType != 6)
    {
        static_cast<OverlayManager*>(g_pOverlayManager)->SlideHUDOut();
        gpNumberDisplay->mVisible = true;
    }
    static_cast<HUDOverlay*>(g_pOverlayManager->GetScene((SceneList)89))->UpdateScore();
}

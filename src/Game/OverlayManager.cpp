#include "NL/nlDLListContainer.inl"
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
#include "NL/nlBindMember.inl"
#include "NL/nlBind_impl.h"
#include "NL/nlFunction.inl"
#include "NL/nlFunctionMemory.h"

BaseGameSceneManager* g_pOverlayManager;
bool g_bGoalScored;

OverlayManager::OverlayManager()
{
    mInGameTextOverlay = 0;
    mIsHUDSlideIn = false;
    mDoHUDSlideIn = false;
    mIsInHighlights = false;
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

void OverlayManager::RegisterEventHandlers()
{
    UnidentifiedFindEvent<UnidentifiedEventNoData>("GetReadyForKickoff", -1)->Add(Function<FnVoidVoid>(BindMember(this, &OverlayManager::OnGetReadyForKickoff)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("Kickoff", -1)->Add(Function<FnVoidVoid>(BindMember(this, &OverlayManager::OnKickoff)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("GameOver", -1)->Add(Function<FnVoidVoid>(BindMember(this, &OverlayManager::OnGameOver)), 0, -1);
    UnidentifiedFindEvent<MegaStrikeMeterData>("MegaStrikeMeterStart", -1)->Add(Function<MegaStrikeMeterData*>(BindMember(this, &OverlayManager::OnMegaStrikeMeterStart)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("MegaStrikeMeterEnd", -1)->Add(Function<FnVoidVoid>(BindMember(this, &OverlayManager::OnMegaStrikeMeterEnd)), 0, -1);
    UnidentifiedFindEvent<MegaStrikeMeterData>("MegaStrikeMeterFirst", -1)->Add(Function<MegaStrikeMeterData*>(BindMember(this, &OverlayManager::OnMegaStrikeMeterFirst)), 0, -1);
    UnidentifiedFindEvent<MegaStrikeMeterData>("MegaStrikeMeterSecond", -1)->Add(Function<MegaStrikeMeterData*>(BindMember(this, &OverlayManager::OnMegaStrikeMeterSecond)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("MegastrikeStart", -1)->Add(Function<FnVoidVoid>(BindMember(this, &OverlayManager::OnMegastrikeStart)), 0, -1);
    UnidentifiedFindEvent<MegaStrikeEndData>("MegastrikeEnd", -1)->Add(Function<MegaStrikeEndData*>(BindMember(this, &OverlayManager::OnMegastrikeEnd)), 0, -1);
    UnidentifiedFindEvent<GoalScoredData>("GoalScored", -1)->Add(Function<GoalScoredData*>(BindMember(this, &OverlayManager::OnGoalScored)), 0, -1);
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

void OverlayManager::SetCurrentTextOverlaySlide(OverlaySlideName slideName)
{
    if (mInGameTextOverlay != 0)
    {
        mInGameTextOverlay->SetSlide(slideName);
    }
}

inline void OverlayManager::SlideHUDOut()
{
    mHUDDelay = 0.0f;
    if (mIsHUDSlideIn == true)
    {
        static_cast<HUDOverlay*>(GetScene(OVERLAY_HUD))->SetSlideOut();
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
                SlideHUDIn(0.0f);
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

        if (sceneType == 90 && mIsInHighlights)
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

void OverlayManager::SlideHUDIn(float delay)
{
    mHUDDelay = delay;
    if (0.0f != delay)
    {
        mDoHUDSlideIn = true;
    }
    else if (!mIsHUDSlideIn)
    {
        static_cast<HUDOverlay*>(GetScene(OVERLAY_HUD))->SetSlideIn();
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

void OverlayManager::RestartGoalOverlay()
{
    ((GoalOverlay*)GetScene((SceneList)95))->Restart();
}

void OverlayManager::OnGetReadyForKickoff()
{
}

void OverlayManager::OnKickoff()
{
    static_cast<OverlayManager*>(g_pOverlayManager)->SetVisible((SceneList)90, false, false);
}

void OverlayManager::OnGameOver()
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
    static_cast<HUDOverlay*>(g_pOverlayManager->GetScene(OVERLAY_HUD))->ResetScores();
}

void OverlayManager::OnMegaStrikeMeterStart(MegaStrikeMeterData* eventData)
{
    static_cast<OverlayManager*>(g_pOverlayManager)->SlideHUDOut();
    MegaStrikeMeterOverlay* overlay = static_cast<MegaStrikeMeterOverlay*>(g_pOverlayManager->GetScene((SceneList)100));
    overlay->SetVisible(true);
    overlay->Start(eventData->pFielder);
}

void OverlayManager::OnMegaStrikeMeterEnd()
{
    MegaStrikeMeterOverlay* overlay = static_cast<MegaStrikeMeterOverlay*>(g_pOverlayManager->GetScene((SceneList)100));
    overlay->SetVisible(false);
    if (!overlay->mMegaStrikeStarted)
    {
        static_cast<OverlayManager*>(g_pOverlayManager)->SlideHUDIn(0.0f);
    }
}

void OverlayManager::OnMegaStrikeMeterFirst(MegaStrikeMeterData* eventData)
{
    MegaStrikeMeterOverlay* overlay = static_cast<MegaStrikeMeterOverlay*>(g_pOverlayManager->GetScene((SceneList)100));
    overlay->SetFirstResult(eventData->fMeterValue);
}

void OverlayManager::OnMegaStrikeMeterSecond(MegaStrikeMeterData* eventData)
{
    MegaStrikeMeterOverlay* overlay = static_cast<MegaStrikeMeterOverlay*>(g_pOverlayManager->GetScene((SceneList)100));
    overlay->SetSecondResult(eventData->fMeterValue);
}

void OverlayManager::OnMegastrikeStart()
{
    g_bGoalScored = false;
    static_cast<OverlayManager*>(g_pOverlayManager)->SlideHUDOut();
}

void OverlayManager::OnMegastrikeEnd(MegaStrikeEndData* eventData)
{
    if (eventData->goals == 0)
    {
        return;
    }
    static_cast<HUDOverlay*>(g_pOverlayManager->GetScene(OVERLAY_HUD))->UpdateScore();
}

void OverlayManager::SetMegaStrikeMeterPosition(nlVector3 position)
{
    MegaStrikeMeterOverlay* ov = static_cast<MegaStrikeMeterOverlay*>(g_pOverlayManager->GetScene((SceneList)100));
    ov->SetPosition(position);
}

void OverlayManager::ResetStrikerTimesVariants()
{
    mStrikerTimesStoryVariant = -1;
    mStrikerTimesHeadlineVariant = -1;
    mStrikerTimesImageVariant = -1;
}

void OverlayManager::OnGoalScored(GoalScoredData* eventData)
{
    g_bGoalScored = true;
    if (eventData->uGoalType != 6)
    {
        static_cast<OverlayManager*>(g_pOverlayManager)->SlideHUDOut();
        gpNumberDisplay->mVisible = true;
    }
    static_cast<HUDOverlay*>(g_pOverlayManager->GetScene(OVERLAY_HUD))->UpdateScore();
}

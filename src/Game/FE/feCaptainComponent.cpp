#include "Game/FE/feCaptainComponent.h"
#include "unclassified/tu_801E4630.h"
#include "Game/FE/feScrollText.h"

#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feText.h"
#include "Game/FE/feBackButton.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/EventDataTypes.h"
#include "Game/OverlayManager.h"
#include "Game/OverlayHandlerInGameText.h"
#include "Game/OverlayHandlerGoal.h"
#include "Game/OverlayHandlerHUD.h"
#include "unclassified/tu_801F6A24.h"
#include "unclassified/tu_801AE530.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feSceneManager.h"
#include "Game/GameInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/SH/SHStrikerTimesBase.h"
#include "Game/TweakQuery.h"
#include "Game/main.h"
#include "unclassified/tu_80284A58.h"
#include "NL/nlMath.h"
#include "NL/nlFunctionMemory.h"
#include "NL/nlTask.h"
#include "NL/nlString.h"
#include "NL/nlLocalizationLookup.h"

extern "C" void fn_801DCCEC(TU801DA134Component* comp);

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

void TU801DA134Component::fn_801DCB28()
{
    int sidekicks[8];
    int count = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (GetCharacterInfo(GetCharacterIndexFromSidekick(i)).unknown_0x24 == 1)
        {
            sidekicks[count++] = i;
        }
    }

    GameRules rules = { 0, 0, 0 };
    rules.unknown_0x0 = sidekicks[nlRandom(count, &nlDefaultSeed)];
    rules.unknown_0x4 = sidekicks[nlRandom(count, &nlDefaultSeed)];
    rules.unknown_0x8 = sidekicks[nlRandom(count, &nlDefaultSeed)];
    mSidekicks[0] = rules.unknown_0x0;
    mSidekicks[1] = rules.unknown_0x4;
    mSidekicks[2] = rules.unknown_0x8;
    nlSingleton<GameInfoManager>::Instance()->SetRules(mUnidentified14, rules);
}

void TU801DA134Component::fn_801DCC28()
{
    int team;
    if (nlSingleton<GameInfoManager>::Instance()->IsInMode3())
    {
        team = g_pCupManager->unknown_0x8A28;
    }
    else
    {
        team = nlSingleton<GameInfoManager>::Instance()->GetTeam((short)mSide);
    }
    nlSingleton<GameInfoManager>::Instance()->ResetRules(team);
    fn_801DCCEC(this);
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

void TU801DCD9CComponent::fn_801E0B20(bool visible)
{
    if (mUnidentified88 != 0 && visible != mUnidentified88->m_bVisible)
    {
        mUnidentified88->m_bVisible = visible;
        if (visible)
        {
            TLSlide* slide = mUnidentified88->GetActiveSlide();
            slide->m_time = 0.0f;
            mUnidentified88->GetActiveSlide()->Update(0.0f);
        }
    }
}

void FEScrollText::SetDisplayMessage(const BasicString<unsigned short, Detail::TempStringAllocator>& theMessage)
{
    m_useMessage = true;
    m_message = theMessage;
    m_controlText->SetString(m_message.c_str());
    RefreshText();
}

void FEScrollText::SetDisplayMessage(const char* locMessage)
{
    const unsigned short* text = LookupLocString(locMessage);
    SetDisplayMessage(BasicString<unsigned short, Detail::TempStringAllocator>(text));
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

void FEScrollText::SetClippingTextInstance(TLTextInstance* controlText)
{
    if (m_scrollAxis == 0)
    {
        nlVector2& boxSize = (controlText->m_OverloadFlags & 0x4)
            ? controlText->m_OverloadedAttributes.BoxSize
            : ((FEText*)controlText->m_component)->m_TextAttributes.BoxSize;
        m_width = (int)boxSize.x;
        SetMetrics((int)(controlText->GetPosition().f.x - controlText->GetPivot().f.x));
    }
    else
    {
        nlVector2& boxSize = (controlText->m_OverloadFlags & 0x4)
            ? controlText->m_OverloadedAttributes.BoxSize
            : ((FEText*)controlText->m_component)->m_TextAttributes.BoxSize;
        m_width = (int)boxSize.y;
        SetMetrics((int)(controlText->GetPosition().f.y - controlText->GetPivot().f.y));
    }
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

void OverlayManager::Pop()
{
    BaseGameSceneManager::Pop();
}

extern "C" void fn_801E2564(BaseGameSceneManager* mgr)
{
    ((GoalOverlay*)mgr->GetScene((SceneList)95))->Restart();
}

inline void OverlayManager::SlideHUDOut()
{
    mHUDDelay = 0.0f;
    if (mIsHUDSlideIn == true)
    {
        static_cast<HUDOverlay*>(GetScene((SceneList)89))->SetSlideOut();
        mIsHUDSlideIn = false;
        gpNumberDisplay->mUnidentified004 = false;
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
        gpNumberDisplay->mUnidentified004 = true;
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

BaseSceneHandler* OverlayManager::Push(SceneList scene, ScreenMovement movement, bool popfirst)
{
    BaseSceneHandler* h = BaseGameSceneManager::Push(scene, movement, popfirst);
    if ((h != 0) && (scene == 90))
    {
        mInGameTextOverlay = static_cast<InGameTextOverlay*>(h);
    }
    return h;
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

OverlayManager::OverlayManager()
{
    mInGameTextOverlay = 0;
    mIsHUDSlideIn = false;
    mDoHUDSlideIn = false;
    mUnidentified10E = 0;
    mIsDemoSlideVisible = false;
    mHUDDelay = 0.0f;
    mUnidentified120 = 0;
    mUnidentified114 = 0xFFFFFFFF;
    mUnidentified118 = 0xFFFFFFFF;
    mUnidentified11C = 0xFFFFFFFF;
}

OverlayManager::~OverlayManager()
{
}

extern bool lbl_806E1864;

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
    TU801F6A24Overlay* overlay = static_cast<TU801F6A24Overlay*>(g_pOverlayManager->GetScene((SceneList)100));
    overlay->SetVisible(true);
    overlay->fn_801F6E2C(eventData->pFielder);
}

void OverlayManager::fn_801E281C()
{
    TU801F6A24Overlay* overlay = static_cast<TU801F6A24Overlay*>(g_pOverlayManager->GetScene((SceneList)100));
    overlay->SetVisible(false);
    if (!overlay->mUnidentified36)
    {
        static_cast<OverlayManager*>(g_pOverlayManager)->fn_801E2498(0.0f);
    }
}

void OverlayManager::fn_801E2920()
{
    lbl_806E1864 = false;
    static_cast<OverlayManager*>(g_pOverlayManager)->SlideHUDOut();
}

void OverlayManager::fn_801E2A28(GoalScoredData* eventData)
{
    lbl_806E1864 = true;
    if (eventData->uGoalType != 6)
    {
        static_cast<OverlayManager*>(g_pOverlayManager)->SlideHUDOut();
        gpNumberDisplay->mUnidentified004 = true;
    }
    static_cast<HUDOverlay*>(g_pOverlayManager->GetScene((SceneList)89))->UpdateScore();
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

void OverlayManager::fn_801E29C0(nlVector3 position)
{
    TU801F6A24Overlay* ov = static_cast<TU801F6A24Overlay*>(g_pOverlayManager->GetScene((SceneList)100));
    ov->fn_801F6E8C(position);
}

void TU801E4630Scene::fn_801E6504(int index, void*)
{
    mUnidentified44C->SetActiveSlide("over", true, false);
    mUnidentified110.SetPointerState(1, index);
    FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, true);
    mUnidentifiedB5 = true;
}

void TU801E4630Scene::fn_801E6578(int index, void*)
{
    mUnidentified44C->SetActiveSlide("off", true, false);
    mUnidentified110.SetPointerState(0, index);
    mUnidentifiedB5 = false;
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
    GameRules rules = nlSingleton<GameInfoManager>::Instance()->mRulesTable[team];
    comp->mSidekicks[0] = rules.unknown_0x0;
    comp->mSidekicks[1] = rules.unknown_0x4;
    comp->mSidekicks[2] = rules.unknown_0x8;
}

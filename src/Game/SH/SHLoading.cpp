#include "Game/SH/SHLoading.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "Game/NetTournManager.h"
#include "Game/Render/Presentation.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" Presentation* fn_801FEEAC();
extern "C" bool fn_801FF168(Presentation* presentation);
extern "C" bool fn_80273B00();

extern BaseGameSceneManager* lbl_806E1860;

struct LoadingFlowState
{
    unsigned char mPadding000[0x92];
    bool mLoading;
};

extern LoadingFlowState* gpHBMManager;
extern TLComponentInstance lbl_80580030;

SuperLoadingScene::SuperLoadingScene()
    : mType(TT_INVALID)
{
    gpHBMManager->mLoading = true;
}

SuperLoadingScene::~SuperLoadingScene()
{
    gpHBMManager->mLoading = false;
}

void SuperLoadingScene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (mType == TT_3D_TRANSITION)
    {
        if (!fn_801FF168(fn_801FEEAC()))
        {
            nlTaskManager::SetNextState(0x200000);
        }
    }
    else
    {
        TLSlide* slide = mFEScene->m_pFEPackage->GetPresentation()->GetActiveSlide();
        float duration = slide->m_duration;
        float start = slide->m_start;
        float time = slide->m_time;
        if (time >= start + duration)
        {
            if (mType == TT_IN)
            {
                nlTaskManager::SetNextState(0x200000);
            }
            else if (mType == TT_OUT)
            {
                lbl_806E1860->Pop();
            }
        }
    }
}

LoadingScene_801CDB4C::LoadingScene_801CDB4C()
    : mTransitionActive(false)
    , mWidescreen(false)
{
}

LoadingScene_801CDB4C::~LoadingScene_801CDB4C()
{
}

void LoadingScene_801CDB4C::Update(float dt)
{
    BaseSceneHandler::Update(dt);

    if (mTransitionActive)
    {
        TLSlide* slide = mTransitionComponent->GetActiveSlide();
        float duration = slide->m_duration;
        float start = slide->m_start;
        float time = slide->m_time;
        if (time >= start + duration)
        {
            mTransitionComponent->m_bVisible = false;
            mTransitionActive = false;
        }
    }
}

LoadingScene_801CDC2C::LoadingScene_801CDC2C()
{
}

LoadingScene_801CDC2C::~LoadingScene_801CDC2C()
{
}

void LoadingScene_801CDC2C::Update(float dt)
{
    BaseSceneHandler::Update(dt);

    if (mTransitionActive)
    {
        TLSlide* slide = mTransitionComponent->GetActiveSlide();
        float duration = slide->m_duration;
        float start = slide->m_start;
        float time = slide->m_time;
        if (time >= start + duration)
        {
            mTransitionComponent->m_bVisible = false;
            mTransitionActive = false;
        }
    }
}

extern "C" void fn_801CDFC8(void*, TLTextInstance* stadiumText)
{
    const char* stringID = GetStadiumTickerStringID(GameInfoManager::Instance()->GetStadium());
    stadiumText->SetStringId(stringID);
}

static inline TLTextInstance* FindLoadingText(FEPresentation* presentation, const char* name)
{
    TLTextInstance* text = (TLTextInstance*)fn_8030677C(
        presentation, nlStringLowerHash("Layer"), nlStringLowerHash(name), 0, 0, 0, 0);
    return text != 0 ? text : &UnidentifiedFallbackTextInstance;
}

void SuperLoadingScene::SceneCreated()
{
    FEPresentation* pres = mFEScene->m_pFEPackage->GetPresentation();
    if (mType == TT_IN)
    {
        pres->SetActiveSlide("appear", true);
    }
    else if (mType == TT_OUT)
    {
        pres->SetActiveSlide("disappear", true);
    }
    else if (mType == TT_3D_TRANSITION)
    {
        pres->SetActiveSlide("3dtransition", true);

        GameInfoManager* gameInfo = GameInfoManager::Instance();
        if (gameInfo->unknown_0x120)
        {
            if (gameInfo->mCurrentMode == 0)
            {
                fn_801CBCA0(0x7FEC4468, 0, 0, 1);
                fn_801FEEAC()->Call("StartOnlineGrudgeMatchSequence");
            }
            else if (NetTournManager::Instance()->mState != 0)
            {
                fn_801FEEAC()->Call("TransitionOnlineTournamentToGame");
            }
        }
        else if (gameInfo->mCurrentMode == 0)
        {
            fn_801FEEAC()->Call("StartGrudgeMatchSequence");
        }
        else if (gameInfo->mCurrentMode == 3)
        {
            fn_801FEEAC()->Call("StartCupMatchSequence");
        }
    }
}

void LoadingScene_801CDB4C::SceneCreated()
{
    TLInstance* transitionComponent
        = FEFinder<TLComponentInstance, 2>::_Find<TLSlide>(
            mPresentation->GetActiveSlide(), nlStringLowerHash("Layer"), nlStringLowerHash("no home"), 0, 0, 0, 0);
    if (transitionComponent == 0)
    {
        transitionComponent = &lbl_80580030;
    }
    mTransitionComponent = (TLComponentInstance*)transitionComponent;
    mTransitionComponent->m_bVisible = false;

    if (fn_80273B00())
    {
        mWidescreen = true;
        mTransitionComponent->SetActiveSlide("widescreen", true, false);
    }

    gpHBMManager->mLoading = false;
}

void LoadingScene_801CDB4C::fn_801CE274()
{
    if (mFEScene == 0 || mFEScene->mState != 6 || mTransitionActive)
    {
        return;
    }

    mTransitionComponent->m_bVisible = true;
    if (mWidescreen)
    {
        mTransitionComponent->SetActiveSlide("widescreen", true, false);
    }
    else
    {
        mTransitionComponent->SetActiveSlide("Slide1", true, false);
    }
    mTransitionActive = true;
}

void LoadingScene_801CDC2C::SceneCreated()
{
    LoadingScene_801CDB4C::SceneCreated();

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    mTextInstances[0] = FindLoadingText(presentation, "TOP TEXT");
    mTextInstances[1] = FindLoadingText(presentation, "BOTTOM TEXT");
    mTextInstances[2] = FindLoadingText(presentation, "HOME_STATS1");
    mTextInstances[3] = FindLoadingText(presentation, "HOME_STATS2");
    mTextInstances[4] = FindLoadingText(presentation, "AWAY_STATS1");
    mTextInstances[5] = FindLoadingText(presentation, "AWAY_STATS2");
}

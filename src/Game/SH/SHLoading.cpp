#include "Game/HBMManager.h"
#include "Game/SH/SHLoading.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/FE/FEAudio.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/BaseSceneHandler.h"
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


SuperLoadingScene::SuperLoadingScene()
    : mType(TT_INVALID)
{
    gpHBMManager->mBlocked = true;
}

SuperLoadingScene::~SuperLoadingScene()
{
    gpHBMManager->mBlocked = false;
}

void SuperLoadingScene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (mType == TT_3D_TRANSITION)
    {
        if (!Presentation::GetInstance()->IsActive())
        {
            nlTaskManager::SetNextState(0x200000);
        }
    }
    else
    {
        TLSlide* slide = mFEScene->m_pFEPackage->GetPresentation()->m_currentSlide;
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
                g_pOverlayManager->Pop();
            }
        }
    }
}

AsyncLoadingScene::AsyncLoadingScene()
    : mHomeButtonWarningActive(false)
    , mWidescreen(false)
{
}

AsyncLoadingScene::~AsyncLoadingScene()
{
}

void AsyncLoadingScene::Update(float dt)
{
    BaseSceneHandler::Update(dt);

    if (mHomeButtonWarningActive)
    {
        TLSlide* slide = mHomeButtonWarning->GetActiveSlide();
        float duration = slide->m_duration;
        float start = slide->m_start;
        float time = slide->m_time;
        if (time >= start + duration)
        {
            mHomeButtonWarning->m_bVisible = false;
            mHomeButtonWarningActive = false;
        }
    }
}

WidescreenLoadingScene::WidescreenLoadingScene()
{
}

WidescreenLoadingScene::~WidescreenLoadingScene()
{
}

void WidescreenLoadingScene::Update(float dt)
{
    BaseSceneHandler::Update(dt);

    if (mHomeButtonWarningActive)
    {
        TLSlide* slide = mHomeButtonWarning->GetActiveSlide();
        float duration = slide->m_duration;
        float start = slide->m_start;
        float time = slide->m_time;
        if (time >= start + duration)
        {
            mHomeButtonWarning->m_bVisible = false;
            mHomeButtonWarningActive = false;
        }
    }
}

void WidescreenLoadingScene::SetStadiumText(TLTextInstance* stadiumText)
{
    const char* stringID = GetStadiumTickerStringID(GameInfoManager::Instance()->GetStadium());
    stadiumText->SetStringId(stringID);
}

static inline TLTextInstance* FindLoadingText(FEPresentation* presentation, const char* name)
{
    TLTextInstance* text = (TLTextInstance*)FEFindInstance(
        presentation, nlStringLowerHash("Layer"), nlStringLowerHash(name), 0, 0, 0, 0);
    return text != 0 ? text : &gDefaultTLTextInstance;
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
        if (gameInfo->mIsOnlineMode)
        {
            if (gameInfo->mCurrentMode == 0)
            {
                FEAudio::PlayAnimAudioEvent(0x7FEC4468, 0, 0, 1);
                Presentation::GetInstance()->Call("StartOnlineGrudgeMatchSequence");
            }
            else if (NetTournManager::Instance()->mState != 0)
            {
                Presentation::GetInstance()->Call("TransitionOnlineTournamentToGame");
            }
        }
        else if (gameInfo->mCurrentMode == 0)
        {
            Presentation::GetInstance()->Call("StartGrudgeMatchSequence");
        }
        else if (gameInfo->mCurrentMode == 3)
        {
            Presentation::GetInstance()->Call("StartCupMatchSequence");
        }
    }
}

void AsyncLoadingScene::SceneCreated()
{
    TLInstance* transitionComponent
        = FEFinder<TLComponentInstance, 2>::_Find<TLSlide>(
            mPresentation->m_currentSlide, nlStringLowerHash("Layer"), nlStringLowerHash("no home"), 0, 0, 0, 0);
    if (transitionComponent == 0)
    {
        transitionComponent = &gDefaultTLComponentInstance;
    }
    mHomeButtonWarning = (TLComponentInstance*)transitionComponent;
    mHomeButtonWarning->m_bVisible = false;

    if (IsWidescreen())
    {
        mWidescreen = true;
        mHomeButtonWarning->SetActiveSlide("widescreen", true, false);
    }

    gpHBMManager->mBlocked = false;
}

void AsyncLoadingScene::ShowHomeButtonWarning()
{
    if (mFEScene == 0 || mFEScene->mState != 6 || mHomeButtonWarningActive)
    {
        return;
    }

    mHomeButtonWarning->m_bVisible = true;
    if (mWidescreen)
    {
        mHomeButtonWarning->SetActiveSlide("widescreen", true, false);
    }
    else
    {
        mHomeButtonWarning->SetActiveSlide("Slide1", true, false);
    }
    mHomeButtonWarningActive = true;
}

void WidescreenLoadingScene::SceneCreated()
{
    AsyncLoadingScene::SceneCreated();

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    mTextInstances[0] = FindLoadingText(presentation, "TOP TEXT");
    mTextInstances[1] = FindLoadingText(presentation, "BOTTOM TEXT");
    mTextInstances[2] = FindLoadingText(presentation, "HOME_STATS1");
    mTextInstances[3] = FindLoadingText(presentation, "HOME_STATS2");
    mTextInstances[4] = FindLoadingText(presentation, "AWAY_STATS1");
    mTextInstances[5] = FindLoadingText(presentation, "AWAY_STATS2");
}

FEPresentation* BaseSceneHandler::GetPresentation()
{
    return mPresentation;
}

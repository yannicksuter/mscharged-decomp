#include "Game/SH/SHCupNews.h"
#include "Game/FE/FEAudio.h"

#include "Game/GameSceneManager.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/Render/Presentation.h"
#include "Game/FE/feDPD.h"


extern "C" void fn_802083A0();
extern "C" void fn_802084A8();
extern "C" void fn_80208568();
extern "C" void fn_80208718();

int GetCupState(CupManager* cupManager)
{
    return cupManager->mState;
}

CupNewsScene::CupNewsScene()
    : mShowAwardsOnClose(false)
    , mShowWinnerRewardsOnClose(false)
{
}

CupNewsScene::~CupNewsScene()
{
}

void CupNewsScene::SetDisplayMode(unsigned int transition)
{
    SHStrikerTimesBase::SetDisplayMode(transition);
}

void CupNewsScene::Update(float dt)
{
    SHStrikerTimesBase::Update(dt);
}

void CupNewsScene::OnDoneTransitionComplete()
{
    SHStrikerTimesBase::OnDoneTransitionComplete();

    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
    }

    if (mDisplayMode == 1)
    {
        GameSceneManager::Instance()->Pop();
        FEAudio::PlayAnimAudioEvent(0xD276AFE5, 0, 0, 1);
        Presentation::GetInstance()->Call("TransitionToStrikerCupHub");
    }
    else if (mDisplayMode == 0)
    {
        GameSceneManager::Instance()->Pop();
        FEAudio::PlayAnimAudioEvent(0xD276AFE5, 0, 0, 1);
        Presentation::GetInstance()->Call("TransitionToInitialStrikerCupHub");
    }
    else if (mDisplayMode == 7)
    {
        GameSceneManager::Instance()->Pop();
        fn_802084A8();
    }
    else if (mDisplayMode == 6)
    {
        GameSceneManager::Instance()->Pop();
        fn_80208568();
    }
    else if (mShowAwardsOnClose)
    {
        GameSceneManager::Instance()->Pop();
        fn_802083A0();
        mShowAwardsOnClose = false;
    }
    else if (mShowWinnerRewardsOnClose)
    {
        GameSceneManager::Instance()->Pop();
        fn_80208718();
        mShowWinnerRewardsOnClose = false;
    }
    else
    {
        GameSceneManager::Instance()->Push((SceneList)31, SCREEN_NOTHING, true);
    }
}

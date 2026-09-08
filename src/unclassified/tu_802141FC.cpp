#include "unclassified/tu_802141FC.h"
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

int fn_80214DEC(CupManager* cupManager)
{
    return cupManager->mUnidentified8680;
}

TU802141FCScene::TU802141FCScene()
    : mUnidentified5D4(false)
    , mUnidentified5D5(false)
{
}

TU802141FCScene::~TU802141FCScene()
{
}

void TU802141FCScene::SHSceneVirtual2C(unsigned int transition)
{
    UnidentifiedSHSceneBase::SHSceneVirtual2C(transition);
}

void TU802141FCScene::Update(float dt)
{
    UnidentifiedSHSceneBase::Update(dt);
}

void TU802141FCScene::SHSceneVirtual30()
{
    UnidentifiedSHSceneBase::SHSceneVirtual30();

    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
    }

    if (mUnidentified28 == 1)
    {
        GameSceneManager::Instance()->Pop();
        FEAudio::PlayAnimAudioEvent(0xD276AFE5, 0, 0, 1);
        Presentation::GetInstance()->Call("TransitionToStrikerCupHub");
    }
    else if (mUnidentified28 == 0)
    {
        GameSceneManager::Instance()->Pop();
        FEAudio::PlayAnimAudioEvent(0xD276AFE5, 0, 0, 1);
        Presentation::GetInstance()->Call("TransitionToInitialStrikerCupHub");
    }
    else if (mUnidentified28 == 7)
    {
        GameSceneManager::Instance()->Pop();
        fn_802084A8();
    }
    else if (mUnidentified28 == 6)
    {
        GameSceneManager::Instance()->Pop();
        fn_80208568();
    }
    else if (mUnidentified5D4)
    {
        GameSceneManager::Instance()->Pop();
        fn_802083A0();
        mUnidentified5D4 = false;
    }
    else if (mUnidentified5D5)
    {
        GameSceneManager::Instance()->Pop();
        fn_80208718();
        mUnidentified5D5 = false;
    }
    else
    {
        GameSceneManager::Instance()->Push((SceneList)31, SCREEN_NOTHING, true);
    }
}

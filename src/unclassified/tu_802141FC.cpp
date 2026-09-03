#include "unclassified/tu_802141FC.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/DB/tu_8010A40C.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/Render/Presentation.h"

extern BaseGameSceneManager* lbl_806E1838;
extern TLComponentInstance* lbl_80578450[4];

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" Presentation* fn_801FEEAC();
extern "C" void fn_802083A0();
extern "C" void fn_802084A8();
extern "C" void fn_80208568();
extern "C" void fn_80208718();

int fn_80214DEC(UnidentifiedCupManager* cupManager)
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
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }

    if (mUnidentified28 == 1)
    {
        lbl_806E1838->Pop();
        fn_801CBCA0(0xD276AFE5, 0, 0, 1);
        fn_801FEEAC()->Call("TransitionToStrikerCupHub");
    }
    else if (mUnidentified28 == 0)
    {
        lbl_806E1838->Pop();
        fn_801CBCA0(0xD276AFE5, 0, 0, 1);
        fn_801FEEAC()->Call("TransitionToInitialStrikerCupHub");
    }
    else if (mUnidentified28 == 7)
    {
        lbl_806E1838->Pop();
        fn_802084A8();
    }
    else if (mUnidentified28 == 6)
    {
        lbl_806E1838->Pop();
        fn_80208568();
    }
    else if (mUnidentified5D4)
    {
        lbl_806E1838->Pop();
        fn_802083A0();
        mUnidentified5D4 = false;
    }
    else if (mUnidentified5D5)
    {
        lbl_806E1838->Pop();
        fn_80208718();
        mUnidentified5D5 = false;
    }
    else
    {
        lbl_806E1838->Push((SceneList)31, SCREEN_NOTHING, true);
    }
}

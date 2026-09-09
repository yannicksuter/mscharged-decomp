#include "Game/SH/SHNavigation.h"
#include "Game/FE/feHelpFuncs.h"
#include "Game/FE/feFinder.h"
#include "unclassified/tu_80210600.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feFinder.h"

#include "Game/GameSceneManager.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "NL/nlBind.h"
#include "NL/nlString.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/FEAudio.h"
#include "Game/SH/SHNavigation.h"

class SHNavigation;


TU80210600Scene::TU80210600Scene()
    : mUnidentified3C(0)
    , mUnidentified40(0)
    , mUnidentified204(false)
    , mUnidentified206(false)
    , mUnidentified207(false)
    , mUnidentified218()
    , mUnidentified3CC()
    , mUnidentified4A8()
    , mUnidentified55C()
    , mUnidentified610()
    , mUnidentified9A8(0)
    , mUnidentified9B0(0)
{
    mUnidentified55C.mContext = 0;
    mUnidentified4A8.mContext = (void*)1;

    mUnidentified208[0] = 0;
    mUnidentified208[1] = 0;
    mUnidentified208[2] = 0;
    mUnidentified208[3] = 0;

    for (int i = 0; i < 5; ++i)
    {
        mUnidentified610[i].mContext = (void*)i;
        mUnidentified610[i].mSpeakerEnabled = false;
    }
}

TU80210600Scene::~TU80210600Scene()
{
}

void TU80210600Scene::fn_802107AC(int index, void* context)
{
    ++mUnidentified208[index];
    if (context == 0)
    {
        if (!mUnidentified55C.HasOtherPointerState(1, index))
        {
            mUnidentified9A8->SetActiveSlide("over", true, false);
            FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, 1);
            mUnidentified55C.SetPointerState(1, index);
        }
        return;
    }
    if (context == (void*)1)
    {
        if (!mUnidentified4A8.HasOtherPointerState(1, index))
        {
            mUnidentified9AC->SetActiveSlide("over", true, false);
            FEAudio::PlayAnimAudioEvent(0xACCDCA48, 0, 0, 1);
            mUnidentified4A8.SetPointerState(1, index);
        }
    }
}

void TU80210600Scene::fn_802108B0(int index, void* context)
{
    --mUnidentified208[index];
    if (context == 0)
    {
        if (!mUnidentified55C.HasOtherPointerState(1, index))
        {
            mUnidentified9A8->SetActiveSlide("off", true, false);
            mUnidentified55C.SetPointerState(0, index);
        }
        return;
    }
    if (context == (void*)1)
    {
        if (!mUnidentified4A8.HasOtherPointerState(1, index))
        {
            mUnidentified9AC->SetActiveSlide("off", true, false);
            mUnidentified4A8.SetPointerState(0, index);
        }
    }
}

void TU80210600Scene::fn_80210984(int, void* context)
{
    mUnidentified205 = true;
    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
    }

    switch ((unsigned int)context)
    {
    case 0:
    {
        FEAudio::PlayAnimAudioEvent(0x6E5C794C, 0, 0, 1);
        FEAudio::PlayAnimAudioEvent(0x2ECB0035, 0, 0, 1);
        mUnidentified9B0 = 2;

        SHNavigation* object = GetNavigationScene();
        if (object != 0)
        {
            object->HideButtons();
        }

        mPresentation->SetActiveSlide("out", true);
        mPresentation->Update(0.0f);
        break;
    }
    case 1:
    {
        FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
            (SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x26, FEPopupMenu::Nothing);
        break;
    }
    }
}

void TU80210600Scene::fn_80210B00(int index, void* context)
{
    mUnidentified610[(unsigned int)context].SetPointerState(1, index);
}

void TU80210600Scene::fn_80210B1C(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    mUnidentified1C[item]->SetActiveSlide("off", true, false);
    mUnidentified610[item].SetPointerState(0, index);
}

void TU80210600Scene::fn_80211FA4()
{
    FEPointerListener::Callback componentOver(
        Bind<void>(MemFun(&TU80210600Scene::fn_802107AC), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback componentOff(
        Bind<void>(MemFun(&TU80210600Scene::fn_802108B0), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback componentSelect(
        Bind<void>(MemFun(&TU80210600Scene::fn_80210984), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback teamOver(
        Bind<void>(MemFun(&TU80210600Scene::fn_80210B00), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback teamOff(
        Bind<void>(MemFun(&TU80210600Scene::fn_80210B1C), this, Placeholder<0>(), Placeholder<1>()));

    SetPlayButtonBounds(&mUnidentified55C, mUnidentified9A8);
    mUnidentified55C.SetPointerEnterCallback(componentOver);
    mUnidentified55C.SetPointerLeaveCallback(componentOff);
    mUnidentified55C.SetPointerPressCallback(componentSelect);

    TLInstance* upperOver = (TLInstance*)FEFindInstance(mUnidentified9AC,
        nlStringLowerHash("OVER"),
        nlStringLowerHash("list_high_250x60"),
        0,
        0,
        0,
        0);
    if (upperOver == 0)
    {
        upperOver = &gDefaultTLImageInstance;
    }
    feVector3 upperPosition = mUnidentified9AC->GetAssetPosition();
    mUnidentified4A8.SetInstanceBounds(
        upperOver, true, upperPosition.f.x, upperPosition.f.y, 1.0f, 1.0f);
    mUnidentified4A8.SetPointerEnterCallback(componentOver);
    mUnidentified4A8.SetPointerLeaveCallback(componentOff);

    for (int i = 0; i < 5; ++i)
    {
        feVector3 position = mUnidentified1C[i]->GetAssetPosition();
        TLInstance* over = (TLInstance*)FEFindInstance(mUnidentified1C[i],
            nlStringLowerHash("over"),
            nlStringLowerHash("CHALLENGE_0"),
            nlStringLowerHash("list_back_480x70 "),
            0,
            0,
            0);
        if (over == 0)
        {
            over = &gDefaultTLImageInstance;
        }
        mUnidentified610[i].SetInstanceBounds(
            over, true, position.f.x, position.f.y, 0.95f, 0.6f);
        mUnidentified610[i].SetPointerEnterCallback(teamOver);
        mUnidentified610[i].SetPointerLeaveCallback(teamOff);
    }
}

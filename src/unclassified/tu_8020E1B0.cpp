#include "unclassified/tu_8020E1B0.h"
#include "Game/FE/feHelpFuncs.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/FEAudio.h"

#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "NL/nlBind.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/FEAudio.h"
#include "Game/SH/SHNavigation.h"

class SHNavigation;


TU8020E1B0Scene::TU8020E1B0Scene()
    : mUnidentifiedCC(false)
    , mUnidentifiedCE(false)
    , mUnidentifiedCF(false)
    , mNavigationComponent()
    , mMatchupComponents()
    , mBracketComponent()
    , mBracketInstance(0)
    , mState(0)
{
    mBracketComponent.mContext = 0;
    mSelectionCounts[0] = 0;
    mSelectionCounts[1] = 0;
    mSelectionCounts[2] = 0;
    mSelectionCounts[3] = 0;
    mMatchupComponents[0].mContext = 0;
    mMatchupComponents[1].mContext = (void*)1;
    mMatchupComponents[2].mContext = (void*)2;
}

TU8020E1B0Scene::~TU8020E1B0Scene()
{
}

void TU8020E1B0Scene::fn_8020E300()
{
    FEPointerListener::Callback bracketOver(
        Bind<void>(MemFun(&TU8020E1B0Scene::fn_802102C0), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback bracketOff(
        Bind<void>(MemFun(&TU8020E1B0Scene::fn_80210364), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback bracketSelect(
        Bind<void>(MemFun(&TU8020E1B0Scene::fn_8020E8FC), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback matchupOver(
        Bind<void>(MemFun(&TU8020E1B0Scene::fn_802103F0), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback matchupOff(
        Bind<void>(MemFun(&TU8020E1B0Scene::fn_80210490), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback matchupSelect(
        Bind<void>(MemFun(&TU8020E1B0Scene::fn_8020E9C8), this, Placeholder<0>(), Placeholder<1>()));

    SetPlayButtonBounds(&mBracketComponent, mBracketInstance);
    mBracketComponent.SetPointerEnterCallback(bracketOver);
    mBracketComponent.SetPointerLeaveCallback(bracketOff);
    mBracketComponent.SetPointerPressCallback(bracketSelect);

    for (int i = 0; i < 3; ++i)
    {
        mMatchupComponents[i].SetInstanceBounds(
            mMatchupInstances[i], true, 0.0f, 0.0f, 1.0f, 1.0f);
        mMatchupComponents[i].SetPointerEnterCallback(matchupOver);
        mMatchupComponents[i].SetPointerLeaveCallback(matchupOff);
        mMatchupComponents[i].SetPointerPressCallback(matchupSelect);
    }
}

void TU8020E1B0Scene::fn_8020E8FC(int, void* context)
{
    mUnidentifiedCD = true;
    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
    }

    if (context == 0)
    {
        FEAudio::PlayAnimAudioEvent(0x6E5C794C, 0, 0, 1);
        FEAudio::PlayAnimAudioEvent(0x2ECB0035, 0, 0, 1);
        mState = 2;

        SHNavigation* object = GetNavigationScene();
        if (object != 0)
        {
            object->HideButtons();
        }

        mPresentation->SetActiveSlide("out", true);
    }
}

void TU8020E1B0Scene::fn_802102C0(int index, void* context)
{
    ++mSelectionCounts[index];
    if (context == 0 && !mBracketComponent.HasOtherPointerState(1, index))
    {
        mBracketInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, 1);
        mBracketComponent.SetPointerState(1, index);
    }
}

void TU8020E1B0Scene::fn_80210364(int index, void* context)
{
    --mSelectionCounts[index];
    if (context == 0 && !mBracketComponent.HasOtherPointerState(1, index))
    {
        mBracketInstance->SetActiveSlide("off", true, false);
        mBracketComponent.SetPointerState(0, index);
    }
}

void TU8020E1B0Scene::fn_802103F0(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    ++mSelectionCounts[index];
    mMatchupInstances[item]->SetActiveSlide("over", true, false);
    FEAudio::PlayAnimAudioEvent(0x50204AFA, 0, 0, 1);
    mMatchupComponents[item].SetPointerState(1, index);
}

void TU8020E1B0Scene::fn_80210490(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    --mSelectionCounts[index];
    mMatchupInstances[item]->SetActiveSlide("off", true, false);
    mMatchupComponents[item].SetPointerState(0, index);
}

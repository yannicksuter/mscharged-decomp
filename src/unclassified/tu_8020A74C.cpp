#include "unclassified/tu_8020A74C.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/FEAudio.h"

#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/NetworkSession.h"
#include "NL/nlBind.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/FEAudio.h"
#include "Game/SH/SHNavigation.h"

class SHNavigation;


TU8020A74CScene::TU8020A74CScene()
    : mUnidentified2DC(true)
    , mTournamentData(0)
    , mUnidentified2E8(0.0f)
    , mUnidentified2EC(-1)
    , mUnidentified2F0(false)
    , mUnidentified2F1(false)
    , mUnidentified2F2(false)
    , mUnidentified2F3(false)
    , mUnidentified2F4(false)
    , mUnidentified2F5(false)
    , mUnidentified2F6(false)
    , mUnidentified2F8(false)
    , mUnidentified2FA(false)
    , mUnidentified2FB(false)
    , mNavigationComponent()
    , mMatchupComponents()
    , mBracketComponent()
    , mBracketInstance(0)
    , mState(0)
{
    if (g_pNetworkSessionBase->GetSessionMode())
    {
        mNetworkTournament = true;
    }
    else
    {
        mNetworkTournament = false;
    }
    mBracketComponent.mContext = 0;
    mSelectionCounts[0] = 0;
    mSelectionCounts[1] = 0;
    mSelectionCounts[2] = 0;
    mSelectionCounts[3] = 0;
}

TU8020A74CScene::~TU8020A74CScene()
{
}

void TU8020A74CScene::fn_8020D5C8()
{
    FEPointerListener::Callback matchupOver(
        Bind<void>(MemFun(&TU8020A74CScene::fn_8020DDC0), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback matchupOff(
        Bind<void>(MemFun(&TU8020A74CScene::fn_8020DE60), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback matchupSelect(
        Bind<void>(MemFun(&TU8020A74CScene::fn_8020DEE8), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback bracketOver(
        Bind<void>(MemFun(&TU8020A74CScene::fn_8020DBC4), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback bracketOff(
        Bind<void>(MemFun(&TU8020A74CScene::fn_8020DC68), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback bracketSelect(
        Bind<void>(MemFun(&TU8020A74CScene::fn_8020DCF4), this, Placeholder<0>(), Placeholder<1>()));

    SetPlayButtonBounds(&mBracketComponent, mBracketInstance);
    mBracketComponent.SetPointerEnterCallback(bracketOver);
    mBracketComponent.SetPointerLeaveCallback(bracketOff);
    mBracketComponent.SetPointerPressCallback(bracketSelect);

    for (int i = 0; i < 7; ++i)
    {
        mMatchupComponents[i].SetInstanceBounds(
            mMatchupInstances[i], false, 0.0f, 0.0f, 0.8f, 0.77f);
        mMatchupComponents[i].SetPointerEnterCallback(matchupOver);
        mMatchupComponents[i].SetPointerLeaveCallback(matchupOff);
        mMatchupComponents[i].SetPointerPressCallback(matchupSelect);
    }
}

void TU8020A74CScene::fn_8020DBC4(int index, void* context)
{
    ++mSelectionCounts[index];
    if (context == 0 && !mBracketComponent.HasOtherPointerState(1, index))
    {
        mBracketInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, 1);
        mBracketComponent.SetPointerState(1, index);
    }
}

void TU8020A74CScene::fn_8020DC68(int index, void* context)
{
    --mSelectionCounts[index];
    if (context == 0 && !mBracketComponent.HasOtherPointerState(1, index))
    {
        mBracketInstance->SetActiveSlide("off", true, false);
        mBracketComponent.SetPointerState(0, index);
    }
}

void TU8020A74CScene::fn_8020DCF4(int, void* context)
{
    mUnidentified2F9 = true;
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    switch ((int)context)
    {
    case 0:
        FEAudio::PlayAnimAudioEvent(0x6E5C794C, 0, 0, 1);
        FEAudio::PlayAnimAudioEvent(0x2ECB0035, 0, 0, 1);
        mState = 2;

        SHNavigation* object = GetNavigationScene();
        if (object != 0)
        {
            object->HideButtons();
        }

        mPresentation->SetActiveSlide("out", true);
        break;
    }
}

void TU8020A74CScene::fn_8020DDC0(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    ++mSelectionCounts[index];
    mMatchupInstances[item]->SetActiveSlide("over", true, false);
    FEAudio::PlayAnimAudioEvent(0x50204AFA, 0, 0, 1);
    mMatchupComponents[item].SetPointerState(1, index);
}

void TU8020A74CScene::fn_8020DE60(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    --mSelectionCounts[index];
    mMatchupInstances[item]->SetActiveSlide("off", true, false);
    mMatchupComponents[item].SetPointerState(0, index);
}

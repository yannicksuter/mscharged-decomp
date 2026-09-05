#include "unclassified/tu_8020A74C.h"

#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/NetworkSession.h"
#include "NL/nlBind.h"

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" void fn_801CC988(TU80219248Component* component, TLComponentInstance* instance);
class TU80252180Scene;
extern "C" TU80252180Scene* fn_80253E18();
extern "C" void fn_80253474(TU80252180Scene* scene);

extern TLComponentInstance* lbl_80578450[4];

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
    UnidentifiedNetworkOnlineInterface& online = *lbl_806E20D8;
    if (online.OnlineVirtual0C())
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
    TU80300104Base::Callback matchupOver(
        Bind<void>(MemFun(&TU8020A74CScene::fn_8020DDC0), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback matchupOff(
        Bind<void>(MemFun(&TU8020A74CScene::fn_8020DE60), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback matchupSelect(
        Bind<void>(MemFun(&TU8020A74CScene::fn_8020DEE8), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback bracketOver(
        Bind<void>(MemFun(&TU8020A74CScene::fn_8020DBC4), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback bracketOff(
        Bind<void>(MemFun(&TU8020A74CScene::fn_8020DC68), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback bracketSelect(
        Bind<void>(MemFun(&TU8020A74CScene::fn_8020DCF4), this, Placeholder<0>(), Placeholder<1>()));

    fn_801CC988(&mBracketComponent, mBracketInstance);
    mBracketComponent.fn_803007C0(bracketOver);
    mBracketComponent.fn_80300864(bracketOff);
    mBracketComponent.fn_803009AC(bracketSelect);

    for (int i = 0; i < 7; ++i)
    {
        mMatchupComponents[i].fn_80300D74(
            mMatchupInstances[i], false, 0.0f, 0.0f, 0.8f, 0.77f);
        mMatchupComponents[i].fn_803007C0(matchupOver);
        mMatchupComponents[i].fn_80300864(matchupOff);
        mMatchupComponents[i].fn_803009AC(matchupSelect);
    }
}

void TU8020A74CScene::fn_8020DBC4(int index, void* context)
{
    ++mSelectionCounts[index];
    if (context == 0 && !mBracketComponent.fn_802192FC(1, index))
    {
        mBracketInstance->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xAA73EF34, 0, 0, 1);
        mBracketComponent.mValues[index] = 1;
    }
}

void TU8020A74CScene::fn_8020DC68(int index, void* context)
{
    --mSelectionCounts[index];
    if (context == 0 && !mBracketComponent.fn_802192FC(1, index))
    {
        mBracketInstance->SetActiveSlide("off", true, false);
        mBracketComponent.mValues[index] = 0;
    }
}

void TU8020A74CScene::fn_8020DCF4(int, void* context)
{
    mUnidentified2F9 = true;
    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }

    if (context == 0)
    {
        fn_801CBCA0(0x6E5C794C, 0, 0, 1);
        fn_801CBCA0(0x2ECB0035, 0, 0, 1);
        mState = 2;

        TU80252180Scene* object = fn_80253E18();
        if (object != 0)
        {
            fn_80253474(object);
        }

        mPresentation->SetActiveSlide("out", true);
    }
}

void TU8020A74CScene::fn_8020DDC0(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    ++mSelectionCounts[index];
    mMatchupInstances[item]->SetActiveSlide("over", true, false);
    fn_801CBCA0(0x50204AFA, 0, 0, 1);
    mMatchupComponents[item].mValues[index] = 1;
}

void TU8020A74CScene::fn_8020DE60(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    --mSelectionCounts[index];
    mMatchupInstances[item]->SetActiveSlide("off", true, false);
    mMatchupComponents[item].mValues[index] = 0;
}

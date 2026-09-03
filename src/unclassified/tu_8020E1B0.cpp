#include "unclassified/tu_8020E1B0.h"

#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "NL/nlBind.h"

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" void fn_801CC988(TU80219248Component* component, TLComponentInstance* instance);
extern "C" void* fn_80253E18();
extern "C" void fn_80253474(void* object);

extern TLComponentInstance* lbl_80578450[4];

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
    TU80300104Base::Callback bracketOver(
        Bind<void>(MemFun(&TU8020E1B0Scene::fn_802102C0), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback bracketOff(
        Bind<void>(MemFun(&TU8020E1B0Scene::fn_80210364), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback bracketSelect(
        Bind<void>(MemFun(&TU8020E1B0Scene::fn_8020E8FC), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback matchupOver(
        Bind<void>(MemFun(&TU8020E1B0Scene::fn_802103F0), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback matchupOff(
        Bind<void>(MemFun(&TU8020E1B0Scene::fn_80210490), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback matchupSelect(
        Bind<void>(MemFun(&TU8020E1B0Scene::fn_8020E9C8), this, Placeholder<0>(), Placeholder<1>()));

    fn_801CC988(&mBracketComponent, mBracketInstance);
    mBracketComponent.fn_803007C0(bracketOver);
    mBracketComponent.fn_80300864(bracketOff);
    mBracketComponent.fn_803009AC(bracketSelect);

    for (int i = 0; i < 3; ++i)
    {
        mMatchupComponents[i].fn_80300D74(
            mMatchupInstances[i], true, 0.0f, 0.0f, 1.0f, 1.0f);
        mMatchupComponents[i].fn_803007C0(matchupOver);
        mMatchupComponents[i].fn_80300864(matchupOff);
        mMatchupComponents[i].fn_803009AC(matchupSelect);
    }
}

void TU8020E1B0Scene::fn_8020E8FC(int, void* context)
{
    mUnidentifiedCD = true;
    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }

    if (context == 0)
    {
        fn_801CBCA0(0x6E5C794C, 0, 0, 1);
        fn_801CBCA0(0x2ECB0035, 0, 0, 1);
        mState = 2;

        void* object = fn_80253E18();
        if (object != 0)
        {
            fn_80253474(object);
        }

        mPresentation->SetActiveSlide("out", true);
    }
}

void TU8020E1B0Scene::fn_802102C0(int index, void* context)
{
    ++mSelectionCounts[index];
    if (context == 0 && !mBracketComponent.fn_802192FC(1, index))
    {
        mBracketInstance->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xAA73EF34, 0, 0, 1);
        mBracketComponent.mValues[index] = 1;
    }
}

void TU8020E1B0Scene::fn_80210364(int index, void* context)
{
    --mSelectionCounts[index];
    if (context == 0 && !mBracketComponent.fn_802192FC(1, index))
    {
        mBracketInstance->SetActiveSlide("off", true, false);
        mBracketComponent.mValues[index] = 0;
    }
}

void TU8020E1B0Scene::fn_802103F0(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    ++mSelectionCounts[index];
    mMatchupInstances[item]->SetActiveSlide("over", true, false);
    fn_801CBCA0(0x50204AFA, 0, 0, 1);
    mMatchupComponents[item].mValues[index] = 1;
}

void TU8020E1B0Scene::fn_80210490(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    --mSelectionCounts[index];
    mMatchupInstances[item]->SetActiveSlide("off", true, false);
    mMatchupComponents[item].mValues[index] = 0;
}

#include "unclassified/tu_80210600.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/tlComponentInstance.h"
#include "NL/nlBind.h"
#include "NL/nlString.h"

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" void fn_801CC988(TU80219248Component* component, TLComponentInstance* instance);
extern "C" void* fn_80253E18();
extern "C" void fn_80253474(void* object);
extern "C" TLInstance* fn_803068F4(TLInstance* instance, unsigned long level1,
    unsigned long level2, unsigned long level3, unsigned long level4,
    unsigned long level5, unsigned long level6);

extern TLInstance lbl_80580248;
extern TLComponentInstance* lbl_80578450[4];
extern BaseGameSceneManager* lbl_806E1838;

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
    unsigned int which = index;
    ++mUnidentified208[index];
    if (context == 0)
    {
        if (!mUnidentified55C.fn_802192FC(1, which))
        {
            mUnidentified9A8->SetActiveSlide("over", true, false);
            fn_801CBCA0(0xAA73EF34, 0, 0, 1);
            mUnidentified55C.mValues[which] = 1;
        }
        return;
    }
    if (context == (void*)1)
    {
        if (!mUnidentified4A8.fn_802192FC(1, which))
        {
            mUnidentified9AC->SetActiveSlide("over", true, false);
            fn_801CBCA0(0xACCDCA48, 0, 0, 1);
            mUnidentified4A8.mValues[which] = 1;
        }
    }
}

void TU80210600Scene::fn_802108B0(int index, void* context)
{
    unsigned int which = index;
    --mUnidentified208[index];
    if (context == 0)
    {
        if (!mUnidentified55C.fn_802192FC(1, which))
        {
            mUnidentified9A8->SetActiveSlide("off", true, false);
            mUnidentified55C.mValues[which] = 0;
        }
        return;
    }
    if (context == (void*)1)
    {
        if (!mUnidentified4A8.fn_802192FC(1, which))
        {
            mUnidentified9AC->SetActiveSlide("off", true, false);
            mUnidentified4A8.mValues[which] = 0;
        }
    }
}

void TU80210600Scene::fn_80210984(int, void* context)
{
    mUnidentified205 = true;
    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }

    switch ((unsigned int)context)
    {
    case 0:
    {
        fn_801CBCA0(0x6E5C794C, 0, 0, 1);
        fn_801CBCA0(0x2ECB0035, 0, 0, 1);
        mUnidentified9B0 = 2;

        void* object = fn_80253E18();
        if (object != 0)
        {
            fn_80253474(object);
        }

        mPresentation->SetActiveSlide("out", true);
        mPresentation->Update(0.0f);
        break;
    }
    case 1:
    {
        fn_801CBCA0(0xF0AFD586, 0, 0, 1);
        FEPopupMenu* popup = (FEPopupMenu*)lbl_806E1838->Push(
            (SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x26, FEPopupMenu::Nothing);
        break;
    }
    }
}

void TU80210600Scene::fn_80210B00(int index, void* context)
{
    mUnidentified610[(unsigned int)context].mValues[index] = 1;
}

void TU80210600Scene::fn_80210B1C(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    mUnidentified1C[item]->SetActiveSlide("off", true, false);
    mUnidentified610[item].mValues[index] = 0;
}

void TU80210600Scene::fn_80211FA4()
{
    TU80300104Base::Callback componentOver(
        Bind<void>(MemFun(&TU80210600Scene::fn_802107AC), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback componentOff(
        Bind<void>(MemFun(&TU80210600Scene::fn_802108B0), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback componentSelect(
        Bind<void>(MemFun(&TU80210600Scene::fn_80210984), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback teamOver(
        Bind<void>(MemFun(&TU80210600Scene::fn_80210B00), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback teamOff(
        Bind<void>(MemFun(&TU80210600Scene::fn_80210B1C), this, Placeholder<0>(), Placeholder<1>()));

    fn_801CC988(&mUnidentified55C, mUnidentified9A8);
    mUnidentified55C.fn_803007C0(componentOver);
    mUnidentified55C.fn_80300864(componentOff);
    mUnidentified55C.fn_803009AC(componentSelect);

    TLInstance* upperOver = fn_803068F4(mUnidentified9AC,
        nlStringLowerHash("OVER"),
        nlStringLowerHash("list_high_250x60"),
        0,
        0,
        0,
        0);
    if (upperOver == 0)
    {
        upperOver = &lbl_80580248;
    }
    feVector3 upperPosition = mUnidentified9AC->GetAssetPosition();
    mUnidentified4A8.fn_80300D74(
        upperOver, true, upperPosition.f.x, upperPosition.f.y, 1.0f, 1.0f);
    mUnidentified4A8.fn_803007C0(componentOver);
    mUnidentified4A8.fn_80300864(componentOff);

    for (int i = 0; i < 5; ++i)
    {
        feVector3 position = mUnidentified1C[i]->GetAssetPosition();
        TLInstance* over = fn_803068F4(mUnidentified1C[i],
            nlStringLowerHash("over"),
            nlStringLowerHash("CHALLENGE_0"),
            nlStringLowerHash("list_back_480x70 "),
            0,
            0,
            0);
        if (over == 0)
        {
            over = &lbl_80580248;
        }
        mUnidentified610[i].fn_80300D74(
            over, true, position.f.x, position.f.y, 0.95f, 0.6f);
        mUnidentified610[i].fn_803007C0(teamOver);
        mUnidentified610[i].fn_80300864(teamOff);
    }
}

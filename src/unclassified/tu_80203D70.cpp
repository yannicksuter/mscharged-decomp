#include "unclassified/tu_80203D70.h"

#include "Game/DB/tu_8010A40C.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" void fn_802081C0();
class TU80252180Scene;
extern "C" void fn_80253474(TU80252180Scene* scene);
extern "C" TU80252180Scene* fn_80253E18();

extern TLComponentInstance* lbl_80578450[4];

TU80203D70Scene::TU80203D70Scene()
    : mUnidentified21C(false)
    , mUnidentified21E(false)
    , mUnidentified21F(false)
    , mUnidentified230(-1)
    , mNavigationComponent()
    , mButtonComponent0()
    , mButtonComponent1()
    , mButtonComponent2()
    , mButtonComponent3()
    , mUnidentified5DC(0)
    , mUnidentified5EC(0)
{
    mButtonComponent0.mContext = (void*)0;
    mButtonComponent1.mContext = (void*)1;
    mButtonComponent2.mContext = (void*)2;
    mButtonComponent3.mContext = (void*)3;

    mButtonStates[0] = 0;
    mButtonStates[1] = 0;
    mButtonStates[2] = 0;
    mButtonStates[3] = 0;
}

TU80203D70Scene::~TU80203D70Scene()
{
}

void TU80203D70Scene::fn_802066C8(int index, void* context)
{
    ++mButtonStates[index];

    if (context == (void*)0)
    {
        if (!mButtonComponent0.fn_802192FC(1, index))
        {
            mUnidentified5DC->SetActiveSlide("over", true, false);
            fn_801CBCA0(0xAA73EF34, 0, 0, 1);
            mButtonComponent0.mValues[index] = 1;
        }
    }
    else if (context == (void*)1)
    {
        if (!mButtonComponent1.fn_802192FC(1, index))
        {
            mUnidentified5E0->SetActiveSlide("over", true, false);
            fn_801CBCA0(0x96DFB5C3, 0, 0, 1);
            mButtonComponent1.mValues[index] = 1;
        }
    }
    else if (context == (void*)2)
    {
        if (!mButtonComponent2.fn_802192FC(1, index))
        {
            mUnidentified5E4->SetActiveSlide("over", true, false);
            fn_801CBCA0(0x96DFB5C3, 0, 0, 1);
            mButtonComponent2.mValues[index] = 1;
        }
    }
    else if (context == (void*)3
        && !mButtonComponent3.fn_802192FC(1, index))
    {
        mUnidentified5E8->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xACCECA48, 0, 0, 1);
        mButtonComponent3.mValues[index] = 1;
    }
}

void TU80203D70Scene::fn_8020688C(int index, void* context)
{
    --mButtonStates[index];

    if (context == (void*)0)
    {
        if (!mButtonComponent0.fn_802192FC(1, index))
        {
            mUnidentified5DC->SetActiveSlide("off", true, false);
            mButtonComponent0.mValues[index] = 0;
        }
    }
    else if (context == (void*)1)
    {
        if (!mButtonComponent1.fn_802192FC(1, index))
        {
            mUnidentified5E0->SetActiveSlide("off", true, false);
            mButtonComponent1.mValues[index] = 0;
        }
    }
    else if (context == (void*)2)
    {
        if (!mButtonComponent2.fn_802192FC(1, index))
        {
            mUnidentified5E4->SetActiveSlide("off", true, false);
            mButtonComponent2.mValues[index] = 0;
        }
    }
    else if (context == (void*)3
        && !mButtonComponent3.fn_802192FC(1, index))
    {
        mUnidentified5E8->SetActiveSlide("off", true, false);
        mButtonComponent3.mValues[index] = 0;
    }
}

void TU80203D70Scene::fn_802069F0(int, void* context)
{
    mUnidentified21D = true;
    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }

    int value = (int)context;
    if (value != 0)
    {
        fn_801CBCA0(0xF0B0D586, 0, 0, 1);
    }

    if ((unsigned int)(value - 1) <= 1)
    {
        fn_801CBCA0(0xE4023EE3, 0, 0, 1);
        fn_801CBCA0(0xEA7BD449, 0, 0, 1);
    }

    if (value != 3)
    {
        fn_801CBCA0(0x6E5C794C, 0, 0, 1);
        mUnidentified5EC = 2;

        TU80252180Scene* object = fn_80253E18();
        if (object != 0)
        {
            fn_80253474(object);
        }

        mPresentation->SetActiveSlide("OUT", true);
        mUnidentified230 = value;
    }
    else
    {
        fn_802081C0();
    }

    if (value == 0)
    {
        fn_801CBCA0(0x4A52995D, 0, 0, 1);
    }
}

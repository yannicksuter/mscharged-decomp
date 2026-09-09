#include "unclassified/tu_80203D70.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/FEAudio.h"

#include "Game/DB/GameProgress.h"
#include "Game/FE/feCupFlow.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/FEAudio.h"
#include "Game/SH/SHNavigation.h"

extern "C" void ShowCupRulesPopup();
class SHNavigation;


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
        if (!mButtonComponent0.HasOtherPointerState(1, index))
        {
            mUnidentified5DC->SetActiveSlide("over", true, false);
            FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, 1);
            mButtonComponent0.SetPointerState(1, index);
        }
    }
    else if (context == (void*)1)
    {
        if (!mButtonComponent1.HasOtherPointerState(1, index))
        {
            mUnidentified5E0->SetActiveSlide("over", true, false);
            FEAudio::PlayAnimAudioEvent(0x96DFB5C3, 0, 0, 1);
            mButtonComponent1.SetPointerState(1, index);
        }
    }
    else if (context == (void*)2)
    {
        if (!mButtonComponent2.HasOtherPointerState(1, index))
        {
            mUnidentified5E4->SetActiveSlide("over", true, false);
            FEAudio::PlayAnimAudioEvent(0x96DFB5C3, 0, 0, 1);
            mButtonComponent2.SetPointerState(1, index);
        }
    }
    else if (context == (void*)3
        && !mButtonComponent3.HasOtherPointerState(1, index))
    {
        mUnidentified5E8->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xACCECA48, 0, 0, 1);
        mButtonComponent3.SetPointerState(1, index);
    }
}

void TU80203D70Scene::fn_8020688C(int index, void* context)
{
    --mButtonStates[index];

    if (context == (void*)0)
    {
        if (!mButtonComponent0.HasOtherPointerState(1, index))
        {
            mUnidentified5DC->SetActiveSlide("off", true, false);
            mButtonComponent0.SetPointerState(0, index);
        }
    }
    else if (context == (void*)1)
    {
        if (!mButtonComponent1.HasOtherPointerState(1, index))
        {
            mUnidentified5E0->SetActiveSlide("off", true, false);
            mButtonComponent1.SetPointerState(0, index);
        }
    }
    else if (context == (void*)2)
    {
        if (!mButtonComponent2.HasOtherPointerState(1, index))
        {
            mUnidentified5E4->SetActiveSlide("off", true, false);
            mButtonComponent2.SetPointerState(0, index);
        }
    }
    else if (context == (void*)3
        && !mButtonComponent3.HasOtherPointerState(1, index))
    {
        mUnidentified5E8->SetActiveSlide("off", true, false);
        mButtonComponent3.SetPointerState(0, index);
    }
}

void TU80203D70Scene::fn_802069F0(int, void* context)
{
    mUnidentified21D = true;
    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
    }

    int value = (int)context;
    if (value != 0)
    {
        FEAudio::PlayAnimAudioEvent(0xF0B0D586, 0, 0, 1);
    }

    if ((unsigned int)(value - 1) <= 1)
    {
        FEAudio::PlayAnimAudioEvent(0xE4023EE3, 0, 0, 1);
        FEAudio::PlayAnimAudioEvent(0xEA7BD449, 0, 0, 1);
    }

    if (value != 3)
    {
        FEAudio::PlayAnimAudioEvent(0x6E5C794C, 0, 0, 1);
        mUnidentified5EC = 2;

        SHNavigation* object = GetNavigationScene();
        if (object != 0)
        {
            object->HideButtons();
        }

        mPresentation->SetActiveSlide("OUT", true);
        mUnidentified230 = value;
    }
    else
    {
        ShowCupRulesPopup();
    }

    if (value == 0)
    {
        FEAudio::PlayAnimAudioEvent(0x4A52995D, 0, 0, 1);
    }
}

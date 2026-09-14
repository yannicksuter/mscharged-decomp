#include "unclassified/tu_802126F8.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/FEAudio.h"

#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "NL/nlBind.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/FEAudio.h"
#include "Game/SH/SHNavigation.h"

class SHNavigation;


TU802126F8Scene::TU802126F8Scene(int mode)
    : mUnidentified2C(0)
    , mMode(mode)
    , mUnidentifiedFC(false)
    , mUnidentifiedFE(false)
    , mUnidentifiedFF(false)
    , mUnidentified110(0)
    , mUnidentified114()
    , mUnidentified2C8()
    , mUnidentified3A4()
    , mUnidentified728(0)
    , mUnidentified76C(0)
    , mUnidentified770(0)
{
    mUnidentified3A4.mContext = 0;

    mUnidentified100[0] = 0;
    mUnidentified100[1] = 0;
    mUnidentified100[2] = 0;
    mUnidentified100[3] = 0;

    mUnidentified458[0].mContext = 0;
    mUnidentified458[0].mSpeakerEnabled = false;
    mUnidentified458[1].mContext = (void*)1;
    mUnidentified458[1].mSpeakerEnabled = false;
    mUnidentified458[2].mContext = (void*)2;
    mUnidentified458[2].mSpeakerEnabled = false;
    mUnidentified458[3].mContext = (void*)3;
    mUnidentified458[3].mSpeakerEnabled = false;
}

TU802126F8Scene::~TU802126F8Scene()
{
}

void TU802126F8Scene::fn_802129EC()
{
    typedef Detail::MemFunImpl<void, void (TU802126F8Scene::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, TU802126F8Scene*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback componentOver(
        PointerBinding(MemFun(&TU802126F8Scene::fn_802140C4), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback componentOff(
        PointerBinding(MemFun(&TU802126F8Scene::fn_80214168), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback componentSelect(
        PointerBinding(MemFun(&TU802126F8Scene::fn_80212F2C), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback teamOver(
        PointerBinding(MemFun(&TU802126F8Scene::fn_80212FF8), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback teamOff(
        PointerBinding(MemFun(&TU802126F8Scene::fn_80213014), this, Placeholder<0>(), Placeholder<1>()));

    SetPlayButtonBounds(&mUnidentified3A4, mUnidentified76C);
    mUnidentified3A4.SetPointerEnterCallback(componentOver);
    mUnidentified3A4.SetPointerLeaveCallback(componentOff);
    mUnidentified3A4.SetPointerPressCallback(componentSelect);

    for (int i = 0; i < 4; ++i)
    {
        mUnidentified458[i].mContext = (void*)i;
        mUnidentified458[i].SetInstanceBounds(
            mUnidentified1C[i], true, 0.0f, 0.0f, 1.0f, 1.0f);
        mUnidentified458[i].SetMaxX(200.0f);
        mUnidentified458[i].SetPointerEnterCallback(teamOver);
        mUnidentified458[i].SetPointerLeaveCallback(teamOff);
    }
}

void TU802126F8Scene::fn_80212F2C(int, void* context)
{
    mUnidentifiedFD = true;
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    switch ((int)context)
    {
    case 0:
        FEAudio::PlayAnimAudioEvent(0x6E5C794C, 0, 0, 1);
        FEAudio::PlayAnimAudioEvent(0x2ECB0035, 0, 0, 1);
        mUnidentified770 = 2;

        SHNavigation* object = GetNavigationScene();
        if (object != 0)
        {
            object->HideButtons();
        }

        mPresentation->SetActiveSlide("out", true);
        break;
    }
}

void TU802126F8Scene::fn_80212FF8(int index, void* context)
{
    mUnidentified458[(unsigned int)context].SetPointerState(1, index);
}

void TU802126F8Scene::fn_80213014(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    mUnidentified458[item].SetPointerState(0, index);
    mUnidentified1C[item]->SetActiveSlide("off", true, false);
    fn_80213D48(item, item + mUnidentified2C);
}

void TU802126F8Scene::fn_802140C4(int index, void* context)
{
    ++mUnidentified100[index];
    if (context == 0 && !mUnidentified3A4.HasOtherPointerState(1, index))
    {
        mUnidentified76C->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, 1);
        mUnidentified3A4.SetPointerState(1, index);
    }
}

void TU802126F8Scene::fn_80214168(int index, void* context)
{
    --mUnidentified100[index];
    if (context == 0 && !mUnidentified3A4.HasOtherPointerState(1, index))
    {
        mUnidentified76C->SetActiveSlide("off", true, false);
        mUnidentified3A4.SetPointerState(0, index);
    }
}

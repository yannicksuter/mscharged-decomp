#include "unclassified/tu_802126F8.h"

#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "NL/nlBind.h"

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" void fn_801CC988(TU80219248Component* component, TLComponentInstance* instance);
extern "C" void* fn_80253E18();
extern "C" void fn_80253474(void* object);

extern TLComponentInstance* lbl_80578450[4];

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
    TU80300104Base::Callback componentOver(
        Bind<void>(MemFun(&TU802126F8Scene::fn_802140C4), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback componentOff(
        Bind<void>(MemFun(&TU802126F8Scene::fn_80214168), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback componentSelect(
        Bind<void>(MemFun(&TU802126F8Scene::fn_80212F2C), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback teamOver(
        Bind<void>(MemFun(&TU802126F8Scene::fn_80212FF8), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback teamOff(
        Bind<void>(MemFun(&TU802126F8Scene::fn_80213014), this, Placeholder<0>(), Placeholder<1>()));

    fn_801CC988(&mUnidentified3A4, mUnidentified76C);
    mUnidentified3A4.fn_803007C0(componentOver);
    mUnidentified3A4.fn_80300864(componentOff);
    mUnidentified3A4.fn_803009AC(componentSelect);

    for (int i = 0; i < 4; ++i)
    {
        mUnidentified458[i].mContext = (void*)i;
        mUnidentified458[i].fn_80300D74(
            mUnidentified1C[i], true, 0.0f, 0.0f, 1.0f, 1.0f);
        mUnidentified458[i].fn_802141F4(200.0f);
        mUnidentified458[i].fn_803007C0(teamOver);
        mUnidentified458[i].fn_80300864(teamOff);
    }
}

void TU802126F8Scene::fn_80212F2C(int, void* context)
{
    mUnidentifiedFD = true;
    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }

    if (context == 0)
    {
        fn_801CBCA0(0x6E5C794C, 0, 0, 1);
        fn_801CBCA0(0x2ECB0035, 0, 0, 1);
        mUnidentified770 = 2;

        void* object = fn_80253E18();
        if (object != 0)
        {
            fn_80253474(object);
        }

        mPresentation->SetActiveSlide("out", true);
    }
}

void TU802126F8Scene::fn_80212FF8(int index, void* context)
{
    mUnidentified458[(unsigned int)context].mValues[index] = 1;
}

void TU802126F8Scene::fn_80213014(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    mUnidentified458[item].mValues[index] = 0;
    mUnidentified1C[item]->SetActiveSlide("off", true, false);
    fn_80213D48(item, item + mUnidentified2C);
}

void TU802126F8Scene::fn_802140C4(int index, void* context)
{
    unsigned int which = index;
    ++mUnidentified100[index];
    if (context == 0 && !mUnidentified3A4.fn_802192FC(1, which))
    {
        mUnidentified76C->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xAA73EF34, 0, 0, 1);
        mUnidentified3A4.mValues[which] = 1;
    }
}

void TU802126F8Scene::fn_80214168(int index, void* context)
{
    unsigned int which = index;
    --mUnidentified100[index];
    if (context == 0 && !mUnidentified3A4.fn_802192FC(1, which))
    {
        mUnidentified76C->SetActiveSlide("off", true, false);
        mUnidentified3A4.mValues[which] = 0;
    }
}

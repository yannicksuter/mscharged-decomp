#include "Game/SH/SHOptions.h"

#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/Render/Presentation.h"
#include "NL/nlBind.h"
#include "NL/nlString.h"

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" Presentation* fn_801FEEAC();
extern "C" void fn_801FC2B4(int value);
extern "C" nlVector2 fn_802197FC(int pad, u8* valid);
extern "C" void fn_80253474(void* object);
extern "C" void fn_802534BC(void* object, int value, bool enabled);
extern "C" TLComponentInstance* fn_80253D70(void* object, int index);
extern "C" void* fn_80253E18();

extern TLComponentInstance* lbl_80578450[4];
extern BaseGameSceneManager* lbl_806E1838;
extern int lbl_806E18B0;

OptionsScene::OptionsScene()
    : mUnidentified244()
    , mUnidentified31C(false)
    , mUnidentified320(0)
    , mUnidentified324(SCENE_INVALID)
{
    mUnidentified28[0].mContext = (void*)0;
    mUnidentified28[1].mContext = (void*)1;
    mUnidentified28[2].mContext = (void*)2;
    mUnidentified244.fn_801D2BE0(false);
}

OptionsScene::~OptionsScene()
{
}

void OptionsScene::fn_801D2A08(int, void* context)
{
    TU80300104Event event;
    for (int i = 0; i < 3; ++i)
    {
        mUnidentified28[i].mDisabled = true;
        mUnidentified28[i].mPreviousEvents[0] = event;
        mUnidentified28[i].mPreviousEvents[1] = event;
        mUnidentified28[i].mPreviousEvents[2] = event;
        mUnidentified28[i].mPreviousEvents[3] = event;
    }

    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }

    fn_801CBCA0(0xF0AFD586, 0, 0, 1);

    switch ((int)context)
    {
    case 1:
        fn_801CBCA0(0x304FDD1E, 0, 0, 1);
        mUnidentified324 = (SceneList)14;
        break;
    case 0:
        fn_801CBCA0(0x304FDD1E, 0, 0, 1);
        mUnidentified324 = (SceneList)15;
        break;
    case 3:
        mUnidentified324 = (SceneList)23;
        break;
    }

    mUnidentified320 = 2;

    void* object = fn_80253E18();
    if (object != 0)
    {
        fn_80253474(object);
    }

    mPresentation->SetActiveSlide("out", true);
    mPresentation->Update(0.0f);
}

void OptionsScene::SceneCreated()
{
    mUnidentified1C[0] = FEFinder<TLComponentInstance, 4>::Find(
        mPresentation,
        nlStringLowerHash("in"),
        nlStringLowerHash("Layer"),
        nlStringLowerHash("options_list"),
        nlStringLowerHash("BTN_0"),
        0,
        0);
    mUnidentified1C[1] = FEFinder<TLComponentInstance, 4>::Find(
        mPresentation,
        nlStringLowerHash("in"),
        nlStringLowerHash("Layer"),
        nlStringLowerHash("options_list"),
        nlStringLowerHash("BTN_1"),
        0,
        0);
    mUnidentified1C[2] = FEFinder<TLComponentInstance, 4>::Find(
        mPresentation,
        nlStringLowerHash("in"),
        nlStringLowerHash("Layer"),
        nlStringLowerHash("options_list"),
        nlStringLowerHash("BTN_2"),
        0,
        0);

    TLComponentInstance* screen = 0;
    void* object = fn_80253E18();
    if (object != 0)
    {
        fn_80253474(object);
        screen = fn_80253D70(object, 4);
    }
    mUnidentified244.fn_8022F194(screen);
    mUnidentified244.fn_801D2BE8(false);

    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }
    fn_801FC2B4(1);
}

void OptionsScene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (mUnidentified320 == 0 || mUnidentified320 == 2 || mUnidentified320 == 3)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->m_time < slide->m_start + slide->m_duration)
        {
            for (int i = 0; i < 4; ++i)
            {
                lbl_80578450[i]->SetActiveSlide("waiting", true, false);
            }
            return;
        }

        if (mUnidentified320 == 0)
        {
            void* object = fn_80253E18();
            if (object != 0)
            {
                fn_802534BC(object, 4, true);
            }
            mUnidentified320 = 1;
        }
        else if (mUnidentified320 == 2)
        {
            lbl_806E1838->Push(mUnidentified324, SCREEN_NOTHING, true);
            return;
        }
        else if (mUnidentified320 == 3)
        {
            fn_801CBCA0(0x4430B152, 0, 0, 1);
            fn_801FEEAC()->Call("TransitionOptionsToMainMenu");
            lbl_806E1838->Pop();
            return;
        }
    }

    if (!mUnidentified31C)
    {
        fn_801D31E0();
        mUnidentified31C = true;
    }

    for (int i = 0; i < 4; ++i)
    {
        if (i != lbl_806E18B0)
        {
            lbl_80578450[i]->SetActiveSlide("waiting", true, false);
            continue;
        }

        lbl_80578450[i]->SetActiveSlide("cursor", true, false);

        bool valid = true;
        TU80300104Event event;
        event.mIndex = i;
        event.mPosition = fn_802197FC(i, (u8*)&valid);
        event.mFlag0 = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 0x1E, true, 0);

        if (mUnidentified244.fn_8022F2E0(event, fDeltaT))
        {
            mUnidentified320 = 3;
            void* object = fn_80253E18();
            if (object != 0)
            {
                fn_80253474(object);
            }
            mPresentation->SetActiveSlide("out", true);
            mPresentation->Update(0.0f);
            return;
        }

        for (int j = 0; j < 3; ++j)
        {
            mUnidentified28[j].fn_80219608(&event);
        }
    }
}

void OptionsScene::fn_801D3098(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (!mUnidentified28[item].fn_802192FC(1, index))
    {
        mUnidentified1C[item]->SetActiveSlide("over", true, false);
        mUnidentified28[item].mValues[index] = 1;
        fn_801CBCA0(0xF6EB899E, 0, 0, 1);
    }
}

void OptionsScene::fn_801D3148(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (!mUnidentified28[item].fn_802192FC(1, index))
    {
        mUnidentified1C[item]->SetActiveSlide("off", true, false);
        mUnidentified28[item].mValues[index] = 0;
    }
}

void OptionsScene::fn_801D31E0()
{
    TU80300104Base::Callback over(
        Bind<void>(MemFun(&OptionsScene::fn_801D3098), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback off(
        Bind<void>(MemFun(&OptionsScene::fn_801D3148), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback select(
        Bind<void>(MemFun(&OptionsScene::fn_801D2A08), this, Placeholder<0>(), Placeholder<1>()));

    for (int i = 0; i < 3; ++i)
    {
        feVector3 position = mUnidentified1C[i]->GetAssetPosition();
        TLComponentInstance* instance = FEFinder<TLComponentInstance, 3>::Find(
            mUnidentified1C[i],
            nlStringLowerHash("off"),
            nlStringLowerHash("BUTTON_0"),
            nlStringLowerHash("list_back_480x70 "),
            0,
            0,
            0);
        mUnidentified28[i].fn_80300D74(
            instance, true, position.f.x, position.f.y, 1.0f, 1.0f);
        mUnidentified28[i].fn_803007C0(over);
        mUnidentified28[i].fn_80300864(off);
        mUnidentified28[i].fn_803009AC(select);
    }
}

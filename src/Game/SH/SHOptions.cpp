#include "Game/SH/SHNavigation.h"
#include "Game/GameSceneManager.h"
#include "Game/SH/SHOptions.h"
#include "Game/FE/FEAudio.h"

#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/Render/Presentation.h"
#include "NL/nlBind.h"
#include "NL/nlString.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/FEAudio.h"
#include "Game/SH/SHNavigation.h"

class SHNavigation;


void BaseSceneHandler::SceneCreated()
{
}

OptionsScene::OptionsScene()
    : mUnidentified244()
    , mUnidentified31C(false)
    , mUnidentified320(0)
    , mUnidentified324(SCENE_INVALID)
{
    mUnidentified28[0].mContext = (void*)0;
    mUnidentified28[1].mContext = (void*)1;
    mUnidentified28[2].mContext = (void*)2;
    mUnidentified244.SetPopScene(false);
}

OptionsScene::~OptionsScene()
{
}

void OptionsScene::fn_801D2A08(int, void* context)
{
    for (int i = 0; i < 3; ++i)
    {
        mUnidentified28[i].Disable();
    }

    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);

    switch ((int)context)
    {
    case 1:
        FEAudio::PlayAnimAudioEvent(0x304FDD1E, 0, 0, 1);
        mUnidentified324 = (SceneList)14;
        break;
    case 0:
        FEAudio::PlayAnimAudioEvent(0x304FDD1E, 0, 0, 1);
        mUnidentified324 = (SceneList)15;
        break;
    case 2:
        mUnidentified324 = (SceneList)23;
        break;
    }

    mUnidentified320 = 2;

    SHNavigation* object = GetNavigationScene();
    if (object != 0)
    {
        object->HideButtons();
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
    SHNavigation* object = GetNavigationScene();
    if (object != 0)
    {
        object->HideButtons();
        screen = object->GetButton(4);
    }
    mUnidentified244.SetButtonInstance(screen);
    mUnidentified244.SetPushBackScene(false);

    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
    }
    FEMusic::StartStreamIfDifferent(1);
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
                gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
            }
            return;
        }

        if (mUnidentified320 == 0)
        {
            SHNavigation* object = GetNavigationScene();
            if (object != 0)
            {
                object->SetButtons(4, true);
            }
            mUnidentified320 = 1;
        }
        else if (mUnidentified320 == 2)
        {
            GameSceneManager::Instance()->Push(mUnidentified324, SCREEN_NOTHING, true);
            return;
        }
        else if (mUnidentified320 == 3)
        {
            FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
            Presentation::GetInstance()->Call("TransitionOptionsToMainMenu");
            GameSceneManager::Instance()->Pop();
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
        if (i != gFEControllerIndex)
        {
            gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
            continue;
        }

        gFEPointerInstances[i]->SetActiveSlide("cursor", true, false);

        bool valid = true;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, (u8*)&valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 0x1E, true, 0);

        if (mUnidentified244.UpdateBackButton(event, fDeltaT))
        {
            mUnidentified320 = 3;
            SHNavigation* object = GetNavigationScene();
            if (object != 0)
            {
                object->HideButtons();
            }
            mPresentation->SetActiveSlide("out", true);
            mPresentation->Update(0.0f);
            return;
        }

        for (int j = 0; j < 3; ++j)
        {
            mUnidentified28[j].HandlePointerEvent(&event);
        }
    }
}

void OptionsScene::fn_801D3098(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (!mUnidentified28[item].HasOtherPointerState(1, index))
    {
        mUnidentified1C[item]->SetActiveSlide("over", true, false);
        mUnidentified28[item].SetPointerState(1, index);
        FEAudio::PlayAnimAudioEvent(0xF6EB899E, 0, 0, 1);
    }
}

void OptionsScene::fn_801D3148(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (!mUnidentified28[item].HasOtherPointerState(1, index))
    {
        mUnidentified1C[item]->SetActiveSlide("off", true, false);
        mUnidentified28[item].SetPointerState(0, index);
    }
}

void OptionsScene::fn_801D31E0()
{
    FEPointerListener::Callback over(
        Bind<void>(MemFun(&OptionsScene::fn_801D3098), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(
        Bind<void>(MemFun(&OptionsScene::fn_801D3148), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback select(
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
        mUnidentified28[i].SetInstanceBounds(
            instance, true, position.f.x, position.f.y, 1.0f, 1.0f);
        mUnidentified28[i].SetPointerEnterCallback(over);
        mUnidentified28[i].SetPointerLeaveCallback(off);
        mUnidentified28[i].SetPointerPressCallback(select);
    }
}

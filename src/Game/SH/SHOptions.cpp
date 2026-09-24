#include "Game/SH/SHNavigation.h"
#include "NL/nlFunction.inl"
#include "Game/GameSceneManager.h"
#include "Game/SH/SHOptions.h"
#include "Game/BaseSceneHandler.inl"
#include "Game/FE/FEAudio.h"

#include "Game/FE/feFinder.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePointer.inl"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/Render/FrontEndPresentation.h"
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

void OptionsScene::SceneCreated()
{
    FEPresentation* presentation = GetPresentation();
    mUnidentified1C[0] = FEFinder<TLComponentInstance, 4>::Find(
        presentation, "in", "Layer", "options_list", "BTN_0");
    mUnidentified1C[1] = FEFinder<TLComponentInstance, 4>::Find(
        presentation, "in", "Layer", "options_list", "BTN_1");
    mUnidentified1C[2] = FEFinder<TLComponentInstance, 4>::Find(
        presentation, "in", "Layer", "options_list", "BTN_2");

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
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
    FEMusic::StartStreamIfDifferent(1);
}

void OptionsScene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (mUnidentified320 == 0 || mUnidentified320 == 2 || mUnidentified320 == 3)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            for (int i = 0; i < 4; ++i)
            {
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
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
            FrontEndPresentation::GetInstance()->Call("TransitionOptionsToMainMenu");
            GameSceneManager::Instance()->Pop();
            return;
        }
    }

    if (!mUnidentified31C)
    {
        InitializePointerButtons();
        mUnidentified31C = true;
    }

    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* pointer = GetPointerInstance(i);
        if (i != gFEControllerIndex)
        {
            pointer->SetActiveSlide("waiting", true, false);
            continue;
        }

        pointer->SetActiveSlide("cursor", true, false);

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

void OptionsScene::OnButtonPointerPress(int, void* context)
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

void OptionsScene::OnButtonPointerEnter(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (!mUnidentified28[item].HasOtherPointerState(1, index))
    {
        mUnidentified1C[item]->SetActiveSlide("over", true, false);
        mUnidentified28[item].SetPointerState(1, index);
        FEAudio::PlayAnimAudioEvent(0xF6EB899E, 0, 0, 1);
    }
}

void OptionsScene::OnButtonPointerLeave(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (!mUnidentified28[item].HasOtherPointerState(1, index))
    {
        mUnidentified1C[item]->SetActiveSlide("off", true, false);
        mUnidentified28[item].SetPointerState(0, index);
    }
}

void OptionsScene::InitializePointerButtons()
{
    typedef Detail::MemFunImpl<void, void (OptionsScene::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, OptionsScene*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback over(
        PointerBinding(MemFun(&OptionsScene::OnButtonPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(
        PointerBinding(MemFun(&OptionsScene::OnButtonPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback select(
        PointerBinding(MemFun(&OptionsScene::OnButtonPointerPress), this, Placeholder<0>(), Placeholder<1>()));

    for (int i = 0; i < 3; ++i)
    {
        feVector3 position = mUnidentified1C[i]->GetAssetPosition();
        TLComponentInstance* instance = FEFinder<TLComponentInstance, 3>::Find(
            mUnidentified1C[i], "off", "BUTTON_0", "list_back_480x70 ");
        mUnidentified28[i].SetInstanceBounds(
            instance, true, position.f.x, position.f.y, 1.0f, 0.5f);
        mUnidentified28[i].SetPointerEnterCallback(over);
        mUnidentified28[i].SetPointerLeaveCallback(off);
        mUnidentified28[i].SetPointerPressCallback(select);
    }
}

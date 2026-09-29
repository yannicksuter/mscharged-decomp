#include "Game/SH/SHNavigation.h"
#include "NL/nlFunction.inl"
#include "Game/GameSceneManager.h"
#include "Game/SH/SHOptions.h"
#include "Game/BaseSceneHandler.inl"
#include "Game/FE/FEAudio.h"

#include "Game/FE/feFinder.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinder_impl.h"
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
    : mBackButton()
    , mInitialized(false)
    , mState(0)
    , mNextScene(SCENE_INVALID)
{
    mOptionButtons[0].mContext = (void*)0;
    mOptionButtons[1].mContext = (void*)1;
    mOptionButtons[2].mContext = (void*)2;
    mBackButton.SetPopScene(false);
}

OptionsScene::~OptionsScene()
{
}

void OptionsScene::SceneCreated()
{
    FEPresentation* presentation = GetPresentation();
    mOptionInstances[0] = FEFinder<TLComponentInstance, 4>::Find(
        presentation, "in", "Layer", "options_list", "BTN_0");
    mOptionInstances[1] = FEFinder<TLComponentInstance, 4>::Find(
        presentation, "in", "Layer", "options_list", "BTN_1");
    mOptionInstances[2] = FEFinder<TLComponentInstance, 4>::Find(
        presentation, "in", "Layer", "options_list", "BTN_2");

    TLComponentInstance* screen = 0;
    SHNavigation* object = GetNavigationScene();
    if (object != 0)
    {
        object->HideButtons();
        screen = object->GetButton(4);
    }
    mBackButton.SetButtonInstance(screen);
    mBackButton.SetPushBackScene(false);

    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
    FEMusic::StartStreamIfDifferent(1);
}

void OptionsScene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (mState == 0 || mState == 2 || mState == 3)
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

        if (mState == 0)
        {
            SHNavigation* object = GetNavigationScene();
            if (object != 0)
            {
                object->SetButtons(4, true);
            }
            mState = 1;
        }
        else if (mState == 2)
        {
            GameSceneManager::Instance()->Push(mNextScene, SCREEN_NOTHING, true);
            return;
        }
        else if (mState == 3)
        {
            FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
            FrontEndPresentation::GetInstance()->Call("TransitionOptionsToMainMenu");
            GameSceneManager::Instance()->Pop();
            return;
        }
    }

    if (!mInitialized)
    {
        InitializePointerButtons();
        mInitialized = true;
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

        if (mBackButton.UpdateBackButton(event, fDeltaT))
        {
            mState = 3;
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
            mOptionButtons[j].HandlePointerEvent(&event);
        }
    }
}

void OptionsScene::OnButtonPointerPress(int, void* context)
{
    for (int i = 0; i < 3; ++i)
    {
        mOptionButtons[i].Disable();
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
        mNextScene = (SceneList)14;
        break;
    case 0:
        FEAudio::PlayAnimAudioEvent(0x304FDD1E, 0, 0, 1);
        mNextScene = (SceneList)15;
        break;
    case 2:
        mNextScene = (SceneList)23;
        break;
    }

    mState = 2;

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
    if (!mOptionButtons[item].HasOtherPointerState(1, index))
    {
        mOptionInstances[item]->SetActiveSlide("over", true, false);
        mOptionButtons[item].SetPointerState(1, index);
        FEAudio::PlayAnimAudioEvent(0xF6EB899E, 0, 0, 1);
    }
}

void OptionsScene::OnButtonPointerLeave(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (!mOptionButtons[item].HasOtherPointerState(1, index))
    {
        mOptionInstances[item]->SetActiveSlide("off", true, false);
        mOptionButtons[item].SetPointerState(0, index);
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
        feVector3 position = mOptionInstances[i]->GetAssetPosition();
        TLComponentInstance* instance = FEFinder<TLComponentInstance, 3>::Find(
            mOptionInstances[i], "off", "BUTTON_0", "list_back_480x70 ");
        mOptionButtons[i].SetInstanceBounds(
            instance, true, position.f.x, position.f.y, 1.0f, 0.5f);
        mOptionButtons[i].SetPointerEnterCallback(over);
        mOptionButtons[i].SetPointerLeaveCallback(off);
        mOptionButtons[i].SetPointerPressCallback(select);
    }
}

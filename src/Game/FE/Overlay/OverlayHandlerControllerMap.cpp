#include "Game/FE/Overlay/OverlayHandlerControllerMap.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/feInput.h"
#include "Game/SH/SHNavigation.h"
#include "Game/main.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlLocalization.h"

ControllerMapOverlay::~ControllerMapOverlay()
{
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
    {
        scene->ResetButtons(true);
        mDoneButtonInstance->m_bVisible = false;
    }
}

ControllerMapOverlay::ControllerMapOverlay()
    : BaseOverlayHandler(1, POSITION_ALL)
    , mDoneButton()
    , mButtonInitialized(false)
    , mDonePressed(false)
    , mAsyncImage("art/fe/controllermapui.res", 0)
{
    if ((GetRegion() == 0
            && g_pLocalization->m_CurrentLanguage == nlLocalization::LangEnglish)
        || g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
    {
        mImageLoaded = true;
    }
    else
    {
        mImageLoaded = false;
    }
}

inline void ControllerMapOverlay::InitializeDoneButton()
{
    typedef Detail::MemFunImpl<void, void (ControllerMapOverlay::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, ControllerMapOverlay*, Placeholder<0>, Placeholder<1> > PointerBinding;
    FEPointerListener::Callback over(PointerBinding(MemFun(&ControllerMapOverlay::OnDonePointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(PointerBinding(MemFun(&ControllerMapOverlay::OnDonePointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback down(PointerBinding(MemFun(&ControllerMapOverlay::OnDonePointerPress), this, Placeholder<0>(), Placeholder<1>()));
    SetDoneButtonBounds(&mDoneButton, mDoneButtonInstance, 0);
    mDoneButton.SetPointerEnterCallback(over);
    mDoneButton.SetPointerLeaveCallback(off);
    mDoneButton.SetPointerPressCallback(down);
}

void ControllerMapOverlay::Update(float fDeltaT)
{
    if (!mImageLoaded)
    {
        mImageLoaded = mAsyncImage.Update(true);
        return;
    }

    mMapImage->m_bVisible = true;
    BaseSceneHandler::Update(fDeltaT);

    if (!mButtonInitialized)
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

        InitializeDoneButton();
        mDoneButtonInstance->m_bVisible = true;
        mButtonInitialized = true;
    }

    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("cursor", true, false);
        unsigned char valid = true;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 30, true, 0);
        mDoneButton.HandlePointerEvent(&event);
        if (mDonePressed)
            break;
    }
}

void ControllerMapOverlay::SceneCreated()
{
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
    {
        scene->SetButtons(0x20, true);
        mDoneButtonInstance = scene->GetButton(0x20);
        mDoneButtonInstance->m_bVisible = false;
    }

    if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
    {
        mMapImage = FEFinder<TLImageInstance, TLAT_IMAGE>::Find<>(mPresentation->m_currentSlide, "Layer", "CONTROLLER_CONFIG_jp");
    }
    else
    {
        mMapImage = FEFinder<TLImageInstance, TLAT_IMAGE>::Find<>(mPresentation->m_currentSlide, "Layer", "CONTROLLER CONFIG_TEST");
    }

    if (!mImageLoaded)
    {
        mAsyncImage.SetImageInstance(mMapImage);
        switch (g_pLocalization->m_CurrentLanguage)
        {
        case nlLocalization::LangFrench:
        case nlLocalization::LangNAFrench:
            mAsyncImage.QueueLoad("fe/controller_maps/CONTROLLER_CONFIG_french", false);
            break;
        case nlLocalization::LangGerman:
            mAsyncImage.QueueLoad("fe/controller_maps/CONTROLLER_CONFIG_german", false);
            break;
        case nlLocalization::LangSpanish:
        case nlLocalization::LangNASpanish:
            mAsyncImage.QueueLoad("fe/controller_maps/CONTROLLER_CONFIG_spanish", false);
            break;
        case nlLocalization::LangItalian:
            mAsyncImage.QueueLoad("fe/controller_maps/CONTROLLER_CONFIG_italian", false);
            break;
        default:
            mAsyncImage.QueueLoad("fe/controller_maps/CONTROLLER_CONFIG_ukenglish", false);
            break;
        }
    }
    mMapImage->m_bVisible = false;
}


void ControllerMapOverlay::OnDonePointerEnter(int index, void*)
{
    if (!mDoneButton.HasOtherPointerState(1, index))
    {
        mDoneButtonInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }

    mDoneButton.SetPointerState(1, index);
}

void ControllerMapOverlay::OnDonePointerLeave(int index, void*)
{
    if (!mDoneButton.HasOtherPointerState(1, index))
    {
        mDoneButtonInstance->SetActiveSlide("off", true, false);
    }

    mDoneButton.SetPointerState(0, index);
}

void ControllerMapOverlay::OnDonePointerPress(int index, void*)
{
    mDoneButtonInstance->SetActiveSlide("down", true, false);
    mDoneButton.SetPointerState(2, index);
    mDonePressed = true;
    FEAudio::PlayAnimAudioEvent(0x9F9BF00F, 0, 0, 1);

    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    g_pOverlayManager->Push((SceneList)80, SCREEN_BACK, true);
}

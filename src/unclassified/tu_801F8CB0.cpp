#include "unclassified/tu_801F8CB0.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/FEAudio.h"

#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/FEAudio.h"
#include "Game/SH/SHNavigation.h"

class SHNavigation;


TU801F8CB0Overlay::TU801F8CB0Overlay(ScreenMovement movement)
    : BaseOverlayHandler(1, POSITION_ALL)
    , mControllerComponent()
    , mMovement(movement)
    , mUnidentified0E4(false)
    , mUnidentified0E5(false)
    , mState(0)
{
    SHNavigation* object = GetNavigationScene();
    if (object != 0)
    {
        object->SetButtons(0, true);
    }
}

TU801F8CB0Overlay::~TU801F8CB0Overlay()
{
}

void TU801F8CB0Overlay::fn_801F9BE4(int index, void*)
{
    if (!mControllerComponent.HasOtherPointerState(1, index))
    {
        mUnidentified0DC->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }

    mControllerComponent.SetPointerState(1, index);
}

void TU801F8CB0Overlay::fn_801F9C6C(int index, void*)
{
    if (!mControllerComponent.HasOtherPointerState(1, index))
    {
        mUnidentified0DC->SetActiveSlide("off", true, false);
    }

    mControllerComponent.SetPointerState(0, index);
}

void TU801F8CB0Overlay::fn_801F9CDC(int index, void*)
{
    mUnidentified0DC->SetActiveSlide("down", true, false);
    mControllerComponent.SetPointerState(2, index);
    mUnidentified0E5 = true;

    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
    }

    mState = 2;
    mPresentation->SetActiveSlide("out", true);
    mPresentation->Update(0.0f);
    FEAudio::PlayAnimAudioEvent(0x9F9BF00F, 0, 0, 1);
}

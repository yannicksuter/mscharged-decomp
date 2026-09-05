#include "unclassified/tu_801F8CB0.h"

#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
class TU80252180Scene;
extern "C" TU80252180Scene* fn_80253E18();
extern "C" void fn_802534BC(TU80252180Scene* scene, int value, bool enabled);

extern TLComponentInstance* lbl_80578450[4];

TU801F8CB0Overlay::TU801F8CB0Overlay(ScreenMovement movement)
    : BaseOverlayHandler(1, POSITION_ALL)
    , mControllerComponent()
    , mMovement(movement)
    , mUnidentified0E4(false)
    , mUnidentified0E5(false)
    , mState(0)
{
    TU80252180Scene* object = fn_80253E18();
    if (object != 0)
    {
        fn_802534BC(object, 0, true);
    }
}

TU801F8CB0Overlay::~TU801F8CB0Overlay()
{
}

void TU801F8CB0Overlay::fn_801F9BE4(int index, void*)
{
    if (!mControllerComponent.fn_802192FC(1, index))
    {
        mUnidentified0DC->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xAA73EF33, 0, 0, 1);
    }

    mControllerComponent.mValues[index] = 1;
}

void TU801F8CB0Overlay::fn_801F9C6C(int index, void*)
{
    if (!mControllerComponent.fn_802192FC(1, index))
    {
        mUnidentified0DC->SetActiveSlide("off", true, false);
    }

    mControllerComponent.mValues[index] = 0;
}

void TU801F8CB0Overlay::fn_801F9CDC(int index, void*)
{
    mUnidentified0DC->SetActiveSlide("down", true, false);
    mControllerComponent.mValues[index] = 2;
    mUnidentified0E5 = true;

    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }

    mState = 2;
    mPresentation->SetActiveSlide("out", true);
    mPresentation->Update(0.0f);
    fn_801CBCA0(0x9F9BF00F, 0, 0, 1);
}

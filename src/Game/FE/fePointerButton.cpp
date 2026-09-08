#include "Game/FE/fePointerButton.h"

#include "Game/FE/feInput.h"
#include "Game/RumbleActions.h"
#include "Game/Audio/AudioGlobals.h"
#include "NL/nlString.h"


FEPointerButton::FEPointerButton()
    : FEPointerRegion(0)
{
    mSpeakerEnabled = true;
    mPointerStates[0] = 0;
    mPointerStates[1] = 0;
    mPointerStates[2] = 0;
    mPointerStates[3] = 0;
}

FEPointerButton::FEPointerButton(void* context)
    : FEPointerRegion(context)
{
    mSpeakerEnabled = true;
    mPointerStates[0] = 0;
    mPointerStates[1] = 0;
    mPointerStates[2] = 0;
    mPointerStates[3] = 0;
}

bool FEPointerButton::HasOtherPointerState(int value, unsigned int which) const
{
    for (unsigned int i = 0; i < 4; i++)
    {
        if (i != which && value == mPointerStates[i])
        {
            return true;
        }
    }
    return false;
}

void FEPointerButton::OnPointerUpdate(int index, void* context)
{
    if (mUpdateCallback)
    {
        mUpdateCallback(index, context);
    }
}

void FEPointerButton::OnPointerEnter(int index, void* context)
{
    if (mEnterCallback)
    {
        mEnterCallback(index, context);
    }

    if (mSpeakerEnabled)
    {
        if (!IsRumbleActionActive(g_pFEInput->GetGlobalPad((eFEINPUT_PAD)index)))
        {
            BeginRumbleAction(RUMBLE_POINTER_HOVER, g_pFEInput->GetGlobalPad((eFEINPUT_PAD)index));
        }
    }
}

void FEPointerButton::OnPointerLeave(int index, void* context)
{
    if (mLeaveCallback)
    {
        mLeaveCallback(index, context);
    }
}

void FEPointerButton::OnPointerInside(int index, void* context)
{
    if (mInsideCallback)
    {
        mInsideCallback(index, context);
    }
}

void FEPointerButton::OnPointerPress(int index, void* context)
{
    if (mPressCallback)
    {
        mPressCallback(index, context);
    }
}

void FEPointerButton::OnPointerRelease(int index, void* context)
{
    if (mReleaseCallback)
    {
        mReleaseCallback(index, context);
    }
}

void FEPointerButton::OnPointerSecondaryAction(int index, void* context)
{
    if (mSecondaryActionCallback)
    {
        mSecondaryActionCallback(index, context);
    }
}

void FEPointerButton::PlayHoverFeedback(int index)
{
    if (!IsRumbleActionActive(g_pFEInput->GetGlobalPad((eFEINPUT_PAD)index)))
    {
        BeginRumbleAction(RUMBLE_POINTER_HOVER, g_pFEInput->GetGlobalPad((eFEINPUT_PAD)index));
    }
}

void FEPointerButton::HandlePointerEvent(const FEPointerEvent* event)
{
    unsigned long hash = nlStringLowerHash("ControllerSpeaker");
    SetAudioEffectContext(&hash, event->mIndex + 1);
    ProcessPointerEvent(event);
}

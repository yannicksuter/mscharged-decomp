#include "Game/FE/fePageControls.h"
#include "NL/nlPrint.h"
#include "Game/FE/feHelpFuncs.h"
#include "Game/FE/FEAudio.h"

#include "Game/FE/feInput.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "NL/nlBind.h"


/**
 * Offset/Address/Size: 0x0 | 0x802384AC | size: 0xAC
 */
FEPageControls::FEPageControls(bool enabled)
    : mInitialized(false)
    , mPointerHeld(false)
{
    mPointerPressed[0] = false;
    mPointerPressed[1] = false;
    mPadPressed[0] = false;
    mPadPressed[1] = false;
    mPointerInside[0] = false;
    mPointerInside[1] = false;
    mPadHeld[0] = false;
    mPadHeld[1] = false;
    mPadInputEnabled = enabled;
    mHeldButton = -1;
    mRepeatTime = 0.0f;
}

/**
 * Offset/Address/Size: 0xAC | 0x80238558 | size: 0x68
 */
FEPageControls::~FEPageControls()
{
}

/**
 * Offset/Address/Size: 0x114 | 0x802385C0 | size: 0x8
 */
void FEPageControls::SetPlusButton(TLComponentInstance* component)
{
    mButtonInstances[0] = component;
}

/**
 * Offset/Address/Size: 0x11C | 0x802385C8 | size: 0x8
 */
void FEPageControls::SetMinusButton(TLComponentInstance* component)
{
    mButtonInstances[1] = component;
}

/**
 * Offset/Address/Size: 0x124 | 0x802385D0 | size: 0x44C
 */
void FEPageControls::Update(FEPointerEvent event, float fDeltaT)
{
    if (!mInitialized)
    {
        InitializeButtons();
        mInitialized = true;
    }
    for (int i = 0; i < 2; ++i)
    {
        mPointerPressed[i] = false;
        mPadPressed[i] = false;
        mButtons[i].HandlePointerEvent(&event);
    }
    if (mPointerInside[mHeldButton] && mPointerHeld)
    {
        mRepeatTime += fDeltaT;
        if (mRepeatTime >= 0.5f)
        {
            mPointerPressed[mHeldButton] = true;
            mRepeatTime = 0.0f;
        }
    }
    if (!mPointerHeld && (mPadHeld[0] || mPadHeld[1]))
    {
        mRepeatTime += fDeltaT;
        if (mRepeatTime >= 0.5f)
        {
            if (mPadHeld[0])
                mPadPressed[0] = true;
            if (mPadHeld[1])
                mPadPressed[1] = true;
            mRepeatTime = 0.0f;
        }
    }
    if (!g_pFEInput->IsPressed((eFEINPUT_PAD)event.mIndex, 30, true, 0) && mPointerHeld)
    {
        mPointerHeld = false;
        mRepeatTime = 0.0f;
        if (!mPadHeld[0] && !mPadHeld[1] && mPointerInside[mHeldButton])
        {
            mButtonInstances[mHeldButton]->SetActiveSlide("over", true, false);
        }
        // The retail condition tests the array address, not either element.
        else if (!mPadHeld)
        {
            mButtonInstances[mHeldButton]->SetActiveSlide("off", true, false);
            mButtonInstances[mHeldButton]->Update(mButtonInstances[!mHeldButton]->GetActiveSlide()->m_time);
        }
    }
    if (!g_pFEInput->IsPressed((eFEINPUT_PAD)event.mIndex, 48, true, 0) && mPadHeld[0])
    {
        mPadHeld[0] = false;
        mRepeatTime = 0.0f;
        if (!mPointerHeld && mPointerInside[0])
        {
            mButtonInstances[0]->SetActiveSlide("over", true, false);
        }
        else if (!mPointerInside[0])
        {
            mButtonInstances[0]->SetActiveSlide("off", true, false);
            mButtonInstances[0]->Update(mButtonInstances[1]->GetActiveSlide()->m_time);
        }
    }
    if (!g_pFEInput->IsPressed((eFEINPUT_PAD)event.mIndex, 49, true, 0) && mPadHeld[1])
    {
        mPadHeld[1] = false;
        mRepeatTime = 0.0f;
        if (!mPointerHeld && mPointerInside[1])
        {
            mButtonInstances[1]->SetActiveSlide("over", true, false);
        }
        else if (!mPointerInside[1])
        {
            mButtonInstances[1]->SetActiveSlide("off", true, false);
            mButtonInstances[1]->Update(mButtonInstances[0]->GetActiveSlide()->m_time);
        }
    }
    if (mPadInputEnabled)
    {
        if (g_pFEInput->JustPressed((eFEINPUT_PAD)gFEControllerIndex, 48, true, 0)
            && !g_pFEInput->JustPressed((eFEINPUT_PAD)gFEControllerIndex, 30, true, 0)
            && !mButtons[0].mDisabled)
        {
            OnPadPress(gFEControllerIndex, (void*)0);
        }
        else if (g_pFEInput->JustPressed((eFEINPUT_PAD)gFEControllerIndex, 49, true, 0)
            && !g_pFEInput->JustPressed((eFEINPUT_PAD)gFEControllerIndex, 30, true, 0)
            && !mButtons[1].mDisabled)
        {
            OnPadPress(gFEControllerIndex, (void*)1);
        }
    }
}

/**
 * Offset/Address/Size: 0x570 | 0x80238A1C | size: 0xEC
 */
void FEPageControls::SetButtonState(int index, bool enabled, bool visible)
{
    int which = !index;
    if (enabled)
    {
        mButtons[which].Enable();
    }
    else
    {
        mButtons[which].Disable();
        mPadHeld[which] = false;
    }
    mButtonInstances[which]->m_bVisible = visible;
}

/**
 * Offset/Address/Size: 0x65C | 0x80238B08 | size: 0x47C
 */
void FEPageControls::InitializeButtons()
{
    FEPointerListener::Callback callback0(Bind<void>(MemFun(&FEPageControls::OnPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback callback1(Bind<void>(MemFun(&FEPageControls::OnPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback callback2(Bind<void>(MemFun(&FEPageControls::OnPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    for (int i = 0; i < 2; ++i)
    {
        mButtonInstances[i]->SetActiveSlide("over", true, false);
        TLSlide* slide = mButtonInstances[i]->GetActiveSlide();
        float end = slide->GetStartTime() + slide->GetDuration();
        mButtonInstances[i]->GetActiveSlide()->m_time = end;
        mButtons[i].SetInstanceBounds(mButtonInstances[i], true, 0.0f, 0.0f, 1.0f, 1.0f);
        mButtonInstances[i]->SetActiveSlide("off", true, false);
        mButtons[i].SetPointerEnterCallback(callback0);
        mButtons[i].SetPointerLeaveCallback(callback1);
        mButtons[i].SetPointerPressCallback(callback2);
        mButtons[i].mContext = (void*)i;
        mButtons[i].SetMaxY(20.0f + mButtons[i].GetMaxY());
        mButtons[i].SetMinY(mButtons[i].GetMinY() - 20.0f);
        if (i == 0)
            mButtons[i].SetMaxX(20.0f + mButtons[i].GetMaxX());
        else if (i == 1)
            mButtons[i].SetMinX(mButtons[i].GetMinX() - 20.0f);
    }
}

/**
 * Offset/Address/Size: 0xAD8 | 0x80238F84 | size: 0xF0
 */
void FEPageControls::OnPointerEnter(int index, void* context)
{
    unsigned int which = (unsigned int)context;
    if (!mButtons[which].HasOtherPointerState(1, index))
    {
        if (mPointerHeld && (unsigned int)mHeldButton == which)
            mButtonInstances[which]->SetActiveSlide("down", true, false);
        else
            mButtonInstances[which]->SetActiveSlide("over", true, false);
        mButtons[which].SetPointerState(1, index);
        mPointerInside[which] = true;
        FEAudio::PlayAnimAudioEvent(0xAA73EF32, 0, 0, 1);
    }
}

/**
 * Offset/Address/Size: 0xBC8 | 0x80239074 | size: 0x114
 */
void FEPageControls::OnPointerLeave(int index, void* context)
{
    unsigned int which = (unsigned int)context;
    if (!mButtons[which].HasOtherPointerState(1, index))
    {
        int button = 49;
        if (which == 0)
            button = 48;
        if (!g_pFEInput->IsPressed((eFEINPUT_PAD)index, button, true, 0))
        {
            mButtonInstances[which]->SetActiveSlide("off", true, false);
            mButtonInstances[which]->Update(mButtonInstances[!which]->GetActiveSlide()->m_time);
        }
        else
        {
            mButtonInstances[which]->SetActiveSlide("down", true, false);
        }
        mButtons[which].SetPointerState(0, index);
        mPointerInside[which] = false;
    }
}

/**
 * Offset/Address/Size: 0xCDC | 0x80239188 | size: 0xAC
 */
void FEPageControls::OnPointerPress(int index, void* context)
{
    unsigned int which = (unsigned int)context;
    mHeldButton = which;
    mPointerHeld = true;
    mRepeatTime = 0.0f;
    mButtonInstances[which]->SetActiveSlide("down", true, false);
    mButtons[which].SetPointerState(1, index);
    if (!mPadHeld[0] && !mPadHeld[1])
        mPointerPressed[which] = true;
}

/**
 * Offset/Address/Size: 0xD88 | 0x80239234 | size: 0xA0
 */
void FEPageControls::OnPadPress(int index, void* context)
{
    unsigned int which = (unsigned int)context;
    mPadHeld[which] = true;
    mRepeatTime = 0.0f;
    mButtonInstances[which]->SetActiveSlide("down", true, false);
    mButtons[which].SetPointerState(1, index);
    if (!mPointerHeld)
        mPadPressed[which] = true;
}

/**
 * Offset/Address/Size: 0xE28 | 0x802392D4 | size: 0x98
 */
void FEPageControls::ClearButtonHighlight(int index)
{
    if (mPointerInside[index])
    {
        mButtonInstances[index]->SetActiveSlide("off", true, false);
        mPointerInside[index] = false;
        mButtonInstances[index]->Update(mButtonInstances[!index]->GetActiveSlide()->m_time);
    }
}

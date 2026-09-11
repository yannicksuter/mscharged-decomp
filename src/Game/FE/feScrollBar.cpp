#include "Game/FE/feScrollBar.h"
#include "NL/nlPrint.h"
#include "Game/FE/feHelpFuncs.h"
#include "Game/FE/FEAudio.h"

#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/tlComponentInstance.h"
#include "NL/nlBind.h"
#include "NL/nlString.h"
#include "Game/FE/UnidentifiedTLDefault.h"


FEScrollBar::FEScrollBar()
    : mThumb(0)
    , mInitialized(false)
    , mUnidentified019(false)
    , mUnidentified01A(true)
    , mIgnoreInputLock(false)
    , mRepeatTimer(0.0f)
    , mScrollStep(0.0f)
    , mTopPosition(0.0f)
    , mCurrentValue(0)
    , mMaxValue(0)
    , mUnidentified048(9999.9f)
{
    mButtons[0].mContext = (void*)0;
    mButtons[0].mSpeakerEnabled = false;
    mButtons[1].mContext = (void*)1;
    mButtons[1].mSpeakerEnabled = false;
    mScrolling[0] = false;
    mScrolling[1] = false;
    mUnidentified014[1] = false;
    mUnidentified014[0] = false;
    mUnidentified016[1] = false;
    mUnidentified016[0] = false;
    mUnidentified03C.x = 0.0f;
    mUnidentified03C.y = 0.0f;
    mUnidentified03C.z = 0.0f;
}

FEScrollBar::~FEScrollBar()
{
}

void FEScrollBar::SetIgnoreInputLock(bool enabled)
{
    mIgnoreInputLock = enabled;
    mButtons[0].mIgnoreInputLock = enabled;
    mButtons[1].mIgnoreInputLock = enabled;
}

void FEScrollBar::Initialize()
{
    mInitialized = true;
    mButtons[0].SetInstanceBounds(mButtonInstances[0], false, mAssetPosition.f.x, mAssetPosition.f.y, 1.0f, 1.0f);
    mButtons[1].SetInstanceBounds(mButtonInstances[1], false, mAssetPosition.f.x, mAssetPosition.f.y, 1.0f, 1.0f);

    FEPointerListener::Callback callback(Bind<void>(MemFun(&FEScrollBar::OnPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    mButtons[0].SetPointerEnterCallback(callback);
    mButtons[1].SetPointerEnterCallback(callback);
    callback = FEPointerListener::Callback(Bind<void>(MemFun(&FEScrollBar::OnPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    mButtons[0].SetPointerLeaveCallback(callback);
    mButtons[1].SetPointerLeaveCallback(callback);

    FEPointerListener::Callback callback2(Bind<void>(MemFun(&FEScrollBar::OnPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    mButtons[0].SetPointerPressCallback(callback2);
    mButtons[1].SetPointerPressCallback(callback2);
    callback2 = FEPointerListener::Callback(Bind<void>(MemFun(&FEScrollBar::OnPointerRelease), this, Placeholder<0>(), Placeholder<1>()));
    mButtons[0].SetPointerReleaseCallback(callback2);
    mButtons[1].SetPointerReleaseCallback(callback2);
}

bool FEScrollBar::IsScrolling(int direction, bool value)
{
    if (mScrolling[direction] && mRepeatTimer >= 0.5f)
    {
        if ((mUnidentified014[direction] || mUnidentified016[direction]) && value)
            mRepeatTimer = 0.0f;
        return true;
    }
    return false;
}

void FEScrollBar::OnPointerLeave(int index, void* context)
{
    int direction = (int)context;
    mButtons[direction].SetPointerState(0, index);
    if (context == 0)
    {
        if (mCurrentValue <= 0)
            return;
    }
    else if (context == (void*)1)
    {
        if (mCurrentValue >= mMaxValue)
            return;
    }
    if (!mUnidentified016[direction])
    {
        mButtonInstances[direction]->SetActiveSlide("off", true, false);
        mUnidentified019 = false;
        mScrolling[direction] = false;
    }
}

void FEScrollBar::OnPointerPress(int, void* context)
{
    if (mUnidentified016[0] || mUnidentified016[1])
        return;
    if (context == 0)
    {
        if (mCurrentValue <= 0 || mUnidentified014[1])
            return;
    }
    else if (context == (void*)1)
    {
        if (mCurrentValue >= mMaxValue || mUnidentified014[0])
            return;
    }
    int direction = (int)context;
    int other = direction == 0 ? 1 : 0;
    mButtonInstances[direction]->SetActiveSlide("slide1", true, false);
    mButtonInstances[other]->SetActiveSlide("off", true, false);
    mUnidentified014[direction] = true;
    mScrolling[direction] = true;
    mRepeatTimer = 0.5f;
    FEAudio::PlayAnimAudioEvent(0x3021A1EE, 0, 0, 1);
}

void FEScrollBar::OnPointerRelease(int index, void* context)
{
    if (mUnidentified016[0] || mUnidentified016[1])
        return;
    mScrolling[0] = false;
    mUnidentified014[0] = false;
    mScrolling[1] = false;
    mUnidentified014[1] = false;
    mRepeatTimer = 0.0f;
    int direction = (int)context;
    if (context == 0)
    {
        if (mCurrentValue <= 0)
        {
            mButtonInstances[direction]->SetActiveSlide("off", true, false);
            return;
        }
    }
    else if (context == (void*)1)
    {
        if (mCurrentValue >= mMaxValue)
        {
            mButtonInstances[direction]->SetActiveSlide("off", true, false);
            return;
        }
    }
    if (mButtons[direction].GetPointerState(index) == 0)
        mButtonInstances[direction]->SetActiveSlide("off", true, false);
    else if (mButtons[direction].GetPointerState(index) == 1)
        mButtonInstances[direction]->SetActiveSlide("over", true, false);
}

void FEScrollBar::OnPadPress(int, void* context)
{
    if (mUnidentified014[0] || mUnidentified014[1])
        return;
    if (context == 0)
    {
        if (mCurrentValue <= 0 || mUnidentified016[1])
            return;
    }
    else if (context == (void*)1)
    {
        if (mCurrentValue >= mMaxValue || mUnidentified016[0])
            return;
    }
    int direction = (int)context;
    int other = direction == 0 ? 1 : 0;
    mButtonInstances[direction]->SetActiveSlide("slide1", true, false);
    mButtonInstances[other]->SetActiveSlide("off", true, false);
    mUnidentified016[direction] = true;
    mScrolling[direction] = true;
    mRepeatTimer = 0.5f;
    FEAudio::PlayAnimAudioEvent(0x3021A1EE, 0, 0, 1);
}

void FEScrollBar::OnPadRelease(int index, void* context)
{
    if (mUnidentified014[0] || mUnidentified014[1])
        return;
    mScrolling[0] = false;
    mUnidentified016[0] = false;
    mScrolling[1] = false;
    mUnidentified016[1] = false;
    mRepeatTimer = 0.0f;
    int direction = (int)context;
    int other = direction == 0 ? 1 : 0;
    if ((context == 0 && mCurrentValue <= 0) || (context == (void*)1 && mCurrentValue >= mMaxValue))
        mButtonInstances[direction]->SetActiveSlide("off", true, false);
    else if (mButtons[direction].GetPointerState(index) == 0)
        mButtonInstances[direction]->SetActiveSlide("off", true, false);
    else if (mButtons[direction].GetPointerState(index) == 1)
        mButtonInstances[direction]->SetActiveSlide("over", true, false);

    if ((other == 0 && mCurrentValue <= 0) || (other == 1 && mCurrentValue >= mMaxValue))
        mButtonInstances[other]->SetActiveSlide("off", true, false);
    else if (mButtons[other].GetPointerState(index) == 0)
        mButtonInstances[other]->SetActiveSlide("off", true, false);
    else if (mButtons[other].GetPointerState(index) == 1)
        mButtonInstances[other]->SetActiveSlide("over", true, false);
}

void FEScrollBar::SetOffset(const nlVector3& value)
{
    nlVec3Set(mUnidentified03C, value.x, value.y, value.z);
    mAssetPosition.f.x += mUnidentified03C.x;
    mAssetPosition.f.y += mUnidentified03C.y;
    mAssetPosition.f.z += mUnidentified03C.z;
}

void FEScrollBar::ResetScrolling()
{
    if (mUnidentified048 != 9999.9f && mThumb != 0)
    {
        feVector3 position = mThumb->GetAssetPosition();
        position.f.y = mUnidentified048;
        mThumb->SetAssetPosition(position.f.x, position.f.y, position.f.z);
    }
    mScrolling[0] = false;
    mScrolling[1] = false;
    mUnidentified014[1] = false;
    mUnidentified014[0] = false;
    mUnidentified016[1] = false;
    mUnidentified016[0] = false;
    mRepeatTimer = 0.0f;
}

void FEScrollBar::Update(FEPointerEvent event, float dt)
{
    if (g_pFEInput->m_InputLockDepth != 0 && !mIgnoreInputLock)
        return;
    if (mScrolling[0] || mScrolling[1])
        mRepeatTimer += dt;
    mButtons[0].HandlePointerEvent(&event);
    mButtons[1].HandlePointerEvent(&event);
    eFEINPUT_PAD pad = (eFEINPUT_PAD)event.mIndex;
    if (g_pFEInput->JustPressed(pad, 13, true, 0) && !mUnidentified014[0] && !mUnidentified014[1])
        OnPadPress(event.mIndex, (void*)0);
    else if (g_pFEInput->JustReleased(pad, 13, true, 0))
        OnPadRelease(event.mIndex, (void*)0);
    else if (g_pFEInput->JustPressed(pad, 14, true, 0) && !mUnidentified014[0] && !mUnidentified014[1])
        OnPadPress(event.mIndex, (void*)1);
    else if (g_pFEInput->JustReleased(pad, 14, true, 0))
        OnPadRelease(event.mIndex, (void*)1);

    if (IsScrolling(0, false))
    {
        if (mCurrentValue > 0)
        {
            --mCurrentValue;
            FEAudio::PlayAnimAudioEvent(0x3021A1EE, 0, 0, 1);
            feVector3 position = mThumb->GetAssetPosition();
            float offset = mCurrentValue * mScrollStep;
            mThumb->SetAssetPosition(position.f.x, mTopPosition - offset, position.f.z);
        }
        else
            mRepeatTimer = 0.0f;
    }
    else if (IsScrolling(1, false))
    {
        if (mCurrentValue < mMaxValue)
        {
            ++mCurrentValue;
            FEAudio::PlayAnimAudioEvent(0x3021A1EE, 0, 0, 1);
            feVector3 position = mThumb->GetAssetPosition();
            float offset = mCurrentValue * mScrollStep;
            mThumb->SetAssetPosition(position.f.x, mTopPosition - offset, position.f.z);
        }
        else
            mRepeatTimer = 0.0f;
    }
    if (mCurrentValue <= 0)
        mButtonInstances[0]->SetActiveSlide("unused", true, false);
    if (mCurrentValue >= mMaxValue)
        mButtonInstances[1]->SetActiveSlide("unused", true, false);
    if (mUnidentified014[0] && !g_pFEInput->IsPressed(pad, 30, true, 0))
        OnPointerRelease(event.mIndex, (void*)0);
    if (mUnidentified014[1] && !g_pFEInput->IsPressed(pad, 30, true, 0))
        OnPointerRelease(event.mIndex, (void*)1);
    if (mUnidentified016[1] && !g_pFEInput->IsPressed(pad, 14, true, 0))
        OnPadRelease(event.mIndex, (void*)1);
    if (mUnidentified016[0] && !g_pFEInput->IsPressed(pad, 13, true, 0))
        OnPadRelease(event.mIndex, (void*)0);
}

void FEScrollBar::SetComponent(TLComponentInstance* instance)
{
    mComponent = (TLComponentInstance*)instance;
    mAssetPosition = instance->GetAssetPosition();
    TLComponentInstance* up = FEFinder<TLComponentInstance, 4>::Find(mComponent->GetActiveSlide(), "up_arrow");
    mButtonInstances[0] = up == 0 ? &UnidentifiedTLComponentDefault::sInstance : up;
    TLComponentInstance* down = FEFinder<TLComponentInstance, 4>::Find(mComponent->GetActiveSlide(), "down_arrow");
    mButtonInstances[1] = down == 0 ? &UnidentifiedTLComponentDefault::sInstance : down;
    TLImageInstance* found = FEFinder<TLImageInstance, 2>::Find(mComponent->GetActiveSlide(), "track", "btn_scroll_minmax");
    mThumb = found == 0 ? &UnidentifiedTLImageDefault::sInstance : found;
}

void FEScrollBar::SetRange(int value)
{
    if (value > 0)
    {
        TLInstance* track = FEFinder<TLInstance, 2>::Find(mComponent->GetActiveSlide(), "track", "btn_track ");
        feVector3 trackScale = track->GetScale();
        feVector3 scale = mThumb->GetScale();
        float distance = 0.63671875 * trackScale.f.y;
        distance = (distance - scale.f.y / 2.0f) * 100.0f;
        mScrollStep = distance / value;
        feVector3 position = mThumb->GetAssetPosition();
        if (mUnidentified048 == 9999.9f)
            mUnidentified048 = position.f.y;
        mTopPosition = distance / 2.0f + position.f.y;
        mThumb->SetAssetPosition(position.f.x, mTopPosition, position.f.z);
        mThumb->m_bVisible = true;
    }
    else
    {
        mScrollStep = 0.0f;
        mTopPosition = 0.0f;
        feVector3 position = mThumb->GetAssetPosition();
        mThumb->SetAssetPosition(position.f.x, mTopPosition, position.f.z);
        mThumb->m_bVisible = false;
    }
    mMaxValue = value;
}

void FEScrollBar::SetValue(int value)
{
    mCurrentValue = value;
    feVector3 position = mThumb->GetAssetPosition();
    float offset = mCurrentValue * mScrollStep;
    mThumb->SetAssetPosition(position.f.x, mTopPosition - offset, position.f.z);
    if (mCurrentValue <= 0)
        mButtonInstances[0]->SetActiveSlide("unused", true, false);
    else
        mButtonInstances[0]->SetActiveSlide("off", true, false);
    if (mCurrentValue >= mMaxValue)
        mButtonInstances[1]->SetActiveSlide("unused", true, false);
    else
        mButtonInstances[1]->SetActiveSlide("off", true, false);
}

void FEScrollBar::OnPointerEnter(int index, void* context)
{
    int direction = (int)context;
    mButtons[direction].SetPointerState(1, index);
    int other = -1;
    if (context == 0)
    {
        other = 1;
        if (mCurrentValue <= 0)
        {
            mUnidentified019 = false;
            return;
        }
    }
    else if (context == (void*)1)
    {
        other = 0;
        if (mCurrentValue >= mMaxValue)
        {
            mUnidentified019 = false;
            return;
        }
    }
    if (mUnidentified014[direction])
        mButtonInstances[direction]->SetActiveSlide("slide1", true, false);
    else if (!mUnidentified016[0] && !mUnidentified016[1] && !mUnidentified014[other])
    {
        mButtonInstances[direction]->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0x96DEB5C3, 0, 0, 1);
        mButtons[direction].PlayHoverFeedback(index);
    }
    mUnidentified019 = true;
    if (mUnidentified014[direction])
        mScrolling[direction] = true;
}

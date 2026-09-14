#include "Game/FE/feBackButton.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/FEAudio.h"

#include "Game/GameSceneManager.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "NL/nlBind.h"
#include "NL/nlString.h"
#include "Game/FE/FEAudio.h"
#include "Game/Render/RLViewLayers.h"


extern BaseGameSceneManager* g_pOverlayManager;

/**
 * Offset/Address/Size: 0x0 | 0x8022EF84 | size: 0x84
 */
FEBackButton::FEBackButton()
    : FEPointerButton((void*)0)
    , mBackScene(SCENE_INVALID)
    , mUnidentifiedB8(0.0f)
    , mButtonInstance(0)
    , mPressed(false)
    , mUnidentifiedCD(false)
    , mUnidentifiedCE(false)
    , mPushBackScene(true)
    , mPopScene(true)
    , mBoundsInitialized(false)
{
    mPointerInside[0] = false;
    mPointerInside[1] = false;
    mPointerInside[2] = false;
    mPointerInside[3] = false;
}

/**
 * Offset/Address/Size: 0x84 | 0x8022F008 | size: 0x5C
 */
FEBackButton::~FEBackButton()
{
}

/**
 * Offset/Address/Size: 0xE0 | 0x8022F064 | size: 0xA0
 */
void FEBackButton::OnPointerEnter(int index, void* context)
{
    if (!HasOtherPointerState(1, index))
    {
        mButtonInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xACCDCA48, 0, 0, 1);
    }
    SetPointerState(1, index);
    FEPointerButton::OnPointerEnter(index, context);
}

/**
 * Offset/Address/Size: 0x180 | 0x8022F104 | size: 0x10
 */
void FEBackButton::OnPointerInside(int index, void*)
{
    mPointerInside[index] = true;
}

/**
 * Offset/Address/Size: 0x190 | 0x8022F114 | size: 0x4C
 */
void FEBackButton::OnPointerPress(int index, void* context)
{
    FEPointerButton::OnPointerPress(index, context);
    FEAudio::PlayAnimAudioEvent(0x6F6A3A07, 0, 0, 1);
    mPressed = true;
}

/**
 * Offset/Address/Size: 0x1DC | 0x8022F160 | size: 0x34
 */
void FEBackButton::OnPointerRelease(int index, void* context)
{
    FEPointerButton::OnPointerRelease(index, context);
    mPressed = false;
}

/**
 * Offset/Address/Size: 0x210 | 0x8022F194 | size: 0x14C
 */
void FEBackButton::SetButtonInstance(TLComponentInstance* instance)
{
    if (IsWidescreen())
    {
        instance->SetActiveSlide("16:9", true, false);
    }
    else
    {
        instance->SetActiveSlide("4:3", true, false);
    }

    mButtonPosition = instance->GetAssetPosition();
    mButtonInstance = (TLComponentInstance*)FEFinder<TLComponentInstance, 2>::_Find<TLSlide>(
        instance->GetActiveSlide(), nlStringLowerHash("back"), 0, 0, 0, 0, 0);
}

/**
 * Offset/Address/Size: 0x35C | 0x8022F2E0 | size: 0x2B0
 */
bool FEBackButton::UpdateBackButton(FEPointerEvent event, float)
{
    typedef Detail::MemFunImpl<void, void (FEBackButton::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, FEBackButton*, Placeholder<0>, Placeholder<1> > PointerBinding;

    if (mDisabled)
    {
        return false;
    }

    if (!mBoundsInitialized)
    {
        mBoundsInitialized = true;

        FEPointerListener::Callback callback(PointerBinding(
            MemFun(&FEBackButton::OnPointerInside), this, Placeholder<0>(), Placeholder<1>()));
        SetPointerInsideCallback(callback);

        TLInstance* found = (TLInstance*)FEFindInstance(mButtonInstance,
            nlStringLowerHash("over"),
            nlStringLowerHash("list_high_250x60"),
            0,
            0,
            0,
            0);
        TLInstance* over;
        if (found == 0)
        {
            over = &UnidentifiedTLImageDefault::sInstance;
        }
        else
        {
            over = found;
        }

        feVector3 position = mButtonInstance->GetAssetPosition();
        SetInstanceBounds(over, true, position.f.x, position.f.y, 1.0f, 1.0f);
    }

    mPointerInside[event.mIndex] = false;
    HandlePointerEvent(&event);

    BaseGameSceneManager* manager = GameSceneManager::Instance();
    if (manager == 0)
    {
        manager = g_pOverlayManager;
    }

    if (mPressed && mPushBackScene)
    {
        if (mBackScene != SCENE_INVALID)
        {
            manager->Push((SceneList)mBackScene, SCREEN_BACK, true);
        }
        mPressed = false;
        return true;
    }

    if (mPressed && !mPushBackScene)
    {
        if (mPopScene)
        {
            manager->Pop();
        }
        mPressed = false;
        return true;
    }

    return false;
}

/**
 * Offset/Address/Size: 0x60C | 0x8022F590 | size: 0x98
 */
void FEBackButton::OnPointerLeave(int index, void* context)
{
    if (!HasOtherPointerState(1, index))
    {
        mUnidentifiedB8 = 0.0f;
        mButtonInstance->SetActiveSlide("off", true, false);
        mUnidentifiedCE = false;
    }
    SetPointerState(0, index);
    FEPointerButton::OnPointerLeave(index, context);
}

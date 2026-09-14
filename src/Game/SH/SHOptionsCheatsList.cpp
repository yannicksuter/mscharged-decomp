#include "Game/SH/SHOptionsCheatsList.h"
#include "Game/SH/SHGameplayOptions.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/FEAudio.h"

#include "Game/GameSceneManager.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "NL/globalpad.h"
#include "NL/nlBind.h"
#include "NL/nlString.h"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/fePointer.h"
#include "Game/FE/FEAudio.h"

SHOptionsCheatsList::SHOptionsCheatsList()
    : mComponents()
    , mScrollWidget()
    , mButtonsInitialized(false)
    , mNavigation()
    , mSavedNavigationButtons(false)
    , mState(0)
    , mFirstVisibleCheat(0)
    , mSettings(0)
    , mCheatCategory(-1)
{
    g_pFEInput->PushExclusiveInputLock(this, -1);

    for (int i = 0; i < 5; ++i)
    {
        mComponents[i].mContext = (void*)i;
        mComponents[i].mIgnoreInputLock = true;
        mComponents[i].mSpeakerEnabled = false;
    }

    mScrollWidget.SetIgnoreInputLock(true);

    mPointerInsideCounts[0] = 0;
    mPointerInsideCounts[1] = 0;
    mPointerInsideCounts[2] = 0;
    mPointerInsideCounts[3] = 0;

    mNavigation.SetPushBackScene(false);
    mNavigation.SetPopScene(false);
    mNavigation.mIgnoreInputLock = true;
}

SHOptionsCheatsList::~SHOptionsCheatsList()
{
    g_pFEInput->PopExclusiveInputLock(this);

    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
    {
        scene->SetButtons(mSavedNavigationButtons, true);
    }
}

void SHOptionsCheatsList::OnCheatPointerInside(int index, void* context)
{
    bool unlocked = false;
    int item = (int)context;

    if (mCheatCategory == 0 && IsEnvironmentCheatUnlocked(item + mFirstVisibleCheat))
    {
        unlocked = true;
    }
    else if (mCheatCategory == 2 && IsPlayerCheatUnlocked(item + mFirstVisibleCheat))
    {
        unlocked = true;
    }
    else if (mCheatCategory == 1 && IsPowerupCheatUnlocked(item + mFirstVisibleCheat))
    {
        unlocked = true;
    }

    if (!unlocked && mComponents[item].GetPointerState(index) == 1)
    {
        --mPointerInsideCounts[index];
        mCheatInstances[item]->SetActiveSlide("off", true, false);
        mComponents[item].SetPointerState(0, index);
    }
    else if (unlocked && mComponents[item].GetPointerState(index) == 0)
    {
        OnCheatPointerEnter(index, context);
    }
}

void SHOptionsCheatsList::OnCheatPointerLeave(int index, void* context)
{
    bool unlocked = false;
    unsigned int item = (unsigned int)context;

    if (mCheatCategory == 0 && IsEnvironmentCheatUnlocked(item + mFirstVisibleCheat))
    {
        unlocked = true;
    }
    else if (mCheatCategory == 2 && IsPlayerCheatUnlocked(item + mFirstVisibleCheat))
    {
        unlocked = true;
    }
    else if (mCheatCategory == 1 && IsPowerupCheatUnlocked(item + mFirstVisibleCheat))
    {
        unlocked = true;
    }

    if (unlocked)
    {
        --mPointerInsideCounts[index];
        if (!mComponents[item].HasOtherPointerState(1, index))
        {
            mCheatInstances[item]->SetActiveSlide("off", true, false);
        }
        mComponents[item].SetPointerState(0, index);
    }
}

void SHOptionsCheatsList::UpdateCheatText(int item)
{
    const char* strings[2] = { "CHEATS_LOCKED", "CHEATS_LOCKED_DESC" };
    const char* slideNames[3] = { "off", "over", "down" };
    bool descriptionVisible = true;

    if (mCheatCategory == 0)
    {
        if (IsEnvironmentCheatUnlocked(item + mFirstVisibleCheat))
        {
            strings[0] = GetLOCEnvironmentCheatName(item + mFirstVisibleCheat);
            strings[1] = GetLOCEnvironmentCheatDescription(item + mFirstVisibleCheat);
        }
        if (item + mFirstVisibleCheat == 0)
        {
            descriptionVisible = false;
        }
    }
    else if (mCheatCategory == 2)
    {
        if (IsPlayerCheatUnlocked(item + mFirstVisibleCheat))
        {
            strings[0] = GetLOCPlayerCheatName(item + mFirstVisibleCheat);
            strings[1] = GetLOCPlayerCheatDescription(item + mFirstVisibleCheat);
        }
        if (item + mFirstVisibleCheat == 0)
        {
            descriptionVisible = false;
        }
    }
    else if (mCheatCategory == 1)
    {
        if (IsPowerupCheatUnlocked(item + mFirstVisibleCheat))
        {
            strings[0] = GetLOCPowerupCheatName(item + mFirstVisibleCheat);
            strings[1] = GetLOCPowerupCheatDescription(item + mFirstVisibleCheat);
        }
        if (item + mFirstVisibleCheat == 0)
        {
            descriptionVisible = false;
        }
    }

    const char* challenge = "CHALLENGE_0";
    const char* stat0 = "stat_0";
    const char* stat1 = "stat_1";
    for (int i = 0; i < 3; ++i)
    {
        TLTextInstance* nameText = FEFinder<TLTextInstance, 3>::Find(
            mCheatInstances[item], nlStringLowerHash(slideNames[i]), nlStringLowerHash(challenge), nlStringLowerHash(stat0), 0, 0, 0);
        TLTextInstance* descriptionText = FEFinder<TLTextInstance, 3>::Find(
            mCheatInstances[item], nlStringLowerHash(slideNames[i]), nlStringLowerHash(challenge), nlStringLowerHash(stat1), 0, 0, 0);

        nameText->SetStringId(strings[0]);
        descriptionText->SetStringId(strings[1]);
        descriptionText->m_bVisible = descriptionVisible;
    }
}

void SHOptionsCheatsList::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    int state = mState;
    if (state == 0 || (unsigned int)(state - 2) <= 1)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->m_start + slide->m_duration)
        {
            for (int pad = 0; pad < 4; ++pad)
            {
                gFEPointerInstances[pad]->SetActiveSlide("waiting", true, false);
            }
            return;
        }

        if (state == 0)
        {
            GetNavigationScene()->SetButtons(4, true);
            if (!mButtonsInitialized)
            {
                InitializeButtons();
                mButtonsInitialized = true;
            }
            mState = 1;
        }
        else if (state == 2)
        {
            GameSceneManager::Instance()->Pop();
            ((SHGameplayOptions*)GameSceneManager::Instance()->GetScene((SceneList)0x1B))->UpdateCheatText();
            return;
        }
        else if (state == 3)
        {
            GameSceneManager::Instance()->Pop();
            return;
        }
    }

    for (int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = gFEPointerInstances[pad];
        if ((unsigned int)pad != gFEControllerIndex)
        {
            controller->SetActiveSlide("waiting", true, false);
            continue;
        }

        if (mPointerInsideCounts[pad] > 0)
        {
            controller->SetActiveSlide("A", true, false);
        }
        else
        {
            controller->SetActiveSlide("cursor", true, false);
        }

        unsigned char valid = 1;
        FEPointerEvent event;
        event.mIndex = pad;
        event.mPosition = GetPointerPosition(pad, &valid);
        g_pPadManager->GetPad(pad)->GetButtonIndex(0x1E, true);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
        event.mReleased = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);

        if (mNavigation.UpdateBackButton(event, fDeltaT))
        {
            mState = 3;
            GetNavigationScene()->HideButtons();
            mPresentation->SetActiveSlide("out", true);
            return;
        }

        for (int i = 0; i < 5; ++i)
        {
            mComponents[i].HandlePointerEvent(&event);
        }
        mScrollWidget.Update(event, fDeltaT);
    }

    if (mScrollWidget.IsScrolling(1, 1))
    {
        ++mFirstVisibleCheat;
        for (int i = 0; i < 5; ++i)
        {
            UpdateCheatText(i);
        }
    }
    else if (mScrollWidget.IsScrolling(0, 1))
    {
        --mFirstVisibleCheat;
        for (int i = 0; i < 5; ++i)
        {
            UpdateCheatText(i);
        }
    }
}

void SHOptionsCheatsList::SceneCreated()
{
    SHNavigation* scene = GetNavigationScene();
    TLComponentInstance* screen = 0;
    if (scene != 0)
    {
        mSavedNavigationButtons = scene->mVisibleButtons;
        scene->HideButtons();
        screen = scene->GetButton(4);
    }
    mNavigation.SetButtonInstance(screen);

    TLSlide* slide = mPresentation->m_currentSlide;
    int itemCount = 0;
    TLComponentInstance* title = FEFinder<TLComponentInstance, 2>::Find(slide,
        nlStringLowerHash("Layer"),
        nlStringLowerHash("TITLE2"),
        0,
        0,
        0,
        0);

    if (mCheatCategory == 0)
    {
        itemCount = 6;
        title->SetActiveSlide("environment", true, false);
    }
    else if (mCheatCategory == 2)
    {
        itemCount = 5;
        title->SetActiveSlide("player", true, false);
    }
    else if (mCheatCategory == 1)
    {
        itemCount = 12;
        title->SetActiveSlide("pups", true, false);
    }

    int scrollRange = itemCount - 5;
    mCheatInstances[0] = FEFinder<TLComponentInstance, 2>::Find(slide,
        nlStringLowerHash("Layer"),
        nlStringLowerHash("cheat_0"),
        0,
        0,
        0,
        0);
    mCheatInstances[1] = FEFinder<TLComponentInstance, 2>::Find(slide,
        nlStringLowerHash("Layer"),
        nlStringLowerHash("cheat_1"),
        0,
        0,
        0,
        0);
    mCheatInstances[2] = FEFinder<TLComponentInstance, 2>::Find(slide,
        nlStringLowerHash("Layer"),
        nlStringLowerHash("cheat_2"),
        0,
        0,
        0,
        0);
    mCheatInstances[3] = FEFinder<TLComponentInstance, 2>::Find(slide,
        nlStringLowerHash("Layer"),
        nlStringLowerHash("cheat_3"),
        0,
        0,
        0,
        0);
    mCheatInstances[4] = FEFinder<TLComponentInstance, 2>::Find(slide,
        nlStringLowerHash("Layer"),
        nlStringLowerHash("cheat_4"),
        0,
        0,
        0,
        0);

    TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, 2>::Find(slide,
        nlStringLowerHash("Layer"),
        nlStringLowerHash("scrollbar"),
        0,
        0,
        0,
        0);
    mScrollWidget.SetComponent(scrollbar);
    mScrollWidget.SetRange(scrollRange);
    mScrollWidget.SetValue(mFirstVisibleCheat);

    for (int i = 0; i < 5; ++i)
    {
        UpdateCheatText(i);
    }
}

void SHOptionsCheatsList::InitializeButtons()
{
    typedef Detail::MemFunImpl<void, void (SHOptionsCheatsList::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, SHOptionsCheatsList*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback over(
        PointerBinding(MemFun(&SHOptionsCheatsList::OnCheatPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(
        PointerBinding(MemFun(&SHOptionsCheatsList::OnCheatPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback down(
        PointerBinding(MemFun(&SHOptionsCheatsList::OnCheatPointerInside), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback select(
        PointerBinding(MemFun(&SHOptionsCheatsList::OnCheatPointerPress), this, Placeholder<0>(), Placeholder<1>()));

    const char* listBack = "list_back_480x70 ";
    const char* challenge = "CHALLENGE_0";
    const char* slide = "off";
    for (int i = 0; i < 5; ++i)
    {
        TLComponentInstance* instance = FEFinder<TLComponentInstance, 3>::Find(
            mCheatInstances[i],
            nlStringLowerHash(slide),
            nlStringLowerHash(challenge),
            nlStringLowerHash(listBack),
            0,
            0,
            0);
        feVector3 position = mCheatInstances[i]->GetAssetPosition();
        mComponents[i].SetInstanceBounds(
            instance, true, position.f.x, position.f.y, 1.0f, 0.5f);
        mComponents[i].SetPointerEnterCallback(over);
        mComponents[i].SetPointerLeaveCallback(off);
        mComponents[i].SetPointerInsideCallback(down);
        mComponents[i].SetPointerPressCallback(select);
    }

    if (!mScrollWidget.mInitialized)
    {
        mScrollWidget.Initialize();
    }
}

void SHOptionsCheatsList::OnCheatPointerEnter(int index, void* context)
{
    bool unlocked = false;
    unsigned int item = (unsigned int)context;

    if (mCheatCategory == 0 && IsEnvironmentCheatUnlocked(item + mFirstVisibleCheat))
    {
        unlocked = true;
    }
    else if (mCheatCategory == 2 && IsPlayerCheatUnlocked(item + mFirstVisibleCheat))
    {
        unlocked = true;
    }
    else if (mCheatCategory == 1 && IsPowerupCheatUnlocked(item + mFirstVisibleCheat))
    {
        unlocked = true;
    }

    if (unlocked)
    {
        ++mPointerInsideCounts[index];
        mComponents[item].PlayHoverFeedback(index);
        if (!mComponents[item].HasOtherPointerState(1, index))
        {
            mCheatInstances[item]->SetActiveSlide("over", true, false);
            FEAudio::PlayAnimAudioEvent(0xF6EB899E, 0, 0, 1);
        }
        mComponents[item].SetPointerState(1, index);
    }
}

void SHOptionsCheatsList::OnCheatPointerPress(int, void* context)
{
    bool unlocked = false;
    int item = (int)context;

    if (mCheatCategory == 0 && IsEnvironmentCheatUnlocked(item + mFirstVisibleCheat))
    {
        unlocked = true;
    }
    else if (mCheatCategory == 2 && IsPlayerCheatUnlocked(item + mFirstVisibleCheat))
    {
        unlocked = true;
    }
    else if (mCheatCategory == 1 && IsPowerupCheatUnlocked(item + mFirstVisibleCheat))
    {
        unlocked = true;
    }

    if (unlocked)
    {
        mCheatInstances[item]->SetActiveSlide("down", true, false);

        if (mCheatCategory == 0)
        {
            mSettings->mEnvironmentCheat = item + mFirstVisibleCheat;
        }
        else if (mCheatCategory == 2)
        {
            mSettings->mPlayerCheat = item + mFirstVisibleCheat;
        }
        else if (mCheatCategory == 1)
        {
            mSettings->mCustomPowerups = item + mFirstVisibleCheat;
        }

        FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
        FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
        mState = 2;
        GetNavigationScene()->HideButtons();
        mPresentation->SetActiveSlide("out", true);
    }
}

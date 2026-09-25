#include "Game/FE/feCharacterPDAComponent.h"

#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/fePresentation.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feDPD.h"
#include "Game/BaseSceneHandler.inl"
#include "Game/GameSceneManager.h"
#include "Game/GameInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/CharacterInfo.h"
#include "NL/nlFunctionMemory.h"
#include "NL/nlFunction.inl"
#include "NL/nlBindMember.h"
#include "NL/nlBind_impl.h"
#include "NL/nlString.h"

FECharacterPDAComponent::FECharacterPDAComponent()
    : mScrollText(1)
    , mRootComponent(0)
    , mPDAScreens(0)
    , mCaptainAttributes(0)
    , mPositions(0)
    , mJoinPrompt(0)
    , mUnidentified290(2)
    , mSelectedCaptain(-1)
    , mDisplayMode(-1)
{
    mCaptainNeedsRefresh = false;
    mSidekickNeedsRefresh = false;
    mScrollBarNeedsReset = false;
    mSlideAnimating = true;
    for (int i = 0; i < 4; ++i)
    {
        mCaptainAttributeBars[i] = 0;
    }
    for (int i = 0; i < 4; ++i)
    {
        mSidekickAttributeBars[i] = 0;
    }
    mScrollText.SetEndBehavior(2);
    mScrollText.SetScrollMode(1);
    mUnidentified2A8 = false;
}

FECharacterPDAComponent::~FECharacterPDAComponent()
{
    for (int i = 0; i < 4; ++i)
    {
        delete mCaptainAttributeBars[i];
    }
    for (int i = 0; i < 4; ++i)
    {
        delete mSidekickAttributeBars[i];
    }
}

void FECharacterPDAComponent::Initialize(TLComponentInstance* component, int side, u32 value)
{
    static const char* captainAttributes[] = { "attribute_movement", "attribute_shooting", "attribute_passing", "attribute_defense" };
    static const char* sidekickAttributes[] = { "attribute_movement", "attribute_shooting", "attribute_passing", "attribute_defense" };

    mRootComponent = component;
    mUnidentified24 = value;
    mSide = side;
    mPDAScreens = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mRootComponent->GetActiveSlide(), "pda_screens");
    mCaptainAttributes = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mPDAScreens->GetActiveSlide(), "attributes_captains");
    mPositions = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mPDAScreens->GetActiveSlide(), "positions");
    mJoinPrompt = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mPDAScreens->GetActiveSlide(), "A to join");
    mReadyPrompt = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mPDAScreens->GetActiveSlide(), "ready_prompt");
    mSidekickAttributes = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mRootComponent->GetActiveSlide(), "attributes_sidekicks");
    mPositionsGroup = FEFinder<UnidentifiedTLGroupInstance, TLAT_GROUP>::FindOrDefault(mPositions->GetActiveSlide(), "positions");
    mTeamLogos = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mPositions->GetActiveSlide(), "team_logos");

    mSidekickAttributeImages[0] = FEFinder<TLImageInstance, 2>::FindOrDefault(mSidekickAttributes->GetActiveSlide(), "attributes_sidekicks", "white_8x8");
    mSidekickAttributeImages[1] = FEFinder<TLImageInstance, 2>::FindOrDefault(mSidekickAttributes->GetActiveSlide(), "attributes_sidekicks", "white_8x9");
    mSidekickAttributeImages[2] = FEFinder<TLImageInstance, 2>::FindOrDefault(mSidekickAttributes->GetActiveSlide(), "attributes_sidekicks", "movement2");
    mSidekickAttributeImages[3] = FEFinder<TLImageInstance, 2>::FindOrDefault(mSidekickAttributes->GetActiveSlide(), "attributes_sidekicks", "shooting2");
    mSidekickAttributeImages[4] = FEFinder<TLImageInstance, 2>::FindOrDefault(mSidekickAttributes->GetActiveSlide(), "attributes_sidekicks", "passing2");
    mSidekickAttributeImages[5] = FEFinder<TLImageInstance, 2>::FindOrDefault(mSidekickAttributes->GetActiveSlide(), "attributes_sidekicks", "defense2");
    mSidekickAttributeImages[6] = FEFinder<TLImageInstance, 2>::FindOrDefault(mSidekickAttributes->GetActiveSlide(), "attributes_sidekicks", "white_8x11");
    mCaptainAttributeImages[0] = FEFinder<TLImageInstance, 2>::FindOrDefault(mCaptainAttributes->GetActiveSlide(), "attributes_captains", "white_8x8");
    mCaptainAttributeImages[1] = FEFinder<TLImageInstance, 2>::FindOrDefault(mCaptainAttributes->GetActiveSlide(), "attributes_captains", "white_8x9");
    mCaptainAttributeImages[2] = FEFinder<TLImageInstance, 2>::FindOrDefault(mCaptainAttributes->GetActiveSlide(), "attributes_captains", "movement2");
    mCaptainAttributeImages[3] = FEFinder<TLImageInstance, 2>::FindOrDefault(mCaptainAttributes->GetActiveSlide(), "attributes_captains", "shooting2");
    mCaptainAttributeImages[4] = FEFinder<TLImageInstance, 2>::FindOrDefault(mCaptainAttributes->GetActiveSlide(), "attributes_captains", "passing2");
    mCaptainAttributeImages[5] = FEFinder<TLImageInstance, 2>::FindOrDefault(mCaptainAttributes->GetActiveSlide(), "attributes_captains", "defense2");
    mCaptainAttributeImages[6] = FEFinder<TLImageInstance, 2>::FindOrDefault(mCaptainAttributes->GetActiveSlide(), "attributes_captains", "white_8x11");

    for (int i = 0; i < 4; ++i)
    {
        mCaptainAttributeBars[i] = new (8, false) FEAttributeBar(
            (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mCaptainAttributes->GetActiveSlide(), "attributes_captains", captainAttributes[i]));
    }
    for (int i = 0; i < 4; ++i)
    {
        mSidekickAttributeBars[i] = new (8, false) FEAttributeBar(
            (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mSidekickAttributes->GetActiveSlide(), "attributes_sidekicks", sidekickAttributes[i]));
    }

    if (side == 0)
    {
        TLTextInstance* captain = FEFinder<TLTextInstance, 3>::FindOrDefault(mCaptainAttributes->GetActiveSlide(), "attributes_captains", "CAPTAIN");
        TLTextInstance* captainAway = FEFinder<TLTextInstance, 3>::FindOrDefault(mCaptainAttributes->GetActiveSlide(), "attributes_captains", "AWAY");
        TLTextInstance* team = FEFinder<TLTextInstance, 3>::FindOrDefault(mPositions->GetActiveSlide(), "positions", "TEAM");
        TLTextInstance* teamAway = FEFinder<TLTextInstance, 3>::FindOrDefault(mPositions->GetActiveSlide(), "positions", "AWAY");
        if (GameInfoManager::Instance()->IsInMode3())
        {
            captain->SetVisible(true);
            captainAway->SetVisible(false);
            team->SetVisible(true);
            teamAway->SetVisible(false);
        }
        else
        {
            captain->SetVisible(false);
            captainAway->SetVisible(true);
            team->SetVisible(false);
            teamAway->SetVisible(true);
        }
    }

    mDescriptions = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mPDAScreens->GetActiveSlide(), "descriptions");
    mScrollArrows = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mPDAScreens->GetActiveSlide(), "scroll arrows");
    mContinueGroup = FEFinder<UnidentifiedTLGroupInstance, TLAT_GROUP>::FindOrDefault(mPDAScreens->GetActiveSlide(), "cONTINUE");
    SetVisible(false);
    SetCupIconsVisible(false, false, false);
}

static char SLIDE_IN[] = "in";

static const nlColour sAttributeBarEmptyColour = { 0, 0, 0, 160 };

static const nlColour sAttributeBarFilledColour = { 169, 208, 70, 255 };

inline void FEAttributeBar::SetValue(int value)
{
    if (mComponent != 0)
    {
        for (int i = 1; i <= 10; ++i)
        {
            char name[50];
            nlSNPrintf(name, sizeof(name), "attributes_bar%d", i);
            TLImageInstance* image = FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
                mComponent->GetActiveSlide(), name);
            if (image != 0)
            {
                image->SetAssetColour(i <= value ? sAttributeBarFilledColour : sAttributeBarEmptyColour);
            }
        }
    }
    mValue = value;
}

FEAttributeBar::FEAttributeBar(TLComponentInstance* component)
    : mComponent(component)
{
    SetValue(0);
}

void FECharacterPDAComponent::SetVisible(bool visible)
{
    if (mRootComponent == 0)
    {
        return;
    }
    mRootComponent->m_bVisible = visible;
}

inline void FECharacterPDAComponent::ResetAttributes()
{
    if (mDisplayMode != 6 && mDisplayMode != -1)
    {
        for (int i = 0; i < 4; ++i)
        {
            mCaptainAttributeBars[i]->SetValue(0);
        }
        for (int i = 0; i < 4; ++i)
        {
            mSidekickAttributeBars[i]->SetValue(0);
        }
    }
}

void FECharacterPDAComponent::Update(float dt)
{
    mTimers.fn_803065F0(dt);
    bool animating = 2.0f + mRootComponent->GetActiveSlide()->GetCurrentTime() < mRootComponent->GetActiveSlide()->GetDuration();
    if (mSlideAnimating && !animating && mDisplayMode == 0)
    {
        mSlideAnimating = false;
        mCaptainNeedsRefresh = true;
        ResetAttributes();
        SetCaptainInfo(mSelectedCaptain, -1, 0);
    }
    mSlideAnimating = animating;
    mScrollText.m_scrollSpeed = 600.0f;
    if (mDisplayMode == 6 && mScrollBarNeedsReset)
    {
        mScrollText.Update(0.0167f);
        mScrollBar.SetComponent(FEFinder<TLComponentInstance, 4>::FindOrDefault(mRootComponent->GetActiveSlide(), "scrollbar"));
        mScrollBar.ResetScrolling();
        mScrollBar.SetRange(mScrollText.GetScrollSteps(0.0167f));
        mScrollBar.SetOffset(mRootComponent->GetAssetPosition());
        mScrollBar.SetValue(0);
        mScrollBarNeedsReset = false;
        if (!mScrollBar.mInitialized)
        {
            mScrollBar.Initialize();
        }
    }
    mUnidentified2A8 = false;
    for (int i = 0; i < 4; ++i)
    {
        if (mDisplayMode == 6 && mSide == 1 && i == gFEControllerIndex)
        {
            mUnidentified2A8 = false;
            bool valid = true;
            FEPointerEvent event;
            event.mIndex = i;
            event.mPosition = GetPointerPosition(i, (u8*)&valid);
            event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 0x1E, true, 0);
            event.mReleased = g_pFEInput->JustReleased((eFEINPUT_PAD)i, 0x1E, true, 0);
            mScrollBar.Update(event, dt);
            int direction = 0;
            if (mScrollBar.IsScrolling(1, true))
            {
                direction = 1;
            }
            else if (mScrollBar.IsScrolling(0, true))
            {
                direction = -1;
            }
            mScrollText.SetScrollDirection(direction);
            mScrollText.Update(0.0167f);
        }
    }
}

void FECharacterPDAComponent::StepAttributeBar(FETimer* timer, FEAttributeBar* component, int value)
{
    int current = component->mValue;
    if (current < value)
    {
        component->SetValue(current + 1);
    }
    else if (current > value)
    {
        component->SetValue(current - 1);
    }
    else
    {
        mTimers.fn_8030648C(timer);
    }
}

static void StartAttributeAnimation(FECharacterPDAComponent* owner, FEAttributeBar* component, float value);

void FECharacterPDAComponent::SetCaptainInfo(int captain, int, unsigned long flag)
{
    if (captain == -1)
    {
        mSelectedCaptain = captain;
        mPDAScreens->SetActiveSlide("empty", true, false);
        return;
    }
    mPDAScreens->SetActiveSlide("Slide1", false, false);
    if (flag == 1)
    {
        for (int i = 0; i < 7; ++i)
        {
            TintInstanceForCaptain(mCaptainAttributeImages[i], captain, 180);
        }
    }
    if ((mSelectedCaptain == captain || mRootComponent == 0) && !mCaptainNeedsRefresh)
    {
        return;
    }
    mSelectedCaptain = captain;
    mCaptainNeedsRefresh = false;
    if (mDisplayMode == 6)
    {
        mDescriptions->SetActiveSlide(GetTeamName((eTeamID)captain), false, false);
        mScrollText.SetScrollDirection(-1);
        mScrollText.ApplyNewTextInstancePointer(FEFinder<TLTextInstance, 3>::FindOrDefault(mPDAScreens, "descriptions", "text"), -1, -1, 0);
        BaseSceneHandler* scene = GameSceneManager::Instance()->GetScene(SCENE_CHOOSE_CAPTAINS_STRIKER_CUP);
        if (scene != 0)
        {
            TLSlide* slide = scene->mPresentation->GetActiveSlide();
            mScrollText.SetClippingTextInstance(FEFinder<TLTextInstance, 3>::FindOrDefault(slide, "Layer", "Description_clip"));
        }
        return;
    }
    mTimers.fn_80306524();
    const CharacterInfo& info = GetCharacterInfo(GetCharacterIndexFromCaptain(captain));
    if (flag == 1)
    {
        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mCaptainAttributes->GetActiveSlide(), "attributes_captains", "captains_pda");
        if (component != 0)
        {
            component->SetActiveSlide(GetTeamName((eTeamID)captain), true, false);
        }
    }
    mTimers.fn_80306524();
    if (!mSlideAnimating)
    {
        StartAttributeAnimation(this, mCaptainAttributeBars[0], info.unknown_0x38);
        StartAttributeAnimation(this, mCaptainAttributeBars[1], info.unknown_0x3C);
        StartAttributeAnimation(this, mCaptainAttributeBars[2], info.unknown_0x40);
        StartAttributeAnimation(this, mCaptainAttributeBars[3], info.unknown_0x44);
    }
    TLComponentInstance* overall = FEFinder<TLComponentInstance, 4>::FindOrDefault(mCaptainAttributes->GetActiveSlide(), "attributes_captains", "overall");
    if (overall != 0)
    {
        switch (info.unknown_0x48)
        {
        case 0:
            overall->SetActiveSlide("offensive", true, false);
            break;
        case 1:
            overall->SetActiveSlide("defensive", true, false);
            break;
        case 2:
            overall->SetActiveSlide("playmaker", true, false);
            break;
        case 3:
            overall->SetActiveSlide("power", true, false);
            break;
        case 4:
            overall->SetActiveSlide("balanced", true, false);
            break;
        }
    }
    TLComponentInstance* names = FEFinder<TLComponentInstance, 4>::FindOrDefault(mCaptainAttributes->GetActiveSlide(), "attributes_captains", "names");
    names->SetActiveSlide(GetTeamName((eTeamID)captain), false, false);
}

static void StartAttributeAnimation(FECharacterPDAComponent* owner, FEAttributeBar* component, float value)
{
    Function<FETimer*> callback(Bind<void>(MemFun(&FECharacterPDAComponent::StepAttributeBar), owner, placeholder0, component, (int)(0.5 + value * 10.0f)));
    owner->mTimers.fn_8030639C(0.07f, callback);
}

void FECharacterPDAComponent::SetSidekickInfo(int sidekick, int, unsigned long)
{
    if ((mSelectedSidekick == sidekick || mRootComponent == 0) && !mSidekickNeedsRefresh)
    {
        return;
    }
    mSidekickNeedsRefresh = false;
    mTimers.fn_80306524();
    mSelectedSidekick = sidekick;
    if (sidekick == -1)
    {
        for (int i = 0; i < 4; ++i)
        {
            mSidekickAttributeBars[i]->SetValue(0);
        }
        return;
    }
    const CharacterInfo& info = GetCharacterInfo(GetCharacterIndexFromSidekick(sidekick));
    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mSidekickAttributes->GetActiveSlide(), "attributes_sidekicks", "sidekicks_pda");
    if (component != 0)
    {
        component->SetActiveSlide(GetSidekickName((eSidekickID)sidekick), true, false);
    }
    mTimers.fn_80306524();
    StartAttributeAnimation(this, mSidekickAttributeBars[0], info.unknown_0x38);
    StartAttributeAnimation(this, mSidekickAttributeBars[1], info.unknown_0x3C);
    StartAttributeAnimation(this, mSidekickAttributeBars[2], info.unknown_0x40);
    StartAttributeAnimation(this, mSidekickAttributeBars[3], info.unknown_0x44);
    TLComponentInstance* overall = FEFinder<TLComponentInstance, 4>::FindOrDefault(mSidekickAttributes->GetActiveSlide(), "attributes_sidekicks", "overall");
    if (overall != 0)
    {
        switch (info.unknown_0x48)
        {
        case 0:
            overall->SetActiveSlide("offensive", true, false);
            break;
        case 1:
            overall->SetActiveSlide("defensive", true, false);
            break;
        case 2:
            overall->SetActiveSlide("playmaker", true, false);
            break;
        case 3:
            overall->SetActiveSlide("power", true, false);
            break;
        case 4:
            overall->SetActiveSlide("balanced", true, false);
            break;
        }
    }
    TLComponentInstance* names = FEFinder<TLComponentInstance, 4>::FindOrDefault(mSidekickAttributes->GetActiveSlide(), "attributes_sidekicks", "names");
    if (names != 0)
    {
        names->SetActiveSlide(GetSidekickName((eSidekickID)sidekick), true, false);
    }
    for (int i = 0; i < 7; ++i)
    {
        if (GameInfoManager::Instance()->IsInMode3())
        {
            TintInstanceForCaptain(mSidekickAttributeImages[i], g_pCupManager->unknown_0x8A28, 180);
        }
        else
        {
            TintInstanceForCaptain(mSidekickAttributeImages[i], GameInfoManager::Instance()->GetTeam((short)mSide), 180);
        }
    }
}

void FECharacterPDAComponent::SetDisplayMode(int value)
{
    if (value == mDisplayMode)
    {
        if (value == 6 && mSide == 1)
        {
            mScrollBarNeedsReset = true;
        }
        return;
    }
    if (value == 0 && mDisplayMode == 6)
    {
        mScrollBar.ResetScrolling();
        mScrollBar.SetRange(0);
        mScrollBar.SetOffset(mRootComponent->GetAssetPosition());
        mScrollBar.SetValue(0);
        mScrollBarNeedsReset = false;
    }
    mDisplayMode = value;
    ResetAttributes();
    mCaptainNeedsRefresh = true;
    mSidekickNeedsRefresh = true;
    mDescriptions->m_bVisible = false;
    mScrollArrows->m_bVisible = false;
    mContinueGroup->m_bVisible = false;
    switch (mDisplayMode)
    {
    case 0:
        if (mCaptainAttributes != 0)
        {
            mCaptainAttributes->m_bVisible = true;
        }
        if (mPositions != 0)
        {
            mPositions->m_bVisible = false;
        }
        if (mJoinPrompt != 0)
        {
            mJoinPrompt->m_bVisible = false;
        }
        if (mPositionsGroup != 0)
        {
            mPositionsGroup->m_bVisible = false;
        }
        if (mSidekickAttributes != 0)
        {
            mSidekickAttributes->m_bVisible = false;
        }
        if (mTeamLogos != 0)
        {
            mTeamLogos->m_bVisible = false;
        }
        SetReadyPromptVisible(false);
        break;
    case 1:
        if (mCaptainAttributes != 0)
        {
            mCaptainAttributes->m_bVisible = false;
        }
        if (mPositions != 0)
        {
            mPositions->m_bVisible = true;
        }
        if (mJoinPrompt != 0)
        {
            mJoinPrompt->m_bVisible = false;
        }
        if (mPositionsGroup != 0)
        {
            mPositionsGroup->m_bVisible = true;
        }
        if (mSidekickAttributes != 0)
        {
            mSidekickAttributes->m_bVisible = false;
        }
        if (mTeamLogos != 0)
        {
            mTeamLogos->m_bVisible = true;
        }
        SetReadyPromptVisible(false);
        break;
    case 2:
        if (mCaptainAttributes != 0)
        {
            mCaptainAttributes->m_bVisible = false;
        }
        if (mPositions != 0)
        {
            mPositions->m_bVisible = true;
        }
        if (mJoinPrompt != 0)
        {
            mJoinPrompt->m_bVisible = false;
        }
        if (mPositionsGroup != 0)
        {
            mPositionsGroup->m_bVisible = false;
        }
        if (mSidekickAttributes != 0)
        {
            mSidekickAttributes->m_bVisible = true;
        }
        if (mTeamLogos != 0)
        {
            mTeamLogos->m_bVisible = true;
        }
        SetReadyPromptVisible(false);
        break;
    case 3:
        if (mCaptainAttributes != 0)
        {
            mCaptainAttributes->m_bVisible = false;
        }
        if (mPositions != 0)
        {
            mPositions->m_bVisible = true;
        }
        if (mJoinPrompt != 0)
        {
            mJoinPrompt->m_bVisible = false;
        }
        if (mPositionsGroup != 0)
        {
            mPositionsGroup->m_bVisible = true;
        }
        if (mSidekickAttributes != 0)
        {
            mSidekickAttributes->m_bVisible = true;
        }
        if (mTeamLogos != 0)
        {
            mTeamLogos->m_bVisible = true;
        }
        SetReadyPromptVisible(false);
        break;
    case 4:
        if (mCaptainAttributes != 0)
        {
            mCaptainAttributes->m_bVisible = false;
        }
        if (mPositions != 0)
        {
            mPositions->m_bVisible = false;
        }
        if (mJoinPrompt != 0)
        {
            mJoinPrompt->m_bVisible = true;
        }
        if (mPositionsGroup != 0)
        {
            mPositionsGroup->m_bVisible = false;
        }
        if (mSidekickAttributes != 0)
        {
            mSidekickAttributes->m_bVisible = false;
        }
        if (mTeamLogos != 0)
        {
            mTeamLogos->m_bVisible = false;
        }
        SetReadyPromptVisible(false);
        break;
    case -1:
        if (mCaptainAttributes != 0)
        {
            mCaptainAttributes->m_bVisible = false;
        }
        if (mPositions != 0)
        {
            mPositions->m_bVisible = false;
        }
        if (mJoinPrompt != 0)
        {
            mJoinPrompt->m_bVisible = false;
        }
        if (mPositionsGroup != 0)
        {
            mPositionsGroup->m_bVisible = false;
        }
        if (mSidekickAttributes != 0)
        {
            mSidekickAttributes->m_bVisible = false;
        }
        if (mTeamLogos != 0)
        {
            mTeamLogos->m_bVisible = false;
        }
        SetReadyPromptVisible(false);
        break;
    case 5:
        if (mCaptainAttributes != 0)
        {
            mCaptainAttributes->m_bVisible = false;
        }
        if (mPositions != 0)
        {
            mPositions->m_bVisible = false;
        }
        if (mJoinPrompt != 0)
        {
            mJoinPrompt->m_bVisible = false;
        }
        if (mPositionsGroup != 0)
        {
            mPositionsGroup->m_bVisible = false;
        }
        if (mSidekickAttributes != 0)
        {
            mSidekickAttributes->m_bVisible = false;
        }
        if (mTeamLogos != 0)
        {
            mTeamLogos->m_bVisible = false;
        }
        SetReadyPromptVisible(false);
        break;
    case 6:
        if (mCaptainAttributes != 0)
        {
            mCaptainAttributes->m_bVisible = false;
        }
        if (mPositions != 0)
        {
            mPositions->m_bVisible = false;
        }
        if (mJoinPrompt != 0)
        {
            mJoinPrompt->m_bVisible = false;
        }
        if (mPositionsGroup != 0)
        {
            mPositionsGroup->m_bVisible = false;
        }
        if (mSidekickAttributes != 0)
        {
            mSidekickAttributes->m_bVisible = false;
        }
        if (mTeamLogos != 0)
        {
            mTeamLogos->m_bVisible = false;
        }
        SetReadyPromptVisible(false);
        mDescriptions->m_bVisible = true;
        mScrollArrows->m_bVisible = true;
        if (mSide == 1)
        {
            mScrollBarNeedsReset = true;
        }
        break;
    }
}

void FECharacterPDAComponent::ShowSlideIn()
{
    mRootComponent->SetActiveSlide(SLIDE_IN, true, false);
    if (mRootComponent != 0)
    {
        mRootComponent->m_bVisible = true;
    }
}

void FECharacterPDAComponent::SetReadyPromptVisible(bool visible)
{
    if (mReadyPrompt != 0 && visible != mReadyPrompt->m_bVisible)
    {
        mReadyPrompt->m_bVisible = visible;
        if (visible)
        {
            TLSlide* slide = mReadyPrompt->GetActiveSlide();
            slide->m_time = 0.0f;
            mReadyPrompt->GetActiveSlide()->Update(0.0f);
        }
    }
}

void FECharacterPDAComponent::ApplyCaptainColours(int captain, int opponent)
{
    const CharacterInfo& teamInfo = GetCharacterInfo(GetCharacterIndexFromCaptain(captain));
    const CharacterInfo& opponentInfo = GetCharacterInfo(GetCharacterIndexFromCaptain(opponent));
    nlColour teamColour;
    teamColour = GetTeamColour(teamInfo, opponentInfo, true);
    bool alternate = NeedsAlternateColour(teamInfo, opponentInfo);
    nlColour assetColour;
    nlColourSet(assetColour, teamColour.c[0], teamColour.c[1], teamColour.c[2], 180);
    for (int i = 0; i < 7; ++i)
    {
        mCaptainAttributeImages[i]->SetAssetColour(assetColour);
    }

    if (captain != -1)
    {
        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find<>(
            mCaptainAttributes->GetActiveSlide(), "attributes_captains", "captains_pda");
        if (alternate)
        {
            char slide[20];
            nlSNPrintf(slide, sizeof(slide), "%s_alt", GetTeamName((eTeamID)captain));
            component->SetActiveSlide(slide, true, false);
        }
        else
        {
            component->SetActiveSlide(GetTeamName((eTeamID)captain), true, false);
        }
    }
}

void FECharacterPDAComponent::TintInstanceForCaptain(TLInstance* instance, int captain, unsigned char alpha)
{
    nlColour colour;
    if (mDisplayMode == 3 && GameInfoManager::Instance()->IsInFriendlyMode() && !GameInfoManager::Instance()->mIsOnlineMode)
    {
        int team0 = GameInfoManager::Instance()->GetTeam(0);
        int team1 = GameInfoManager::Instance()->GetTeam(1);
        const CharacterInfo& info0 = GetCharacterInfo(GetCharacterIndexFromCaptain(team0));
        const CharacterInfo& info1 = GetCharacterInfo(GetCharacterIndexFromCaptain(team1));
        if (captain == team0)
        {
            colour = GetTeamColour(info0, info1, true);
        }
        else
        {
            colour = GetTeamColour(info1, info0, true);
        }
    }
    else
    {
        const CharacterInfo& info = GetCharacterInfo(GetCharacterIndexFromCaptain(captain));
        const CharacterInfo& opponent = GetCharacterInfo(GetCharacterIndexFromCaptain(0));
        colour = GetTeamColour(info, opponent, false);
    }

    nlColour assetColour = { colour.c[0], colour.c[1], colour.c[2], alpha };
    if (instance != 0)
    {
        instance->SetAssetColour(assetColour);
    }
}

void FECharacterPDAComponent::SetCupIconsVisible(bool fire, bool crystal, bool striker)
{
    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mPDAScreens->GetActiveSlide(), "Fire_cup_icon");
    component->m_bVisible = fire;
    component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mPDAScreens->GetActiveSlide(), "Crystal_cup_icon");
    component->m_bVisible = crystal;
    component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mPDAScreens->GetActiveSlide(), "Striker_cup_icon");
    component->m_bVisible = striker;
}

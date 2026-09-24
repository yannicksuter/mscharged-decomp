#include "Game/FE/feCaptainComponent.h"

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

TU801DCD9CComponent::TU801DCD9CComponent()
    : mScrollText(1)
    , mUnidentified74(0)
    , mUnidentified78(0)
    , mUnidentified7C(0)
    , mUnidentified80(0)
    , mUnidentified84(0)
    , mUnidentified290(2)
    , mUnidentified294(-1)
    , mUnidentified29C(-1)
{
    mUnidentified2A4 = false;
    mUnidentified2A5 = false;
    mUnidentified2A7 = false;
    mUnidentified2A6 = true;
    for (int i = 0; i < 4; ++i)
    {
        mUnidentified04[i] = 0;
    }
    for (int i = 0; i < 4; ++i)
    {
        mUnidentified14[i] = 0;
    }
    mScrollText.SetEndBehavior(2);
    mScrollText.SetScrollMode(1);
    mUnidentified2A8 = false;
}

TU801DCD9CComponent::~TU801DCD9CComponent()
{
    for (int i = 0; i < 4; ++i)
    {
        delete mUnidentified04[i];
    }
    for (int i = 0; i < 4; ++i)
    {
        delete mUnidentified14[i];
    }
}

void TU801DCD9CComponent::fn_801DCFD8(TLComponentInstance* component, int side, u32 value)
{
    static const char* captainAttributes[] = { "attribute_movement", "attribute_shooting", "attribute_passing", "attribute_defense" };
    static const char* sidekickAttributes[] = { "attribute_movement", "attribute_shooting", "attribute_passing", "attribute_defense" };

    mUnidentified74 = component;
    mUnidentified24 = value;
    mUnidentified2A0 = side;
    mUnidentified78 = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mUnidentified74->GetActiveSlide(), "pda_screens");
    mUnidentified7C = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mUnidentified78->GetActiveSlide(), "attributes_captains");
    mUnidentified80 = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mUnidentified78->GetActiveSlide(), "positions");
    mUnidentified84 = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mUnidentified78->GetActiveSlide(), "A to join");
    mUnidentified88 = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mUnidentified78->GetActiveSlide(), "ready_prompt");
    mUnidentified90 = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mUnidentified74->GetActiveSlide(), "attributes_sidekicks");
    mUnidentified8C = FEFinder<UnidentifiedTLGroupInstance, TLAT_GROUP>::FindOrDefault(mUnidentified80->GetActiveSlide(), "positions");
    mUnidentified94 = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mUnidentified80->GetActiveSlide(), "team_logos");

    mUnidentified98[0] = FEFinder<TLImageInstance, 2>::FindOrDefault(mUnidentified90->GetActiveSlide(), "attributes_sidekicks", "white_8x8");
    mUnidentified98[1] = FEFinder<TLImageInstance, 2>::FindOrDefault(mUnidentified90->GetActiveSlide(), "attributes_sidekicks", "white_8x9");
    mUnidentified98[2] = FEFinder<TLImageInstance, 2>::FindOrDefault(mUnidentified90->GetActiveSlide(), "attributes_sidekicks", "movement2");
    mUnidentified98[3] = FEFinder<TLImageInstance, 2>::FindOrDefault(mUnidentified90->GetActiveSlide(), "attributes_sidekicks", "shooting2");
    mUnidentified98[4] = FEFinder<TLImageInstance, 2>::FindOrDefault(mUnidentified90->GetActiveSlide(), "attributes_sidekicks", "passing2");
    mUnidentified98[5] = FEFinder<TLImageInstance, 2>::FindOrDefault(mUnidentified90->GetActiveSlide(), "attributes_sidekicks", "defense2");
    mUnidentified98[6] = FEFinder<TLImageInstance, 2>::FindOrDefault(mUnidentified90->GetActiveSlide(), "attributes_sidekicks", "white_8x11");
    mUnidentifiedB4[0] = FEFinder<TLImageInstance, 2>::FindOrDefault(mUnidentified7C->GetActiveSlide(), "attributes_captains", "white_8x8");
    mUnidentifiedB4[1] = FEFinder<TLImageInstance, 2>::FindOrDefault(mUnidentified7C->GetActiveSlide(), "attributes_captains", "white_8x9");
    mUnidentifiedB4[2] = FEFinder<TLImageInstance, 2>::FindOrDefault(mUnidentified7C->GetActiveSlide(), "attributes_captains", "movement2");
    mUnidentifiedB4[3] = FEFinder<TLImageInstance, 2>::FindOrDefault(mUnidentified7C->GetActiveSlide(), "attributes_captains", "shooting2");
    mUnidentifiedB4[4] = FEFinder<TLImageInstance, 2>::FindOrDefault(mUnidentified7C->GetActiveSlide(), "attributes_captains", "passing2");
    mUnidentifiedB4[5] = FEFinder<TLImageInstance, 2>::FindOrDefault(mUnidentified7C->GetActiveSlide(), "attributes_captains", "defense2");
    mUnidentifiedB4[6] = FEFinder<TLImageInstance, 2>::FindOrDefault(mUnidentified7C->GetActiveSlide(), "attributes_captains", "white_8x11");

    for (int i = 0; i < 4; ++i)
    {
        mUnidentified04[i] = new (8, false) TU801DE42CComponent(
            (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mUnidentified7C->GetActiveSlide(), "attributes_captains", captainAttributes[i]));
    }
    for (int i = 0; i < 4; ++i)
    {
        mUnidentified14[i] = new (8, false) TU801DE42CComponent(
            (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mUnidentified90->GetActiveSlide(), "attributes_sidekicks", sidekickAttributes[i]));
    }

    if (side == 0)
    {
        TLTextInstance* captain = FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified7C->GetActiveSlide(), "attributes_captains", "CAPTAIN");
        TLTextInstance* captainAway = FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified7C->GetActiveSlide(), "attributes_captains", "AWAY");
        TLTextInstance* team = FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified80->GetActiveSlide(), "positions", "TEAM");
        TLTextInstance* teamAway = FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified80->GetActiveSlide(), "positions", "AWAY");
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

    mUnidentifiedD0 = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mUnidentified78->GetActiveSlide(), "descriptions");
    mUnidentifiedD4 = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(mUnidentified78->GetActiveSlide(), "scroll arrows");
    mUnidentifiedD8 = FEFinder<UnidentifiedTLGroupInstance, TLAT_GROUP>::FindOrDefault(mUnidentified78->GetActiveSlide(), "cONTINUE");
    fn_801DE570(false);
    fn_801E0F14(false, false, false);
}

static const nlColour sAttributeBarEmptyColour = { 0, 0, 0, 160 };

static const nlColour sAttributeBarFilledColour = { 169, 208, 70, 255 };

inline void TU801DE42CComponent::SetValue(int value)
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
    mUnidentified00 = value;
}

TU801DE42CComponent::TU801DE42CComponent(TLComponentInstance* component)
    : mComponent(component)
{
    SetValue(0);
}

void TU801DCD9CComponent::fn_801DE570(bool visible)
{
    if (mUnidentified74 == 0)
    {
        return;
    }
    mUnidentified74->m_bVisible = visible;
}

inline void TU801DCD9CComponent::ResetAttributes()
{
    if (mUnidentified29C != 6 && mUnidentified29C != -1)
    {
        for (int i = 0; i < 4; ++i)
        {
            mUnidentified04[i]->SetValue(0);
        }
        for (int i = 0; i < 4; ++i)
        {
            mUnidentified14[i]->SetValue(0);
        }
    }
}

void TU801DCD9CComponent::fn_801DE584(float dt)
{
    mTimers.fn_803065F0(dt);
    bool animating = 2.0f + mUnidentified74->GetActiveSlide()->GetCurrentTime() < mUnidentified74->GetActiveSlide()->GetDuration();
    if (mUnidentified2A6 && !animating && mUnidentified29C == 0)
    {
        mUnidentified2A6 = false;
        mUnidentified2A4 = true;
        ResetAttributes();
        fn_801DEDD0(mUnidentified294, -1, 0);
    }
    mUnidentified2A6 = animating;
    mScrollText.m_scrollSpeed = 600.0f;
    if (mUnidentified29C == 6 && mUnidentified2A7)
    {
        mScrollText.Update(0.0167f);
        mScrollBar.SetComponent(FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified74->GetActiveSlide(), "scrollbar"));
        mScrollBar.ResetScrolling();
        mScrollBar.SetRange(mScrollText.GetScrollSteps(0.0167f));
        mScrollBar.SetOffset(mUnidentified74->GetAssetPosition());
        mScrollBar.SetValue(0);
        mUnidentified2A7 = false;
        if (!mScrollBar.mInitialized)
        {
            mScrollBar.Initialize();
        }
    }
    mUnidentified2A8 = false;
    for (int i = 0; i < 4; ++i)
    {
        if (mUnidentified29C == 6 && mUnidentified2A0 == 1 && i == gFEControllerIndex)
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

void TU801DCD9CComponent::fn_801DEB50(FETimer* timer, TU801DE42CComponent* component, int value)
{
    int current = component->mUnidentified00;
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

static void StartAttributeAnimation(TU801DCD9CComponent* owner, TU801DE42CComponent* component, float value);

void TU801DCD9CComponent::fn_801DEDD0(int captain, int, unsigned long flag)
{
    if (captain == -1)
    {
        mUnidentified294 = captain;
        mUnidentified78->SetActiveSlide("empty", true, false);
        return;
    }
    mUnidentified78->SetActiveSlide("Slide1", false, false);
    if (flag == 1)
    {
        for (int i = 0; i < 7; ++i)
        {
            fn_801E0D8C(mUnidentifiedB4[i], captain, 180);
        }
    }
    if ((mUnidentified294 == captain || mUnidentified74 == 0) && !mUnidentified2A4)
    {
        return;
    }
    mUnidentified294 = captain;
    mUnidentified2A4 = false;
    if (mUnidentified29C == 6)
    {
        mUnidentifiedD0->SetActiveSlide(GetTeamName((eTeamID)captain), false, false);
        mScrollText.SetScrollDirection(-1);
        mScrollText.ApplyNewTextInstancePointer(FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified78, "descriptions", "text"), -1, -1, 0);
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
        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified7C->GetActiveSlide(), "attributes_captains", "captains_pda");
        if (component != 0)
        {
            component->SetActiveSlide(GetTeamName((eTeamID)captain), true, false);
        }
    }
    mTimers.fn_80306524();
    if (!mUnidentified2A6)
    {
        StartAttributeAnimation(this, mUnidentified04[0], info.unknown_0x38);
        StartAttributeAnimation(this, mUnidentified04[1], info.unknown_0x3C);
        StartAttributeAnimation(this, mUnidentified04[2], info.unknown_0x40);
        StartAttributeAnimation(this, mUnidentified04[3], info.unknown_0x44);
    }
    TLComponentInstance* overall = FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified7C->GetActiveSlide(), "attributes_captains", "overall");
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
    TLComponentInstance* names = FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified7C->GetActiveSlide(), "attributes_captains", "names");
    names->SetActiveSlide(GetTeamName((eTeamID)captain), false, false);
}

static void StartAttributeAnimation(TU801DCD9CComponent* owner, TU801DE42CComponent* component, float value)
{
    Function<FETimer*> callback(Bind<void>(MemFun(&TU801DCD9CComponent::fn_801DEB50), owner, placeholder0, component, (int)(0.5 + value * 10.0f)));
    owner->mTimers.fn_8030639C(0.07f, callback);
}

void TU801DCD9CComponent::fn_801DF85C(int sidekick, int, unsigned long)
{
    if ((mUnidentified298 == sidekick || mUnidentified74 == 0) && !mUnidentified2A5)
    {
        return;
    }
    mUnidentified2A5 = false;
    mTimers.fn_80306524();
    mUnidentified298 = sidekick;
    if (sidekick == -1)
    {
        for (int i = 0; i < 4; ++i)
        {
            mUnidentified14[i]->SetValue(0);
        }
        return;
    }
    const CharacterInfo& info = GetCharacterInfo(GetCharacterIndexFromSidekick(sidekick));
    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified90->GetActiveSlide(), "attributes_sidekicks", "sidekicks_pda");
    if (component != 0)
    {
        component->SetActiveSlide(GetSidekickName((eSidekickID)sidekick), true, false);
    }
    mTimers.fn_80306524();
    StartAttributeAnimation(this, mUnidentified14[0], info.unknown_0x38);
    StartAttributeAnimation(this, mUnidentified14[1], info.unknown_0x3C);
    StartAttributeAnimation(this, mUnidentified14[2], info.unknown_0x40);
    StartAttributeAnimation(this, mUnidentified14[3], info.unknown_0x44);
    TLComponentInstance* overall = FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified90->GetActiveSlide(), "attributes_sidekicks", "overall");
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
    TLComponentInstance* names = FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified90->GetActiveSlide(), "attributes_sidekicks", "names");
    if (names != 0)
    {
        names->SetActiveSlide(GetSidekickName((eSidekickID)sidekick), true, false);
    }
    for (int i = 0; i < 7; ++i)
    {
        if (GameInfoManager::Instance()->IsInMode3())
        {
            fn_801E0D8C(mUnidentified98[i], g_pCupManager->unknown_0x8A28, 180);
        }
        else
        {
            fn_801E0D8C(mUnidentified98[i], GameInfoManager::Instance()->GetTeam((short)mUnidentified2A0), 180);
        }
    }
}

void TU801DCD9CComponent::fn_801E0280(int value)
{
    if (value == mUnidentified29C)
    {
        if (value == 6 && mUnidentified2A0 == 1)
        {
            mUnidentified2A7 = true;
        }
        return;
    }
    if (value == 0 && mUnidentified29C == 6)
    {
        mScrollBar.ResetScrolling();
        mScrollBar.SetRange(0);
        mScrollBar.SetOffset(mUnidentified74->GetAssetPosition());
        mScrollBar.SetValue(0);
        mUnidentified2A7 = false;
    }
    mUnidentified29C = value;
    ResetAttributes();
    mUnidentified2A4 = true;
    mUnidentified2A5 = true;
    mUnidentifiedD0->m_bVisible = false;
    mUnidentifiedD4->m_bVisible = false;
    mUnidentifiedD8->m_bVisible = false;
    switch (mUnidentified29C)
    {
    case 0:
        if (mUnidentified7C != 0)
        {
            mUnidentified7C->m_bVisible = true;
        }
        if (mUnidentified80 != 0)
        {
            mUnidentified80->m_bVisible = false;
        }
        if (mUnidentified84 != 0)
        {
            mUnidentified84->m_bVisible = false;
        }
        if (mUnidentified8C != 0)
        {
            mUnidentified8C->m_bVisible = false;
        }
        if (mUnidentified90 != 0)
        {
            mUnidentified90->m_bVisible = false;
        }
        if (mUnidentified94 != 0)
        {
            mUnidentified94->m_bVisible = false;
        }
        fn_801E0B20(false);
        break;
    case 1:
        if (mUnidentified7C != 0)
        {
            mUnidentified7C->m_bVisible = false;
        }
        if (mUnidentified80 != 0)
        {
            mUnidentified80->m_bVisible = true;
        }
        if (mUnidentified84 != 0)
        {
            mUnidentified84->m_bVisible = false;
        }
        if (mUnidentified8C != 0)
        {
            mUnidentified8C->m_bVisible = true;
        }
        if (mUnidentified90 != 0)
        {
            mUnidentified90->m_bVisible = false;
        }
        if (mUnidentified94 != 0)
        {
            mUnidentified94->m_bVisible = true;
        }
        fn_801E0B20(false);
        break;
    case 2:
        if (mUnidentified7C != 0)
        {
            mUnidentified7C->m_bVisible = false;
        }
        if (mUnidentified80 != 0)
        {
            mUnidentified80->m_bVisible = true;
        }
        if (mUnidentified84 != 0)
        {
            mUnidentified84->m_bVisible = false;
        }
        if (mUnidentified8C != 0)
        {
            mUnidentified8C->m_bVisible = false;
        }
        if (mUnidentified90 != 0)
        {
            mUnidentified90->m_bVisible = true;
        }
        if (mUnidentified94 != 0)
        {
            mUnidentified94->m_bVisible = true;
        }
        fn_801E0B20(false);
        break;
    case 3:
        if (mUnidentified7C != 0)
        {
            mUnidentified7C->m_bVisible = false;
        }
        if (mUnidentified80 != 0)
        {
            mUnidentified80->m_bVisible = true;
        }
        if (mUnidentified84 != 0)
        {
            mUnidentified84->m_bVisible = false;
        }
        if (mUnidentified8C != 0)
        {
            mUnidentified8C->m_bVisible = true;
        }
        if (mUnidentified90 != 0)
        {
            mUnidentified90->m_bVisible = true;
        }
        if (mUnidentified94 != 0)
        {
            mUnidentified94->m_bVisible = true;
        }
        fn_801E0B20(false);
        break;
    case 4:
        if (mUnidentified7C != 0)
        {
            mUnidentified7C->m_bVisible = false;
        }
        if (mUnidentified80 != 0)
        {
            mUnidentified80->m_bVisible = false;
        }
        if (mUnidentified84 != 0)
        {
            mUnidentified84->m_bVisible = true;
        }
        if (mUnidentified8C != 0)
        {
            mUnidentified8C->m_bVisible = false;
        }
        if (mUnidentified90 != 0)
        {
            mUnidentified90->m_bVisible = false;
        }
        if (mUnidentified94 != 0)
        {
            mUnidentified94->m_bVisible = false;
        }
        fn_801E0B20(false);
        break;
    case -1:
        if (mUnidentified7C != 0)
        {
            mUnidentified7C->m_bVisible = false;
        }
        if (mUnidentified80 != 0)
        {
            mUnidentified80->m_bVisible = false;
        }
        if (mUnidentified84 != 0)
        {
            mUnidentified84->m_bVisible = false;
        }
        if (mUnidentified8C != 0)
        {
            mUnidentified8C->m_bVisible = false;
        }
        if (mUnidentified90 != 0)
        {
            mUnidentified90->m_bVisible = false;
        }
        if (mUnidentified94 != 0)
        {
            mUnidentified94->m_bVisible = false;
        }
        fn_801E0B20(false);
        break;
    case 5:
        if (mUnidentified7C != 0)
        {
            mUnidentified7C->m_bVisible = false;
        }
        if (mUnidentified80 != 0)
        {
            mUnidentified80->m_bVisible = false;
        }
        if (mUnidentified84 != 0)
        {
            mUnidentified84->m_bVisible = false;
        }
        if (mUnidentified8C != 0)
        {
            mUnidentified8C->m_bVisible = false;
        }
        if (mUnidentified90 != 0)
        {
            mUnidentified90->m_bVisible = false;
        }
        if (mUnidentified94 != 0)
        {
            mUnidentified94->m_bVisible = false;
        }
        fn_801E0B20(false);
        break;
    case 6:
        if (mUnidentified7C != 0)
        {
            mUnidentified7C->m_bVisible = false;
        }
        if (mUnidentified80 != 0)
        {
            mUnidentified80->m_bVisible = false;
        }
        if (mUnidentified84 != 0)
        {
            mUnidentified84->m_bVisible = false;
        }
        if (mUnidentified8C != 0)
        {
            mUnidentified8C->m_bVisible = false;
        }
        if (mUnidentified90 != 0)
        {
            mUnidentified90->m_bVisible = false;
        }
        if (mUnidentified94 != 0)
        {
            mUnidentified94->m_bVisible = false;
        }
        fn_801E0B20(false);
        mUnidentifiedD0->m_bVisible = true;
        mUnidentifiedD4->m_bVisible = true;
        if (mUnidentified2A0 == 1)
        {
            mUnidentified2A7 = true;
        }
        break;
    }
}

void TU801DCD9CComponent::fn_801E0AD0()
{
    mUnidentified74->SetActiveSlide("in", true, false);
    if (mUnidentified74 != 0)
    {
        mUnidentified74->m_bVisible = true;
    }
}

void TU801DCD9CComponent::fn_801E0B20(bool visible)
{
    if (mUnidentified88 != 0 && visible != mUnidentified88->m_bVisible)
    {
        mUnidentified88->m_bVisible = visible;
        if (visible)
        {
            TLSlide* slide = mUnidentified88->GetActiveSlide();
            slide->m_time = 0.0f;
            mUnidentified88->GetActiveSlide()->Update(0.0f);
        }
    }
}

void TU801DCD9CComponent::fn_801E0B8C(int captain, int opponent)
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
        mUnidentifiedB4[i]->SetAssetColour(assetColour);
    }

    if (captain != -1)
    {
        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find<>(
            mUnidentified7C->GetActiveSlide(), "attributes_captains", "captains_pda");
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

void TU801DCD9CComponent::fn_801E0D8C(TLInstance* instance, int captain, unsigned char alpha)
{
    nlColour colour;
    if (mUnidentified29C == 3 && GameInfoManager::Instance()->IsInFriendlyMode() && !GameInfoManager::Instance()->mIsOnlineMode)
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

void TU801DCD9CComponent::fn_801E0F14(bool fire, bool crystal, bool striker)
{
    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified78->GetActiveSlide(), "Fire_cup_icon");
    component->m_bVisible = fire;
    component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified78->GetActiveSlide(), "Crystal_cup_icon");
    component->m_bVisible = crystal;
    component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified78->GetActiveSlide(), "Striker_cup_icon");
    component->m_bVisible = striker;
}

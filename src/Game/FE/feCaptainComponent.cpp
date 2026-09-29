#include "Game/FE/feCaptainComponent.h"

#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/fePresentation.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/BaseSceneHandler.inl"
#include "Game/GameSceneManager.h"
#include "Game/GameInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/CharacterInfo.h"
#include "NL/nlMath.h"
#include "NL/nlString.h"

FECaptainComponent::FECaptainComponent()
    : mComponent(0)
    , mPositions(0)
    , mCaptain(-1)
{
}

FECaptainComponent::~FECaptainComponent()
{
}

void FECaptainComponent::Initialize(TLComponentInstance* component, int side)
{
    mSide = side;
    mComponent = component;
    mPositions = 0;
    if (mComponent != 0)
    {
        TLComponentInstance* screens = FEFinder<TLComponentInstance, 4>::Find<>(mComponent->GetActiveSlide(), "pda_screens");
        mPositions = FEFinder<TLComponentInstance, 4>::Find<>(screens->GetActiveSlide(), "positions");
    }
    if (mPositions != 0)
    {
        TLComponentInstance* indicator = FEFinder<TLComponentInstance, 4>::FindOrDefault(mPositions, "sidekick_indicator");
        indicator->m_bVisible = false;
        TLInstance* highlight = FEFinder<TLInstance, 1>::Find<>(
            mPositions->GetActiveSlide(), "positions", "field_positions", "idle", "highlight");
        highlight->m_bVisible = false;
    }

    TLComponentInstance* logos = FEFinder<TLComponentInstance, 4>::Find<>(mPositions->GetActiveSlide(), "team_logos");
    int team;
    if (GameInfoManager::Instance()->IsInMode3())
    {
        GameRules rules = g_pCupManager->mPendingCupSidekicks;
        mSidekicks[0] = rules.mValues[0];
        mSidekicks[1] = rules.mValues[1];
        mSidekicks[2] = rules.mValues[2];
        team = g_pCupManager->mPendingCupTeam;
    }
    else
    {
        mSidekicks[0] = GameInfoManager::Instance()->GetSidekick((short)side, 0);
        mSidekicks[1] = GameInfoManager::Instance()->GetSidekick((short)side, 1);
        mSidekicks[2] = GameInfoManager::Instance()->GetSidekick((short)side, 2);
        team = GameInfoManager::Instance()->GetTeam((short)side);
    }
    char name[16];
    nlSNPrintf(name, sizeof(name), "%s", GetCharacterInfo(GetCharacterIndexFromCaptain(team)).mName);
    logos->SetActiveSlide(name, true, false);
    TLInstance* background = FEFinder<TLInstance, 1>::Find<>(mPositions->GetActiveSlide(), "positions", "white_8x8");
    ApplyTeamColour(background, team, 180);
    SetRecycleState(0, 1);
    SetRecycleState(1, 1);
    SetRecycleState(2, 1);
}

void FECaptainComponent::ApplyTeamColour(TLInstance* instance, int captain, unsigned char alpha)
{
    nlColour colour;
    if (GameInfoManager::Instance()->IsInMode3() || GameInfoManager::Instance()->mIsOnlineMode)
    {
        const CharacterInfo& info = GetCharacterInfo(GetCharacterIndexFromCaptain(captain));
        const CharacterInfo& opponent = GetCharacterInfo(GetCharacterIndexFromCaptain(0));
        colour = GetTeamColour(info, opponent, false);
    }
    else
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
    nlColour assetColour = { colour.c[0], colour.c[1], colour.c[2], alpha };
    instance->SetAssetColour(assetColour);
}

void FECaptainComponent::Show()
{
    if (mComponent != 0)
    {
        mComponent->SetActiveSlide("in", false, false);
        mComponent->m_bVisible = true;
    }

    if (mPositions != 0)
    {
        mPositions->m_bVisible = true;
    }

    mUnidentified24 = 1;
    SetSlotVisibility(1, 1, 1);
    mPositions->SetActiveSlide("Slide1", true, false);
}

TLImageInstance* FECaptainComponent::FindPositionImage(int index, const char* name)
{
    const char* textureName;
    switch (index)
    {
    case 3:
        textureName = "00_dummy_texture_positions";
        break;
    case 0:
        textureName = "01_dummy_texture_positions";
        break;
    case 1:
        textureName = "02_dummy_texture_positions";
        break;
    case 2:
        textureName = "03_dummy_texture_positions";
        break;
    default:
        return 0;
    }

    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find<>(
        mPositions->GetActiveSlide(), "positions", "field_positions", "idle", "dummies", textureName);
    if (name == 0)
    {
        return FEFinder<TLImageInstance, 2>::FindOrDefault(component->GetActiveSlide(), textureName);
    }
    else
    {
        return FEFinder<TLImageInstance, 2>::FindOrDefault(component, name, textureName);
    }
}

void FECaptainComponent::SetRecycleState(int index, int state)
{
    int team;
    if (GameInfoManager::Instance()->IsInMode3())
    {
        team = g_pCupManager->mPendingCupTeam;
    }
    else
    {
        team = GameInfoManager::Instance()->GetTeam((short)mSide);
    }

    const char* textureName;
    switch (index)
    {
    case 0:
        textureName = "01_dummy_texture_positions";
        break;
    case 1:
        textureName = "02_dummy_texture_positions";
        break;
    case 2:
        textureName = "03_dummy_texture_positions";
        break;
    default:
        return;
    }

    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find<>(
        mPositions->GetActiveSlide(), "positions", "field_positions", "idle", "dummies", textureName);
    component = FEFinder<TLComponentInstance, 4>::FindOrDefault(component->GetActiveSlide(), "recycle");
    if (state == 1)
    {
        component->SetActiveSlide("Slide1", true, false);
        TLInstance* button = FEFinder<TLInstance, 1>::Find<>(component->GetActiveSlide(), "button_recycle_or_die");
        ApplyTeamColour(button, team, 102);
    }
    else
    {
        component->SetActiveSlide("on", true, false);
        TLInstance* button = FEFinder<TLInstance, 1>::Find<>(component->GetActiveSlide(), "button_recycle_or_die");
        nlColour colour;
        nlColourSet(colour, 255, 255, 255, 255);
        button->SetAssetColour(colour);
    }
}

static inline bool SidekickFacingFlag(int sidekick)
{
    switch (sidekick)
    {
    case 3:
        return false;
    case 5:
        return false;
    default:
        return true;
    }
}

TLImageInstance* FECaptainComponent::FindSidekickImage(int sidekick, int captain)
{
    FEPresentation* presentation = GameSceneManager::Instance()->GetCurrentScene()->GetPresentation();
    const CharacterInfo& sidekickInfo = GetCharacterInfo(GetCharacterIndexFromSidekick(sidekick));
    const CharacterInfo& captainInfo = GetCharacterInfo(GetCharacterIndexFromCaptain(captain));
    char name[64];
    nlSNPrintf(name, sizeof(name), "position_%s_%s", sidekickInfo.mName, captainInfo.mName);
    return FEFinder<TLImageInstance, TLAT_IMAGE>::Find(presentation, "art", "Layer", name);
}

static inline void SetSidekickImage(const FECaptainComponent* component, TLImageInstance* image, int sidekick)
{
    if (image != 0 && sidekick != -1)
    {
        int captain = 4;
        if (component->mSide == 0)
        {
            captain = 0;
        }
        TLImageInstance* source = FECaptainComponent::FindSidekickImage(sidekick, captain);
        if (source->m_pTextureResource != 0)
        {
            image->m_pTextureResource = source->m_pTextureResource;
        }
        if ((SidekickFacingFlag(sidekick) && component->mSide == 0) || (!SidekickFacingFlag(sidekick) && component->mSide == 1))
        {
            image->SetAssetRotation(0.0f, 3.1415927f, 0.0f);
        }
        else
        {
            image->SetAssetRotation(0.0f, 0.0f, 0.0f);
        }
    }
}

TLImageInstance* FECaptainComponent::FindCaptainImage(int captain, bool left)
{
    FEPresentation* presentation = GameSceneManager::Instance()->GetCurrentScene()->GetPresentation();
    const CharacterInfo& captainInfo = GetCharacterInfo(GetCharacterIndexFromCaptain(captain));
    TLImageInstance* source = 0;
    char name[64];
    if (!left)
    {
        nlSNPrintf(name, sizeof(name), "%s_right", captainInfo.mName);
        source = FEFinder<TLImageInstance, TLAT_IMAGE>::Find(presentation, "art", "Layer", name);
    }
    if (source == 0)
    {
        nlSNPrintf(name, sizeof(name), "positions_%s", captainInfo.mName);
        source = FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(presentation, "art", "Layer", name);
    }
    return source;
}

static inline void SetCaptainImage(const FECaptainComponent* component, TLImageInstance* image, int captain)
{
    if (image != 0 && captain != -1)
    {
        bool left = component->mSide == 0;
        TLImageInstance* source = FECaptainComponent::FindCaptainImage(component->mCaptain, left);
        if (source->m_pTextureResource != 0)
        {
            image->m_pTextureResource = source->m_pTextureResource;
        }
    }
}

static char SLIDE_OFF[] = "off";
static char SLIDE_OVER[] = "over";
static char SLIDE_DOWN[] = "down";

void FECaptainComponent::SetOverallSlide(TLComponentInstance* overall, const CharacterInfo& info)
{
    if (overall != 0)
    {
        switch (info.unknown_0x48)
        {
        case 0:
            overall->SetActiveSlide("offensive", false, false);
            break;
        case 1:
            overall->SetActiveSlide("defensive", false, false);
            break;
        case 2:
            overall->SetActiveSlide("playmaker", false, false);
            break;
        case 3:
            overall->SetActiveSlide("power", false, false);
            break;
        case 4:
            overall->SetActiveSlide("balanced", false, false);
            break;
        }
    }
}

void FECaptainComponent::UpdateOverallSlides()
{
    if (mCaptain == -1)
    {
        return;
    }

    TLComponentInstance* overall = FEFinder<TLComponentInstance, 4>::Find<>(mPositions->GetActiveSlide(), "positions", "overall_0");
    const CharacterInfo& info = GetCharacterInfo(GetCharacterIndexFromCaptain(mCaptain));
    SetOverallSlide(overall, info);

    overall = FEFinder<TLComponentInstance, 4>::Find<>(mPositions->GetActiveSlide(), "positions", "overall_1");
    if (mSidekicks[0] == -1)
    {
        overall->m_bVisible = false;
    }
    else
    {
        overall->m_bVisible = true;
        const CharacterInfo& info = GetCharacterInfo(GetCharacterIndexFromSidekick(mSidekicks[0]));
        SetOverallSlide(overall, info);
    }

    overall = FEFinder<TLComponentInstance, 4>::Find<>(mPositions->GetActiveSlide(), "positions", "overall_2");
    if (mSidekicks[1] == -1)
    {
        overall->m_bVisible = false;
    }
    else
    {
        overall->m_bVisible = true;
        const CharacterInfo& info = GetCharacterInfo(GetCharacterIndexFromSidekick(mSidekicks[1]));
        SetOverallSlide(overall, info);
    }

    overall = FEFinder<TLComponentInstance, 4>::Find<>(mPositions->GetActiveSlide(), "positions", "overall_3");
    if (mSidekicks[2] == -1)
    {
        overall->m_bVisible = false;
    }
    else
    {
        overall->m_bVisible = true;
        const CharacterInfo& info = GetCharacterInfo(GetCharacterIndexFromSidekick(mSidekicks[2]));
        SetOverallSlide(overall, info);
    }
}

void FECaptainComponent::LoadSlotImages(float)
{
    SetSidekickImage(this, FindPositionImage(0, SLIDE_OFF), mSidekicks[0]);
    SetSidekickImage(this, FindPositionImage(1, SLIDE_OFF), mSidekicks[1]);
    SetSidekickImage(this, FindPositionImage(2, SLIDE_OFF), mSidekicks[2]);
    SetSidekickImage(this, FindPositionImage(0, SLIDE_OVER), mSidekicks[0]);
    SetSidekickImage(this, FindPositionImage(1, SLIDE_OVER), mSidekicks[1]);
    SetSidekickImage(this, FindPositionImage(2, SLIDE_OVER), mSidekicks[2]);
    SetSidekickImage(this, FindPositionImage(0, SLIDE_DOWN), mSidekicks[0]);
    SetSidekickImage(this, FindPositionImage(1, SLIDE_DOWN), mSidekicks[1]);
    SetSidekickImage(this, FindPositionImage(2, SLIDE_DOWN), mSidekicks[2]);
    SetCaptainImage(this, FindPositionImage(3, 0), mCaptain);
    UpdateOverallSlides();
}

void FECaptainComponent::SetSlotVisibility(bool visible0, bool visible1, bool visible2)
{
    TLImageInstance* image = FindPositionImage(0, 0);
    if (image != 0)
    {
        image->m_bVisible = visible0;
    }
    image = FindPositionImage(1, 0);
    if (image != 0)
    {
        image->m_bVisible = visible1;
    }
    image = FindPositionImage(2, 0);
    if (image != 0)
    {
        image->m_bVisible = visible2;
    }

    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mPositions->GetActiveSlide(), "positions", "overall_1");
    component->m_bVisible = visible0;
    component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mPositions->GetActiveSlide(), "positions", "overall_2");
    component->m_bVisible = visible1;
    component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mPositions->GetActiveSlide(), "positions", "overall_3");
    component->m_bVisible = visible2;
}

void FECaptainComponent::RandomizeSidekicks()
{
    int sidekicks[8];
    int count = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (GetCharacterInfo(GetCharacterIndexFromSidekick(i)).unknown_0x24 == 1)
        {
            sidekicks[count++] = i;
        }
    }

    GameRules rules;
    rules.mValues[0] = (eSidekickID)sidekicks[nlRandom(count, &nlDefaultSeed)];
    rules.mValues[1] = (eSidekickID)sidekicks[nlRandom(count, &nlDefaultSeed)];
    rules.mValues[2] = (eSidekickID)sidekicks[nlRandom(count, &nlDefaultSeed)];
    mSidekicks[0] = rules.mValues[0];
    mSidekicks[1] = rules.mValues[1];
    mSidekicks[2] = rules.mValues[2];
    nlSingleton<GameInfoManager>::Instance()->SetRules(mCaptain, rules);
}

void FECaptainComponent::ResetSidekicks()
{
    int team;
    if (nlSingleton<GameInfoManager>::Instance()->IsInMode3())
    {
        team = g_pCupManager->mPendingCupTeam;
    }
    else
    {
        team = nlSingleton<GameInfoManager>::Instance()->GetTeam((short)mSide);
    }
    nlSingleton<GameInfoManager>::Instance()->ResetRules(team);
    ReloadSidekicks();
}

void FECaptainComponent::ReloadSidekicks()
{
    int team;
    if (nlSingleton<GameInfoManager>::Instance()->IsInMode3())
    {
        team = g_pCupManager->mPendingCupTeam;
    }
    else
    {
        team = nlSingleton<GameInfoManager>::Instance()->GetTeam((short)mSide);
    }
    GameRules rules = nlSingleton<GameInfoManager>::Instance()->mRulesTable[team];
    mSidekicks[0] = rules.mValues[0];
    mSidekicks[1] = rules.mValues[1];
    mSidekicks[2] = rules.mValues[2];
}

int FECaptainComponent::GetSidekick(int index)
{
    return mSidekicks[index];
}

void FECaptainComponent::SetCaptain(int value)
{
    mCaptain = value;
}

void FECaptainComponent::SetSidekick(int index, int value)
{
    mSidekicks[index] = value;
}

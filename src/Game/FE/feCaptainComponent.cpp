#include "Game/FE/feCaptainComponent.h"

#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/fePresentation.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/BaseSceneHandler.inl"
#include "Game/GameSceneManager.h"
#include "Game/GameInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/CharacterInfo.h"
#include "NL/nlMath.h"
#include "NL/nlString.h"

TU801DA134Component::TU801DA134Component()
    : mComponent(0)
    , mUnidentified08(0)
    , mUnidentified14(-1)
{
}

TU801DA134Component::~TU801DA134Component()
{
}

void TU801DA134Component::fn_801DA198(TLComponentInstance* component, int side)
{
    mSide = side;
    mComponent = component;
    mUnidentified08 = 0;
    if (mComponent != 0)
    {
        TLComponentInstance* screens = FEFinder<TLComponentInstance, 4>::Find<>(mComponent->GetActiveSlide(), "pda_screens");
        mUnidentified08 = FEFinder<TLComponentInstance, 4>::Find<>(screens->GetActiveSlide(), "positions");
    }
    if (mUnidentified08 != 0)
    {
        TLComponentInstance* indicator = FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified08, "sidekick_indicator");
        indicator->m_bVisible = false;
        TLInstance* highlight = FEFinder<TLInstance, 1>::Find<>(
            mUnidentified08->GetActiveSlide(), "positions", "field_positions", "idle", "highlight");
        highlight->m_bVisible = false;
    }

    TLComponentInstance* logos = FEFinder<TLComponentInstance, 4>::Find<>(mUnidentified08->GetActiveSlide(), "team_logos");
    int team;
    if (GameInfoManager::Instance()->IsInMode3())
    {
        GameRules rules = g_pCupManager->unknown_0x8A2C;
        mSidekicks[0] = rules.mValues[0];
        mSidekicks[1] = rules.mValues[1];
        mSidekicks[2] = rules.mValues[2];
        team = g_pCupManager->unknown_0x8A28;
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
    TLInstance* background = FEFinder<TLInstance, 1>::Find<>(mUnidentified08->GetActiveSlide(), "positions", "white_8x8");
    fn_801DA718(background, team, 180);
    fn_801DABAC(0, 1);
    fn_801DABAC(1, 1);
    fn_801DABAC(2, 1);
}

void TU801DA134Component::fn_801DA718(TLInstance* instance, int captain, unsigned char alpha)
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

void TU801DA134Component::fn_801DA88C()
{
    if (mComponent != 0)
    {
        mComponent->SetActiveSlide("in", false, false);
        mComponent->m_bVisible = true;
    }

    if (mUnidentified08 != 0)
    {
        mUnidentified08->m_bVisible = true;
    }

    mUnidentified24 = 1;
    fn_801DC824(1, 1, 1);
    mUnidentified08->SetActiveSlide("Slide1", true, false);
}

TLImageInstance* TU801DA134Component::fn_801DA924(int index, const char* name)
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
        mUnidentified08->GetActiveSlide(), "positions", "field_positions", "idle", "dummies", textureName);
    if (name == 0)
    {
        return FEFinder<TLImageInstance, 2>::FindOrDefault(component->GetActiveSlide(), textureName);
    }
    else
    {
        return FEFinder<TLImageInstance, 2>::FindOrDefault(component, name, textureName);
    }
}

void TU801DA134Component::fn_801DABAC(int index, int state)
{
    int team;
    if (GameInfoManager::Instance()->IsInMode3())
    {
        team = g_pCupManager->unknown_0x8A28;
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
        mUnidentified08->GetActiveSlide(), "positions", "field_positions", "idle", "dummies", textureName);
    component = FEFinder<TLComponentInstance, 4>::FindOrDefault(component->GetActiveSlide(), "recycle");
    if (state == 1)
    {
        component->SetActiveSlide("Slide1", true, false);
        TLInstance* button = FEFinder<TLInstance, 1>::Find<>(component->GetActiveSlide(), "button_recycle_or_die");
        fn_801DA718(button, team, 102);
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

TLImageInstance* TU801DA134Component::FindSidekickImage(int sidekick, int captain)
{
    FEPresentation* presentation = GameSceneManager::Instance()->GetCurrentScene()->GetPresentation();
    const CharacterInfo& sidekickInfo = GetCharacterInfo(GetCharacterIndexFromSidekick(sidekick));
    const CharacterInfo& captainInfo = GetCharacterInfo(GetCharacterIndexFromCaptain(captain));
    char name[64];
    nlSNPrintf(name, sizeof(name), "position_%s_%s", sidekickInfo.mName, captainInfo.mName);
    return FEFinder<TLImageInstance, TLAT_IMAGE>::Find(presentation, "art", "Layer", name);
}

static inline void SetSidekickImage(const TU801DA134Component* component, TLImageInstance* image, int sidekick)
{
    if (image != 0 && sidekick != -1)
    {
        int captain = 4;
        if (component->mSide == 0)
        {
            captain = 0;
        }
        TLImageInstance* source = TU801DA134Component::FindSidekickImage(sidekick, captain);
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

TLImageInstance* TU801DA134Component::FindCaptainImage(int captain, bool left)
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

static inline void SetCaptainImage(const TU801DA134Component* component, TLImageInstance* image, int captain)
{
    if (image != 0 && captain != -1)
    {
        bool left = component->mSide == 0;
        TLImageInstance* source = TU801DA134Component::FindCaptainImage(component->mUnidentified14, left);
        if (source->m_pTextureResource != 0)
        {
            image->m_pTextureResource = source->m_pTextureResource;
        }
    }
}

void TU801DA134Component::SetOverallSlide(TLComponentInstance* overall, const CharacterInfo& info)
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

void TU801DA134Component::fn_801DAFC8()
{
    if (mUnidentified14 == -1)
    {
        return;
    }

    TLComponentInstance* overall = FEFinder<TLComponentInstance, 4>::Find<>(mUnidentified08->GetActiveSlide(), "positions", "overall_0");
    const CharacterInfo& info = GetCharacterInfo(GetCharacterIndexFromCaptain(mUnidentified14));
    SetOverallSlide(overall, info);

    overall = FEFinder<TLComponentInstance, 4>::Find<>(mUnidentified08->GetActiveSlide(), "positions", "overall_1");
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

    overall = FEFinder<TLComponentInstance, 4>::Find<>(mUnidentified08->GetActiveSlide(), "positions", "overall_2");
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

    overall = FEFinder<TLComponentInstance, 4>::Find<>(mUnidentified08->GetActiveSlide(), "positions", "overall_3");
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

void TU801DA134Component::fn_801DB69C(float)
{
    SetSidekickImage(this, fn_801DA924(0, "off"), mSidekicks[0]);
    SetSidekickImage(this, fn_801DA924(1, "off"), mSidekicks[1]);
    SetSidekickImage(this, fn_801DA924(2, "off"), mSidekicks[2]);
    SetSidekickImage(this, fn_801DA924(0, "over"), mSidekicks[0]);
    SetSidekickImage(this, fn_801DA924(1, "over"), mSidekicks[1]);
    SetSidekickImage(this, fn_801DA924(2, "over"), mSidekicks[2]);
    SetSidekickImage(this, fn_801DA924(0, "down"), mSidekicks[0]);
    SetSidekickImage(this, fn_801DA924(1, "down"), mSidekicks[1]);
    SetSidekickImage(this, fn_801DA924(2, "down"), mSidekicks[2]);
    SetCaptainImage(this, fn_801DA924(3, 0), mUnidentified14);
    fn_801DAFC8();
}

void TU801DA134Component::fn_801DC824(bool visible0, bool visible1, bool visible2)
{
    TLImageInstance* image = fn_801DA924(0, 0);
    if (image != 0)
    {
        image->m_bVisible = visible0;
    }
    image = fn_801DA924(1, 0);
    if (image != 0)
    {
        image->m_bVisible = visible1;
    }
    image = fn_801DA924(2, 0);
    if (image != 0)
    {
        image->m_bVisible = visible2;
    }

    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified08->GetActiveSlide(), "positions", "overall_1");
    component->m_bVisible = visible0;
    component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified08->GetActiveSlide(), "positions", "overall_2");
    component->m_bVisible = visible1;
    component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified08->GetActiveSlide(), "positions", "overall_3");
    component->m_bVisible = visible2;
}

void TU801DA134Component::fn_801DCB28()
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
    nlSingleton<GameInfoManager>::Instance()->SetRules(mUnidentified14, rules);
}

void TU801DA134Component::fn_801DCC28()
{
    int team;
    if (nlSingleton<GameInfoManager>::Instance()->IsInMode3())
    {
        team = g_pCupManager->unknown_0x8A28;
    }
    else
    {
        team = nlSingleton<GameInfoManager>::Instance()->GetTeam((short)mSide);
    }
    nlSingleton<GameInfoManager>::Instance()->ResetRules(team);
    fn_801DCCEC();
}

void TU801DA134Component::fn_801DCCEC()
{
    int team;
    if (nlSingleton<GameInfoManager>::Instance()->IsInMode3())
    {
        team = g_pCupManager->unknown_0x8A28;
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

int TU801DA134Component::fn_801DCD74(int index)
{
    return mSidekicks[index];
}

void TU801DA134Component::fn_801DCD84(int value)
{
    mUnidentified14 = value;
}

void TU801DA134Component::fn_801DCD8C(int index, int value)
{
    mSidekicks[index] = value;
}

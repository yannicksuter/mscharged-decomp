#include "Game/FE/feCaptainComponent.h"

#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feDPD.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/GameSceneManager.h"
#include "Game/EventDataTypes.h"
#include "Game/EventRegistry.h"
#include "Game/OverlayManager.h"
#include "Game/OverlayHandlerInGameText.h"
#include "Game/OverlayHandlerGoal.h"
#include "Game/OverlayHandlerHUD.h"
#include "unclassified/tu_801F6A24.h"
#include "Game/Render/NumberDisplay.h"
#include "Game/FE/feSceneManager.h"
#include "Game/GameInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/SH/SHStrikerTimesBase.h"
#include "Game/TweakQuery.h"
#include "Game/main.h"
#include "unclassified/tu_80284A58.h"
#include "NL/nlMath.h"
#include "NL/nlFunctionMemory.h"
#include "NL/nlFunction.inl"
#include "NL/nlBindMember.h"
#include "NL/nlBind_impl.h"
#include "NL/nlTask.h"
#include "NL/nlString.h"

extern "C" void fn_801DCCEC(TU801DA134Component* comp);

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
        TLComponentInstance* screens = FEFinder<TLComponentInstance, 4>::Find(mComponent->GetActiveSlide(), "pda_screens");
        mUnidentified08 = FEFinder<TLComponentInstance, 4>::Find(screens->GetActiveSlide(), "positions");
    }
    if (mUnidentified08 != 0)
    {
        TLComponentInstance* indicator = FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified08, "sidekick_indicator");
        indicator->m_bVisible = false;
        TLInstance* highlight = FEFinder<TLInstance, 1>::Find(
            mUnidentified08->GetActiveSlide(), "positions", "field_positions", "idle", "highlight");
        highlight->m_bVisible = false;
    }

    TLComponentInstance* logos = FEFinder<TLComponentInstance, 4>::Find(mUnidentified08->GetActiveSlide(), "team_logos");
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
    TLInstance* background = FEFinder<TLInstance, 1>::Find(mUnidentified08->GetActiveSlide(), "positions", "white_8x8");
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

    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(
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

    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(
        mUnidentified08->GetActiveSlide(), "positions", "field_positions", "idle", "dummies", textureName);
    component = FEFinder<TLComponentInstance, 4>::FindOrDefault(component->GetActiveSlide(), "recycle");
    if (state == 1)
    {
        component->SetActiveSlide("Slide1", true, false);
        TLInstance* button = FEFinder<TLInstance, 1>::Find(component->GetActiveSlide(), "button_recycle_or_die");
        fn_801DA718(button, team, 102);
    }
    else
    {
        component->SetActiveSlide("on", true, false);
        TLInstance* button = FEFinder<TLInstance, 1>::Find(component->GetActiveSlide(), "button_recycle_or_die");
        nlColour colour;
        nlColourSet(colour, 255, 255, 255, 255);
        button->SetAssetColour(colour);
    }
}

void TU801DA134Component::fn_801DAFC8()
{
    if (mUnidentified14 == -1)
    {
        return;
    }

    TLComponentInstance* overall = FEFinder<TLComponentInstance, 4>::Find(mUnidentified08->GetActiveSlide(), "positions", "overall_0");
    const CharacterInfo& info = GetCharacterInfo(GetCharacterIndexFromCaptain(mUnidentified14));
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

    overall = FEFinder<TLComponentInstance, 4>::Find(mUnidentified08->GetActiveSlide(), "positions", "overall_1");
    if (mSidekicks[0] == -1)
    {
        overall->m_bVisible = false;
    }
    else
    {
        overall->m_bVisible = true;
        const CharacterInfo& info = GetCharacterInfo(GetCharacterIndexFromSidekick(mSidekicks[0]));
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

    overall = FEFinder<TLComponentInstance, 4>::Find(mUnidentified08->GetActiveSlide(), "positions", "overall_2");
    if (mSidekicks[1] == -1)
    {
        overall->m_bVisible = false;
    }
    else
    {
        overall->m_bVisible = true;
        const CharacterInfo& info = GetCharacterInfo(GetCharacterIndexFromSidekick(mSidekicks[1]));
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

    overall = FEFinder<TLComponentInstance, 4>::Find(mUnidentified08->GetActiveSlide(), "positions", "overall_3");
    if (mSidekicks[2] == -1)
    {
        overall->m_bVisible = false;
    }
    else
    {
        overall->m_bVisible = true;
        const CharacterInfo& info = GetCharacterInfo(GetCharacterIndexFromSidekick(mSidekicks[2]));
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

static inline TLImageInstance* FindSidekickImage(int sidekick, int captain)
{
    FEPresentation* presentation = GameSceneManager::Instance()->GetCurrentScene()->mPresentation;
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
        TLImageInstance* source = FindSidekickImage(sidekick, captain);
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

static inline TLImageInstance* FindCaptainImage(int captain, bool left)
{
    FEPresentation* presentation = GameSceneManager::Instance()->GetCurrentScene()->mPresentation;
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
    int captain = mUnidentified14;
    TLImageInstance* image = fn_801DA924(3, 0);
    if (image != 0 && captain != -1)
    {
        bool left = mSide == 0;
        TLImageInstance* source = FindCaptainImage(mUnidentified14, left);
        if (source->m_pTextureResource != 0)
        {
            image->m_pTextureResource = source->m_pTextureResource;
        }
    }
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
    fn_801DCCEC(this);
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
                mComponent->GetActiveSlide(), InlineHasher(nlStringLowerHash(name)));
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
            TLSlide* slide = scene->mPresentation->m_currentSlide;
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
        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(
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

void OverlayManager::fn_801E1514()
{
    {
        Function<FnVoidVoid> callback(BindMember(this, &OverlayManager::fn_801E258C));
        UnidentifiedFindEvent<UnidentifiedEventNoData>("GetReadyForKickoff", -1)->Add(callback, 0, -1);
    }
    {
        Function<FnVoidVoid> callback(BindMember(this, &OverlayManager::fn_801E2590));
        UnidentifiedFindEvent<UnidentifiedEventNoData>("Kickoff", -1)->Add(callback, 0, -1);
    }
    {
        Function<FnVoidVoid> callback(BindMember(this, &OverlayManager::fn_801E2608));
        UnidentifiedFindEvent<UnidentifiedEventNoData>("GameOver", -1)->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData_8006701C*> callback(BindMember(this, &OverlayManager::fn_801E2784));
        UnidentifiedFindEvent<UnidentifiedEventData_8006701C>("MegaStrikeMeterStart", -1)->Add(callback, 0, -1);
    }
    {
        Function<FnVoidVoid> callback(BindMember(this, &OverlayManager::fn_801E281C));
        UnidentifiedFindEvent<UnidentifiedEventNoData>("MegaStrikeMeterEnd", -1)->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData_8006701C*> callback(BindMember(this, &OverlayManager::fn_801E28A8));
        UnidentifiedFindEvent<UnidentifiedEventData_8006701C>("MegaStrikeMeterFirst", -1)->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData_8006701C*> callback(BindMember(this, &OverlayManager::fn_801E28E4));
        UnidentifiedFindEvent<UnidentifiedEventData_8006701C>("MegaStrikeMeterSecond", -1)->Add(callback, 0, -1);
    }
    {
        Function<FnVoidVoid> callback(BindMember(this, &OverlayManager::fn_801E2920));
        UnidentifiedFindEvent<UnidentifiedEventNoData>("MegastrikeStart", -1)->Add(callback, 0, -1);
    }
    {
        Function<MegaStrikeEndData*> callback(BindMember(this, &OverlayManager::fn_801E2988));
        UnidentifiedFindEvent<MegaStrikeEndData>("MegastrikeEnd", -1)->Add(callback, 0, -1);
    }
    {
        Function<GoalScoredData*> callback(BindMember(this, &OverlayManager::fn_801E2A28));
        UnidentifiedFindEvent<GoalScoredData>("GoalScored", -1)->Add(callback, 0, -1);
    }
}

void OverlayManager::fn_801E258C()
{
}

extern "C" void fn_801E2A14(void* p)
{
    *(int*)((char*)p + 276) = -1;
    *(int*)((char*)p + 280) = -1;
    *(int*)((char*)p + 284) = -1;
}

void TU801DCD9CComponent::fn_801DE570(bool visible)
{
    if (mUnidentified74 == 0)
    {
        return;
    }
    mUnidentified74->m_bVisible = visible;
}

extern "C" void fn_801E2230(void* p, int arg)
{
    void* q = *(void**)((char*)p + 264);
    if (q == 0)
    {
        return;
    }
    ((InGameTextOverlay*)q)->SetSlide((OverlaySlideName)arg);
}

void OverlayManager::Pop()
{
    BaseGameSceneManager::Pop();
}

extern "C" void fn_801E2564(BaseGameSceneManager* mgr)
{
    ((GoalOverlay*)mgr->GetScene((SceneList)95))->Restart();
}

inline void OverlayManager::SlideHUDOut()
{
    mHUDDelay = 0.0f;
    if (mIsHUDSlideIn == true)
    {
        static_cast<HUDOverlay*>(GetScene((SceneList)89))->SetSlideOut();
        mIsHUDSlideIn = false;
        gpNumberDisplay->mVisible = false;
    }
}

void OverlayManager::Update(float deltaTime)
{
    if (mHUDDelay > 0.0f && nlTaskManager::m_pInstance->mCurrentState == 2)
    {
        mHUDDelay -= deltaTime;
        if (mHUDDelay <= 0.0f)
        {
            mHUDDelay = 0.0f;
            if (mDoHUDSlideIn)
            {
                fn_801E2498(0.0f);
            }
            else
            {
                SlideHUDOut();
            }
        }
    }
}

void OverlayManager::SetVisible(SceneList scene, bool visibility, bool overrideStateSettings)
{
    if (nlSingleton<GameInfoManager>::Instance()->mCurrentMode == 2 && scene != OVERLAY_HUD)
    {
        return;
    }

    const char* fileName = GetFileName(scene);
    unsigned long hashID = nlStringLowerHash(fileName);
    BaseOverlayHandler* handler = static_cast<BaseOverlayHandler*>(nlSingleton<FESceneManager>::Instance()->GetSceneHandler(hashID));
    if (handler != 0)
    {
        u32 state = nlTaskManager::m_pInstance->mCurrentState;
        if (overrideStateSettings || (handler->mVisibilityMask & state))
        {
            handler->SetVisible(visibility);
        }
    }
}

void OverlayManager::HandleStateTransition(u32 from, u32 to)
{
    if (to == 0x02000000)
    {
        return;
    }
    if (from == 0x02000000)
    {
        return;
    }

    for (u32 i = 0; i < mCurrentStackDepth; i++)
    {
        SceneList sceneType = m_sceneStack[i];
        if (sceneType <= 88)
        {
            continue;
        }

        BaseSceneHandler* handler = mBaseSceneHandlerStack[i];
        if (handler == 0)
        {
            continue;
        }

        if (sceneType == 90 && mUnidentified10E)
        {
            continue;
        }

        BaseOverlayHandler* overlayHandler = static_cast<BaseOverlayHandler*>(handler);
        if ((overlayHandler->mVisibilityMask & to) != 0)
        {
            if (!overlayHandler->mWasLastVisible)
            {
                continue;
            }
            overlayHandler->SetVisible(true);
        }
        else
        {
            overlayHandler->mWasLastVisible = overlayHandler->mVisible;
            overlayHandler->SetVisible(false);
        }
    }
}

void OverlayManager::fn_801E2498(float delay)
{
    mHUDDelay = delay;
    if (0.0f != delay)
    {
        mDoHUDSlideIn = true;
    }
    else if (!mIsHUDSlideIn)
    {
        static_cast<HUDOverlay*>(GetScene((SceneList)89))->SetSlideIn();
        mIsHUDSlideIn = true;
        gpNumberDisplay->mVisible = true;
    }
}

void OverlayManager::ShowDemoSlide()
{
    if (!mIsDemoSlideVisible)
    {
        BaseSceneHandler* scene = GetScene((SceneList)96);
        if (scene != 0)
        {
            scene->SetVisible(true);
            mIsDemoSlideVisible = true;
        }
    }
}

BaseSceneHandler* OverlayManager::Push(SceneList scene, ScreenMovement movement, bool popfirst)
{
    BaseSceneHandler* h = BaseGameSceneManager::Push(scene, movement, popfirst);
    if ((h != 0) && (scene == 90))
    {
        mInGameTextOverlay = static_cast<InGameTextOverlay*>(h);
    }
    return h;
}

extern "C" void fn_801E0AD0(void* p)
{
    ((TLComponentInstance*)(*(void**)((char*)p + 116)))->SetActiveSlide("in", true, false);
    void* q = *(void**)((char*)p + 116);
    if (q != 0)
    {
        *(unsigned char*)((char*)q + 142) = 1;
    }
}

OverlayManager::OverlayManager()
{
    mInGameTextOverlay = 0;
    mIsHUDSlideIn = false;
    mDoHUDSlideIn = false;
    mUnidentified10E = 0;
    mIsDemoSlideVisible = false;
    mHUDDelay = 0.0f;
    mUnidentified120 = 0;
    mStrikerTimesStoryVariant = -1;
    mStrikerTimesHeadlineVariant = -1;
    mStrikerTimesImageVariant = -1;
}

OverlayManager::~OverlayManager()
{
}

extern bool lbl_806E1864;

void OverlayManager::fn_801E2590()
{
    static_cast<OverlayManager*>(g_pOverlayManager)->SetVisible((SceneList)90, false, false);
}

void OverlayManager::fn_801E2608()
{
    if (GetTweakBool("/user/dosoak", false)
        || (g_e3_Build && GameInfoManager::Instance()->IsInMode2()))
    {
        nlTaskManager::SetNextState(0x400000);
        return;
    }

    if (GameInfoManager::Instance()->mCurrentMode == GameInfoManager::GM_FRIENDLY)
    {
        SHStrikerTimesBase* scene = static_cast<SHStrikerTimesBase*>(g_pOverlayManager->Push((SceneList)91, SCREEN_NOTHING, false));
        scene->SetDisplayMode(10);
    }
    else if (GameInfoManager::Instance()->IsInMode4())
    {
        SHStrikerTimesBase* scene = static_cast<SHStrikerTimesBase*>(g_pOverlayManager->Push((SceneList)77, SCREEN_NOTHING, false));
        if (scene != 0)
        {
            scene->SetDisplayMode(9);
        }
    }
    else
    {
        SHStrikerTimesBase* scene = static_cast<SHStrikerTimesBase*>(g_pOverlayManager->Push((SceneList)91, SCREEN_NOTHING, false));
        scene->SetDisplayMode(11);
    }

    static_cast<OverlayManager*>(g_pOverlayManager)->SlideHUDOut();
    GetPresentation()->StopOverlay();
    static_cast<HUDOverlay*>(g_pOverlayManager->GetScene((SceneList)89))->ResetScores();
}

void OverlayManager::fn_801E2784(UnidentifiedEventData_8006701C* eventData)
{
    static_cast<OverlayManager*>(g_pOverlayManager)->SlideHUDOut();
    TU801F6A24Overlay* overlay = static_cast<TU801F6A24Overlay*>(g_pOverlayManager->GetScene((SceneList)100));
    overlay->SetVisible(true);
    overlay->fn_801F6E2C(eventData->pFielder);
}

void OverlayManager::fn_801E281C()
{
    TU801F6A24Overlay* overlay = static_cast<TU801F6A24Overlay*>(g_pOverlayManager->GetScene((SceneList)100));
    overlay->SetVisible(false);
    if (!overlay->mUnidentified36)
    {
        static_cast<OverlayManager*>(g_pOverlayManager)->fn_801E2498(0.0f);
    }
}

void OverlayManager::fn_801E2920()
{
    lbl_806E1864 = false;
    static_cast<OverlayManager*>(g_pOverlayManager)->SlideHUDOut();
}

void OverlayManager::fn_801E2A28(GoalScoredData* eventData)
{
    lbl_806E1864 = true;
    if (eventData->uGoalType != 6)
    {
        static_cast<OverlayManager*>(g_pOverlayManager)->SlideHUDOut();
        gpNumberDisplay->mVisible = true;
    }
    static_cast<HUDOverlay*>(g_pOverlayManager->GetScene((SceneList)89))->UpdateScore();
}

void OverlayManager::fn_801E2988(MegaStrikeEndData* eventData)
{
    if (eventData->goals == 0)
    {
        return;
    }
    static_cast<HUDOverlay*>(g_pOverlayManager->GetScene(OVERLAY_HUD))->UpdateScore();
}

void OverlayManager::fn_801E28A8(UnidentifiedEventData_8006701C* eventData)
{
    TU801F6A24Overlay* overlay = static_cast<TU801F6A24Overlay*>(g_pOverlayManager->GetScene((SceneList)100));
    overlay->fn_801F6D94(eventData->fMeterValue);
}

void OverlayManager::fn_801E28E4(UnidentifiedEventData_8006701C* eventData)
{
    TU801F6A24Overlay* overlay = static_cast<TU801F6A24Overlay*>(g_pOverlayManager->GetScene((SceneList)100));
    overlay->fn_801F6E18(eventData->fMeterValue);
}

void OverlayManager::fn_801E29C0(nlVector3 position)
{
    TU801F6A24Overlay* ov = static_cast<TU801F6A24Overlay*>(g_pOverlayManager->GetScene((SceneList)100));
    ov->fn_801F6E8C(position);
}

extern "C" void fn_801DCCEC(TU801DA134Component* comp)
{
    int team;
    if (nlSingleton<GameInfoManager>::Instance()->IsInMode3())
    {
        team = g_pCupManager->unknown_0x8A28;
    }
    else
    {
        team = nlSingleton<GameInfoManager>::Instance()->GetTeam((short)comp->mSide);
    }
    GameRules rules = nlSingleton<GameInfoManager>::Instance()->mRulesTable[team];
    comp->mSidekicks[0] = rules.mValues[0];
    comp->mSidekicks[1] = rules.mValues[1];
    comp->mSidekicks[2] = rules.mValues[2];
}

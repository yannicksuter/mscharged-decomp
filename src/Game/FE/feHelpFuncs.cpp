#include "Game/FE/feHelpFuncs_decl.h"

#include <cmath>
#include <stdio.h>

#include "Game/DB/CharacterInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feModelManager.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feText.h"
#include "Game/FE/tlComponent.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/Font/fontmanager.h"
#include "Game/GameInfo.h"
#include "Game/SH/SHNavigation.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/MemAlloc.h"
#include "NL/nlBasicString.h"
#include "NL/nlFont.h"
#include "NL/nlLexicalCast.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"

// The predecessor's retail map keeps this unit's own copies of the Format
// family (Format, FormatImpl) inside the TakeGameMemSnapshot namespace, and
// R4QE01 likewise links a second FormatImpl conversion operator here next to
// the shared one. LexicalCast stays global: the int instantiation used by this
// Format is the shared copy.
namespace TakeGameMemSnapshot
{
#include "NL/nlFormat.h"
} // namespace TakeGameMemSnapshot

static const char* ModeToStringName[10] = {
    "FRIENDLY",
    "MUSHROOM_CUP",
    "FLOWER_CUP",
    "STAR_CUP",
    "BOWSER_CUP",
    "SUPER_MUSHROOM_CUP",
    "SUPER_FLOWER_CUP",
    "SUPER_STAR_CUP",
    "SUPER_BOWSER_CUP",
    "TOURNAMENT",
};

static const float sDoneButtonBounds[4] = { -84.0f, 84.0f, -165.0f, -259.0f };
static const float sPlayButtonBounds[4] = { -84.0f, 84.0f, -165.0f, -259.0f };

// Retail's .sdata2 pool for this unit opens with 0.0f, ahead of the constants
// EnableAutoPressed creates, although no surviving function generated before
// EnableAutoPressed uses a float. Under -ipa file a non-static, non-inline
// function is generated at its definition and creates its literals there, and
// the release link strips its body when nothing references it. So a function
// defined here used 0.0f and was stripped whole; R4QE01 keeps no byte of it.
// This never-called placeholder reproduces the pool position only. Its real
// name, signature and body are unknown (final_review.md).
void UnidentifiedZeroFloat(float* value)
{
    *value = 0.0f;
}

const char* GetLOCCharacterName(eTeamID teamid)
{
    return GetLOCTeamName(teamid);
}

const char* GetLOCTeamName(eTeamID teamID)
{
    return GetCharacterInfo(GetCharacterIndexFromCaptain(teamID)).mDisplayNameKey;
}

eCharacterClass ConvertToCharacterClass(eTeamID teamID)
{
    return (eCharacterClass)GetCharacterIndexFromCaptain(teamID);
}

eCharacterClass ConvertToCharacterClass(eSidekickID sidekickID)
{
    return (eCharacterClass)GetCharacterIndexFromSidekick(sidekickID);
}

const char* GetTeamName(eTeamID teamID)
{
    return GetCharacterInfo(GetCharacterIndexFromCaptain(teamID)).mName;
}

const char* GetSidekickName(eSidekickID sidekickID)
{
    return GetCharacterInfo(GetCharacterIndexFromSidekick(sidekickID)).mName;
}

eTeamID ConvertToTeamID(const char* name)
{
    return (eTeamID)GetCharacterInfo(GetCharacterIndexFromName(name)).mCaptainId;
}

eSidekickID ConvertToSidekickID(const char* name)
{
    return (eSidekickID)GetCharacterInfo(GetCharacterIndexFromName(name)).mSidekickId;
}

eStadiumID ConvertToStadiumID(const char* name)
{
    int stadium = -1;
    for (int i = 0; i < 18; ++i)
    {
        if (nlStrICmp(GetStadiumName(i), name) == 0)
        {
            stadium = i;
            break;
        }
    }
    return (eStadiumID)stadium;
}

const char* GetLOCModeName(int mode)
{
    return ModeToStringName[mode];
}

void EnableAutoPressed()
{
    g_pFEInput->Reset(false);
    g_pFEInput->SetAutoRepeatParams(FE_ALL_PADS, 0xE, 0.7f, 0.3f);
    g_pFEInput->SetAutoRepeatParams(FE_ALL_PADS, 0xD, 0.7f, 0.3f);
    g_pFEInput->SetAutoRepeatParams(FE_ALL_PADS, 0xB, 0.7f, 0.3f);
    g_pFEInput->SetAutoRepeatParams(FE_ALL_PADS, 0xC, 0.7f, 0.3f);
}

unsigned long FECharacterSound::GetCaptainAcceptSound(eTeamID teamID)
{
    static const unsigned long CHARACTER_ACCEPT_SOUNDS[12] = {
        0xC62D125A,
        0x7326DD54,
        0xC58A10DC,
        0xFCC1D431,
        0xC625CAE8,
        0xC6654443,
        0x071B6614,
        0xC6E20764,
        0xC70DE9CE,
        0xD8538C50,
        0xC58E5CB0,
        0xC6659569,
    };
    return CHARACTER_ACCEPT_SOUNDS[teamID];
}

unsigned long FECharacterSound::GetSidekickAcceptSound(eSidekickID sidekickID)
{
    static const unsigned long SIDEKICK_SOUNDS[8] = {
        0x441E2551,
        0xC7403063,
        0xA7484743,
        0xC69A14D9,
        0xDB466EC9,
        0x1F071B6F,
        0x7A095D6D,
        0xC175A6B2,
    };
    return SIDEKICK_SOUNDS[sidekickID];
}

static unsigned long GetLargestFreeBlock()
{
    return VirtualAllocator.LargestFreeBlock();
}

void TakeGameMemSnapshot::Update(float dt)
{
    if (gTakenSnapshot)
    {
        return;
    }

    gTimeElapsed += dt;
    if (gTimeElapsed >= 5.0f)
    {
        WriteToDisk();
        gTakenSnapshot = 1;
    }
}

namespace TakeGameMemSnapshot
{
unsigned char gTakenSnapshot;
float gTimeElapsed;
} // namespace TakeGameMemSnapshot

void TakeGameMemSnapshot::ResetTimers()
{
    gTakenSnapshot = 0;
    gTimeElapsed = 0.0f;
}

// The Wii build has no virtual-memory statistics; the two columns the
// predecessor filled from them are written as -1.
static int freeVM = -1;
static int largestFreeVM = -1;

void TakeGameMemSnapshot::WriteToDisk()
{
    const char* filename = "gamesnapshot.txt";
    FILE* pFile = fopen(filename, "r");

    if (!pFile)
    {
        pFile = fopen(filename, "wt");
        NLString header;
        header.AppendInPlace("hcaptain,hsidekick0,hsidekick1,hsidekick2,acaptain,asidekick0,asidekick1,asidekick2,stadium,largestfree,freevm,largestfreevm\n");
        int size = header.size();
        fwrite(header.c_str(), 1, size, pFile);
    }
    fclose(pFile);

    pFile = fopen(filename, "at");

    NLString data;
    data.AppendInPlace(GetTeamName((eTeamID)GameInfoManager::Instance()->GetTeam(0)));
    data.AppendInPlace(",");
    data.AppendInPlace(GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(0, 0)));
    data.AppendInPlace(",");
    data.AppendInPlace(GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(0, 1)));
    data.AppendInPlace(",");
    data.AppendInPlace(GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(0, 2)));
    data.AppendInPlace(",");
    data.AppendInPlace(GetTeamName((eTeamID)GameInfoManager::Instance()->GetTeam(1)));
    data.AppendInPlace(",");
    data.AppendInPlace(GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(1, 0)));
    data.AppendInPlace(",");
    data.AppendInPlace(GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(1, 1)));
    data.AppendInPlace(",");
    data.AppendInPlace(GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(1, 2)));
    data.AppendInPlace(",");
    data.AppendInPlace(GetStadiumName(GameInfoManager::Instance()->GetStadium()));
    data.AppendInPlace(",");

    fwrite(data.c_str(), 1, data.size(), pFile);

    NLString stats;
    {
        NLString fmt("{0},{1},{2}\n");
        unsigned long largestFree;

        largestFree = GetLargestFreeBlock();

        stats = Format<NLString, unsigned long, int, int>(fmt, largestFree, freeVM, largestFreeVM);
    }

    fwrite(stats.c_str(), 1, stats.size(), pFile);
    fclose(pFile);
}

void MakeTextBoxReallyWide(TLTextInstance& textInstance)
{
    nlVector2& boxSize = ((textInstance.m_OverloadFlags & 0x4) != 0)
        ? textInstance.m_OverloadedAttributes.BoxSize
        : ((FEText*)textInstance.m_component)->m_TextAttributes.BoxSize;
    nlVector2 bb = boxSize;
    bb.x = 999.9f;
    textInstance.m_OverloadedAttributes.BoxSize = bb;
    textInstance.m_OverloadFlags |= 0x4;
}

nlVector2 fn_801CC48C(TLTextInstance* pText)
{
    FEText* pFeText = (FEText*)pText->m_component;
    nlFont* pFont;
    if (pFeText->m_pFeFontResource == 0)
    {
        pFont = FontManager::Instance()->GetFontByHashID(0);
    }
    else
    {
        pFont = pFeText->m_pFeFontResource->GetFontReference();
    }

    float width = pFont->GetStringWidth(pText->GetString(), false, 640, true);

    nlTextBox::StringDrawInfo drawInfo = pText->m_DrawInfo;
    float height = pFont->m_Metrics.Height * drawInfo.RowCount;

    nlVector2 size;
    nlVec2Set(size, width, height);
    return size;
}

// The same pool keeps 0.5f between fn_801CC48C's constants and the idle
// animation constants, while SetBreadcrumbs, its only surviving reader, is the
// unit's last function. A stripped function defined here created it; its
// identity is unknown (final_review.md).
void UnidentifiedHalveFloat(float* value)
{
    *value *= 0.5f;
}

static float sCharacterIdleTime;

void ResetCharacterIdleAnimation(FEModelHandle* model)
{
    if (model != 0)
    {
        model->PlayAnimation("fe_idle", PM_CYCLIC, 0.2f, 0.0f, false);
    }
}

void UpdateCharacterIdleAnimations(float dt)
{
    sCharacterIdleTime += dt;
    if (sCharacterIdleTime >= 2.0f)
    {
        sCharacterIdleTime = 0.0f;
    }

    if (sCharacterIdleTime == 0.0f)
    {
        for (int i = 0; i < 2; ++i)
        {
            if (nlRandom(100, &nlDefaultSeed) < 25)
            {
                FEModelHandle* model = FEModelManager::Instance()->GetModel(
                    i == 0 ? "homemodel" : "awaymodel");
                if (model != 0 && model->IsLoaded()
                    && model->IsPlayingAnimation("fe_idle"))
                {
                    model->PlayAnimation(
                        "fe_idle_action_01", PM_HOLD, 0.2f, 0.0f, false);
                    model->SetAnimationCompleteCallback(ResetCharacterIdleAnimation);
                }
            }
        }
    }
}

void SetPlayButtonBounds(
    FEPointerButton* component, TLComponentInstance*)
{
    component->SetBounds(sPlayButtonBounds[0], sPlayButtonBounds[1],
        sPlayButtonBounds[2], sPlayButtonBounds[3]);
}

void SetDoneButtonBounds(
    FEPointerButton* component, TLComponentInstance*, int value)
{
    if (value)
    {
        component->SetBounds(sPlayButtonBounds[0], sPlayButtonBounds[1],
            sPlayButtonBounds[2], sPlayButtonBounds[3]);
    }
    else
    {
        component->SetBounds(sDoneButtonBounds[0], sDoneButtonBounds[1],
            sDoneButtonBounds[2], sDoneButtonBounds[3]);
    }
}

// Retail leaves the result unspecified for an invalid cheat ID.
#pragma warning off(10184) // return value expected
const char* GetLOCEnvironmentCheatName(int cheat)
{
    switch (cheat)
    {
    case 0:
        return "CHEATS_NONE";
    case 1:
        return "CHEATS_ENVIRONMENT_SECURE";
    case 2:
        return "CHEATS_ENVIRONMENT_POWER";
    case 3:
        return "CHEATS_ENVIRONMENT_VOLTAGE";
    case 4:
        return "CHEATS_ENVIRONMENT_TILT";
    case 5:
        return "CHEATS_ENVIRONMENT_WHITE_BALL";
    }
}

const char* GetLOCEnvironmentCheatDescription(int cheat)
{
    switch (cheat)
    {
    case 0:
        return "CHEATS_NONE_DESC";
    case 1:
        return "CHEATS_ENVIRONMENT_SECURE_DESC";
    case 2:
        return "CHEATS_ENVIRONMENT_POWER_DESC";
    case 3:
        return "CHEATS_ENVIRONMENT_VOLTAGE_DESC";
    case 4:
        return "CHEATS_ENVIRONMENT_TILT_DESC";
    case 5:
        return "CHEATS_ENVIRONMENT_WHITE_BALL_DESC";
    }
}

const char* GetLOCPlayerCheatName(int cheat)
{
    switch (cheat)
    {
    case 0:
        return "CHEATS_NONE";
    case 1:
        return "CHEATS_PLAYER_DEVASTATING";
    case 2:
        return "CHEATS_PLAYER_SAFE";
    case 3:
        return "CHEATS_PLAYER_SKILL_SHOT";
    case 4:
        return "CHEATS_PLAYER_GLASS_JAW";
    }
}

const char* GetLOCPlayerCheatDescription(int cheat)
{
    switch (cheat)
    {
    case 0:
        return "CHEATS_NONE_DESC";
    case 1:
        return "CHEATS_PLAYER_DEVASTATING_DESC";
    case 2:
        return "CHEATS_PLAYER_SAFE_DESC";
    case 3:
        return "CHEATS_PLAYER_SKILL_SHOT_DESC";
    case 4:
        return "CHEATS_PLAYER_GLASS_JAW_DESC";
    }
}

const char* GetLOCPowerupCheatName(int cheat)
{
    switch (cheat)
    {
    case 0:
        return "CHEATS_NONE";
    case 5:
        return "CHEATS_POWERUPS_ACCELERATOR";
    case 1:
        return "CHEATS_POWERUPS_EXPLOSIVES";
    case 2:
        return "CHEATS_POWERUPS_FREEZING";
    case 4:
        return "CHEATS_POWERUPS_GIANT";
    case 3:
        return "CHEATS_POWERUPS_SHELLS";
    case 10:
        return "CHEATS_POWERUPS_INFINITE";
    case 9:
        return "CHEATS_POWERUPS_SUPER";
    case 6:
        return "CHEATS_POWERUPS_PEELINOUT";
    case 7:
        return "CHEATS_POWERUPS_HEATSEEEKERS";
    case 8:
        return "CHEATS_POWERUPS_BOMBSAWAY";
    case 11:
        return "CHEATS_PLAYER_BUTTERFINGERS";
    }
}

const char* GetLOCPowerupCheatDescription(int cheat)
{
    switch (cheat)
    {
    case 0:
        return "CHEATS_NONE_DESC";
    case 5:
        return "CHEATS_POWERUPS_ACCELERATOR_DESC";
    case 1:
        return "CHEATS_POWERUPS_EXPLOSIVES_DESC";
    case 2:
        return "CHEATS_POWERUPS_FREEZING_DESC";
    case 4:
        return "CHEATS_POWERUPS_GIANT_DESC";
    case 3:
        return "CHEATS_POWERUPS_SHELLS_DESC";
    case 10:
        return "CHEATS_POWERUPS_INFINITE_DESC";
    case 9:
        return "CHEATS_POWERUPS_SUPER_DESC";
    case 6:
        return "CHEATS_POWERUPS_PEELINOUT_DESC";
    case 7:
        return "CHEATS_POWERUPS_HEATSEEEKERS_DESC";
    case 8:
        return "CHEATS_POWERUPS_BOMBSAWAY_DESC";
    case 11:
        return "CHEATS_PLAYER_BUTTERFINGERS_DESC";
    }
}
#pragma warning reset(10184)

bool IsEnvironmentCheatUnlocked(int cheat)
{
    bool unlocked = true;
    switch (cheat)
    {
    case 1:
        unlocked = IsSecureEnvironmentCheatUnlocked();
        break;
    case 2:
        unlocked = IsPowerEnvironmentCheatUnlocked();
        break;
    case 3:
        unlocked = IsVoltageEnvironmentCheatUnlocked();
        break;
    case 4:
        unlocked = IsTiltEnvironmentCheatUnlocked();
        break;
    case 5:
        unlocked = IsWhiteBallEnvironmentCheatUnlocked();
        break;
    }
    return unlocked;
}

bool IsPlayerCheatUnlocked(int cheat)
{
    bool unlocked = true;
    switch (cheat)
    {
    case 1:
        unlocked = IsDevastatingPlayerCheatUnlocked();
        break;
    case 2:
        unlocked = IsSafePlayerCheatUnlocked();
        break;
    case 3:
        unlocked = IsSkillShotPlayerCheatUnlocked();
        break;
    case 4:
        unlocked = IsGlassJawPlayerCheatUnlocked();
        break;
    }
    return unlocked;
}

bool IsPowerupCheatUnlocked(int cheat)
{
    bool unlocked = true;
    switch (cheat)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 10:
        unlocked = IsPowerupCheatsUnlocked();
        break;
    case 11:
        unlocked = IsButterfingersPlayerCheatUnlocked();
        break;
    case 9:
        unlocked = IsSuperPowerupsCheatUnlocked();
        break;
    }
    return unlocked;
}

void SetBreadcrumbs(int numBreadcrumbs, int currentBreadcrumb)
{
    SHNavigation* pNavigation = GetNavigationScene();
    if (pNavigation == 0)
    {
        return;
    }

    int middle = (int)std::ceil(17 / 2.0f);
    int half = (int)std::floor(numBreadcrumbs / 2.0f);
    int first = middle - half;
    int last = middle + half;
    bool odd = (bool)(numBreadcrumbs % 2);

    TLComponentInstance* pButton = pNavigation->GetButton(NAVIGATION_BUTTON_BREADCRUMBS);
    TLComponentInstance* pBreadcrumbs = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
        pButton->GetActiveSlide(), "breadcrumb_17");

    for (int i = 0; i < 17; ++i)
    {
        char name[16];
        nlSNPrintf(name, sizeof(name), "breadcrumb_%d", i + 1);
        TLComponentInstance* pBreadcrumb = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
            pBreadcrumbs->GetActiveSlide(), name);
        TLComponentInstance* pBox = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
            pBreadcrumb->GetActiveSlide(), "box");
        TLImageInstance* pDot = FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
            pBreadcrumb->GetActiveSlide(), "breadcrumb_dot3");

        if (!odd)
        {
            if (i == middle - 1)
            {
                pBox->m_bVisible = false;
                pDot->m_bVisible = false;
            }
            else if (i + 1 < first || i + 1 > last)
            {
                pBox->m_bVisible = false;
                pDot->m_bVisible = false;
            }
            else
            {
                int index = i - first + 1;
                if (i >= middle)
                {
                    index = i - first;
                }
                if (index == currentBreadcrumb)
                {
                    pBox->m_bVisible = true;
                    pDot->m_bVisible = false;
                }
                else
                {
                    pBox->m_bVisible = false;
                    pDot->m_bVisible = true;
                }
            }
        }
        else
        {
            if (i + 1 < first || i + 1 > last)
            {
                pBox->m_bVisible = false;
                pDot->m_bVisible = false;
            }
            else
            {
                int index = i - first + 1;
                if (index == currentBreadcrumb)
                {
                    pBox->m_bVisible = true;
                    pDot->m_bVisible = false;
                }
                else
                {
                    pBox->m_bVisible = false;
                    pDot->m_bVisible = true;
                }
            }
        }
    }
}

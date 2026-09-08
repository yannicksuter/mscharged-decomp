#include "Game/FE/feHelpFuncs.h"

#include "Game/DB/CharacterInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feModelManager.h"
#include "Game/FE/feText.h"
#include "Game/FE/tlComponent.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "NL/MemAlloc.h"
#include "NL/nlString.h"
#include "Game/FE/fePointerButton.h"
#include "NL/nlstring_tmpl.h"

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
    return StandardAllocator.LargestFreeBlock();
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

static const float sDoneButtonBounds[4] = { -84.0f, 84.0f, -165.0f, -259.0f };
static const float sPlayButtonBounds[4] = { -84.0f, 84.0f, -165.0f, -259.0f };

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

void SetPlayButtonBounds(
    FEPointerButton* component, TLComponentInstance*)
{
    component->SetBounds(sPlayButtonBounds[0], sPlayButtonBounds[1],
        sPlayButtonBounds[2], sPlayButtonBounds[3]);
}


static float sCharacterIdleTime;

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

void ResetCharacterIdleAnimation(FEModelHandle* model)
{
    if (model != 0)
    {
        model->PlayAnimation("fe_idle", PM_CYCLIC, 0.2f, 0.0f, false);
    }
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

#include "Game/FE/feHelpFuncs.h"

#include "Game/DB/CharacterInfo.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feModelManager.h"
#include "Game/FE/tlComponent.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "NL/MemAlloc.h"
#include "NL/nlString.h"
#include "unclassified/tu_80219248.h"

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

extern "C" const char* fn_801CBE28(int captain)
{
    return GetCharacterInfo(GetCharacterIndexFromCaptain(captain)).mDisplayNameKey;
}

extern "C" const char* fn_801CBE50(int captain)
{
    return GetCharacterInfo(GetCharacterIndexFromCaptain(captain)).mDisplayNameKey;
}

extern "C" int fn_801CBE78(int captain)
{
    return GetCharacterIndexFromCaptain(captain);
}

extern "C" int fn_801CBE7C(int sidekick)
{
    return GetCharacterIndexFromSidekick(sidekick);
}

extern "C" const char* fn_801CBE80(int captain)
{
    return GetCharacterInfo(GetCharacterIndexFromCaptain(captain)).mName;
}

extern "C" const char* fn_801CBEA8(int sidekick)
{
    return GetCharacterInfo(GetCharacterIndexFromSidekick(sidekick)).mName;
}

extern "C" int fn_801CBED0(const char* name)
{
    return GetCharacterInfo(GetCharacterIndexFromName(name)).mCaptainId;
}

extern "C" int fn_801CBEF8(const char* name)
{
    return GetCharacterInfo(GetCharacterIndexFromName(name)).mSidekickId;
}

extern "C" const char* fn_801B6188(int stadium);

extern "C" int fn_801CBF20(const char* name)
{
    int stadium = -1;
    for (int i = 0; i < 18; ++i)
    {
        if (nlStrICmp(fn_801B6188(i), name) == 0)
        {
            stadium = i;
            break;
        }
    }
    return stadium;
}

extern "C" const char* fn_801CBFF4(int mode)
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

extern "C" unsigned long fn_801CC090(int captain)
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
    return CHARACTER_ACCEPT_SOUNDS[captain];
}

extern "C" unsigned long fn_801CC0A4(int sidekick)
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
    return SIDEKICK_SOUNDS[sidekick];
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

extern "C" bool fn_8011050C();
extern "C" bool fn_801105B8();
extern "C" bool fn_80110660();
extern "C" bool fn_80110708();
extern "C" bool fn_801107B0();
extern "C" bool fn_80110858();
extern "C" bool fn_80110900();
extern "C" bool fn_801109A8();
extern "C" bool fn_80110A50();
extern "C" bool fn_80110AF8();
extern "C" bool fn_80110BA0();
extern "C" bool fn_80110C48();

extern "C" bool fn_801CCE30(int cheat)
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
        unlocked = fn_80110858();
        break;
    case 11:
        unlocked = fn_80110C48();
        break;
    case 9:
        unlocked = fn_80110900();
        break;
    }
    return unlocked;
}

extern "C" bool fn_801CCDB8(int cheat)
{
    bool unlocked = true;
    switch (cheat)
    {
    case 1:
        unlocked = fn_801109A8();
        break;
    case 2:
        unlocked = fn_80110A50();
        break;
    case 3:
        unlocked = fn_80110AF8();
        break;
    case 4:
        unlocked = fn_80110BA0();
        break;
    }
    return unlocked;
}

extern "C" bool fn_801CCD30(int cheat)
{
    bool unlocked = true;
    switch (cheat)
    {
    case 1:
        unlocked = fn_8011050C();
        break;
    case 2:
        unlocked = fn_801105B8();
        break;
    case 3:
        unlocked = fn_80110660();
        break;
    case 4:
        unlocked = fn_80110708();
        break;
    case 5:
        unlocked = fn_801107B0();
        break;
    }
    return unlocked;
}

extern "C" const char* fn_801CCC80(int cheat)
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

extern "C" const char* fn_801CCBD0(int cheat)
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

extern "C" const char* fn_801CCB68(int cheat)
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

extern "C" const char* fn_801CCB00(int cheat)
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

extern "C" const char* fn_801CCA84(int cheat)
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

extern "C" const char* fn_801CCA08(int cheat)
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

static const float lbl_804E82F8[4] = { -84.0f, 84.0f, -165.0f, -259.0f };
static const float lbl_804E8308[4] = { -84.0f, 84.0f, -165.0f, -259.0f };

extern "C" void fn_801CC9B0(
    TU80219248Component* component, TLComponentInstance*, int value)
{
    if (value)
    {
        component->fn_801CD9D0(lbl_804E8308[0], lbl_804E8308[1],
            lbl_804E8308[2], lbl_804E8308[3]);
    }
    else
    {
        component->fn_801CD9D0(lbl_804E82F8[0], lbl_804E82F8[1],
            lbl_804E82F8[2], lbl_804E82F8[3]);
    }
}

extern "C" void fn_801CC988(
    TU80219248Component* component, TLComponentInstance*)
{
    component->fn_801CD9D0(lbl_804E8308[0], lbl_804E8308[1],
        lbl_804E8308[2], lbl_804E8308[3]);
}

extern "C" void fn_801CC864(FEModelHandle* model);

static float lbl_806E1850;

extern "C" void fn_801CC888(float dt)
{
    lbl_806E1850 += dt;
    if (lbl_806E1850 >= 2.0f)
    {
        lbl_806E1850 = 0.0f;
    }

    if (lbl_806E1850 == 0.0f)
    {
        for (int i = 0; i < 2; ++i)
        {
            if (nlRandom(100, &nlDefaultSeed) < 25)
            {
                FEModelHandle* model = fn_801C2FB4(
                    FEModelManager::Instance(),
                    i == 0 ? "homemodel" : "awaymodel");
                if (model != 0 && model->IsLoaded()
                    && model->IsPlayingAnimation("fe_idle"))
                {
                    model->PlayAnimation(
                        "fe_idle_action_01", PM_HOLD, 0.2f, 0.0f, false);
                    model->SetAnimationCompleteCallback(fn_801CC864);
                }
            }
        }
    }
}

extern "C" void fn_801CC864(FEModelHandle* model)
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
        : textInstance.m_component->m_BoxSize;
    nlVector2 bb = boxSize;
    bb.x = 999.9f;
    textInstance.m_OverloadedAttributes.BoxSize = bb;
    textInstance.m_OverloadFlags |= 0x4;
}

#include "NL/nlIntersection.h"
#include "Game/AI/FielderAbility.h"
#include "Game/AI/DesireSuperPower.h"
#include "Game/AI/Fuzzy.h"

#include "Game/AI/AIPad.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/AvoidableObject.h"
#include "Game/AI/DesireUpdate.inl"
#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/AI/TeamPlayMachine.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/CharacterTriggers.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DebugWriteCache.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Effects/EmitterCallbacks.h"
#include "Game/Event.h"
#include "Game/Game.h"
#include "Game/Goalie.h"
#include "Game/MathHelpers.h"
#include "Game/EventDataTypes.h"
#include "Game/Field.h"
#include "Game/PoseAccumulator.h"
#include "Game/GameInfo.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/DaisyFist.h"
#include "Game/Render/FlyingCamera.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/RumbleActions.h"
#include "Game/Sys/audio.h"
#include "Game/Team.h"
#include "NL/nlMath.h"
#include "NL/nlString.h"
#include "Game/Render/YoshiEggObject.h"
#include "Game/FE/Overlay/OverlayHandlerSuperAbility.h"
#include "Game/Physics/PhysicsWaluigiWall.h"
#include <stdlib.h>
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Audio/AudioResourceRuntime.h"
#include "Game/EventRegistry.h"

extern "C" cGame* g_pGame;
extern "C" const nlVector3* fn_80040234(cFielder*);
extern "C" void fn_8003EBD0(cFielder*, int, UnidentifiedVariantCollection*);
extern "C" float fn_800DAFCC(const nlVector3*, const nlVector3*, cFielder*,
    int, float, float, float, float);
extern "C" void fn_803198F4();
extern "C" bool fn_8002EDC8(cFielder*, int);
extern "C" void fn_8002E340(cFielder*);
extern "C" void fn_8002E3F8(cFielder*);
extern "C" void fn_80060A00(void*, cFielder*);
extern "C" int fn_800CFCC8(cFielder*, const unsigned short*, int,
    const nlVector2*, float*);
extern FuzzyVariant fvNotSet;
extern "C" float fabsf(float);
// Shared position constants used by the super-power desires.
extern const nlVector3 lbl_804DC338 = { 0.0f, 0.0f, 0.0f };
extern "C" const nlVector2 lbl_804DC348[6] = {
    { 3.5f, -6.0f }, { 8.0f, -6.0f }, { 8.0f, 3.0f },
    { 12.0f, 3.0f }, { 12.0f, 10.0f }, { 15.0f, 10.0f }
};
extern "C" const nlVector2 lbl_804DC378[2] = {
    { -5.15f, 9.375f }, { -5.15f, -9.375f }
};
extern const nlVector3 lbl_804DC388 = { 0.0f, 0.0f, 0.0f };

// Runtime tunables and script parameters retained in small data.
float lbl_806DC240 = 2e+01f;
float lbl_806DC244 = 2.1f;
float lbl_806DC248 = 2.95f;
float lbl_806DC24C = 7.2f;
float lbl_806DC250 = 0.45f;
float lbl_806DC254 = 0.25f;
float lbl_806DC258 = 1e+01f;
float lbl_806DC25C = 3.73f;
float lbl_806DC260 = 1.35f;
float lbl_806DC264 = 1.75f;
float lbl_806DC268 = 0.6f;
float lbl_806DC26C = 0.5f;
float lbl_806DC270 = 0.5f;
float lbl_806DC274 = 0.3f;
float lbl_806DC278 = 0.5f;
float lbl_806DC27C = 1.0f;
float lbl_806DC280 = 0.13f;
float lbl_806DC284 = 0.21f;
float lbl_806DC288 = 1e+01f;
float lbl_806DC28C = 23.0f;
float lbl_806DC290 = 0.02f;
float lbl_806DC294 = 0.02f;
float lbl_806DC298 = 0.38f;
float lbl_806DC29C = -1.5f;
int lbl_806DC2A0 = 40;
float lbl_806DC2A4 = 9e+01f;
float lbl_806DC2A8 = 8.0f;
float lbl_806DC2AC = 27.0f;
float lbl_806DC2B0 = 6e+01f;
float lbl_806DC2B4 = 0.02f;
float lbl_806DC2B8 = 0.6f;
float lbl_806DC2BC = 3.0f;
float lbl_806DC2C0 = 45.0f;
float lbl_806DC2C4 = 0.4f;
float lbl_806DC2C8 = 1.33f;
float lbl_806DC2CC = 1.6f;
float lbl_806DC2D0 = 1e+01f;
float lbl_806DC2D4 = 1234567.0f;
float lbl_806DC2D8 = 0.1f;
int lbl_806DC2DC = 6;
float lbl_806DC2E0 = 1234567.0f;
float lbl_806DC2E4 = 0.25f;
float lbl_806DC2E8 = 5.0f;
float lbl_806DC2EC = 0.7f;
float lbl_806DC2F0 = 1.0f;
float lbl_806DC2F4 = 18.5f;
bool lbl_806DC2F8 = true;
float lbl_806DC2FC = 1234567.0f;
float lbl_806DC300 = 2.75f;
float lbl_806DC304 = 0.6f;
float lbl_806DC308 = 2.75f;
float lbl_806DC30C = 3.0f;
float lbl_806DC310 = 0.05f;
bool lbl_806DC314 = true;
float lbl_806DC318 = 1234567.0f;
float lbl_806DC31C = 1234567.0f;
float lbl_806DC320 = 1234567.0f;
float lbl_806DC324 = 1234567.0f;
float lbl_806DC328 = 1234567.0f;
float lbl_806DC32C = 5.5f;
static unsigned short sDesireSuperPowerType = 0xFFFF;
float lbl_806DC334 = 3.0f;
#pragma explicit_zero_data on
int lbl_806DC338 = 0;
#pragma explicit_zero_data off
float lbl_806DC33C = 2.0f;
float lbl_806DC340 = 2.0f;
float lbl_806DC344 = 5.5f;
float lbl_806DC348 = 3.0f;
float lbl_806DC34C = 2.0f;
float lbl_806DC350 = 3.0f;
#pragma explicit_zero_data on
int lbl_806DC354 = 0;
#pragma explicit_zero_data off
int lbl_806DC358 = 1;
eFielderDesireState lbl_806DC35C = (eFielderDesireState)19;
eFielderDesireState lbl_806DC360 = (eFielderDesireState)12;
bool lbl_806DC364 = true;
float lbl_806DC368 = 2.0f;
#pragma explicit_zero_data on
int lbl_806DC36C = 0;
#pragma explicit_zero_data off
int lbl_806DC370 = 1;
eFielderDesireState lbl_806DC374 = (eFielderDesireState)19;
eFielderDesireState lbl_806DC378 = (eFielderDesireState)19;
eFielderDesireState lbl_806DC37C = (eFielderDesireState)12;
bool lbl_806DC380 = true;
float lbl_806DC384 = 5.5f;
float lbl_806DC388 = 3.0f;
float lbl_806DC38C = 3.0f;
float lbl_806DC390 = 3.0f;
#pragma explicit_zero_data on
int lbl_806DC394 = 0;
#pragma explicit_zero_data off

/**
 * Offset/Address/Size: 0x0 | 0x800C86FC | size: 0x60
 */
DesireSuperPower::DesireSuperPower()
    : Desire(23, TransitionFunc(g_UnsetTransitionFunc))
    , mpDKShockAvoidable(0)
    , mpTarget(0)
{
}

/**
 * Offset/Address/Size: 0x60 | 0x800C875C | size: 0x274
 */
void DesireSuperPower::UnidentifiedSetContext(
    UnidentifiedScriptMachine* context)
{
    Desire::UnidentifiedSetContext(context);

    if (m_pFielder->mUnidentified024.m_eCharacterClass == YOSHI)
    {
        UnidentifiedFindEvent<void>("CollisionPatchGround", -1)->Add(Function<void*>(fn_800D1140), 0, -1);
        UnidentifiedFindEvent<void>("CollisionPatchPlayer", -1)->Add(Function<void*>(fn_800D1140), 0, -1);
        UnidentifiedFindEvent<void>("CollisionPatchWall", -1)->Add(Function<void*>(fn_800D12E8), 0, -1);
    }
}

/**
 * Offset/Address/Size: 0x2D4 | 0x800C89D0 | size: 0x5A8
 */
bool DesireSuperPower::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    fn_8002E340(m_pFielder);

    switch (m_pFielder->mUnidentified024.m_eCharacterClass)
    {
    case DAISY:
    {
        mpTarget = 0;
        fn_8002E3F8(m_pFielder);
        mUnidentified078 = lbl_806DC320;
        m_pFielder->fn_800501F0(
            (bool)UserControlledT(m_pFielder->m_pTeam));
        EmitBowserSmoke(m_pFielder);
        bool userControlled = (bool)m_pFielder->GetGlobalPad();
        if (!userControlled
            && (m_pFielder->fn_8002E060() == 21
                || m_pFielder->fn_8002E060() == 18
                || m_pFielder->fn_8002E060() == 9))
        {
            m_pFielder->EndDesire();
        }
        result = true;
        break;
    }
    case WALUIGI:
        result = fn_800C9F4C(this, context);
        break;
    case DONKEYKONG:
        m_pFielder->InitDesire(
            (eFielderDesireState)21, 0.5f, -1.0f, fvNotSet, fvNotSet);
        m_pFielder->SetAction((eFielderActionState)29);
        m_pFielder->muInvincibleStatus |= 1;
        m_pFielder->SetAnimState(104, true, 0.2f, false, false);
        m_pFielder->InitMovementFromAnim(
            0, lbl_804DC338, 1.0f, false);
        mUnidentified078 = lbl_806DC2D4;
        result = m_pFielder->m_eActionState == (eFielderActionState)29;
        break;
    case WARIO:
        result = fn_800CAC8C(this, context);
        break;
    case HAMMERBROS:
        mpDKShockAvoidable = new (nlMalloc(sizeof(AvoidablePoint), 8, false))
            AvoidablePoint(AVOID_BOWSER,
                (const nlVector2&)m_pFielder->mUnidentified024.m_v3Position,
                4.0f + lbl_806DB9D8);
        m_pFielder->fn_8004F828();
        mUnidentified078 = lbl_806DC318;
        result = m_pFielder->m_eActionState == (eFielderActionState)29;
        break;
    case KOOPA:
        m_pFielder->m_pTweaks = m_pFielder->mUnidentified328;
        m_pFielder->fn_8001EE74(lbl_806DC25C, lbl_806DC250, -1.0f);
        EmitSuperGrow(m_pFielder);
        mUnidentified078 = lbl_806DC258;
        fn_800367B4(m_pFielder);
        result = true;
        break;
    case BIRDO:
        m_pFielder->m_pTweaks = m_pFielder->mUnidentified328;
        m_pFielder->fn_8001EE74(lbl_806DC25C, lbl_806DC250, -1.0f);
        EmitSuperGrow(m_pFielder);
        mUnidentified078 = lbl_806DC258;
        fn_800367B4(m_pFielder);
        result = true;
        break;
    case LUIGI:
        m_pFielder->fn_8004FB04();
        mUnidentified078 = lbl_806DC31C;
        result = m_pFielder->m_eActionState == (eFielderActionState)29;
        break;
    case YOSHI:
    {
        mpTarget = 0;
        fn_8002E3F8(m_pFielder);
        mUnidentified078 = lbl_806DC328;
        m_pFielder->fn_800501F0(
            (bool)UserControlledT(m_pFielder->m_pTeam));
        m_pFielder->mUnidentified408 = 0.0f;
        m_pFielder->mUnidentified40C = 0.0f;
        bool userControlled = (bool)m_pFielder->GetGlobalPad();
        if (!userControlled
            && (m_pFielder->fn_8002E060() == 21
                || m_pFielder->fn_8002E060() == 18
                || m_pFielder->fn_8002E060() == 9))
        {
            m_pFielder->EndDesire();
        }
        result = true;
        break;
    }
    case MARIO:
        mpTarget = 0;
        mUnidentified078 = lbl_806DC324;
        m_pFielder->fn_800501F0(
            (bool)UserControlledT(m_pFielder->m_pTeam));
        result = true;
        break;
    case PEACH:
    {
        mpTarget = 0;
        mUnidentified078 = lbl_806DC324;
        m_pFielder->fn_800501F0(
            (bool)UserControlledT(m_pFielder->m_pTeam));
        bool userControlled = (bool)m_pFielder->GetGlobalPad();
        if (!userControlled
            && (m_pFielder->fn_8002E060() == 21
                || m_pFielder->fn_8002E060() == 18
                || m_pFielder->fn_8002E060() == 9))
        {
            m_pFielder->EndDesire();
        }
        result = true;
        break;
    }
    case TOAD:
        result = fn_800D0DB0(this, context);
        break;
    }

    if (result)
    {
        mUnidentified018->SetTransition("SuperPowerPlayDesire");
        fn_800A6968(m_pFielder->m_pTeam);
        fn_803198F4();
        cFielder* fielder = m_pFielder;
        if (fielder->m_pBall != 0 && g_pGame->IsGameplayOrOvertime())
        {
            gSuperAbilityTeam = (eTeamID)GameInfoManager::Instance()->GetTeam(fielder->m_pTeam->m_nSide);
            ((SuperAbilityOverlay*)g_pOverlayManager->GetScene((SceneList)102))->Start();
            PlaySound(fielder->m_uSoundSlotId, 0x790F135F, 0, 0);
            fn_80060A00(g_pGame, fielder);
        }

        unsigned long sound = PowerupBase::GetSoundType(
            (ePowerUpType)m_pFielder->mUnidentified11C->unknown_0x14,
            PowerupBase::PWRUP_SOUND_ACTIVATE);
        if (m_pFielder->mUnidentified024.m_eCharacterClass == BIRDO
            || m_pFielder->mUnidentified024.m_eCharacterClass == KOOPA)
        {
            PlayCaptainPowerupStream(18, sound, m_pFielder);
            u32 hash = nlStringLowerHash("MarioPowerup");
            fn_802F4E84(&hash, 0, 0);
            PauseSuddenDeathMusic();
        }
        else
        {
            PlayCaptainPowerupStream(m_pFielder->m_uSoundSlotId,
                sound, m_pFielder);
        }
    }
    return result;
}

/**
 * Offset/Address/Size: 0x87C | 0x800C8F78 | size: 0x174
 */
void DesireSuperPower::Update(
    DesireUpdate* update, float fDeltaT)
{
    if (!fn_800D1458(g_pGame))
    {
        *update = 1;
        return;
    }

    switch (fn_800D1440(m_pFielder))
    {
    case DAISY:
        fn_800C93A4(update, fDeltaT);
        break;
    case WALUIGI:
        fn_800CA07C(update, fDeltaT);
        break;
    case DONKEYKONG:
        fn_800CA57C(update, fDeltaT);
        break;
    case WARIO:
        fn_800CADBC(update, fDeltaT);
        break;
    case HAMMERBROS:
        fn_800CB2BC(update, fDeltaT);
        break;
    case KOOPA:
        fn_800CB7A8(update, fDeltaT);
        break;
    case BIRDO:
        fn_800CBF64(update, fDeltaT);
        break;
    case LUIGI:
        fn_800CC720(update, fDeltaT);
        break;
    case YOSHI:
        fn_800CCC0C(update, fDeltaT);
        break;
    case MARIO:
        fn_800CD61C(update, fDeltaT);
        break;
    case PEACH:
        fn_800D01A0(update, fDeltaT);
        break;
    case TOAD:
        fn_800D0EAC(update, fDeltaT);
        break;
    }
}

/**
 * Offset/Address/Size: 0x9F0 | 0x800C90EC | size: 0x2B8
 */
void DesireSuperPower::UnidentifiedCleanup()
{
    fn_800A6968(m_pFielder->m_pTeam);
    fn_803198F4();

    switch (m_pFielder->mUnidentified024.m_eCharacterClass)
    {
    case DAISY:
        m_pFielder->fn_80050008();
        m_pFielder->m_pTeam->ClearCurrentPowerUp();
        EndBowserSmoke(m_pFielder);
        break;
    case WALUIGI:
        fn_80038158(m_pFielder, 0);
        {
            EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup(
                "bowserjr_shriek_mouth");
            if (group != 0)
            {
                EmissionManager::Instance()->Kill(group);
            }
        }
        break;
    case DONKEYKONG:
        fn_80038158(m_pFielder, 0);
        break;
    case WARIO:
        m_pFielder->mUnidentified181 = false;
        m_pFielder->mUnidentified182 = false;
        fn_80038158(m_pFielder, 0);
        break;
    case HAMMERBROS:
        KillDKSuperCharge(m_pFielder);
        fn_80038158(m_pFielder, 0);
        delete (AvoidablePoint*)mpDKShockAvoidable;
        mpDKShockAvoidable = 0;
        break;
    case KOOPA:
        m_pFielder->m_pTweaks
            = m_pFielder->mUnidentified32C;
        m_pFielder->fn_8001EE74(1.0f, lbl_806DC254, 1.0f);
        EmitSuperShrink(m_pFielder);
        break;
    case BIRDO:
        m_pFielder->m_pTweaks
            = m_pFielder->mUnidentified32C;
        m_pFielder->fn_8001EE74(1.0f, lbl_806DC254, 1.0f);
        EmitSuperShrink(m_pFielder);
        break;
    case LUIGI:
        m_pFielder->fn_8004FA34();
        SetFlyingCameraTarget((cFielder*)0);
        break;
    case YOSHI:
        fn_80038158(m_pFielder, 0);
        m_pFielder->mUnidentified404 = 0.0f;
        m_pFielder->mUnidentified408 = 0.0f;
        m_pFielder->fn_80050008();
        m_pFielder->m_pTeam->ClearCurrentPowerUp();
        if (m_pFielder->m_eAnimID == 104)
        {
            m_pFielder->EndDesire();
            m_pFielder->StartRunning();
        }
        break;
    case MARIO:
        if (m_pFielder->fn_8002E060() == 12)
        {
            m_pFielder->EndDesire();
        }
        m_pFielder->fn_80050008();
        m_pFielder->m_pTeam->ClearCurrentPowerUp();
        break;
    case PEACH:
        m_pFielder->fn_80050008();
        m_pFielder->m_pTeam->ClearCurrentPowerUp();
        if (m_pFielder->m_eAnimID == 104)
        {
            m_pFielder->EndDesire();
            m_pFielder->StartRunning();
        }
        break;
    case TOAD:
        m_pFielder->m_pTweaks
            = m_pFielder->mUnidentified32C;
        fn_80039CF0(m_pFielder, 0);
        m_pFielder->bYoshiInWindup = false;
        EmitYoshiShellBreak(m_pFielder);
        gNPCManager->mUnidentified024->Break();
        break;
    }

    unsigned long sound = PowerupBase::GetSoundType(
        (ePowerUpType)m_pFielder->mUnidentified11C->unknown_0x14,
        PowerupBase::PWRUP_SOUND_ACTIVATE);
    StopCaptainPowerupStream(sound, m_pFielder);
    if ((m_pFielder->mUnidentified024.m_eCharacterClass == BIRDO)
        || (m_pFielder->mUnidentified024.m_eCharacterClass == KOOPA))
    {
        ResumeSuddenDeathMusic();
        u32 hash = nlStringLowerHash("MarioPowerup");
        fn_802F4E84(&hash, 1, 0);
    }
}

void DesireSuperPower::fn_800C93A4(DesireUpdate* update, float fDeltaT)
{
    if (update->mData.i == 3)
    {
        if (update->ExtraData.Get(11)->mData.b)
        {
            mpTarget = (cFielder*)update->ExtraData.Get(14)->mData.pointer;
            if (!m_pFielder->mUnidentified3DC)
            {
                m_pFielder->fn_80050284();
            }
        }
        else if (m_pFielder->mUnidentified3DC)
        {
            m_pFielder->fn_8005001C(false);
        }
        *update = 0;
    }
    if (update->mData.i == 0)
    {
        bool active = m_pFielder->mUnidentified3E0 > 0.0f;
        if (!active)
        {
            *update = 1;
            return;
        }
        if (m_pFielder->mUnidentified3DC)
        {
            if (!fn_8002EDC8(m_pFielder, -1))
            {
                m_pFielder->fn_8005001C(true);
                return;
            }
            m_pFielder->fn_800D0534(fDeltaT);
            bool active = m_pFielder->mUnidentified3E0 > 0.0f;
            if (active)
            {
                m_pFielder->mUnidentified3E8.fireballStageTime += fDeltaT;
                m_pFielder->mUnidentified3E8.nextFireballTime -= fDeltaT;
                if (m_pFielder->mUnidentified3E8.nextFireballTime <= 0.0f)
                {
                    nlVector3 pos;
                    nlVector3 direction;
                    const nlMatrix4& mat = m_pFielder->m_pPoseAccumulator->GetNodeMatrix(
                        m_pFielder->m_nHeadJointIndex);
                    nlVec3Set(direction, mat.m21, mat.m22, mat.m23);
                    nlVec3ScaleAdd(pos, lbl_806DC26C, direction, (const nlVector3&)mat.m41);
                    nlVector3 flat = direction;
                    flat.z = -0.01f;
                    float inverseLength = nlRecipSqrt(nlVec3DotProduct(flat, flat), true);
                    nlVec3Set(flat, inverseLength * flat.x, inverseLength * flat.y, inverseLength * flat.z);
                    if (nlVec3DotProduct(direction, flat) > 0.9f)
                    {
                        direction = flat;
                    }
                    float stage = InterpolateRangeClamped(0.0f, 1.0f, 0.0f, lbl_806DC270,
                        m_pFielder->mUnidentified3E8.fireballStageTime);
                    m_pFielder->mUnidentified3E8.nextFireballTime
                        += Interpolate(lbl_806DC290, lbl_806DC294, stage);
                    float speed = Interpolate(lbl_806DC288, lbl_806DC28C, stage);
                    nlVec3ScaleAdd(direction, speed,
                        direction, m_pFielder->mUnidentified024.m_v3Velocity);
                    float radius = Interpolate(lbl_806DC278, lbl_806DC27C, stage);
                    float lifetime = Interpolate(lbl_806DC280, lbl_806DC284, stage);
                    lbl_806E12C8->fn_801743A8(1, m_pFielder, pos, direction,
                        lbl_806DC274, radius, lifetime);
                    PlayRumbleAction(1, m_pFielder->GetGlobalPad());
                }
            }
            else
            {
                *update = 1;
            }
        }
        else
        {
            m_pFielder->mUnidentified3E8.nextFireballTime = 0.0f;
        }
    }
}

/**
 * Offset/Address/Size: 0x1678 | 0x800C9D74 | size: 0x40
 */
extern "C" void fn_800C9D74(DesireSuperPower* self, int param)
{
    if (param != 0)
    {
        self->m_pFielder->fn_8004FF40();
    }
    fn_80316968(self);
}

/**
 * Offset/Address/Size: 0x16B8 | 0x800C9DB4 | size: 0x198
 */
void EmitBowserJrShriek(DesireSuperPower* self)
{
    fn_80038158(self->m_pFielder, 0);
    nlVector3 vel;
    vel.z = 0.0f;
    nlPolarToCartesian(vel.x, vel.y,
        self->m_pFielder->mUnidentified024.m_aActualFacingDirection, 1.0f);
    nlVec3Scale(vel, vel, lbl_806DC2F4);
    nlVector3 pos = self->m_pFielder->GetJointPosition(
        self->m_pFielder->m_nHeadJointIndex);
    PhysicsPatch* patch = lbl_806E12C8->fn_801743A8(7,
        self->m_pFielder, pos, vel,
        lbl_806DC2E4, lbl_806DC2E8, lbl_806DC2EC);
    patch->fn_80173B08(lbl_806DC2F0);
    PlayRumbleAction(1, self->m_pFielder->GetGlobalPad());
    EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup(
        "bowserjr_shriek_mouth");
    if (group != 0)
    {
        EmissionController* controller = EmissionManager::Instance()->Create(
            group, 3, true, 0);
        controller->m_uUserData = (u32)self->m_pFielder;
        controller->SetPosition(
            self->m_pFielder->mUnidentified024.m_v3Position);
        controller->SetVelocity(
            self->m_pFielder->mUnidentified024.m_v3Velocity);
        controller->SetUpdateCallback(
            Function1<void, EmissionController&>(
                UpdateEmitterFromCharacterForward));
    }
}

/**
 * Offset/Address/Size: 0x1850 | 0x800C9F4C | size: 0x130
 */
extern "C" bool fn_800C9F4C(DesireSuperPower* self, void*)
{
    if (self->m_pFielder->GetGlobalPad() != 0)
    {
        if (self->m_pFielder->m_pController
                ->GetMovementStickMagnitude() > 0.01f)
        {
            self->m_pFielder->m_pController
                ->GetMovementStickDirection();
        }
    }
    short dir = 0;
    cFielder* target = FindPowerupTarget(
        self->m_pFielder, (ePowerUpType)-1);
    self->mpTarget = target;
    if ((target != 0) && (lbl_806DC2F8 != 0))
    {
        dir = self->m_pFielder->GetFacingDeltaToPosition(
            target->mUnidentified024.m_v3Position);
    }
    self->m_pFielder->InitDesire(
        (eFielderDesireState)21, 0.5f, -1.0f, fvNotSet, fvNotSet);
    self->m_pFielder->SetAction((eFielderActionState)29);
    self->m_pFielder->muInvincibleStatus |= 1;
    self->m_pFielder->SetAnimState(104, true, 0.2f, false, false);
    self->m_pFielder->InitMovementFromAnim(
        dir, lbl_804DC338, 0.15f, false);
    self->mUnidentified078 = lbl_806DC2E0;
    return self->m_pFielder->m_eActionState
        == (eFielderActionState)29;
}

/**
 * Offset/Address/Size: 0x1980 | 0x800CA07C | size: 0x500
 */
void DesireSuperPower::fn_800CA07C(DesireUpdate* update, float fDeltaT)
{
    if (m_pFielder->ShouldStartCrossBlend(4))
    {
        m_pFielder->EndAction();
    }

    if (m_pFielder->IsActionDone() || !g_pGame->IsGameplayOrOvertime())
    {
        m_pFielder->EndDesire();
        *update = 1;
    }
    if (m_pFielder->m_eActionState != (eFielderActionState)29)
    {
        *update = 1;
    }
}

/**
 * Offset/Address/Size: 0x1E80 | 0x800CA57C | size: 0x59C
 */
void DesireSuperPower::fn_800CA57C(DesireUpdate* update, float fDeltaT)
{
    if (m_pFielder->ShouldStartCrossBlend(4))
    {
        m_pFielder->EndAction();
    }

    if (m_pFielder->IsActionDone() || !g_pGame->IsGameplayOrOvertime())
    {
        m_pFielder->EndDesire();
        *update = 1;
    }
    if (m_pFielder->m_eActionState != (eFielderActionState)29)
    {
        *update = 1;
    }

    if (m_pFielder->m_pCurrentAnimController->TestTrigger(lbl_806DC2D8))
    {
        int step = 65536 / lbl_806DC2DC;
        unsigned short angle = m_pFielder->mUnidentified024.m_aActualFacingDirection;
        for (int i = 0; i < lbl_806DC2DC; i++)
        {
            DaisyFistObject* fist = gNPCManager->fn_801A9CA4(-1);
            if (fist != 0)
            {
                fist->Spawn(m_pFielder, angle);
            }
            angle += step;
        }
    }
    else if (m_pFielder->m_pCurrentAnimController->TestTrigger(0.04f + lbl_806DC2D8))
    {
        fn_80038158(m_pFielder, 0);
    }
}

void DesireSuperPower::fn_800CAB18()
{
    fn_80038158(m_pFielder, 0);
    nlVector3 direction;
    nlPolarToCartesian(direction.x, direction.y,
        m_pFielder->mUnidentified024.m_aActualFacingDirection, 1.0f);
    direction.z = 0.0f;
    nlVector3 pos = m_pFielder->GetJointPosition(
        m_pFielder->m_nRightHandJointIndex);
    nlVector3 offset = direction;
    pos.z = 0.0f;
    float inverseLength = nlRecipSqrt(nlVec3DotProduct(offset, offset), true);
    nlVec3Set(offset, inverseLength * offset.x, inverseLength * offset.y, inverseLength * offset.z);
    nlVec3Scale(offset, offset, lbl_806DC300 + lbl_806DC304);
    nlVec3Add(pos, pos, offset);
    cField::FixOutOfBoundsPosition(pos, 0.9f * lbl_806DC308, true);
    PhysicsPatch* patch = lbl_806E12C8->fn_801743A8(2,
        m_pFielder, pos, lbl_804DC338,
        lbl_806DC300, lbl_806DC308, lbl_806DC30C);
    patch->fn_80173B08(lbl_806DC310);
    PlayRumbleAction(1, m_pFielder->GetGlobalPad());
}

/**
 * Offset/Address/Size: 0x2590 | 0x800CAC8C | size: 0x130
 */
extern "C" bool fn_800CAC8C(DesireSuperPower* self, void*)
{
    if (self->m_pFielder->GetGlobalPad() != 0)
    {
        if (self->m_pFielder->m_pController
                ->GetMovementStickMagnitude() > 0.01f)
        {
            self->m_pFielder->m_pController
                ->GetMovementStickDirection();
        }
    }
    short dir = 0;
    cFielder* target = FindPowerupTarget(
        self->m_pFielder, (ePowerUpType)-1);
    self->mpTarget = target;
    if ((target != 0) && (lbl_806DC314 != 0))
    {
        dir = self->m_pFielder->GetFacingDeltaToPosition(
            target->mUnidentified024.m_v3Position);
    }
    self->m_pFielder->InitDesire(
        (eFielderDesireState)21, 0.5f, -1.0f, fvNotSet, fvNotSet);
    self->m_pFielder->SetAction((eFielderActionState)29);
    self->m_pFielder->muInvincibleStatus |= 1;
    self->m_pFielder->SetAnimState(104, true, 0.2f, false, false);
    self->m_pFielder->InitMovementFromAnim(
        dir, lbl_804DC338, 0.15f, false);
    self->mUnidentified078 = lbl_806DC2FC;
    return self->m_pFielder->m_eActionState
        == (eFielderActionState)29;
}

/**
 * Offset/Address/Size: 0x26C0 | 0x800CADBC | size: 0x500
 */
void DesireSuperPower::fn_800CADBC(DesireUpdate* update, float fDeltaT)
{
    if (m_pFielder->ShouldStartCrossBlend(4))
    {
        m_pFielder->EndAction();
    }

    if (m_pFielder->IsActionDone() || !g_pGame->IsGameplayOrOvertime())
    {
        m_pFielder->EndDesire();
        *update = 1;
    }
    if (m_pFielder->m_eActionState != (eFielderActionState)29)
    {
        *update = 1;
    }
}

/**
 * Offset/Address/Size: 0x2BC0 | 0x800CB2BC | size: 0x4EC
 */
void DesireSuperPower::fn_800CB2BC(DesireUpdate* update, float fDeltaT)
{
    m_pFielder->fn_8004F974(fDeltaT);

    if (m_pFielder->IsActionDone() || !g_pGame->IsGameplayOrOvertime())
    {
        m_pFielder->EndDesire();
        *update = 1;
    }
    if (m_pFielder->m_eActionState != (eFielderActionState)29)
    {
        *update = 1;
    }
}

void DesireSuperPower::fn_800CB7A8(DesireUpdate* update, float fDeltaT)
{
    m_pFielder->fn_800305DC(lbl_806DC268);
    if (m_pFielder->m_eActionState == ACTION_SHOOT_TO_SCORE)
    {
        float scaledDelta = fDeltaT * lbl_806DC240;
        mUnidentifiedTimer.Countup(scaledDelta - fDeltaT, 10.0f);
    }
    if (update->mData.i != 0)
    {
        switch (m_pFielder->m_eActionState)
        {
        case ACTION_ELECTROCUTION:
        case ACTION_LOOSE_BALL_PASS:
        case ACTION_LOOSE_BALL_SHOT:
        case ACTION_ONETIMER:
        case ACTION_RECEIVE_PASS:
            *update = 0;
            break;
        default:
            return;
        }
    }
    m_pFielder->fn_8001EF6C(1.0f);
    switch (m_pFielder->m_eActionState)
    {
    case (eFielderActionState)1:
        m_pFielder->fn_8001EF6C(lbl_806DC260);
        break;
    case ACTION_HIT:
        m_pFielder->fn_8001EF6C(lbl_806DC264);
        break;
    case ACTION_SHOT:
        *update = 1;
        break;
    default:
        if (!g_pGame->IsGameplayOrOvertime())
        {
            *update = 1;
        }
        break;
    }
}

void DesireSuperPower::fn_800CBF64(DesireUpdate* update, float fDeltaT)
{
    m_pFielder->fn_800305DC(lbl_806DC268);
    if (m_pFielder->m_eActionState == ACTION_SHOOT_TO_SCORE)
    {
        float scaledDelta = fDeltaT * lbl_806DC240;
        mUnidentifiedTimer.Countup(scaledDelta - fDeltaT, 10.0f);
    }
    if (update->mData.i != 0)
    {
        switch (m_pFielder->m_eActionState)
        {
        case ACTION_ELECTROCUTION:
        case ACTION_LOOSE_BALL_PASS:
        case ACTION_LOOSE_BALL_SHOT:
        case ACTION_ONETIMER:
        case ACTION_RECEIVE_PASS:
            *update = 0;
            break;
        default:
            return;
        }
    }
    m_pFielder->fn_8001EF6C(1.0f);
    switch (m_pFielder->m_eActionState)
    {
    case (eFielderActionState)1:
        m_pFielder->fn_8001EF6C(lbl_806DC260);
        break;
    case ACTION_HIT:
        m_pFielder->fn_8001EF6C(lbl_806DC264);
        break;
    case ACTION_SHOT:
        *update = 1;
        break;
    default:
        if (!g_pGame->IsGameplayOrOvertime())
        {
            *update = 1;
        }
        break;
    }
}

/**
 * Offset/Address/Size: 0x4024 | 0x800CC720 | size: 0x4EC
 */
void DesireSuperPower::fn_800CC720(DesireUpdate* update, float fDeltaT)
{
    m_pFielder->fn_8004FC90(fDeltaT);

    if (m_pFielder->IsActionDone() || !g_pGame->IsGameplayOrOvertime())
    {
        m_pFielder->EndDesire();
        *update = 1;
    }
    if (m_pFielder->m_eActionState != (eFielderActionState)29)
    {
        *update = 1;
    }
}

inline bool DesireSuperPower::fn_800CCF90() const
{
    bool fire = false;
    if (m_pFielder->mUnidentified40C - m_pFielder->mUnidentified404 > lbl_806DC2B4
        && m_pFielder->IsFallenDown() != true)
        fire = true;
    return fire;
}

void DesireSuperPower::fn_800CCC0C(DesireUpdate* update, float fDeltaT)
{
    m_pFielder->mUnidentified40C += fDeltaT;
    m_pFielder->mUnidentified408 += lbl_806DC2B0 * fDeltaT;
    m_pFielder->mUnidentified408 = nlMinEquals(
        nlMaxEquals(m_pFielder->mUnidentified408, lbl_806DC2A8), lbl_806DC2AC);
    if (update->mData.i == 3)
    {
        if (update->ExtraData.Get(11)->mData.b)
        {
            mpTarget = (cFielder*)update->ExtraData.Get(14)->mData.pointer;
            if (!m_pFielder->mUnidentified3DC)
            {
                m_pFielder->fn_80050284();
            }
        }
        else if (m_pFielder->mUnidentified3DC)
        {
            m_pFielder->fn_8005001C(false);
        }
        *update = 0;
    }
    if (update->mData.i == 0)
    {
        bool active = m_pFielder->mUnidentified3E0 > 0.0f;
        if (active)
        {
            if (m_pFielder->m_eAnimID == 104
                && m_pFielder->ShouldStartCrossBlend(4))
            {
                m_pFielder->EndDesire();
                m_pFielder->StartRunning();
            }
            if (m_pFielder->mUnidentified3DC)
            {
                if (fn_800CCF90() == true)
                {
                    m_pFielder->mUnidentified404 = m_pFielder->mUnidentified40C;
                    m_pFielder->fn_800D0534(1.0f / (float)lbl_806DC2A0);
                    nlVector3 pos;
                    nlVector3 direction;
                    const nlMatrix4& mat = m_pFielder->m_pPoseAccumulator->GetNodeMatrix(
                        m_pFielder->m_nHeadJointIndex);
                    nlVec3Set(direction, mat.m21, mat.m22, mat.m23);
                    nlVec3ScaleAdd(pos, lbl_806DC2B8, direction, (const nlVector3&)mat.m41);
                    nlPolar polar;
                    nlCartesianToPolar(polar, direction);
                    unsigned short angle = polar.a;
                    unsigned short range = DegreesToAngle(lbl_806DC2A4);
                    angle += nlRandom(range) - 0.5f * range;
                    direction.x = m_pFielder->mUnidentified408 * nlSin(angle + 0x4000);
                    direction.y = m_pFielder->mUnidentified408 * nlSin(angle);
                    direction.z = lbl_806DC2BC;
                    nlVec3Add(direction, direction, m_pFielder->mUnidentified024.m_v3Velocity);
                    PhysicsPatch* patch = lbl_806E12C8->fn_801743A8(3, m_pFielder,
                        pos, direction, lbl_806DC2C4, lbl_806DC2C4, 9999.0f);
                    patch->m_Gravity = lbl_806DC2C0;
                }
            }
        }
        else if (m_pFielder->m_eAnimID == 104)
        {
            if (m_pFielder->ShouldStartCrossBlend(4))
            {
                m_pFielder->EndDesire();
                m_pFielder->StartRunning();
                *update = 1;
            }
        }
        else
        {
            *update = 1;
        }
    }
}

void DesireSuperPower::fn_800CD61C(DesireUpdate* update, float fDeltaT)
{
    bool userControlled = (bool)m_pFielder->GetGlobalPad();
    if (!userControlled)
    {
        fn_800CDBF0(update, fDeltaT);
    }
    if (update->mData.i == 0)
    {
        bool active = m_pFielder->mUnidentified3E0 > 0.0f;
        if (!active)
        {
            *update = 1;
        }
    }
}

/**
 * Offset/Address/Size: 0x51E8 | 0x800CD8E4 | size: 0x14
 */
extern "C" void fn_800CD8E4(nlVector2* dst, const nlVector2* src)
{
    *dst = *src;
}

inline bool AvoidablePolygon::IntersectsSegment(const nlVector2& start, const nlVector2& end) const
{
    int i;
    const nlVector2* points = mPoints;
    for (i = 0; i < 4; i++)
    {
        nlVector2 edgeStart;
        nlVector2 edgeEnd;
        if (i < 3)
        {
            edgeStart = points[i];
            edgeEnd = points[i + 1];
        }
        else
        {
            edgeStart = points[i];
            edgeEnd = points[0];
        }
        float a, b;
        if (nlIntersectLineSegments2D(&start, &end, &edgeStart, &edgeEnd, &a, &b))
            return true;
    }
    return false;
}

extern "C" bool fn_800CD8F8(const nlVector2* direction, cFielder* fielder)
{
    float distance = 5.0f;
    nlVector2 normal;
    nlVector2 left[2], right[2];
    float c, s;
    nlSinCos(&s, &c, 0x4000);
    normal.x = direction->x * c - direction->y * s;
    normal.y = direction->y * c + direction->x * s;
    float radius = fielder->mUnidentified320->GetRadius();
    nlVec2Set(left[1], radius * normal.x + fielder->mUnidentified024.m_v3Position.x,
        radius * normal.y + fielder->mUnidentified024.m_v3Position.y);
    nlVec2Set(left[1], distance * direction->x + left[1].x, distance * direction->y + left[1].y);
    nlVec2Set(left[0], -(distance - 1.2f) * direction->x + left[1].x,
        -(distance - 1.2f) * direction->y + left[1].y);
    float c2, s2;
    nlSinCos(&s2, &c2, 0xC000);
    normal.x = direction->x * c2 - direction->y * s2;
    normal.y = direction->y * c2 + direction->x * s2;
    radius = fielder->mUnidentified320->GetRadius();
    nlVec2Set(right[1], radius * normal.x + fielder->mUnidentified024.m_v3Position.x,
        radius * normal.y + fielder->mUnidentified024.m_v3Position.y);
    nlVec2Set(right[1], distance * direction->x + right[1].x, distance * direction->y + right[1].y);
    nlVec2Set(right[0], -(distance - 1.2f) * direction->x + right[1].x,
        -(distance - 1.2f) * direction->y + right[1].y);
    bool result = false;
    for (int i = 0; i < 20; i++)
    {
        PhysicsWaluigiWall* wall = fielder->mUnidentified3F8.mUnidentified08->GetWall(i);
        if (wall != 0)
        {
            AvoidablePolygon* polygon = wall->GetAvoidablePolygon();
            if (polygon->IntersectsSegment(left[0], left[1])
                || polygon->IntersectsSegment(right[0], right[1]))
            {
                result = true;
                break;
            }
        }
    }
    return result;
}

void DesireSuperPower::fn_800CDBF0(DesireUpdate*, float)
{
    if (m_pFielder->fn_8002E060() == 13 && m_pFielder->mUnidentified3DC)
    {
        nlVector2 direction;
        nlSinCos(&direction.y, &direction.x, m_pFielder->mUnidentified024.m_aActualFacingDirection);
        if (m_pFielder->m_pBall == 0 && NearToBall(m_pFielder) >= 0.7f
            || fn_800CD8F8(&direction, m_pFielder))
            m_pFielder->fn_8005001C(true);
    }
    else if (!m_pFielder->mUnidentified3DC
        && (m_pFielder->fn_8002E060() == 12 || m_pFielder->fn_8002E060() == 13))
    {
        if (!(bool)UserControlledT(m_pFielder->m_pTeam))
        {
            m_pFielder->fn_800305DC(0.0f);
            nlVector3 direction;
            const nlVector3& target = *fn_80040234(m_pFielder);
            nlVec3Sub(direction, target, m_pFielder->mUnidentified024.m_v3Position);
            bool valid;
            float lengthSq = direction.GetLengthSq3D();
            if (lengthSq == 0.0f)
            {
                valid = false;
            }
            else
            {
                nlVec3Scale(direction, nlRecipSqrt(lengthSq, true));
                valid = true;
            }
            if (valid)
            {
                unsigned short angle = fn_800D1CCC(direction.y, direction.x);
                short delta = m_pFielder->mUnidentified024.m_aActualFacingDirection - angle;
                float sideline = CloseToSideline(m_pFielder);
                float question = fn_800DD744(m_pFielder);
                int difference = fn_800D1D10(delta);
                bool near = difference < 0x800
                    && (m_pFielder->m_pBall != 0 || (bool)(NearToBall(m_pFielder) < 0.7f));
                bool clear = false;
                if (near && sideline < 0.9f && question < 0.9f)
                    clear = true;
                bool moving = clear && m_pFielder->mUnidentified024.m_fActualSpeed > 1.0f;
                bool start = moving && !fn_800CD8F8((const nlVector2*)&direction, m_pFielder);
                if (start)
                {
                    bool active = m_pFielder->mUnidentified3E0 > 0.0f;
                    if (active && fn_8002EDC8(m_pFielder, -1))
                    {
                        m_pFielder->fn_80050284();
                        m_pFielder->fn_8003057C(0);
                    }
                }
            }
        }
    }
    else if (m_pFielder->fn_8002E060() != 12
        && m_pFielder->fn_8002E060() != 13
        && (!(bool)UserControlledT(m_pFielder->m_pTeam) || m_pFielder->mUnidentified3DC)
        && (bool)(1.0f - ReceivingPass(m_pFielder))
        && (bool)(1.0f - fn_800DEAB4(m_pFielder))
        && fn_8002EDC8(m_pFielder, -1))
    {
        if ((bool)UserControlledT(m_pFielder->m_pTeam))
        {
            int count = fn_800D0004();
            nlVector3 pos;
            nlVec3Set(pos, mUnidentifiedPositions[0].x, mUnidentifiedPositions[0].y, 0.0f);
            nlVector3 direction;
            nlVec3Sub(direction, pos, m_pFielder->mUnidentified024.m_v3Position);
            float distance = nlVec2Length((const nlVector2&)direction);
            unsigned short angle = fn_800D1CCC(direction.y, direction.x);
            UnidentifiedVariantCollection params;
            params.Set(7, FuzzyVariant(lbl_806DC334));
            params.Set(17, FuzzyVariant((unsigned long)angle));
            params.Set(18, FuzzyVariant(distance));
            params.Set(0, FuzzyVariant(lbl_806DC338));
            params.Set(1, FuzzyVariant(count));
            params.Set(13, FuzzyVariant(lbl_806DC33C));
            params.Set(10, FuzzyVariant((void*)fn_800CE588));
            fn_8003EBD0(m_pFielder, 12, &params);
        }
        else if (m_pFielder->mUnidentified3DC)
        {
            m_pFielder->fn_8005001C(true);
        }
        else if (m_pFielder->m_pBall != 0 && InDefensiveZone(m_pFielder) < 0.5f)
        {
            float x;
            if (m_pFielder->m_pBall != 0)
                x = AIsgn(m_pFielder->GetAIOffNetLocation(0).x);
            else
                x = AIsgn(m_pFielder->GetAIDefNetLocation(0).x);
            nlVector2 direction;
            nlVec2Set(direction, x, 0.0f);
            unsigned short facing = m_pFielder->mUnidentified024.m_aActualFacingDirection;
            unsigned short angles[4] = { facing, facing + 0x4000, facing - 0x4000, facing + 0x8000 };
            float score = 0.0f;
            unsigned short angle = angles[fn_800CFCC8(m_pFielder, angles, 4, &direction, &score)];
            if (score > 0.0f)
            {
                UnidentifiedVariantCollection params;
                params.Set(7, FuzzyVariant(lbl_806DC340));
                params.Set(17, FuzzyVariant((unsigned long)angle));
                params.Set(18, FuzzyVariant(lbl_806DC344));
                params.Set(13, FuzzyVariant(lbl_806DC348));
                params.Set(10, FuzzyVariant((void*)fn_800CEA20));
                fn_8003EBD0(m_pFielder, 12, &params);
            }
        }
        else if (!(bool)Offensive(m_pFielder->m_pTeam)
            && NearToBall(fn_800D66C4(m_pFielder)) < 0.35f
            && NearToBall(fn_800D66A0(m_pFielder)) < 0.35f
            && FarToBall(m_pFielder) < 0.35f)
        {
            UnidentifiedVariantCollection params;
            params.Set(7, FuzzyVariant(lbl_806DC34C));
            params.Set(14, FuzzyVariant(g_pBall));
            params.Set(13, FuzzyVariant(lbl_806DC350));
            params.Set(10, FuzzyVariant((unsigned long)nlStringHash("TransDesireInterceptBall")));
            fn_8003EBD0(m_pFielder, 13, &params);
        }
    }
}

UnidentifiedVariant_80054AB8 DesireSuperPower::fn_800CE588(
    const FuzzyVariant& value, shdStateMachine* machine)
{
    UnidentifiedVariant_80054AB8 result(lbl_806DC354, -1.0f, -1.0f);
    if (fn_800D1D34(machine) != 12)
        return UnidentifiedVariant_80054AB8(lbl_806DC358, -1.0f, -1.0f);
    cFielder* fielder = (cFielder*)value.GetPlayer();
    DesireSuperPower* desire = (DesireSuperPower*)fn_8002E08C(fielder, 23);
    int index = fn_800D1D3C(machine)->Get(0)->fn_800C2BD4();
    int count = fn_800D1D3C(machine)->Get(1)->fn_800C2BD4();
    nlVector2& current = desire->mUnidentifiedPositions[index];
    nlVector3 oldPos;
    oldPos.Set(current.x, current.y, 0.0f);
    fn_800D1D44((DesireRunInDirection*)machine);
    fn_800D1D4C((DesireRunInDirection*)machine);
    if (fabsf(fn_800D1D4C((DesireRunInDirection*)machine)
            - fn_800D1D44((DesireRunInDirection*)machine)) < 1.5f)
    {
        ++index;
        if (index == count)
        {
            if ((bool)UserControlledT(fielder->GetTeam()))
                result = 1;
            else if (fn_800DBAB0(fielder) > 0.1f && fielder->fn_800C2F40())
            {
                result = 3;
                result.SetParameter(8, FuzzyVariant(lbl_806DC35C));
                result.SetParameter(10, FuzzyVariant((unsigned long)nlStringHash("TransDesireWindupMegastrike")));
            }
            else
            {
                fielder->fn_8005001C(false);
                result = 1;
            }
        }
        else
        {
            fn_800CD8E4(&current, &desire->mUnidentifiedPositions[index]);
            nlVector3 pos;
            pos.Set(current.x, current.y, 0.0f);
            nlVector3 delta;
            fn_800D1C4C(&delta, &pos, GetCharacterPosition(fielder));
            float distance = fn_800D1C80(oldPos, pos);
            unsigned short angle = fn_800D1CCC(delta.y, delta.x);
            result = 3;
            result.SetParameter(8, FuzzyVariant(lbl_806DC360));
            result.SetParameter(12, FuzzyVariant(lbl_806DC364));
            result.SetParameter(17, FuzzyVariant((unsigned long)angle));
            result.SetParameter(18, FuzzyVariant(distance));
            result.SetParameter(0, FuzzyVariant(index));
            result.SetParameter(1, FuzzyVariant(count));
            result.SetParameter(13, FuzzyVariant(lbl_806DC368));
            result.SetParameter(10, FuzzyVariant((void*)fn_800CE588));
            if (fn_800D1C34(fielder) && fn_800D1D34(machine) == 12)
            {
                unsigned short absolute = fn_800D1D10(fn_800D1D10(fn_800D1D04(fn_800D1448(fielder), angle)));
                short folded = absolute % 0x4000;
                bool okay = folded < 0x2000 || (unsigned int)fn_800D1D24(folded - 0x4000) < 0x2000;
                if (!okay)
                    fielder->fn_8005001C(true);
            }
        }
    }
    return UnidentifiedVariant_80054AB8(result, -1.0f, -1.0f);
}

struct UnidentifiedFielderRef
{
    cFielder* mFielder;
};

UnidentifiedVariant_80054AB8 DesireSuperPower::fn_800CEA20(
    const FuzzyVariant& value, shdStateMachine* machine)
{
    UnidentifiedVariant_80054AB8 result(FT_INT, lbl_806DC36C);
    if (machine->UnidentifiedGetState() != 12)
        return UnidentifiedVariant_80054AB8(FT_INT, lbl_806DC370);
    UnidentifiedFielderRef fielder = { (cFielder*)value.mData.pointer };
    fn_8002E08C(fielder.mFielder, 23);
    float maxDistance = ((DesireRunInDirection*)machine)->GetMaxDistance();
    float distanceTravelled = ((DesireRunInDirection*)machine)->GetDistanceTravelled();
    float danger = fn_80041B0C(((UnidentifiedFuzzyRuntimeValue*)&value)->GetRuntime(),
        fielder.mFielder, "InDangerForMegastrike").mData.f;
    float question = fn_800DBB0C(fielder.mFielder);
    bool good = ((1.0f - danger) / 2.0f + question / 2.0f) > 0.75f;
    bool ready = good
        || (CloseToSideline(fielder.mFielder) > 0.9f && fn_800DD744(fielder.mFielder) > 0.7f);
    bool shoot = true;
    if (!ready)
    {
        bool active = fielder.mFielder->mUnidentified3E0 > 0.0f;
        if (active)
            shoot = false;
    }
    if (fielder.mFielder->m_pBall != 0 && shoot && InOffensiveZone(fielder.mFielder) > 0.9f)
    {
        result = 3;
        result.SetParameter(8, FuzzyVariant(lbl_806DC374));
        result.SetParameter(10, FuzzyVariant((unsigned long)nlStringHash("TransDesireWindupMegastrike")));
        if (fielder.mFielder->mUnidentified3DC)
            fielder.mFielder->fn_8005001C(true);
    }
    else if (fielder.mFielder->mUnidentified3DC && distanceTravelled >= 2.0f)
    {
        bool turn = false;
        cPlayer* bestPlayer = fn_800D674C(fielder.mFielder);
        float q = CloseTo(fielder.mFielder, bestPlayer);
        float pass = fn_800DDF54(fielder.mFielder, bestPlayer);
        float best = pass / 2.0f + q / 2.0f;
        for (int i = 0; i < 5; i++)
        {
            cPlayer* player = fn_800D6688(fielder.mFielder)->GetPlayer(i);
            if (player != bestPlayer)
            {
                float q = CloseTo(fielder.mFielder, bestPlayer);
                float pass = fn_800DDF54(fielder.mFielder, bestPlayer);
                float score = pass / 2.0f + q / 2.0f;
                if (score > best)
                {
                    best = score;
                    bestPlayer = player;
                }
            }
        }
        float question1 = fn_800DD234(fielder.mFielder);
        float question2 = fn_800DD744(fielder.mFielder);
        nlVector2 facingDirection;
        nlSinCos(&facingDirection.y, &facingDirection.x,
            fielder.mFielder->mUnidentified024.m_aActualFacingDirection);
        if (best > 0.6f || FMIN(question1, question2) > 0.75f
            || (float)fabs(distanceTravelled - maxDistance) < 1.5f
            || fn_800CD8F8(&facingDirection, fielder.mFielder))
            turn = true;
        if (turn)
        {
            float x;
            if (fielder.mFielder->m_pBall != 0)
                x = AIsgn(fielder.mFielder->GetAIOffNetLocation(0).x);
            else
                x = AIsgn(fielder.mFielder->GetAIDefNetLocation(0).x);
            nlVector2 direction;
            nlVec2Set(direction, x, 0.0f);
            unsigned short facing = fielder.mFielder->mUnidentified024.m_aActualFacingDirection;
            unsigned short angles[3] = { facing, facing + 0x4000, facing - 0x4000 };
            float score = 0.0f;
            unsigned short angle = angles[fn_800CFCC8(fielder.mFielder, angles, 3, &direction, &score)];
            if (score == 0.0f && fielder.mFielder->m_pBall != 0 && InOffensiveZone(fielder.mFielder) > 0.0f)
            {
                result = 3;
                result.SetParameter(8, FuzzyVariant(lbl_806DC378));
                result.SetParameter(10, FuzzyVariant((unsigned long)nlStringHash("TransDesireWindupMegastrike")));
                if (fielder.mFielder->mUnidentified3DC)
                    fielder.mFielder->fn_8005001C(true);
            }
            else
            {
                result = 3;
                result.SetParameter(8, FuzzyVariant(lbl_806DC37C));
                result.SetParameter(12, FuzzyVariant(lbl_806DC380));
                result.SetParameter(17, FuzzyVariant((unsigned long)angle));
                result.SetParameter(18, FuzzyVariant(lbl_806DC384));
                result.SetParameter(13, FuzzyVariant(lbl_806DC388));
                result.SetParameter(10, FuzzyVariant((void*)fn_800CEA20));
            }
            if (fielder.mFielder->mUnidentified3DC && machine->UnidentifiedGetState() == 12)
            {
                unsigned short absolute = fn_800D1D10(fn_800D1D10(
                    fn_800D1D04(fielder.mFielder->mUnidentified024.m_aActualFacingDirection, angle)));
                short folded = absolute % 0x4000;
                bool okay = folded < 0x2000 || (unsigned int)fn_800D1D24(folded - 0x4000) < 0x2000;
                if (!okay)
                    fielder.mFielder->fn_8005001C(true);
            }
        }
    }
    return result;
}

extern "C" int fn_800CFCC8(cFielder* fielder, const unsigned short* angles,
    int count, const nlVector2* forward, float* bestScore)
{
    unsigned short desiredAngle = fn_800D1CCC(forward->y, forward->x);
    float best = 0.0f;
    int bestIndex = 0;
    float question1 = fn_800DD234(fielder);
    float question2 = fn_800DD744(fielder);
    nlVector3 farPos;
    nlVector3 nearPos;
    nlVector3 zero = lbl_804DC338;
    for (int i = 0; i < count; i++)
    {
        unsigned short angle = angles[i];
        if (forward->x * fielder->mUnidentified024.m_v3Position.x < 12.360001f
            && fn_800D1D10((short)(angle - desiredAngle)) > 0x5555)
            continue;
        farPos = zero;
        nearPos = zero;
        nlPolarToCartesian(farPos.x, farPos.y, angle, 6.0f);
        nlPolarToCartesian(nearPos.x, nearPos.y, angle, 4.0f);
        nlVec3Add(nearPos, nearPos, fielder->mUnidentified024.m_v3Position);
        nlVec3Add(farPos, farPos, fielder->mUnidentified024.m_v3Position);
        cField::FixOutOfBoundsPosition(nearPos, 0.2f, true);
        cField::FixOutOfBoundsPosition(farPos, 0.2f, true);
        nlVector2 direction;
        nlSinCos(&direction.y, &direction.x, angle);
        if (!fn_800CD8F8(&direction, fielder))
        {
            float lane = fn_800DAFCC(&fielder->mUnidentified024.m_v3Position, &farPos,
                fielder, 0, 0.0f, 1.0f, 1.0f, 0.0f);
            float support = FuzzyNot(fn_800DCB4C(&nearPos,
                &fn_800D66C4(fielder)->mUnidentified024.m_v3Position));
            float sideline = 1.0f;
            if (question1 > 0.1f
                || (angle == fielder->mUnidentified024.m_aActualFacingDirection && question2 >= 0.9f))
                sideline = FuzzyNot(CloseToSideline(nearPos, 0, false, 0));
            support = FMIN(support, sideline);
            lane = FMIN(lane, support);
            if (lane > best)
            {
                bestIndex = i;
                best = lane;
            }
        }
    }
    *bestScore = best;
    return bestIndex;
}

int DesireSuperPower::fn_800D0004()
{
    int count;
    if (UserControlledT(m_pFielder->m_pTeam))
    {
        mUnidentifiedPositions[0] = (const nlVector2&)m_pFielder->mUnidentified024.m_v3Position;
        count = 2;
        nlVector3 pos;
        pos.z = 0.0f;
        nlPolarToCartesian(pos.x, pos.y,
            m_pFielder->mUnidentified024.m_aActualFacingDirection, 4.0f);
        nlVec3Add(pos, m_pFielder->mUnidentified024.m_v3Position, pos);
        cField::FixOutOfBoundsPosition(pos,
            m_pFielder->mUnidentified320->GetRadius(), true);
        mUnidentifiedPositions[1] = (const nlVector2&)pos;
    }
    else
    {
        const nlVector2* positions;
        if (m_pFielder->m_pBall != 0)
        {
            count = 6;
            positions = lbl_804DC348;
        }
        else
        {
            count = 2;
            positions = lbl_804DC378;
        }
        bool flip = m_pFielder->mUnidentified024.m_v3Position.y > 0.0f;
        for (int i = 0; i < count; i++)
        {
            mUnidentifiedPositions[i] = positions[i];
            if (flip)
            {
                mUnidentifiedPositions[i].y = -mUnidentifiedPositions[i].y;
            }
            if (m_pFielder->m_pTeam->m_nSide == 1)
            {
                nlVec2Scale(mUnidentifiedPositions[i], mUnidentifiedPositions[i], -1.0f);
            }
        }
    }
    return count;
}

void DesireSuperPower::fn_800D01A0(DesireUpdate* update, float fDeltaT)
{
    if (update->mData.i == 3)
    {
        if (update->ExtraData.Get(11)->mData.b)
        {
            mpTarget = (cFielder*)update->ExtraData.Get(14)->mData.pointer;
            if (!m_pFielder->mUnidentified3DC)
                m_pFielder->fn_80050284();
        }
        else if (m_pFielder->mUnidentified3DC)
            m_pFielder->fn_8005001C(false);
        *update = 0;
    }
    if (update->mData.i == 0)
    {
        bool active = m_pFielder->mUnidentified3E0 > 0.0f;
        if (active)
        {
            if (m_pFielder->m_eAnimID == 104
                && m_pFielder->ShouldStartCrossBlend(4))
            {
                m_pFielder->EndDesire();
                m_pFielder->StartRunning();
            }
            if (m_pFielder->mUnidentified3DC)
            {
                if (!fn_8002EDC8(m_pFielder, -1))
                {
                    m_pFielder->fn_8005001C(true);
                    return;
                }
                m_pFielder->mUnidentified3F4 -= fDeltaT;
                if (m_pFielder->mUnidentified3F4 <= 0.0f)
                {
                    unsigned long sound = PowerupBase::GetSoundType(
                        (ePowerUpType)m_pFielder->mUnidentified11C->unknown_0x14,
                        PowerupBase::PWRUP_SOUND_ACTIVATE);
                    PlaySound(m_pFielder->m_uSoundSlotId, sound, 0, 0);
                    fn_800367B4(m_pFielder);
                    m_pFielder->fn_800D0534(lbl_806DC298);
                    m_pFielder->mUnidentified3F4 = lbl_806DC298;
                    nlVector3 pos;
                    nlVector3 offset;
                    nlPolarToCartesian(offset.x, offset.y,
                        m_pFielder->mUnidentified024.m_aActualFacingDirection, lbl_806DC29C);
                    offset.z = 0.0f;
                    const nlMatrix4& mat = m_pFielder->m_pPoseAccumulator->GetNodeMatrix(
                        m_pFielder->m_nBip01JointIndex_0xA4);
                    nlVec3Add(pos, (const nlVector3&)mat.m41, offset);
                    lbl_806E12C8->fn_801743A8(0, m_pFielder, pos, lbl_804DC338,
                        lbl_806DC244, lbl_806DC248, lbl_806DC24C);
                    PlayRumbleAction(1, m_pFielder->GetGlobalPad());
                    EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup("wario_ignition");
                    if (group != 0)
                    {
                        EmissionController* emitter = EmissionManager::Instance()->Create(group, 3, true, 0);
                        emitter->m_uUserData = (unsigned int)m_pFielder;
                        emitter->SetPosition(m_pFielder->mUnidentified024.m_v3Position);
                        emitter->SetVelocity(m_pFielder->mUnidentified024.m_v3Velocity);
                        emitter->SetUpdateCallback(UpdateEmitterFromCharacterBackward);
                    }
                }
                if ((bool)UserControlledT(m_pFielder->m_pTeam))
                {
                    bool userControlled = (bool)m_pFielder->GetGlobalPad();
                    if (!userControlled && m_pFielder->fn_8002E060() != 12)
                    {
                        nlVector3 pos;
                        pos.z = 0.0f;
                        nlPolarToCartesian(pos.x, pos.y,
                            m_pFielder->mUnidentified024.m_aActualFacingDirection, 5.0f);
                        nlVec3Add(pos, m_pFielder->mUnidentified024.m_v3Position, pos);
                        cField::FixOutOfBoundsPosition(pos, m_pFielder->mUnidentified320->GetRadius(), true);
                        nlVector3 delta;
                        nlVec3Sub(delta, pos, m_pFielder->mUnidentified024.m_v3Position);
                        float distance = nlSqrt(delta.GetLengthSq2D(), true);
                        unsigned short angle = fn_800D1CCC(delta.y, delta.x);
                        UnidentifiedVariantCollection params;
                        params.Set(7, FuzzyVariant(lbl_806DC38C));
                        params.Set(17, FuzzyVariant((unsigned long)angle));
                        params.Set(18, FuzzyVariant(distance));
                        params.Set(13, FuzzyVariant(lbl_806DC390));
                        params.Set(10, FuzzyVariant(lbl_806DC394));
                        fn_8003EBD0(m_pFielder, 12, &params);
                    }
                }
            }
            else
            {
                m_pFielder->mUnidentified3F4 = 0.0f;
            }
        }
        else if (m_pFielder->m_eAnimID == 104)
        {
            if (m_pFielder->ShouldStartCrossBlend(4))
            {
                m_pFielder->EndDesire();
                m_pFielder->StartRunning();
                *update = 1;
            }
        }
        else
        {
            *update = 1;
        }
    }
}

/**
 * Offset/Address/Size: 0x86B4 | 0x800D0DB0 | size: 0xFC
 */
extern "C" bool fn_800D0DB0(DesireSuperPower* self, void*)
{
    gNPCManager->mUnidentified024->Activate(self->m_pFielder);
    self->m_pFielder->m_pTweaks
        = self->m_pFielder->mUnidentified328;
    fn_8002E52C(self->m_pFielder);
    if (self->m_pFielder->mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
    {
        self->m_pFielder->fn_8009750C();
        self->m_pFielder->EndAction();
    }
    self->m_pFielder->bYoshiInWindup
        = (self->m_pFielder->m_eActionState
            == ACTION_UNKNOWN_30);
    if ((self->m_pFielder->fn_8002E060() == 21)
        || (self->m_pFielder->fn_8002E060() == 19)
        || (self->m_pFielder->fn_8002E060() == 18)
        || (self->m_pFielder->fn_8002E060() == 9))
    {
        self->m_pFielder->EndDesire();
        self->m_pFielder->StartRunning();
    }
    else if (self->m_pFielder->m_eActionState
        == ACTION_UNKNOWN_30)
    {
        self->m_pFielder->StartRunning();
    }
    fn_800395C0(self->m_pFielder);
    self->mUnidentified078 = lbl_806DC32C;
    return true;
}

void DesireSuperPower::fn_800D0EAC(DesireUpdate* update, float)
{
    if (update->mData.i != 0)
    {
        switch (m_pFielder->m_eActionState)
        {
        case ACTION_ELECTROCUTION:
        case ACTION_LOOSE_BALL_PASS:
        case ACTION_LOOSE_BALL_SHOT:
        case ACTION_ONETIMER:
        case ACTION_RECEIVE_PASS:
            *update = 0;
            break;
        }
    }
}

static inline bool UnidentifiedCheck_800D11BC(nlVector3 point)
{
    if (lbl_806E12C8 != 0)
    {
        for (int i = 0; i < 60; i++)
        {
            PhysicsPatch* patch = lbl_806E12C8->fn_801745B8(i);
            if (patch != 0 && patch->m_Type == 4)
            {
                nlVector3 delta;
                nlVec3Sub(delta, point, patch->GetPosition());
                if (nlVec3DotProduct(delta, delta) < lbl_806DC2CC * lbl_806DC2CC)
                    return false;
            }
        }
    }
    return true;
}

extern "C" void fn_800D1140(void* context)
{
    UnidentifiedEventData24* event = (UnidentifiedEventData24*)context;
    bool hit = false;
    if (event->mUnidentified10->GetPosition().z < 0.0f
        && event->mUnidentified10->m_Velocity.z < 0.0f)
    {
        hit = true;
    }
    if (event->mUnidentified0C != 0
        && event->mUnidentified0C->mUnidentified024.m_eCharacterClass != YOSHI)
    {
        hit = true;
    }
    if (hit == true && event->mUnidentified10->m_Type == 3)
    {
        nlVector3 pos = event->mUnidentified10->GetPosition();
        pos.z = 0.0f;
        if (UnidentifiedCheck_800D11BC(pos) == true)
        {
            lbl_806E12C8->fn_801743A8(4, event->mUnidentified10->m_pOwner,
                pos, lbl_804DC338, lbl_806DC2C8, lbl_806DC2C8, lbl_806DC2D0);
        }
        event->mUnidentified10->Unknown0();
    }
}

extern "C" void fn_800D12E8(void* context)
{
    PhysicsPatch* patch = (PhysicsPatch*)context;
    if (patch->m_Type == 3)
    {
        float length = fabs(cField::GetGoalLineX(0U));
        float width = fabs(0.5f * (2.0f * cField::mv3FieldPosition.y));
        float x = patch->GetPosition().x;
        float y = patch->GetPosition().y;
        float threshold = 0.05f * patch->GetRadius();
        nlVector3 velocity = patch->m_Velocity;
        if (x - length < threshold || x - (-1.0f * length) < threshold)
        {
            velocity.x *= -0.5f;
        }
        if (y - width < threshold || y - (-1.0f * width) < threshold)
        {
            velocity.y *= -0.5f;
        }
        patch->m_Velocity = velocity;
    }
}

/**
 * Offset/Address/Size: 0x8D44 | 0x800D1440 | size: 0x8
 */
extern "C" eCharacterClass fn_800D1440(const cCharacter* character)
{
    return character->mUnidentified024.m_eCharacterClass;
}

/**
 * Offset/Address/Size: 0x8D4C | 0x800D1448 | size: 0x8
 */
extern "C" unsigned short fn_800D1448(const cCharacter* character)
{
    return character->mUnidentified024.m_aActualFacingDirection;
}

/**
 * Offset/Address/Size: 0x8D54 | 0x800D1450 | size: 0x8
 */
const nlVector3* GetCharacterPosition(const cCharacter* character)
{
    return &character->mUnidentified024.m_v3Position;
}

/**
 * Offset/Address/Size: 0x8D5C | 0x800D1458 | size: 0x20
 */
extern "C" bool fn_800D1458(const cGame* game)
{
    return game->m_eGameState == 5 || game->m_eGameState == 6;
}

#include "Game/AI/Fielder.inl"
#include "NL/nlMath.inl"
#include "Game/AI/Desire.inl"
#include "Game/AI/DesireSuperPower.inl"

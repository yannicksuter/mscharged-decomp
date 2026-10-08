#include "NL/nlDLListContainer.inl"
#include "Game/Sys/audio.h"
#include "Game/CharacterTriggers.h"
#include "Game/RumbleActions.h"
#include "Game/Sys/debug.h"
#include "Game/AnimInventory.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/Ball.h"
#include "Game/Character.h"
#include "Game/CharacterQueries.h"
#include "Game/CharacterEffects.h"
#include "Game/CharacterTemplate.h"
#include "Game/Triggers/AnimTrigger.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmitterCallbacks.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Game.h"
#include "Game/Goalie.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsShockwave.h"
#include "Game/Player.h"
#include "Game/SAnim.h"
#include "NL/nlString.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/Render/BulletBill.h"
#include "Game/AI/AiUtil.h"
#include "Game/Render/ElectricFence.h"
#include "Game/AI/ShotMeter.h"
#include "Game/PoseAccumulator.h"
#include "Game/SHierarchy.h"
#include "Game/Team.h"
#include "Game/GameTweaks.h"
#include "Game/AI/Desire.h"
#include "Game/AI/DesireSuperPower.h"
#include "Game/Effects/EffectsGroup.h"
#include "Game/TweakValue.h"
#include "NL/nlstring_tmpl.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Camera/CameraMan.h"

static bool sAwardPowerupForBallCharge;
static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };
static float sWarioGroundPoundRadius = 2.0f;
static float sBowserJrGroundPoundRadius = 2.0f;
static float sHammerBroHammerRadius = 2.0f;
static float sHammerBroHammerForwardOffset = 0.25f;
static float sPowerupMeterShotSpeedThreshold = 1.0f;
static float sPowerupMeterBallChargeThreshold = 1.0f;
static bool sAwardPowerupForShotSpeed = true;
static float sGoalieArmInGroundOffsetX = 1.0f;
static float sGoalieArmInGroundOffsetY = -0.3f;
static nlVector3 sBallEffectVelocity = { 0.0f, 0.0f, 1.0f };

static inline void SetDefaultVelocity(EmissionController* pController)
{
    const nlVector3 vel = { 0.0f, 0.0f, 1.0f };
    pController->SetVelocity(vel);
}

static inline void SetPoseUpdateCallback(EmissionController* controller)
{
    Function1<void, EmissionController&> update(
        UpdateEmitterPoseFromCharacter);
    controller->SetUpdateCallback(update);
}

static inline EffectsGroup* GetCharacterEffect(const char* szCharName, const char* szBaseName)
{
    char effectName[0x100];
    char fallbackName[0x100];
    EffectsGroup* pGroup;

    nlStrNCat<char>(effectName, szCharName, "_", 0x100);
    nlStrNCat<char>(effectName, effectName, szBaseName, 0x100);
    nlStrNCat<char>(effectName, effectName, "_", 0x100);
    nlStrNCat<char>(effectName, effectName, "grass", 0x100);
    pGroup = EmissionManager::Instance()->GetEffectsGroup(effectName);

    if (pGroup == 0 && szCharName[0] != '\0')
    {
        nlStrNCat<char>(fallbackName, "", "_", 0x100);
        nlStrNCat<char>(fallbackName, fallbackName, szBaseName, 0x100);
        nlStrNCat<char>(fallbackName, fallbackName, "_", 0x100);
        nlStrNCat<char>(fallbackName, fallbackName, "grass", 0x100);
        pGroup = EmissionManager::Instance()->GetEffectsGroup(fallbackName);
    }
    return pGroup;
}

static inline void SetFreeUpdateCallback(EmissionController* controller,
    void (*callback)(EmissionController&))
{
    Function1<void, EmissionController&> update(callback);
    controller->SetUpdateCallback(update);
}

static inline void SetZeroVelocity(EmissionController* pController)
{
    pController->SetVelocity(v3Zero);
}

static inline EmissionController* CreateBallEffect(
    unsigned long uEffectHash, cBall* pBall)
{
    EffectsGroup* pGroup = fxGetGroup(EmissionManager::Instance(), uEffectHash);
    EmissionController* pControl
        = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    pControl->m_uUserData = (unsigned long)pBall;

    nlVector3 v3Direction = sBallEffectVelocity;
    pControl->SetVelocity(v3Direction);
    pControl->m_fGround = 0.02f;
    return pControl;
}

static inline void SetBallUpdateCallback(EmissionController* pController)
{
    Function1<void, EmissionController&> update(UpdateEmitterFromBall);
    pController->SetUpdateCallback(update);
}

static inline void EmitCharacterEffect(cCharacter* pCharacter, const char* szName)
{
    EmissionController* pController = EmitGeneric(pCharacter, szName, 0);
    {
        Function1<void, EmissionController&> update2(UpdateEmitterFromCharacter);
        pController->SetUpdateCallback(update2);
    }
}

EmissionController* EmitGeneric(cCharacter* pCharacter, const char* baseName,
    const char* characterName)
{
    EffectsGroup* pGroup;

    if (characterName != 0)
    {
        pGroup = GetCharacterEffect(characterName, baseName);
    }
    else
    {
        pGroup = EmissionManager::Instance()->GetEffectsGroup(baseName);
    }

    EmissionController* pControl = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetDefaultVelocity(pControl);
    pControl->m_fGround = 0.02f;
    SetPoseUpdateCallback(pControl);
    pCharacter->AttachEffect(pControl);
    return pControl;
}

void CharacterTriggerHandler(cSAnim* pAnim, unsigned int uParam)
{
    AnimTriggerCallbackInfo* pAnimTriggerCallbackInfo = (AnimTriggerCallbackInfo*)uParam;

    GetCharacterEffectsName(g_pCurrentlyUpdatingCharacter);

    if (GetControllerSAnim(g_pCurrentlyUpdatingCharacter->GetCurrentAnimController()) == pAnim
        || pAnimTriggerCallbackInfo->m_uEventID == 0xC5408AC8
        || pAnimTriggerCallbackInfo->m_uEventID == 0x0F3E9247)
    {
        switch (pAnimTriggerCallbackInfo->m_uEventID)
        {
        case 0xC9F4F4B0:
        {
            u32 hasPad = HasGlobalPad((cPlayer*)g_pCurrentlyUpdatingCharacter);
            bool ownsBall
                = (g_pBall != 0 && g_pCurrentlyUpdatingCharacter == GetBallOwner(g_pBall));
            if (g_pBall != 0 && !ownsBall)
            {
                break;
            }
            PlaySound(0xB, 0x38DF70D6, 0, 0);
            break;
        }

        case 0x3650221E:
        case 0xE272718B:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter))
            {
                EmitSuperFootstep(g_pCurrentlyUpdatingCharacter, pAnimTriggerCallbackInfo->m_uEventID == 0x3650221E);
            }
            break;

        case 0x50DF765D:
        {
            u32 hasPad = HasGlobalPad((cPlayer*)g_pCurrentlyUpdatingCharacter);
            bool ownsBall
                = (g_pBall != 0 && g_pCurrentlyUpdatingCharacter == GetBallOwner(g_pBall));
            if (g_pBall != 0 && !ownsBall)
            {
                break;
            }
            PlaySound(0xB, 0x38DF70D6, 0, 0);
            break;
        }

        case 0x05D46E25:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter))
            {
                EmitDKDeke(g_pCurrentlyUpdatingCharacter);
            }
            break;

        case 0x04F80245:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter))
            {
                EmitDivot(g_pCurrentlyUpdatingCharacter);
            }
            break;

        case 0xD1B12A6E:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter))
            {
                TryFielderQueuedPass((cFielder*)g_pCurrentlyUpdatingCharacter);
            }
            break;

        case 0xE26970B8:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter))
            {
                static_cast<cFielder*>(g_pCurrentlyUpdatingCharacter)->BeginDekeIntangibility();
            }
            break;

        case 0x09FC95CF:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter))
            {
                static_cast<cFielder*>(g_pCurrentlyUpdatingCharacter)->RestoreTangibility(false);
            }
            break;

        case 0x6E78F532:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter)
                && ((cPlayer*)g_pCurrentlyUpdatingCharacter)->fn_800C2F40() != 0)
            {
                SetBallVisible(((cPlayer*)g_pCurrentlyUpdatingCharacter)->fn_800C2F40(), false);
            }
            break;

        case 0x63907309:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter)
                && ((cPlayer*)g_pCurrentlyUpdatingCharacter)->fn_800C2F40() != 0)
            {
                SetBallVisible(((cPlayer*)g_pCurrentlyUpdatingCharacter)->fn_800C2F40(), true);
            }
            break;

        case 0xC79FF559:
            if (IsControllerMirrored(g_pCurrentlyUpdatingCharacter->GetCurrentAnimController()))
            {
                SetCharacterPacketAVisible(g_pCurrentlyUpdatingCharacter, true);
            }
            else
            {
                SetCharacterPacketBVisible(g_pCurrentlyUpdatingCharacter, true);
            }
            break;

        case 0x39CB7792:
            SetCharacterPacketAVisible(g_pCurrentlyUpdatingCharacter, false);
            SetCharacterPacketBVisible(g_pCurrentlyUpdatingCharacter, false);
            break;

        case 0x3A7ADECB:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter))
            {
                ((cFielder*)g_pCurrentlyUpdatingCharacter)->fn_8004E8B8();
            }
            break;

        case 0x5D68C1D2:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter))
            {
                Desire* pDesire = GetFielderDesire((cFielder*)g_pCurrentlyUpdatingCharacter, 0x17);
                if (IsDesireActive(pDesire))
                {
                    ((DesireSuperPower*)pDesire)->EmitHeavenlyLight();
                }
            }
            break;

        case 0xCFEAC332:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter))
            {
                Desire* pDesire = GetFielderDesire((cFielder*)g_pCurrentlyUpdatingCharacter, 0x17);
                if (IsDesireActive(pDesire))
                {
                    EmitBowserJrShriek((DesireSuperPower*)pDesire);
                }
            }
            break;

        case 0xB86275EC:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter))
            {
                ((cFielder*)g_pCurrentlyUpdatingCharacter)->ReleaseHammerProjectile();
            }
            break;

        case 0x25590436:
        case 0xBBA47C42:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter))
            {
                ((cFielder*)g_pCurrentlyUpdatingCharacter)->fn_8004E92C();
            }
            break;

        case 0x09823AC3:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter))
            {
                g_pCurrentlyUpdatingCharacter->SetHammerTransformFrozen(true);
            }
            break;

        case 0xA5C86614:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter))
            {
                ((cFielder*)g_pCurrentlyUpdatingCharacter)->fn_8004EA9C();
            }
            break;

        case 0x411A6B8F:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter)
                && GetFielderActionState((cFielder*)g_pCurrentlyUpdatingCharacter) == 1)
            {
                BeginDeke((cFielder*)g_pCurrentlyUpdatingCharacter);
            }
            break;

        case 0x32D5F1D8:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter))
            {
                EndDeke((cFielder*)g_pCurrentlyUpdatingCharacter);
            }
            break;

        case 0xAC352365:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter))
            {
                ((cFielder*)g_pCurrentlyUpdatingCharacter)->DoDKSuperHit();
            }
            break;

        case 0x0E4E0F3F:
            EmitLandingFeetTrigger(g_pCurrentlyUpdatingCharacter);
            break;

        case 0x64D870A7:
            EmitToadGoalDustTrigger(g_pCurrentlyUpdatingCharacter);
            break;

        case 0x5251A784:
            EmitPullHeadOut(g_pCurrentlyUpdatingCharacter);
            break;

        case 0x16893826:
            EmitPushHeadIn((cPlayer*)g_pCurrentlyUpdatingCharacter);
            break;

        case 0xAC6452C8:
            EmitHitTrail(g_pCurrentlyUpdatingCharacter);
            break;

        case 0xE68D3F18:
            EmitSlideTackleTrail(g_pCurrentlyUpdatingCharacter);
            break;

        case 0x6D249451:
            KillHitTrail((cFielder*)g_pCurrentlyUpdatingCharacter, 0);
            break;

        case 0x0F3E9247:
        case 0xC5408AC8:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter))
            {
                ((cPlayer*)g_pCurrentlyUpdatingCharacter)->ClearPowerupAnimState(false);
            }
            break;

        case 0x00266A23:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter)
                && !((cFielder*)g_pCurrentlyUpdatingCharacter)->IsInFallAction())
            {
                EmitDaze((cPlayer*)g_pCurrentlyUpdatingCharacter);
            }
            break;

        case 0x77935C1C:
            EmitLanding(g_pCurrentlyUpdatingCharacter);
            break;

        case 0x1A8EBA53:
            if (IsCharacterFielder(g_pCurrentlyUpdatingCharacter))
            {
                ((cFielder*)g_pCurrentlyUpdatingCharacter)->PlayImpactCameraRumble();
            }
            break;

        case 0x73819990:
            EmitTackleImpact((cPlayer*)g_pCurrentlyUpdatingCharacter);
            break;

        case 0x71FED95E:
            EmitSmallRumble((cPlayer*)g_pCurrentlyUpdatingCharacter);
            break;

        case 0x18F99186:
            EmitMediumRumble((cPlayer*)g_pCurrentlyUpdatingCharacter);
            break;

        case 0xB631C31A:
            if (IsCharacterGoalie(g_pCurrentlyUpdatingCharacter))
            {
                ((Goalie*)g_pCurrentlyUpdatingCharacter)->DoPassRelease();
            }
            break;

        case 0x25642360:
        case 0x35B0F74E:
            if (IsCharacterGoalie(g_pCurrentlyUpdatingCharacter))
            {
                PlaySound(0xB, 0x38DF70D6, 0, 0);
            }
            break;

        case 0xC21A0381:
            CreateHitShockwave((cFielder*)g_pCurrentlyUpdatingCharacter, GetCharacterPosition(g_pCurrentlyUpdatingCharacter),
                GetTweakFloatValue(&gGameTweaks.mFielderTweaks->fBowserExplodeRadius));
            EmitBowserExplode(g_pCurrentlyUpdatingCharacter);
            break;

        case 0x89E63F15:
            if (IsCharacterHammerBro(g_pCurrentlyUpdatingCharacter))
            {
                if (IsControllerMirrored(g_pCurrentlyUpdatingCharacter->GetCurrentAnimController()))
                {
                    SetCharacterLeftPropAnimated(g_pCurrentlyUpdatingCharacter, true);
                }
                else
                {
                    SetCharacterRightPropAnimated(g_pCurrentlyUpdatingCharacter, true);
                }
            }
            break;

        case 0x005B41CF:
            if (IsCharacterHammerBro(g_pCurrentlyUpdatingCharacter))
            {
                if (IsControllerMirrored(g_pCurrentlyUpdatingCharacter->GetCurrentAnimController()))
                {
                    SetCharacterRightPropAnimated(g_pCurrentlyUpdatingCharacter, true);
                }
                else
                {
                    SetCharacterLeftPropAnimated(g_pCurrentlyUpdatingCharacter, true);
                }
            }
            break;

        case 0x002EF345:
        case 0x05120C87:
        case 0x0618ECF3:
        case 0x09656D24:
        case 0x12D0B9BF:
        case 0x1333263B:
        case 0x19076C94:
        case 0x1DB5C7FF:
        case 0x21001B24:
        case 0x2EF4FA11:
        case 0x42E9F9CF:
        case 0x479F48A7:
        case 0x7BEE7EA1:
        case 0x7E987E12:
        case 0x8758B65A:
        case 0x884CBC6E:
        case 0x8F5ED456:
        case 0x8F5F00CF:
        case 0x93E76D8E:
        case 0x95014E78:
        case 0x9913FAA3:
        case 0x9F338B11:
        case 0xA89AC233:
        case 0xA9BF9E5A:
        case 0xACDB2215:
        case 0xB8684601:
        case 0xC19CB638:
        case 0xC7114630:
        case 0xD4DEDCAF:
        case 0xD847ABD3:
        case 0xD900F524:
        case 0xEF7B7383:
        case 0xF2DA216B:
        case 0xFCE1230C:
            break;
        }
    }
}

void GetAnimTriggerInfo(cCharacter* pCharacter, int animID,
    bool (*pInfoCallback)(float, float, unsigned long, float, void*), void* pUserData)
{
    cSAnim* pAnim = pCharacter->m_pAnimInventory->GetAnim(animID);
    cSAnimCallback* cb = pAnim->m_pCallbackList;

    while (cb != 0)
    {
        GetAnimScriptInterpreter();
        cSAnim* pTriggerAnim = (cSAnim*)cb->m_nParam1;
        float numKeys = (float)pAnim->m_nNumKeys;
        if (!pInfoCallback(cb->m_fTime, numKeys / 30.0f, pTriggerAnim->GetHashID(), 0.0f, pUserData))
        {
            break;
        }
        cb = cb->next;
    }
}

void EmitBallImpact(cPlayer* pCharacter, bool bSilent)
{
    const char* groupName = "ball_impact";
    EmissionController* pController = EmitGeneric(pCharacter, groupName, 0);
    pController->SetPosition(g_pBall->m_v3Position);
    pController->SetVelocity(g_pBall->m_v3Velocity);
}

void EmitBallPass(cPlayer* pCharacter)
{
    const char* groupName = "ball_impact";
    EmissionController* pController = EmitGeneric(pCharacter, groupName, 0);
    pController->SetPosition(g_pBall->m_v3Position);
}

void EmitBallShot(cFielder* pCharacter,
    eBallShotEffectType eNewBallEffect, cPlayer* pPassTarget, bool bSilent,
    bool bAwardPowerup)
{
    EmissionController* pGlowControl = 0;
    int nRumble = 1;

    switch (eNewBallEffect)
    {
    case BALL_EFFECT_PERFECT_PASS:
    {
        KillWindups();
        KillBallGlow();
        static unsigned long uHash = nlStringLowerHash("skillshot_ball_meteor");
        pGlowControl = CreateBallEffect(uHash, g_pBall);
        g_pBall->InitiateBallBlur(eNewBallEffect, 0);
        PlayRumbleAction(2, pCharacter->GetGlobalPad());
        break;
    }
    case BALL_EFFECT_REGULAR_SHOT:
    {
        KillWindups();
        KillBallGlow();
        static unsigned long uHash = nlStringLowerHash("skillshot_ball_drybones");
        pGlowControl = CreateBallEffect(uHash, g_pBall);
        g_pBall->InitiateBallBlur(eNewBallEffect, 0);
        PlayRumbleAction(2, pCharacter->GetGlobalPad());
        break;
    }
    case BALL_EFFECT_ONETIMER_SHOT:
    {
        KillWindups();
        KillBallGlow();
        static unsigned long uHash = nlStringLowerHash("skillshot_ball_boo");
        pGlowControl = CreateBallEffect(uHash, g_pBall);
        PlayRumbleAction(2, pCharacter->GetGlobalPad());
        break;
    }
    case BALL_EFFECT_S2S_SHOT:
        if (pCharacter->m_eClassType == FIELDER && bAwardPowerup)
        {
            float fAmount = 0.0f;
            if (sAwardPowerupForBallCharge)
            {
                if (GetBallChargeValue(g_pBall, 0) >= sPowerupMeterBallChargeThreshold)
                {
                    fAmount = 1.0f;
                }
            }
            if (sAwardPowerupForShotSpeed)
            {
                if (pCharacter->m_pShotMeter->m_fSpeedValue >= sPowerupMeterShotSpeedThreshold)
                {
                    fAmount = 1.0f;
                }
            }
            pCharacter->IncrementPowerupMeter(0, fAmount);
        }
    case BALL_EFFECT_PERFECT_SHOT:
        if (GetBallChargeValue(g_pBall, 0) >= 4.0f)
        {
            nRumble = 2;
        }
    default:
        PlayRumbleAction(nRumble, pCharacter->GetGlobalPad());
        EmitBallChargeTransition(g_pBall);
        g_pBall->InitiateBallBlur(eNewBallEffect, pCharacter);
        break;
    }

    pCharacter->m_pTeam->GetOtherNet();

    if (pGlowControl != 0)
    {
        pGlowControl->SetPosition(g_pBall->m_v3Position);
        pGlowControl->SetVelocity(pCharacter->mUnidentified024.m_v3Velocity);
        SetBallUpdateCallback(pGlowControl);
    }
}

void KillBallShot(const char* szEffectName, bool bReallyKill)
{
    EffectsGroup* pGroup
        = EmissionManager::Instance()->GetEffectsGroup(szEffectName);

    if (pGroup != 0 && IsBallEffectPlaying(g_pBall, pGroup))
    {
        if (bReallyKill)
        {
            EmissionManager::Instance()->Destroy(pGroup);
        }
        else
        {
            EmissionManager::Instance()->Kill(pGroup);
        }
    }
}

void UpdateBallGlow(cBall* pBall)
{
    cBall* pGlowBall;

    if (!pBall->m_bVisible)
    {
        KillBallGlow();
        fn_800189C4(pBall);
    }
    else
    {
        unsigned long uHash = GetBallGlowEffectHash(GetBallChargeValue(pBall, 0));
        if (uHash != pBall->m_CurrentGlowEffect)
        {
            if (uHash == 0)
            {
                KillBallGlow();
                fn_800189C4(pBall);
            }
            else
            {
                KillBallGlow();
                pGlowBall = g_pBall;
                SetBallUpdateCallback(CreateBallEffect(uHash, pGlowBall));
                g_pBall->m_CurrentGlowEffect = uHash;
            }
        }
    }
}

unsigned long GetBallGlowEffectHash(float fCharge)
{
    static unsigned long uHash0 = nlStringLowerHash("ball_shot_windup_glow_0");
    static unsigned long uHash1 = nlStringLowerHash("ball_shot_windup_glow_1");
    static unsigned long uHash2 = nlStringLowerHash("ball_shot_windup_glow_2");
    static unsigned long uHash3 = nlStringLowerHash("ball_shot_windup_glow_3");
    static unsigned long uHashMax = nlStringLowerHash("ball_shot_windup_glow_max");

    if (fCharge < 1.0f)
    {
        return uHash0;
    }
    if (fCharge < 2.0f)
    {
        return uHash1;
    }
    if (fCharge < 3.0f)
    {
        return uHash2;
    }
    if (fCharge < 4.0f)
    {
        return uHash3;
    }
    return uHashMax;
}

static int numUpdatesUntilNextToggle;
static bool isEffectOn;

void EmitSmallRumble(cPlayer* pPlayer)
{
    PlayRumbleAction(1, pPlayer->GetGlobalPad());
}

void EmitMediumRumble(cPlayer* pPlayer)
{
    PlayRumbleAction(2, pPlayer->GetGlobalPad());
}

void EmitDivot(cCharacter* pCharacter)
{
    cSHierarchy* pHierarchy;
    EmissionController* pController = EmissionManager::Instance()->Create(
        EmissionManager::Instance()->GetEffectsGroup("divot"), 3, true, 0);
    int nNode;
    nlVector3 v3Position;
    nlVector3 v3Offset;

    if (pCharacter->m_pCurrentAnimController->m_bMirror)
    {
        pHierarchy = pCharacter->m_pPoseAccumulator->m_BaseSHierarchy;
        nNode = pHierarchy->GetNodeIndexByID(nlStringLowerHash("bip01 l prop"));
    }
    else
    {
        pHierarchy = pCharacter->m_pPoseAccumulator->m_BaseSHierarchy;
        nNode = pHierarchy->GetNodeIndexByID(nlStringLowerHash("bip01 r prop"));
    }

    v3Position = pCharacter->GetJointPosition(nNode);
    v3Position.z = 0.0f;

    nlPolarToCartesian(v3Offset.x, v3Offset.y,
        pCharacter->mUnidentified024.m_aActualFacingDirection, 0.65f);
    v3Offset.z = 0.0f;

    nlVec3Add(v3Position, v3Position, v3Offset);
    pController->SetPosition(v3Position);
    SetZeroVelocity(pController);
}

void EmitElectrocutionExplosion(const char* szEffectName, cCharacter* pCharacter)
{
    EmissionController* pController = EmitGeneric(pCharacter, szEffectName, 0);
    Function1<void, EmissionController&> update2(UpdateEmitterFromCharacter);
    pController->SetUpdateCallback(update2);
}

void EmitConfused(cPlayer* pCharacter)
{
    StopSound(pCharacter->IsCaptain() ? 0xD73B11EC : 0x1CCDFC62, pCharacter);

    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("confused");
    if (pCharacter->IsPlayingEffect(pGroup))
    {
        pCharacter->KillEffect(pGroup);
    }

    const char* groupName = "confused";
    pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pCharacter->AttachEffect(pController);
    {
        Function1<void, EmissionController&> update2(
            UpdateEmitterFromCharacter);
        pController->SetUpdateCallback(update2);
    }

    SetPlayerAudioController(pCharacter);
    PlaySound(pCharacter->m_uSoundSlotId,
        pCharacter->IsCaptain() ? 0xD73B11EC : 0x1CCDFC62,
        "confused", pCharacter);
}

bool KillConfused(cFielder* pFielder)
{
    unsigned long soundID = pFielder->IsCaptain()
                              ? 0xD73B11EC
                              : 0x1CCDFC62;
    StopSound(soundID, pFielder);

    const EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("confused");
    if (pFielder->IsPlayingEffect(pGroup))
    {
        pFielder->KillEffect(pGroup);
        return true;
    }
    return false;
}

void EmitDaze(cPlayer* pCharacter)
{
    if (pCharacter->m_eClassType == FIELDER)
    {
        StopSound(pCharacter->IsCaptain() ? 0xFDC268FB : 0x1CCDFC62, pCharacter);
    }
    else
    {
        StopSound(0x8F82BB80, pCharacter);
    }

    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("dazed");
    if (pCharacter->IsPlayingEffect(pGroup))
    {
        pCharacter->KillEffect(pGroup);
    }

    const char* groupName = "dazed";
    pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pCharacter->AttachEffect(pController);
    {
        Function1<void, EmissionController&> update2(
            UpdateEmitterFromCharacter);
        pController->SetUpdateCallback(update2);
    }

    if (pCharacter->m_eClassType == FIELDER)
    {
        PlaySound(pCharacter->m_uSoundSlotId,
            pCharacter->IsCaptain() ? 0xFDC268FB : 0x1CCDFC62,
            "dazed", pCharacter);
    }
    else
    {
        PlaySound(9, 0x8F82BB80, "dazed", pCharacter);
    }
}

bool KillDaze(cPlayer* pCharacter)
{
    if (pCharacter->m_eClassType == FIELDER)
    {
        unsigned long soundID = pCharacter->IsCaptain()
                                  ? 0xFDC268FB
                                  : 0x1CCDFC62;
        StopSound(soundID, pCharacter);
    }
    else
    {
        StopSound(0x8F82BB80, pCharacter);
    }

    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("dazed");
    if (pCharacter->IsPlayingEffect(pGroup))
    {
        pCharacter->KillEffect(pGroup);
        return true;
    }
    return false;
}

void EmitFreeze(cPlayer* pCharacter)
{
    const char* groupName = "freeze";
    EffectsGroup* pGroup;
    EmissionController* pController = EmitGeneric(pCharacter, groupName, 0);
    {
        Function1<void, EmissionController&> update2(
            UpdateEmitterFromCharacter);
        pController->SetUpdateCallback(update2);
    }

    pGroup = EmissionManager::Instance()->GetEffectsGroup("freeze_ground");
    pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    pController->SetPosition(pCharacter->mUnidentified024.m_v3Position);
    pController->SetVelocity(v3Zero);
    PlayRumbleAction(1, pCharacter->GetGlobalPad());
    pCharacter->m_pEffectsTexturing = fxGetTexturing(eFXTex_Freeze);
}

void KillFreeze(cPlayer* pCharacter)
{
    pCharacter->KillEffect(EmissionManager::Instance()->GetEffectsGroup("freeze"));
    pCharacter->m_pEffectsTexturing = 0;
}

void EmitUnFreeze(cPlayer* pCharacter)
{
    pCharacter->KillEffect(EmissionManager::Instance()->GetEffectsGroup("freeze"));
    pCharacter->m_pEffectsTexturing = 0;

    const char* groupName = "unfreeze";
    EmissionController* pController = EmitGeneric(pCharacter, groupName, 0);
    {
        Function1<void, EmissionController&> update2(
            UpdateEmitterFromCharacter);
        pController->SetUpdateCallback(update2);
    }
    PlayRumbleAction(1, pCharacter->GetGlobalPad());
}

void EmitYoshiShellBreak(cCharacter* pCharacter)
{
    const char* groupName = "yoshi_shell_break";
    EmissionController* pController = EmitGeneric(pCharacter, groupName, 0);
    pController->SetPosition(pCharacter->mUnidentified024.m_v3Position);
    pController->SetVelocity(v3Zero);
    {
        Function1<void, EmissionController&> update2(
            UpdateEmitterFromCharacter);
        pController->SetUpdateCallback(update2);
    }
}

void EmitPeachPhoto(cCharacter* pCharacter)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("peach_photo"), 0, true, 0);
    pController->SetPosition(pCharacter->mUnidentified024.m_v3Position);
    pController->SetVelocity(v3Zero);
}

void EmitMontyDekeEnter(cFielder* pFielder)
{
    EmissionController* pController = EmitGeneric(pFielder, "monty_deke_enter", 0);
    pController->SetUpdateCallback(UpdateEmitterFromCharacter);
}

void EmitMontyDekeExit(cPlayer* pPlayer)
{
    EmissionController* pController = EmitGeneric(pPlayer, "monty_deke_exit", 0);
    pController->SetUpdateCallback(UpdateEmitterFromCharacter);
}

void EmitHammerGround(const nlVector3& v3Position)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("hammer_ground"), 3, true, 0);
    pController->SetPosition(v3Position);
    pController->SetVelocity(v3Zero);
}

static char s_szHammerGroundBig[] = "hammer_ground_big";

void EmitHammerDestroyBig(const nlVector3& v3Position)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("hammer_destroy_big"), 3, true, 0);
    pController->SetPosition(v3Position);
    pController->SetVelocity(v3Zero);
}

void EmitGroundPound(cCharacter* pCharacter)
{
    if (pCharacter->mUnidentified024.m_eCharacterClass == 7)
    {
        EmissionController* pController = EmitGeneric(pCharacter, "ground_pound", 0);
        pController->SetUpdateCallback(UpdateEmitterFromCharacter);
    }
    else if (pCharacter->mUnidentified024.m_eCharacterClass == 9)
    {
        EmissionController* pController = EmitGeneric(pCharacter, "bowserjr_ground_pound", 0);
        pController->SetUpdateCallback(UpdateEmitterFromCharacter);
    }
}

void EmitMontySquishEnter(cCharacter* pCharacter)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("monty_squish_enter"), 3, true, 0);
    pController->SetPosition(pCharacter->mUnidentified024.m_v3Position);
    pController->SetVelocity(v3Zero);
}

void EmitMontySquishExit(cCharacter* pCharacter)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("monty_squish_exit"), 3, true, 0);
    pController->SetPosition(pCharacter->mUnidentified024.m_v3Position);
    pController->SetVelocity(v3Zero);
}

void EmitGoalieArmInGround(cPlayer* pPlayer)
{
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("goalie_arm_in_ground");
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);

    nlVector3 v3Point;
    nlVec3Set(v3Point, sGoalieArmInGroundOffsetX, sGoalieArmInGroundOffsetY, 0.2f);
    GetWorldPoint(v3Point, v3Point, pPlayer->mUnidentified024.m_v3Position, pPlayer->mUnidentified024.m_aActualFacingDirection);
    pController->SetPosition(v3Point);
    pController->SetVelocity(v3Zero);
}

void EmitDekeEnter(cCharacter* pCharacter, const char* szEffectName)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup(szEffectName), 3, true, 0);
    pController->SetPosition(pCharacter->mUnidentified024.m_v3Position);
    pController->SetVelocity(v3Zero);
}

void EmitDekeExit(cCharacter* pCharacter, const char* szEffectName)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup(szEffectName), 3, true, 0);
    pController->SetPosition(pCharacter->mUnidentified024.m_v3Position);
    pController->SetVelocity(v3Zero);
}

void EmitShyGuyDeke(cFielder* pFielder)
{
}

void EmitShyGuyBulletStart(cFielder* pFielder)
{
    const char* groupName = "shyguy_bullet_start";
    EmissionController* pController = EmitGeneric(pFielder, groupName, 0);
    pController->SetPosition(g_pBall->m_v3Position);
}

void EmitShyGuyBulletShoot(cFielder* pFielder)
{
    const char* groupName = "shyguy_bullet_shoot";
    EmissionController* pController = EmitGeneric(pFielder, groupName, 0);
    pController->SetPosition(pFielder->m_pBulletBill->position);
}

void EmitShyGuyBulletEnd(cFielder* pFielder)
{
    const char* groupName = "shyguy_bullet_end";
    EmissionController* pController = EmitGeneric(pFielder, groupName, 0);
    pController->SetPosition(pFielder->m_pBulletBill->position);
}

void EmitBooDekePuffStart(cCharacter* pCharacter)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("boo_deke_puff_start"), 3, true, 0);
    pController->SetPosition(pCharacter->mUnidentified024.m_v3Position);
    pController->SetVelocity(v3Zero);
}

void EmitBooDekePuffEnd(cCharacter* pCharacter)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("boo_deke_puff_end"), 3, true, 0);
    pController->SetPosition(pCharacter->mUnidentified024.m_v3Position);
    pController->SetVelocity(v3Zero);
}

void EndElectrocution(cCharacter* pCharacter)
{
    pCharacter->EndEffect(EmissionManager::Instance()->GetEffectsGroup("electrocution"));
    pCharacter->m_pEffectsTexturing = 0;
    pCharacter->SetModelType(0);
}

static int sNumUpdatesBetweenElectrocutionToggles = 4;

void ElectrocutionUpdateCallback(EmissionController& ec)
{
    UpdateEmitterFromCharacterUnculled(ec);

    cCharacter* pCharacter = (cCharacter*)ec.m_uUserData;
    if (pCharacter->m_eClassType == FIELDER && IsFielderDazed((cFielder*)pCharacter))
    {
        return;
    }

    if (numUpdatesUntilNextToggle == 0)
    {
        if (isEffectOn)
        {
            pCharacter->SetElectrocutionTextureEnabled(false);
        }
        else
        {
            pCharacter->SetElectrocutionTextureEnabled(true);
        }
        isEffectOn = !isEffectOn;
        numUpdatesUntilNextToggle = nlRandom(sNumUpdatesBetweenElectrocutionToggles, &nlDefaultSeed);
    }
    else
    {
        numUpdatesUntilNextToggle--;
    }
}

void CharacterElectrocutionEffect(cCharacter* pCharacter, const nlVector3& v3Position,
    const nlVector3& v3Normal)
{
    EmissionManager::Instance()->IsPlaying(GetCharacterIndex(pCharacter),
        EmissionManager::Instance()->GetEffectsGroup("electric_fence_character"));

    const char* szName = "electrocution";
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(szName);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetFreeUpdateCallback(pController, UpdateEmitterPoseFromCharacter);
    pCharacter->AttachEffect(pController);
    SetFreeUpdateCallback(pController, ElectrocutionUpdateCallback);
    pCharacter->SetModelType(1);
    EmitElectricFenceCharacterEffect(v3Position, v3Normal, GetCharacterIndex(pCharacter));
}

void EmitElectrocution(cCharacter* pCharacter)
{
    const char* szName = "electrocution";
    EmissionController* pController = EmitGeneric(pCharacter, szName, 0);
    {
        Function1<void, EmissionController&> update2(ElectrocutionUpdateCallback);
        pController->SetUpdateCallback(update2);
    }
    pCharacter->SetModelType(1);
}

void EmitBowserSmoke(cFielder* pFielder)
{
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("bowser_smoke");
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetFreeUpdateCallback(pController, UpdateEmitterFromCharacterHead);
    pFielder->AttachEffect(pController);
    UpdateEmitterFromCharacterHead(*pController);
}

void EndBowserSmoke(cFielder* pFielder)
{
    const EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("bowser_smoke");
    if (pFielder->IsPlayingEffect(pGroup))
    {
        pFielder->EndEffect(pGroup);
    }
}

static nlVector3 sBallWindupTransitionVelocity = { 0.0f, 0.0f, 1.0f };

void EmitBallWindupTransition(unsigned long uEffectHash)
{
    EmissionController* pControl = EmissionManager::Instance()->Create(
        fxGetGroup(EmissionManager::Instance(), uEffectHash), 3, true, 0);
    nlVector3 v3Direction = sBallWindupTransitionVelocity;

    pControl->SetVelocity(v3Direction);
    pControl->SetPosition(g_pBall->m_v3Position);
}

void EmitGoalieCatch(cPlayer* pPlayer, const char* szEffectName, bool bDoRumble)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup(szEffectName),
        3,
        true,
        0);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pPlayer->AttachEffect(pController);
    pController->SetPosition(g_pBall->m_v3Position);

    GoalieSaveData data;
    data.pGoalie = pPlayer;
    fn_8005D74C(g_pGame, &data);
}

void EmitLandingFeetTrigger(cCharacter* pCharacter)
{
    const char* groupName = "landing_feet";
    EmissionController* pController = EmitGeneric(pCharacter, groupName, 0);
    pController->SetPosition(pCharacter->mUnidentified024.m_v3Position);
}

void EmitToadGoalDustTrigger(cCharacter* pCharacter)
{
    const char* groupName = "toad_goal_hi_0_dust";
    EmissionController* pController = EmitGeneric(pCharacter, groupName, 0);
    pController->SetPosition(pCharacter->mUnidentified024.m_v3Position);
}

bool EmitWindupAtBall(const char* szEffectName)
{
    EffectsGroup* pCheckGroup
        = EmissionManager::Instance()->GetEffectsGroup(szEffectName);
    if (!IsBallEffectPlaying(g_pBall, pCheckGroup))
    {
        cBall* pBall = g_pBall;
        unsigned long uHash = nlStringLowerHash(szEffectName);
        EffectsGroup* pGroup
            = fxGetGroup(EmissionManager::Instance(), uHash);
        EmissionController* pController
            = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
        pController->m_uUserData = (unsigned long)pBall;
        nlVector3 vel = sBallEffectVelocity;
        pController->SetVelocity(vel);
        pController->m_fGround = 0.02f;
        SetBallUpdateCallback(pController);
        return true;
    }
    return false;
}

bool EmitBallGlow(const char* szEffectName)
{
    unsigned long uHash = nlStringLowerHash(szEffectName);

    KillBallGlow();

    EmissionController* pController;
    EffectsGroup* pGroup;
    cBall* pBall;
    pBall = g_pBall;
    pGroup = fxGetGroup(EmissionManager::Instance(), uHash);
    pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    pController->m_uUserData = (unsigned long)pBall;
    nlVector3 vel = sBallEffectVelocity;
    pController->SetVelocity(vel);
    pController->m_fGround = 0.02f;
    SetBallUpdateCallback(pController);
    g_pBall->m_CurrentGlowEffect = uHash;
    return true;
}

static const char* s_szHeaderTarget = "header_target";
static const char* s_szHeaderTargetActive = "header_target_active";

void EmitHeaderTarget(cBall* pBall, nlVector3* pPosition, bool bActive)
{
    const char* szName = bActive ? s_szHeaderTargetActive : s_szHeaderTarget;

    EmissionController* pController = EmissionManager::Instance()->Create(
        fxGetGroup(EmissionManager::Instance(), nlStringLowerHash(szName)),
        3, true, 0);
    pController->m_uUserData = (unsigned long)pBall;
    nlVector3 vel = sBallEffectVelocity;
    pController->SetVelocity(vel);
    pController->m_fGround = 0.02f;
    pController->SetPosition(*pPosition);
    pController->SetVelocity(v3Zero);
    Function1<void, EmissionController&> update(UpdateEmitterFromBallLandingSpot);
    pController->SetUpdateCallback(update);
}

void KillHeaderTarget(cBall* pBall, bool bActive)
{
    const char* szEffectName = bActive ? s_szHeaderTargetActive : s_szHeaderTarget;

    EmissionManager::Instance()->Destroy((unsigned long)pBall,
        EmissionManager::Instance()->GetEffectsGroup(szEffectName));
}

void KillWindups()
{
    static unsigned long sHashBallShotWindup0 =
        nlStringLowerHash("ball_shot_windup_0");
    static unsigned long sHashBallShotWindupGround0 =
        nlStringLowerHash("ball_shot_windup_ground_0");
    static unsigned long sHashBallShotWindup1 =
        nlStringLowerHash("ball_shot_windup_1");
    static unsigned long sHashBallShotWindupGround1 =
        nlStringLowerHash("ball_shot_windup_ground_1");
    static unsigned long sHashBallShotWindup2 =
        nlStringLowerHash("ball_shot_windup_2");
    static unsigned long sHashBallShotWindupGround2 =
        nlStringLowerHash("ball_shot_windup_ground_2");
    static unsigned long sHashBallShotWindup3 =
        nlStringLowerHash("ball_shot_windup_3");
    static unsigned long sHashBallShotWindupGround3 =
        nlStringLowerHash("ball_shot_windup_ground_3");
    static unsigned long sHashBallShotWindupMax =
        nlStringLowerHash("ball_shot_windup_max");
    static unsigned long sHashBallShotWindupGroundMax =
        nlStringLowerHash("ball_shot_windup_ground_max");
    static unsigned long sHashShootToScoreWindup =
        nlStringLowerHash("shoot_to_score_windup");
    static unsigned long sHashBallStsWindup =
        nlStringLowerHash("ball_sts_windup");

    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), sHashBallShotWindup0));
    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), sHashBallShotWindupGround0));
    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), sHashBallShotWindup1));
    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), sHashBallShotWindupGround1));
    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), sHashBallShotWindup2));
    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), sHashBallShotWindupGround2));
    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), sHashBallShotWindup3));
    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), sHashBallShotWindupGround3));
    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), sHashBallShotWindupMax));
    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), sHashBallShotWindupGroundMax));
    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), sHashShootToScoreWindup));
    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), sHashBallStsWindup));
}

void KillBallGlow()
{
    static unsigned long sHashBallShotWindupGlow0 =
        nlStringLowerHash("ball_shot_windup_glow_0");
    static unsigned long sHashBallShotWindupGlow1 =
        nlStringLowerHash("ball_shot_windup_glow_1");
    static unsigned long sHashBallShotWindupGlow2 =
        nlStringLowerHash("ball_shot_windup_glow_2");
    static unsigned long sHashBallShotWindupGlow3 =
        nlStringLowerHash("ball_shot_windup_glow_3");
    static unsigned long sHashBallShotWindupGlowMax =
        nlStringLowerHash("ball_shot_windup_glow_max");

    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), sHashBallShotWindupGlow0));
    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), sHashBallShotWindupGlow1));
    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), sHashBallShotWindupGlow2));
    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), sHashBallShotWindupGlow3));
    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), sHashBallShotWindupGlowMax));
    g_pBall->m_CurrentGlowEffect = 0;
}

void KillWindup(const char* szEffectName)
{
    EmissionManager::Instance()->Destroy(
        fxGetGroup(EmissionManager::Instance(), nlStringLowerHash(szEffectName)));
}

void CreateMushroomEffect(cFielder* pFielder)
{
    const char* szEffectName = "mushroom";
    EmissionController* pController = EmitGeneric(pFielder, szEffectName, 0);
    {
        Function1<void, EmissionController&> update2(
            UpdateEmitterFromCharacter);
        pController->SetUpdateCallback(update2);
    }
    PlayRumbleAction(1, pFielder->GetGlobalPad());
}

void EmitMushroom(cFielder* pFielder, bool bReinitialize)
{
    const char* szEffectName = "mushroom";
    EmissionController* pController = EmitGeneric(pFielder, szEffectName, 0);
    {
        Function1<void, EmissionController&> update2(
            UpdateEmitterFromCharacter);
        pController->SetUpdateCallback(update2);
    }

    PowerupBase::PlayPowerupSound(POWER_UP_MUSHROOM,
        PowerupBase::PWRUP_SOUND_ACTIVATE,
        pFielder->m_pPhysicsCharacter,
        0.0f,
        0);
    if (!bReinitialize)
    {
        PowerupBase::PlayPowerupSound(POWER_UP_MUSHROOM,
            PowerupBase::PWRUP_SOUND_IN_EFFECT,
            pFielder->m_pPhysicsCharacter,
            0.0f,
            pFielder);
        tDebugPrintManager::Print(DC_SOUND, "***EmitMushroom()***\n");
    }
    PlayRumbleAction(1, pFielder->GetGlobalPad());
}

void KillMushroom(cFielder* pFielder)
{
    PowerupBase::StopPowerupInEffectSound(POWER_UP_MUSHROOM,
        PowerupBase::PWRUP_SOUND_IN_EFFECT,
        pFielder);
    PowerupBase::PlayPowerupSound(POWER_UP_MUSHROOM,
        PowerupBase::PWRUP_SOUND_END,
        pFielder->m_pPhysicsCharacter,
        0.0f,
        0);
    pFielder->EndBlur();
    tDebugPrintManager::Print(DC_SOUND, "***KillMushroom()***\n");
}

void EmitStar(cFielder* pFielder, bool bReinitialize)
{
    PowerupBase::PlayPowerupSound(POWER_UP_STAR,
        PowerupBase::PWRUP_SOUND_ACTIVATE,
        pFielder->m_pPhysicsCharacter,
        0.0f,
        0);

    if (!bReinitialize)
    {
        const char* szEffectName = "star";
        EmissionController* pController = EmitGeneric(pFielder, szEffectName, 0);
        {
            Function1<void, EmissionController&> update2(
                UpdateEmitterFromCharacter);
            pController->SetUpdateCallback(update2);
        }

        PowerupBase::PlayPowerupSound(POWER_UP_STAR,
            PowerupBase::PWRUP_SOUND_IN_EFFECT,
            pFielder->m_pPhysicsCharacter,
            0.0f,
            pFielder);
        pFielder->m_pEffectsTexturing = fxGetTexturing(eFXTex_Star);
        tDebugPrintManager::Print(DC_SOUND, "***EmitStar()***\n");
    }
}

void KillStar(cFielder* pFielder)
{
    PowerupBase::StopPowerupInEffectSound(POWER_UP_STAR,
        PowerupBase::PWRUP_SOUND_IN_EFFECT,
        pFielder);
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("star");
    pFielder->KillEffect(pGroup);
    pFielder->EndBlur();
    pFielder->m_pEffectsTexturing = 0;
    tDebugPrintManager::Print(DC_SOUND, "***KillStar()***\n");
}

void EmitPullHeadOut(cCharacter* pPlayer)
{
    const char* szEffectName = "pull_head_out";
    EmissionController* pController = EmitGeneric(pPlayer, szEffectName, 0);
    {
        Function1<void, EmissionController&> update2(
            UpdateEmitterFromCharacter);
        pController->SetUpdateCallback(update2);
    }
    PlayRumbleAction(1, ((cPlayer*)pPlayer)->GetGlobalPad());
}

void EmitPushHeadIn(cPlayer* pPlayer)
{
    EmissionController* pController = EmitGeneric(pPlayer, "push_head_in", 0);
    pController->SetUpdateCallback(UpdateEmitterFromCharacter);
}

void EmitLanding(cCharacter* pCharacter)
{
    if (pCharacter->m_eClassType == FIELDER)
    {
        if (((cFielder*)pCharacter)->IsSuperGrowActive())
        {
            PlayRumbleAction(2, ((cPlayer*)pCharacter)->GetGlobalPad());

            const char* szEffectName = "landing_big";
            EmissionController* pController = EmitGeneric(pCharacter, szEffectName, 0);
        }
        else
        {
            const char* szEffectName = "landing";
            EmissionController* pController = EmitGeneric(pCharacter, szEffectName, 0);
        }
    }
}

void EmitTackleImpact(cPlayer* pCharacter)
{
    EmissionController* pController = EmissionManager::Instance()->Create(
        EmissionManager::Instance()->GetEffectsGroup("tackle_impact"), 3, true, 0);
    pController->SetPosition(
        pCharacter->m_pPoseAccumulator->GetNodeMatrix(pCharacter->m_nBip01JointIndex_0xA4).GetTranslation());
    pController->SetVelocity(v3Zero);
}

void EmitDKSuperCharge(cFielder* pFielder)
{
    EmissionController* pController = EmitGeneric(pFielder, "dk_super_charge", 0);
    pController->SetUpdateCallback(UpdateEmitterFromCharacter);
}

void KillDKSuperCharge(cFielder* pFielder)
{
    const EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("dk_super_charge");
    pFielder->EndEffect(pGroup);
}

void EmitDKSuperHit(cFielder* pFielder)
{
    EmissionController* pController = EmissionManager::Instance()->Create(
        EmissionManager::Instance()->GetEffectsGroup("dk_superhit_shockwave"), 0, true, 0);
    EmissionController* pGroundController = EmissionManager::Instance()->Create(
        EmissionManager::Instance()->GetEffectsGroup("dk_superhit_ground"), 0, true, 0);

    pController->SetVelocity(v3Zero);

    nlVector3 v3Position = pFielder->GetJointPosition(pFielder->m_nHeadJointIndex);
    v3Position.z = 0.0f;
    pController->SetPosition(v3Position);
    pGroundController->SetPosition(v3Position);
}

void EmitBowserExplode(cCharacter* pCharacter)
{
    const char* szEffectName = "bowser_explode";
    EmissionController* pController = EmitGeneric(pCharacter, szEffectName, 0);
    pController->SetPosition(pCharacter->mUnidentified024.m_v3Position);
    pController->SetVelocity(v3Zero);
}

void EmitSlideTackleTrail(cCharacter* pCharacter)
{
    EmissionController* pController = EmitGeneric(pCharacter, "slide_tackle_trail", 0);
    pController->SetUpdateCallback(UpdateEmitterFromCharacter);
}

void EmitHitTrail(cCharacter* pCharacter)
{
    if (pCharacter->m_eClassType == FIELDER && !((cFielder*)pCharacter)->IsInFallAction())
    {
        const char* szEffectName = "hit_trail";
        EmissionController* pController = EmitGeneric(pCharacter, szEffectName, 0);
        {
            Function1<void, EmissionController&> update2(
                UpdateEmitterFromCharacter);
            pController->SetUpdateCallback(update2);
        }

        if (((cFielder*)pCharacter)->IsSuperGrowActive())
        {
            PlayRumbleAction(2, ((cPlayer*)pCharacter)->GetGlobalPad());
        }
    }
}

void KillSlideTackleTrail(cFielder* pFielder, int nKill)
{
    const EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("slide_tackle_trail");
    if (nKill == 0)
    {
        pFielder->EndEffect(pGroup);
    }
    else
    {
        pFielder->KillEffect(pGroup);
    }
}

void KillHitTrail(cFielder* pFielder, int nKill)
{
    const EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("hit_trail");
    if (nKill == 0)
    {
        pFielder->EndEffect(pGroup);
    }
    else
    {
        pFielder->KillEffect(pGroup);
    }
}

void EmitPowerupIcon(cCharacter* pCharacter, int nIcon)
{
    char szIconName[0x20];

    switch (nIcon)
    {
    case 6:
        nlStrNCpy<char>(szIconName, "icon_chomp", 0x20);
        break;
    case 0:
        nlStrNCpy<char>(szIconName, "icon_shell_green", 0x20);
        break;
    case 1:
        nlStrNCpy<char>(szIconName, "icon_shell_red", 0x20);
        break;
    case 2:
        nlStrNCpy<char>(szIconName, "icon_shell_spike", 0x20);
        break;
    case 3:
        nlStrNCpy<char>(szIconName, "icon_shell_blue", 0x20);
        break;
    case 7:
        nlStrNCpy<char>(szIconName, "icon_mushroom", 0x20);
        break;
    case 9:
        nlStrNCpy<char>(szIconName, "icon_mario", 0x20);
        break;
    case 20:
        nlStrNCpy<char>(szIconName, "icon_petey", 0x20);
        break;
    case 10:
        nlStrNCpy<char>(szIconName, "icon_peach", 0x20);
        break;
    case 11:
        nlStrNCpy<char>(szIconName, "icon_dk", 0x20);
        break;
    case 12:
        nlStrNCpy<char>(szIconName, "icon_bowser", 0x20);
        break;
    case 13:
        nlStrNCpy<char>(szIconName, "icon_luigi", 0x20);
        break;
    case 14:
        nlStrNCpy<char>(szIconName, "icon_daisy", 0x20);
        break;
    case 15:
        nlStrNCpy<char>(szIconName, "icon_wario", 0x20);
        break;
    case 16:
        nlStrNCpy<char>(szIconName, "icon_waluigi", 0x20);
        break;
    case 17:
        nlStrNCpy<char>(szIconName, "icon_bowserjr", 0x20);
        break;
    case 18:
        nlStrNCpy<char>(szIconName, "icon_diddy", 0x20);
        break;
    case 19:
        nlStrNCpy<char>(szIconName, "icon_yoshi", 0x20);
        break;
    case 8:
        nlStrNCpy<char>(szIconName, "icon_star", 0x20);
        break;
    case 4:
        nlStrNCpy<char>(szIconName, "icon_banana", 0x20);
        break;
    case 5:
        nlStrNCpy<char>(szIconName, "icon_bobomb", 0x20);
        break;
    }

    EmissionController* pController = EmitGeneric(pCharacter, szIconName, 0);

    nlVector3 v3Position = pCharacter->GetJointPosition(pCharacter->m_nHeadJointIndex);
    v3Position.z += 0.1f;
    pController->SetPosition(v3Position);
    pController->SetVelocity(pCharacter->mUnidentified024.m_v3Velocity);
}

void EmitSuperGrow(cCharacter* pCharacter)
{
    EmissionController* pController;

    if (pCharacter->mUnidentified024.m_eCharacterClass == 0)
    {
        pController = EmitGeneric(pCharacter, "mario_super_grow", 0);
    }
    else
    {
        pController = EmitGeneric(pCharacter, "luigi_super_grow", 0);
    }

    Function1<void, EmissionController&> update2(UpdateEmitterFromCharacter);
    pController->SetUpdateCallback(update2);
}

void EmitSuperShrink(cCharacter* pCharacter)
{
    EmissionController* pController;

    if (pCharacter->mUnidentified024.m_eCharacterClass == 0)
    {
        pController = EmitGeneric(pCharacter, "mario_super_shrink", 0);
    }
    else
    {
        pController = EmitGeneric(pCharacter, "luigi_super_shrink", 0);
    }

    Function1<void, EmissionController&> update2(UpdateEmitterFromCharacter);
    pController->SetUpdateCallback(update2);
}

void EmitSuperFootstep(cCharacter* pCharacter, bool bRight)
{
    EmissionController* pController;

    if (bRight)
    {
        if (pCharacter->mUnidentified024.m_eCharacterClass == 0)
        {
            const char* szEffectName = "mario_right_super_footstep";
            EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(szEffectName);
            pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
            SetDefaultVelocity(pController);
            pController->m_fGround = 0.02f;
            SetPoseUpdateCallback(pController);
            pCharacter->AttachEffect(pController);
        }
        else
        {
            const char* szEffectName = "luigi_right_super_footstep";
            EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(szEffectName);
            pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
            SetDefaultVelocity(pController);
            pController->m_fGround = 0.02f;
            SetPoseUpdateCallback(pController);
            pCharacter->AttachEffect(pController);
        }
    }
    else
    {
        if (pCharacter->mUnidentified024.m_eCharacterClass == 0)
        {
            const char* szEffectName = "mario_left_super_footstep";
            EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(szEffectName);
            pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
            SetDefaultVelocity(pController);
            pController->m_fGround = 0.02f;
            SetPoseUpdateCallback(pController);
            pCharacter->AttachEffect(pController);
        }
        else
        {
            const char* szEffectName = "luigi_left_super_footstep";
            EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(szEffectName);
            pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
            SetDefaultVelocity(pController);
            pController->m_fGround = 0.02f;
            SetPoseUpdateCallback(pController);
            pCharacter->AttachEffect(pController);
        }
    }

    {
        Function1<void, EmissionController&> update2(
            UpdateEmitterFromCharacter);
        pController->SetUpdateCallback(update2);
    }

    nlVector3 v3Shake = { 0.1f, 0.08f, 0.0f };

    for (int nTeam = 0; nTeam < 2; nTeam++)
    {
        cTeam* pTeam = g_pTeams[nTeam];
        for (int i = 0; i < 4; i++)
        {
            cFielder* pFielder = pTeam->GetFielder(i);
            if (pFielder->GetGlobalPad())
            {
                PlayRumbleAction(1, pFielder->GetGlobalPad());
            }
        }
    }

    FireCameraNoiseFilter(v3Shake, 30.0f, 1.0f);
}

void EmitKoopaShellShow(cFielder* pFielder)
{
    EmissionController* pController = EmitGeneric(pFielder, "koopa_shell_show", 0);
    pController->SetUpdateCallback(UpdateEmitterFromCharacter);
}

void EmitBirdoEggShow(cCharacter* pCharacter)
{
    EmissionController* pController = EmitGeneric(pCharacter, "birdo_egg_show", 0);
    pController->SetUpdateCallback(UpdateEmitterFromCharacter);
}

void EmitKoopaShellBurst(const nlVector3& v3Position)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("koopa_shell_burst"), 3, true, 0);
    pController->SetPosition(v3Position);
    pController->SetVelocity(v3Zero);
}

void EmitBirdoEggBurst(const nlVector3& v3Position)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("birdo_egg_burst"), 3, true, 0);
    pController->SetPosition(v3Position);
    pController->SetVelocity(v3Zero);
}

void EmitHammerDestroy(const nlVector3& v3Position)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("hammer_destroy"), 3, true, 0);
    pController->SetPosition(v3Position);
    pController->SetVelocity(v3Zero);
}

void EmitSkillshotHandFire(cCharacter* pCharacter)
{
    if (!pCharacter->IsPlayingEffect(EmissionManager::Instance()->GetEffectsGroup("skillshot_hand_fire")))
    {
        const char* szEffectName = "skillshot_hand_fire";
        EmissionController* pController = EmitGeneric(pCharacter, szEffectName, 0);
        {
            Function1<void, EmissionController&> update2(UpdateEmitterFromCharacter);
            pController->SetUpdateCallback(update2);
        }
    }
}

void KillSkillshotHandFire(cCharacter* pCharacter)
{
    const EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("skillshot_hand_fire");
    if (pCharacter->IsPlayingEffect(pGroup))
    {
        pCharacter->EndEffect(pGroup);
    }
}

void EmitSkillshotPlayerOnFire(cCharacter* pCharacter)
{
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("skillshot_player_on_fire");
    if (pCharacter->IsEffectDying(pGroup))
    {
        EmissionManager::Instance()->Destroy((unsigned long)pCharacter, pGroup);
    }

    if (!pCharacter->IsPlayingEffect(pGroup))
    {
        const char* szEffectName = "skillshot_player_on_fire";
        EffectsGroup* pGroup2 = EmissionManager::Instance()->GetEffectsGroup(szEffectName);
        EmissionController* pController = EmissionManager::Instance()->Create(pGroup2, 3, true, 0);
        SetDefaultVelocity(pController);
        pController->m_fGround = 0.02f;
        SetPoseUpdateCallback(pController);
        pCharacter->AttachEffect(pController);
        {
            Function1<void, EmissionController&> update2(UpdateEmitterFromCharacter);
            pController->SetUpdateCallback(update2);
        }
    }
}

void KillSkillshotPlayerOnFire(cCharacter* pCharacter)
{
    const EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("skillshot_player_on_fire");
    if (pCharacter->IsPlayingEffect(pGroup))
    {
        pCharacter->EndEffect(pGroup);
    }
}

void EmitDeke(cCharacter* pCharacter)
{
    if (pCharacter->m_eAnimID == 0x50)
    {
        cPN_SAnimController* pAnimController = pCharacter->m_pCurrentAnimController;
        if (pAnimController->m_fTime
            < 15.0f / pAnimController->m_pSAnim->m_nNumKeys)
        {
            if (pCharacter->mUnidentified024.m_eCharacterClass == 0xB)
            {
                EmissionController* pController = EmitGeneric(pCharacter, "petey_deke", 0);
                pController->SetUpdateCallback(UpdateEmitterFromCharacter);
            }
            else if (pCharacter->mUnidentified024.m_eCharacterClass == 0xC)
            {
                EmissionController* pController = EmitGeneric(pCharacter, "birdo_deke", 0);
                pController->SetUpdateCallback(UpdateEmitterFromCharacter);
            }
        }
    }
}

void KillDeke(cCharacter* pCharacter)
{
    if (pCharacter->mUnidentified024.m_eCharacterClass == 11)
    {
        EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("petey_deke");
        EmissionManager::Instance()->Kill(pGroup);
    }
    else if (pCharacter->mUnidentified024.m_eCharacterClass == 12)
    {
        EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("birdo_deke");
        EmissionManager::Instance()->Kill(pGroup);
    }
}

void BeginDeke(cFielder* pFielder)
{
    if (pFielder->GetCharacterClass() == 7
        || pFielder->GetCharacterClass() == 9
        || pFielder->GetCharacterClass() == 1
        || pFielder->GetCharacterClass() == 3
        || pFielder->GetCharacterClass() == 11
        || pFielder->GetCharacterClass() == 12)
    {
        pFielder->muInvincibleStatus |= 1;
    }

    EmitDeke(pFielder);

    if (pFielder->GetCharacterClass() == 13 || pFielder->GetCharacterClass() == 7
        || pFielder->GetCharacterClass() == 9)
    {
        pFielder->InitMovementFromAnim(0, v3Zero, 0.0f, false);
    }
}

void EndDeke(cFielder* pFielder)
{
    pFielder->ClearInvincibility(0);

    if (pFielder->mUnidentified024.m_eCharacterClass == 3)
    {
        EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("dk_deke");
        EmissionManager::Instance()->Destroy((unsigned long)g_pBall, pGroup);
    }
    else if (pFielder->m_eActionState == 1)
    {
        if (pFielder->mUnidentified024.m_eCharacterClass == 11)
        {
            EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("petey_deke");
            EmissionManager::Instance()->Kill(pGroup);
        }
        else if (pFielder->mUnidentified024.m_eCharacterClass == 12)
        {
            EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("birdo_deke");
            EmissionManager::Instance()->Kill(pGroup);
        }

        if (pFielder->mUnidentified024.m_eCharacterClass == 1)
        {
            pFielder->InitMovementFromAnim(0, v3Zero, 0.0f, false);
        }
        else if (pFielder->mUnidentified024.m_eCharacterClass == 7)
        {
            CharacterImpactEvent event;
            event.v3Position = pFielder->mUnidentified024.m_v3Position;
            event.fRadius = sWarioGroundPoundRadius;
            event.pCharacter = pFielder;
            fn_800611F0(g_pGame, &event);
            EmitGroundPound(pFielder);
            PlaySound(pFielder->m_uSoundSlotId, 0x5BF8E132, 0, 0);
        }
        else if (pFielder->mUnidentified024.m_eCharacterClass == 9)
        {
            CharacterImpactEvent event;
            event.v3Position = pFielder->mUnidentified024.m_v3Position;
            event.fRadius = sBowserJrGroundPoundRadius;
            event.pCharacter = pFielder;
            fn_800611F0(g_pGame, &event);
            EmitGroundPound(pFielder);
            PlaySound(pFielder->m_uSoundSlotId, 0x560BD5F9, 0, 0);
        }
        else if (pFielder->mUnidentified024.m_eCharacterClass == 13)
        {
            CharacterImpactEvent event;
            nlVector3 v3Offset;
            cSHierarchy* pHierarchy = pFielder->m_pPoseAccumulator->m_BaseSHierarchy;
            int nNodeIndex = pHierarchy->GetNodeIndexByID(
                nlStringLowerHash("bip01 R Prop"));
            event.v3Position = pFielder->GetJointPosition(nNodeIndex);
            event.v3Position.z = 0.0f;
            nlPolarToCartesian(v3Offset.x, v3Offset.y,
                pFielder->mUnidentified024.m_aActualFacingDirection, sHammerBroHammerForwardOffset);
            v3Offset.z = 0.0f;
            float z = event.v3Position.z + v3Offset.z;
            float y = event.v3Position.y + v3Offset.y;
            float x = event.v3Position.x + v3Offset.x;
            nlVec3Set(event.v3Position, x, y, z);
            event.fRadius = sHammerBroHammerRadius;
            event.pCharacter = pFielder;
            fn_80060FF4(g_pGame, &event);

            EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(s_szHammerGroundBig);
            EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
            pController->SetPosition(event.v3Position);
            pController->SetVelocity(v3Zero);
            PlaySound(pFielder->m_uSoundSlotId, 0x560BD5F9, 0, 0);
        }
    }
}

void EmitWind(const nlVector3& v3Position, const nlVector3& v3Direction,
    const nlVector3& v3Velocity)
{
    EmissionController* pController = EmissionManager::Instance()->Create(
        EmissionManager::Instance()->GetEffectsGroup("wind_dry"), 3, true, 0);
    pController->SetPosition(v3Position);
    pController->SetDirection(v3Direction);
    pController->SetVelocity(v3Velocity);
}

void KillWind(bool bReallyKill)
{
    if (bReallyKill)
    {
        EmissionManager::Instance()->Destroy(
            EmissionManager::Instance()->GetEffectsGroup("wind_dry"));
    }
    else
    {
        EmissionManager::Instance()->Kill(EmissionManager::Instance()->GetEffectsGroup("wind_dry"));
    }
}

void EmitLightning(const char* szEffectName, nlVector3 v3Position, bool bStrong)
{
    cBall* pBall = g_pBall;
    unsigned long uHash = nlStringLowerHash(szEffectName);
    EffectsGroup* pGroup = fxGetGroup(EmissionManager::Instance(), uHash);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    pController->m_uUserData = (unsigned long)pBall;
    nlVector3 vel = sBallEffectVelocity;
    pController->SetVelocity(vel);
    pController->m_fGround = 0.02f;
    pController->SetPosition(v3Position);
    pController->SetVelocity(v3Zero);
}

void KillLightning(bool bReallyKill)
{
}

void EmitLightningBall()
{
    static unsigned long sHashWeatherLightningBall =
        nlStringLowerHash("weather_lightning_ball");

    cBall* pBall;
    EmissionController* pController;
    EffectsGroup* pGroup;

    pBall = g_pBall;
    pGroup = fxGetGroup(EmissionManager::Instance(), sHashWeatherLightningBall);
    pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    pController->m_uUserData = (unsigned long)pBall;
    nlVector3 vel = sBallEffectVelocity;
    pController->SetVelocity(vel);
    pController->m_fGround = 0.02f;
    pController->SetPosition(g_pBall->m_v3Position);
    pController->SetVelocity(v3Zero);
    Function1<void, EmissionController&> update(UpdateEmitterFromBall);
    pController->SetUpdateCallback(update);
}

extern "C" void fn_801BDF08(cCharacter* pCharacter)
{
}

void EmitDKDeke(cCharacter* pCharacter)
{
    const char* szEffectName = "dk_deke";
    EmissionController* pController = EmitGeneric(pCharacter, szEffectName, 0);
    {
        Function1<void, EmissionController&> update2(
            UpdateEmitterFromCharacter);
        pController->SetUpdateCallback(update2);
    }
    PlayRumbleAction(1, ((cPlayer*)pCharacter)->GetGlobalPad());
}

void SetEffectsGroupFountainLife(EffectsGroup* group, float life)
{
    EffectsSpec* spec = group->m_specs;
    int numSpecs = group->m_numSpecs;

    if (spec != 0 && numSpecs > 0)
    {
        for (int i = 0; i < numSpecs; i++, spec++)
        {
            EffectsTemplate* pTemplate = spec->m_pTemplate;
            if (pTemplate != 0)
            {
                pTemplate->m_fFountainLife = life;
            }
        }
    }
}

const char* GetCharacterEffectsName(cCharacter* pCharacter)
{
    return pCharacter->m_szEffectsName;
}

void SetCharacterPacketAVisible(cCharacter* pCharacter, bool bValue)
{
    *(bool*)((char*)pCharacter + 0x181) = bValue;
}

void SetCharacterPacketBVisible(cCharacter* pCharacter, bool bValue)
{
    *(bool*)((char*)pCharacter + 0x182) = bValue;
}

bool IsCharacterGoalie(cCharacter* pCharacter)
{
    return pCharacter->m_eClassType == GOALIE;
}

void SetCharacterLeftPropAnimated(cCharacter* pCharacter, bool bAnimated)
{
    *(bool*)((char*)pCharacter + 0x17E) = bAnimated;
}

void SetCharacterRightPropAnimated(cCharacter* pCharacter, bool bAnimated)
{
    *(bool*)((char*)pCharacter + 0x17F) = bAnimated;
}

cSAnim* GetControllerSAnim(cPN_SAnimController* pController)
{
    return pController->m_pSAnim;
}

bool IsControllerMirrored(cPN_SAnimController* pController)
{
    return pController->m_bMirror;
}

#include "Game/CharacterTriggers.inl"

cPlayer* GetBallOwner(cBall* pBall)
{
    return pBall->m_pOwner;
}

void SetBallVisible(cBall* pBall, bool bVisible)
{
    pBall->m_bVisible = bVisible;
}

bool IsDesireActive(Desire* pDesire)
{
    return pDesire->IsActive();
}

int GetFielderActionState(cFielder* pFielder)
{
    return pFielder->m_eActionState;
}

float GetTweakFloatValue(TweakFloatBinding* pTweak)
{
    return *pTweak->m_pValue;
}

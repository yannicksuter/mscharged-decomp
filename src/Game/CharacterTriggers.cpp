#include "Game/CharacterTriggers.h"
#include "Game/AnimInventory.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/Ball.h"
#include "Game/Character.h"
#include "Game/CharacterEffects.h"
#include "Game/CharacterTemplate.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Game.h"
#include "Game/Goalie.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Player.h"
#include "Game/SAnim.h"
#include "NL/nlString.h"

extern "C" EffectsGroup* fn_802E7CDC(
    EmissionManager* manager, const char* name);
extern "C" EmissionController* fn_802E7FE4(EmissionManager* manager,
    EffectsGroup* group, int view, bool persistent, bool unknown);

void UpdateEmitterPoseFromCharacter(EmissionController& emitter);
void UpdateEmitterFromCharacter(EmissionController& emitter);
extern "C" bool fn_8001E168(const cCharacter* pCharacter);
extern "C" void fn_800EC12C(unsigned long soundID, void* pParam);
extern "C" void fn_8005D74C(cGame* game, const GoalieSaveData* pSaveData);
extern "C" void fn_80139D1C(int nPreset, cGlobalPad* pPad);
extern "C" int fn_8004F594(int category, const char* format, ...);

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

void KillStar(cFielder* pFielder)
{
    PowerupBase::StopPowerupInEffectSound(POWER_UP_STAR,
        PowerupBase::PWRUP_SOUND_IN_EFFECT,
        pFielder);
    EffectsGroup* pGroup = fn_802E7CDC(
        EmissionManager::Instance(), "star");
    pFielder->KillEffect(pGroup);
    pFielder->EndBlur();
    pFielder->m_pEffectsTexturing = 0;
    fn_8004F594(10, "***KillStar()***\n");
}

void EmitStar(cFielder* pFielder, bool bParam)
{
    PowerupBase::PlayPowerupSound(POWER_UP_STAR,
        PowerupBase::PWRUP_SOUND_ACTIVATE,
        pFielder->m_pPhysicsCharacter,
        100.0f,
        0);

    if (!bParam)
    {
        EffectsGroup* pGroup = fn_802E7CDC(
            EmissionManager::Instance(), "star");
        EmissionController* pController = fn_802E7FE4(
            EmissionManager::Instance(), pGroup, 3, true, false);
        SetDefaultVelocity(pController);
        pController->m_fGround = 0.02f;
        SetPoseUpdateCallback(pController);
        pFielder->AttachEffect(pController);
        {
            Function1<void, EmissionController&> update2(
                UpdateEmitterFromCharacter);
            pController->SetUpdateCallback(update2);
        }

        PowerupBase::PlayPowerupSound(POWER_UP_STAR,
            PowerupBase::PWRUP_SOUND_IN_EFFECT,
            pFielder->m_pPhysicsCharacter,
            100.0f,
            pFielder);
        pFielder->m_pEffectsTexturing = fxGetTexturing(eFXTex_Star);
        fn_8004F594(10, "***EmitStar()***\n");
    }
}

void KillMushroom(cFielder* pFielder)
{
    PowerupBase::StopPowerupInEffectSound(POWER_UP_MUSHROOM,
        PowerupBase::PWRUP_SOUND_IN_EFFECT,
        pFielder);
    PowerupBase::PlayPowerupSound(POWER_UP_MUSHROOM,
        PowerupBase::PWRUP_SOUND_END,
        pFielder->m_pPhysicsCharacter,
        100.0f,
        0);
    pFielder->EndBlur();
    fn_8004F594(10, "***KillMushroom()***\n");
}

void EmitMushroom(cFielder* pFielder, bool bParam)
{
    EffectsGroup* pGroup = fn_802E7CDC(
        EmissionManager::Instance(), "mushroom");
    EmissionController* pController = fn_802E7FE4(
        EmissionManager::Instance(), pGroup, 3, true, false);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pFielder->AttachEffect(pController);
    {
        Function1<void, EmissionController&> update2(
            UpdateEmitterFromCharacter);
        pController->SetUpdateCallback(update2);
    }

    PowerupBase::PlayPowerupSound(POWER_UP_MUSHROOM,
        PowerupBase::PWRUP_SOUND_ACTIVATE,
        pFielder->m_pPhysicsCharacter,
        100.0f,
        0);
    if (!bParam)
    {
        PowerupBase::PlayPowerupSound(POWER_UP_MUSHROOM,
            PowerupBase::PWRUP_SOUND_IN_EFFECT,
            pFielder->m_pPhysicsCharacter,
            100.0f,
            pFielder);
        fn_8004F594(10, "***EmitMushroom()***\n");
    }
    fn_80139D1C(1, pFielder->GetGlobalPad());
}

void EmitGoalieCatch(cPlayer* pPlayer, const char* name, bool bRumble)
{
    EmissionController* pController = fn_802E7FE4(
        EmissionManager::Instance(),
        fn_802E7CDC(EmissionManager::Instance(), name),
        3,
        true,
        false);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pPlayer->AttachEffect(pController);
    pController->SetPosition(g_pBall->m_v3Position);

    GoalieSaveData data;
    data.pGoalie = pPlayer;
    fn_8005D74C(g_pGame, &data);
}

bool KillDaze(cPlayer* player)
{
    if (player->m_eClassType == FIELDER)
    {
        unsigned long soundID = fn_8001E168(player)
                                  ? 0xFDC268FB
                                  : 0x1CCDFC62;
        fn_800EC12C(soundID, player);
    }
    else
    {
        fn_800EC12C(0x8F82BB80, player);
    }

    EffectsGroup* pGroup = fn_802E7CDC(
        EmissionManager::Instance(), "dazed");
    if (player->IsPlayingEffect(pGroup))
    {
        player->KillEffect(pGroup);
        return true;
    }
    return false;
}

void GetAnimTriggerInfo(cCharacter* pCharacter, int animIndex,
    bool (*callback)(float, float, unsigned long, float, void*), void* pData)
{
    cSAnim* pAnim = pCharacter->m_pAnimInventory->GetAnim(animIndex);
    cSAnimCallback* cb = pAnim->m_pCallbackList;

    while (cb != 0)
    {
        fn_80025E9C();
        cSAnim* pTriggerAnim = (cSAnim*)cb->m_nParam1;
        float numKeys = (float)pAnim->m_nNumKeys;
        if (!callback(cb->m_fTime, numKeys / 30.0f, pTriggerAnim->GetHashID(), 0.0f, pData))
        {
            break;
        }
        cb = cb->next;
    }
}

EmissionController* EmitGeneric(cCharacter* pCharacter, const char* baseName,
    const char* characterName)
{
    EffectsGroup* pGroup;

    if (characterName != 0)
    {
        char effectName[0x100];
        nlStrNCat<char>(effectName, characterName, "_", 0x100);
        nlStrNCat<char>(effectName, effectName, baseName, 0x100);
        nlStrNCat<char>(effectName, effectName, "_", 0x100);
        nlStrNCat<char>(effectName, effectName, "grass", 0x100);

        pGroup = fn_802E7CDC(
            EmissionManager::Instance(), effectName);
        if (pGroup == 0 && characterName[0] != '\0')
        {
            char fallbackName[0x100];
            nlStrNCat<char>(fallbackName, "", "_", 0x100);
            nlStrNCat<char>(fallbackName, fallbackName, baseName, 0x100);
            nlStrNCat<char>(fallbackName, fallbackName, "_", 0x100);
            nlStrNCat<char>(fallbackName, fallbackName, "grass", 0x100);
            pGroup = fn_802E7CDC(
                EmissionManager::Instance(), fallbackName);
        }
    }
    else
    {
        pGroup = fn_802E7CDC(EmissionManager::Instance(), baseName);
    }

    EmissionController* pControl = fn_802E7FE4(
        EmissionManager::Instance(), pGroup, 3, true, false);
    SetDefaultVelocity(pControl);
    pControl->m_fGround = 0.02f;
    SetPoseUpdateCallback(pControl);
    pCharacter->AttachEffect(pControl);
    return pControl;
}

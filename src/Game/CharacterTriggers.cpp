#include "Game/Sys/audio.h"
#include "Game/CharacterTriggers.h"
#include "Game/RumbleActions.h"
#include "Game/Sys/debug.h"
#include "Game/AnimInventory.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/Ball.h"
#include "Game/Character.h"
#include "Game/CharacterEffects.h"
#include "Game/CharacterTemplate.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmitterCallbacks.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Game.h"
#include "Game/Goalie.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Player.h"
#include "Game/SAnim.h"
#include "NL/nlString.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/BulletBill.h"
#include "Game/AI/AiUtil.h"
#include "Game/Render/ElectricFence.h"
#include "Game/AI/ShotMeter.h"
#include "Game/PoseAccumulator.h"
#include "Game/SHierarchy.h"
#include "Game/Team.h"
#include "Game/GameTweaks.h"
#include "Game/AI/Desire.h"
#include "Game/Effects/EffectsGroup.h"
#include "Game/TweakValue.h"

extern "C" bool fn_8001E168(const cCharacter* pCharacter);
extern "C" void fn_8005D74C(cGame* game, const GoalieSaveData* pSaveData);
static bool lbl_806E16F8;
static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };
static float lbl_806DD1C0 = 2.0f;
static float lbl_806DD1C4 = 2.0f;
static float lbl_806DD1C8 = 2.0f;
static float lbl_806DD1CC = 0.25f;
static float lbl_806DD1D0 = 1.0f;
static float lbl_806DD1D4 = 1.0f;
static bool lbl_806DD1D8 = true;
static float lbl_806DD1DC = 1.0f;
static float lbl_806DD1E0 = -0.3f;
extern "C" EffectsGroup* fn_802E7D54(EmissionManager* pManager, unsigned long uHashID);
extern "C" void fn_802E8A2C(EmissionManager* pManager, const EffectsGroup* pEffectsGroup);
static nlVector3 lbl_80515478 = { 0.0f, 0.0f, 1.0f };
extern "C" void fn_800EDCE8(cPlayer* pPlayer);
extern "C" bool fn_800EBBFC(int slotId, unsigned long cueId, const void* debugName, void* context);
extern "C" void fn_8001C510(cCharacter* pCharacter, bool bEnable);
extern "C" bool fn_8001E2C0(cCharacter* pCharacter, EffectsGroup* pGroup);
extern "C" bool fn_8003877C(cFielder* pFielder);
extern "C" bool fn_80014EA4(cBall* pBall, const EffectsGroup* pGroup);
extern "C" void fn_800152B4(cBall* pBall);
extern "C" void fn_800189C4(cBall* pBall);
extern "C" void fn_80036A38(cFielder* pFielder, int nParam, float fAmount);
extern "C" void fn_802E83C4(EmissionManager* pManager, const EffectsGroup* pGroup);
extern "C" void fn_802E8A2C(EmissionManager* pManager, const EffectsGroup* pGroup);
extern "C" void fn_800F026C(const nlVector3& v3Shake, float fParam1, float fParam2);
extern "C" {
} // extern "C"
extern "C" void fn_80038158(cFielder* pFielder, int nParam);
extern "C" void fn_800611F0(cGame* pGame, const void* pEventData);
extern "C" void fn_80060FF4(cGame* pGame, const void* pEventData);
struct CharacterImpactEvent
{
    nlVector3 v3Position;
    float fMagnitude;
    cCharacter* pCharacter;
};
extern "C" EffectsGroup* fn_802E7D54(EmissionManager* pManager, unsigned long uHash);
extern "C" void fn_802E83C4(EmissionManager* pManager, const EffectsGroup* pEffectsGroup);
class PhysicsSphere_80175F8C;
extern cCharacter* lbl_806E0C34;
extern "C" bool fn_8019464C(cCharacter*);
extern "C" bool fn_80194660(cCharacter*);
extern "C" cPN_SAnimController* fn_800C2F64(cCharacter*);
extern "C" cBall* fn_800C2F40(cPlayer*);
extern "C" const nlVector3* fn_800D1450(const cCharacter*);
extern "C" bool fn_800EBBFC(int, unsigned long, const void*, void*);
extern "C" void fn_800CAB18(Desire*);
extern "C" void fn_800C9DB4(Desire*);
extern "C" void fn_8001EFE4(cFielder*, int);
extern "C" void fn_800318F8(cFielder*);
extern "C" void fn_800395C0(cFielder*);
extern "C" void fn_800367B4(cFielder*);
extern "C" void fn_80039CF0(cFielder*, int);
extern "C" PhysicsSphere_80175F8C* fn_801765C8(cFielder*, const nlVector3*, float);

extern "C" float fn_801BE138(TweakValueImpl_804F4DC8* pTweak);
extern "C" int fn_801BE130(cFielder* pFielder);
extern "C" bool fn_801BE128(Desire* pDesire);
extern "C" void fn_801BE120(cBall* pBall, bool bVisible);
extern "C" cPlayer* fn_801BE118(cBall* pBall);
extern "C" bool fn_801BE0EC(cPlayer* pPlayer);
extern "C" bool fn_801BE0E0(cPN_SAnimController* pController);
extern "C" cSAnim* fn_801BE0D8(cPN_SAnimController* pController);
extern "C" void fn_801BE0D0(cCharacter* pCharacter, bool bValue);
extern "C" void fn_801BE0C8(cCharacter* pCharacter, bool bValue);
extern "C" bool fn_801BE0B4(cCharacter* pCharacter);
extern "C" void fn_801BE0AC(cCharacter* pCharacter, bool bValue);
extern "C" void fn_801BE0A4(cCharacter* pCharacter, bool bValue);
extern "C" const char* fn_801BE09C(cCharacter* pCharacter);
void SetEffectsGroupFountainLife(EffectsGroup* group, float life);
extern "C" void fn_801BDF0C(cCharacter* pCharacter);
extern "C" void fn_801BDF08(cCharacter* pCharacter);
extern "C" void fn_801BDDE4();
extern "C" void fn_801BDDE0(cCharacter* pCharacter);
extern "C" void fn_801BDD24(const char* name, const nlVector3& v3Position, bool bParam);
extern "C" void fn_801BDCB4(int bParam);
extern "C" void fn_801BDC1C(const nlVector3& v3Position, const nlVector3& v3Direction, const nlVector3& v3Velocity);
extern "C" void fn_801BD4EC(cFielder* pFielder);
extern "C" void fn_801BD1C0(cFielder* pFielder);
extern "C" void fn_801BD144(cCharacter* pCharacter);
extern "C" void fn_801BCE90(cCharacter* pCharacter);
extern "C" void fn_801BCE2C(cCharacter* pCharacter);
extern "C" void fn_801BCC9C(cCharacter* pCharacter);
extern "C" void fn_801BCC38(cCharacter* pCharacter);
extern "C" void fn_801BCAD4(cCharacter* pCharacter);
extern "C" void fn_801BCA5C(const nlVector3& v3Position);
extern "C" void fn_801BC9E4(const nlVector3& v3Position);
extern "C" void fn_801BC96C(const nlVector3& v3Position);
extern "C" void fn_801BC828(cCharacter* pCharacter);
extern "C" void fn_801BC6E4(cFielder* pFielder);
extern "C" void fn_801BC2A8(cCharacter* pCharacter, bool bRight);
extern "C" void fn_801BC094(cCharacter* pCharacter);
extern "C" void fn_801BBE80(cCharacter* pCharacter);
extern "C" void fn_801BB6A4(cCharacter* pCharacter, int nIcon);
extern "C" void fn_801BB640(cFielder* pFielder, int nParam);
extern "C" void fn_801BB5DC(cFielder* pFielder, int nParam);
extern "C" void fn_801BB45C(cCharacter* pCharacter);
extern "C" void fn_801BB318(cCharacter* pCharacter);
extern "C" void fn_801BB20C(cCharacter* pCharacter);
extern "C" void fn_801BB120(cFielder* pFielder);
extern "C" void fn_801BB0DC(cFielder* pFielder);
extern "C" void fn_801BAF98(cFielder* pFielder);
extern "C" void fn_801BAF0C(cPlayer* pCharacter);
extern "C" void fn_801BAD30(cCharacter* pCharacter);
extern "C" void fn_801BABEC(cPlayer* pPlayer);
extern "C" void fn_801BAA94(cCharacter* pCharacter);
void KillStar(cFielder* pFielder);
void EmitStar(cFielder* pFielder, bool bParam);
void KillMushroom(cFielder* pFielder);
void EmitMushroom(cFielder* pFielder, bool bParam);
extern "C" void fn_801BA510(cFielder* pFielder);
extern "C" void fn_801BA4C8(const char* szName);
extern "C" void fn_801BA358();
extern "C" void fn_801BA034();
extern "C" void fn_801B9FD0(cBall* pBall, bool bActive);
extern "C" void fn_801B9EAC(cBall* pBall, nlVector3* pPosition, bool bActive);
extern "C" bool fn_801B9DAC(const char* szEffectName);
extern "C" bool fn_801B9C90(const char* szEffectName);
extern "C" void fn_801B9B94(cCharacter* pCharacter);
extern "C" void fn_801B9A98(cCharacter* pCharacter);
void EmitGoalieCatch(cPlayer* pPlayer, const char* name, bool bRumble);
extern "C" void fn_801B9904(unsigned long uEffectHash);
extern "C" void fn_801B98A0(cFielder* pFielder);
extern "C" void fn_801B97DC(cFielder* pFielder);
extern "C" void fn_801B968C(cCharacter* pCharacter);
extern "C" void fn_801B94EC(cCharacter* pCharacter, const nlVector3& v3Position, const nlVector3& v3Normal);
extern "C" void fn_801B9440(EmissionController& ec);
extern "C" void fn_801B93E8(cCharacter* pCharacter);
extern "C" void fn_801B9370(cCharacter* pCharacter);
extern "C" void fn_801B92F8(cCharacter* pCharacter);
extern "C" void fn_801B91F8(cFielder* pFielder);
extern "C" void fn_801B90F8(cFielder* pFielder);
extern "C" void fn_801B8FF8(cFielder* pFielder);
extern "C" void fn_801B8FF4(cFielder* pFielder);
extern "C" void fn_801B8F7C(cCharacter* pCharacter, const char* name);
extern "C" void fn_801B8F04(cCharacter* pCharacter, const char* name);
extern "C" void fn_801B8E5C(cPlayer* pPlayer);
extern "C" void fn_801B8DE4(cCharacter* pCharacter);
extern "C" void fn_801B8D6C(cCharacter* pCharacter);
extern "C" void fn_801B8CF4(const nlVector3& v3Position);
extern "C" void fn_801B8C7C(const nlVector3& v3Position);
extern "C" void fn_801B8B38(cPlayer* pPlayer);
extern "C" void fn_801B89F4(cFielder* pFielder);
extern "C" void fn_801B897C(cCharacter* pCharacter);
extern "C" void fn_801B881C(cCharacter* pCharacter);
extern "C" void fn_801B86A4(cPlayer* pCharacter);
extern "C" void fn_801B865C(cPlayer* pCharacter);
extern "C" void fn_801B84B4(cPlayer* pCharacter);
bool KillDaze(cPlayer* player);
extern "C" void fn_801B81F8(cPlayer* pCharacter);
extern "C" bool fn_801B8164(cFielder* pFielder);
extern "C" void fn_801B7F8C(cPlayer* pCharacter);
extern "C" void fn_801B7E4C(const char* pName, cCharacter* pCharacter);
extern "C" void fn_801B7D00(cCharacter* pCharacter);
extern "C" void fn_801B7CD4(cPlayer* pPlayer);
extern "C" void fn_801B7CA8(cPlayer* pPlayer);
extern "C" unsigned long fn_801B7B70(float fLevel);
extern "C" void fn_801B7A28(cBall* pBall);
extern "C" void fn_801B79A4(const char* szEffectName, bool bReallyKill);
extern "C" void fn_801B75C8(cFielder* pCharacter, eBallShotEffectType eNewBallEffect, cPlayer* pPassTarget, bool bSilent, bool bParam5);
extern "C" void fn_801B74C8(cPlayer* pCharacter);
extern "C" void fn_801B73B8(cPlayer* pCharacter);
void GetAnimTriggerInfo(cCharacter* pCharacter, int animIndex, bool (*callback)(float, float, unsigned long, float, void*), void* pData);

EmissionController* EmitGeneric(cCharacter* pCharacter, const char* baseName, const char* characterName);

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
    EmissionController* pControl = EmissionManager::Instance()->Create(
        fn_802E7D54(EmissionManager::Instance(), uEffectHash), 3, true, 0);
    pControl->m_uUserData = (unsigned long)pBall;

    nlVector3 v3Direction = lbl_80515478;
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
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(szName);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pCharacter->AttachEffect(pController);
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
    class AnimTriggerCallbackInfo
    {
    public:
        long m_uEventID;
        float m_fIntensity;
    };

    AnimTriggerCallbackInfo* pInfo = (AnimTriggerCallbackInfo*)uParam;

    fn_801BE09C(lbl_806E0C34);

    if (fn_801BE0D8(fn_800C2F64(lbl_806E0C34)) == pAnim
        || pInfo->m_uEventID == 0xC5408AC8
        || pInfo->m_uEventID == 0x0F3E9247)
    {
        switch (pInfo->m_uEventID)
        {
        case 0xC9F4F4B0:
        {
            u32 hasPad = fn_801BE0EC((cPlayer*)lbl_806E0C34);
            bool ownsBall
                = (g_pBall != 0 && lbl_806E0C34 == fn_801BE118(g_pBall));
            if (g_pBall != 0 && !ownsBall)
            {
                break;
            }
            fn_800EBBFC(0xB, 0x38DF70D6, 0, 0);
            break;
        }

        case 0x3650221E:
        case 0xE272718B:
            if (fn_8019464C(lbl_806E0C34))
            {
                fn_801BC2A8(lbl_806E0C34, pInfo->m_uEventID == 0x3650221E);
            }
            break;

        case 0x50DF765D:
        {
            u32 hasPad = fn_801BE0EC((cPlayer*)lbl_806E0C34);
            bool ownsBall
                = (g_pBall != 0 && lbl_806E0C34 == fn_801BE118(g_pBall));
            if (g_pBall != 0 && !ownsBall)
            {
                break;
            }
            fn_800EBBFC(0xB, 0x38DF70D6, 0, 0);
            break;
        }

        case 0x05D46E25:
            if (fn_8019464C(lbl_806E0C34))
            {
                fn_801BDF0C(lbl_806E0C34);
            }
            break;

        case 0x04F80245:
            if (fn_8019464C(lbl_806E0C34))
            {
                fn_801B7D00(lbl_806E0C34);
            }
            break;

        case 0xD1B12A6E:
            if (fn_8019464C(lbl_806E0C34))
            {
                fn_800318F8((cFielder*)lbl_806E0C34);
            }
            break;

        case 0xE26970B8:
            if (fn_8019464C(lbl_806E0C34))
            {
                fn_800395C0((cFielder*)lbl_806E0C34);
            }
            break;

        case 0x09FC95CF:
            if (fn_8019464C(lbl_806E0C34))
            {
                fn_80039CF0((cFielder*)lbl_806E0C34, 0);
            }
            break;

        case 0x6E78F532:
            if (fn_8019464C(lbl_806E0C34)
                && fn_800C2F40((cPlayer*)lbl_806E0C34) != 0)
            {
                fn_801BE120(fn_800C2F40((cPlayer*)lbl_806E0C34), false);
            }
            break;

        case 0x63907309:
            if (fn_8019464C(lbl_806E0C34)
                && fn_800C2F40((cPlayer*)lbl_806E0C34) != 0)
            {
                fn_801BE120(fn_800C2F40((cPlayer*)lbl_806E0C34), true);
            }
            break;

        case 0xC79FF559:
            if (fn_801BE0E0(fn_800C2F64(lbl_806E0C34)))
            {
                fn_801BE0A4(lbl_806E0C34, true);
            }
            else
            {
                fn_801BE0AC(lbl_806E0C34, true);
            }
            break;

        case 0x39CB7792:
            fn_801BE0A4(lbl_806E0C34, false);
            fn_801BE0AC(lbl_806E0C34, false);
            break;

        case 0x3A7ADECB:
            if (fn_8019464C(lbl_806E0C34))
            {
                ((cFielder*)lbl_806E0C34)->fn_8004E8B8();
            }
            break;

        case 0x5D68C1D2:
            if (fn_8019464C(lbl_806E0C34))
            {
                Desire* pDesire = fn_8002E08C((cFielder*)lbl_806E0C34, 0x17);
                if (fn_801BE128(pDesire))
                {
                    fn_800CAB18(pDesire);
                }
            }
            break;

        case 0xCFEAC332:
            if (fn_8019464C(lbl_806E0C34))
            {
                Desire* pDesire = fn_8002E08C((cFielder*)lbl_806E0C34, 0x17);
                if (fn_801BE128(pDesire))
                {
                    fn_800C9DB4(pDesire);
                }
            }
            break;

        case 0xB86275EC:
            if (fn_8019464C(lbl_806E0C34))
            {
                ((cFielder*)lbl_806E0C34)->fn_8004E6B4();
            }
            break;

        case 0x25590436:
        case 0xBBA47C42:
            if (fn_8019464C(lbl_806E0C34))
            {
                ((cFielder*)lbl_806E0C34)->fn_8004E92C();
            }
            break;

        case 0x09823AC3:
            if (fn_8019464C(lbl_806E0C34))
            {
                fn_8001EFE4((cFielder*)lbl_806E0C34, 1);
            }
            break;

        case 0xA5C86614:
            if (fn_8019464C(lbl_806E0C34))
            {
                ((cFielder*)lbl_806E0C34)->fn_8004EA9C();
            }
            break;

        case 0x411A6B8F:
            if (fn_8019464C(lbl_806E0C34)
                && fn_801BE130((cFielder*)lbl_806E0C34) == 1)
            {
                fn_801BD1C0((cFielder*)lbl_806E0C34);
            }
            break;

        case 0x32D5F1D8:
            if (fn_8019464C(lbl_806E0C34))
            {
                fn_801BD4EC((cFielder*)lbl_806E0C34);
            }
            break;

        case 0xAC352365:
            if (fn_8019464C(lbl_806E0C34))
            {
                ((cFielder*)lbl_806E0C34)->fn_8004F8E8();
            }
            break;

        case 0x0E4E0F3F:
            fn_801B9A98(lbl_806E0C34);
            break;

        case 0x64D870A7:
            fn_801B9B94(lbl_806E0C34);
            break;

        case 0x5251A784:
            fn_801BAA94(lbl_806E0C34);
            break;

        case 0x16893826:
            fn_801BABEC((cPlayer*)lbl_806E0C34);
            break;

        case 0xAC6452C8:
            fn_801BB45C(lbl_806E0C34);
            break;

        case 0xE68D3F18:
            fn_801BB318(lbl_806E0C34);
            break;

        case 0x6D249451:
            fn_801BB640((cFielder*)lbl_806E0C34, 0);
            break;

        case 0x0F3E9247:
        case 0xC5408AC8:
            if (fn_8019464C(lbl_806E0C34))
            {
                ((cPlayer*)lbl_806E0C34)->ClearPowerupAnimState(false);
            }
            break;

        case 0x00266A23:
            if (fn_8019464C(lbl_806E0C34)
                && !((cFielder*)lbl_806E0C34)->fn_800344B0())
            {
                fn_801B81F8((cPlayer*)lbl_806E0C34);
            }
            break;

        case 0x77935C1C:
            fn_801BAD30(lbl_806E0C34);
            break;

        case 0x1A8EBA53:
            if (fn_8019464C(lbl_806E0C34))
            {
                fn_800367B4((cFielder*)lbl_806E0C34);
            }
            break;

        case 0x73819990:
            fn_801BAF0C((cPlayer*)lbl_806E0C34);
            break;

        case 0x71FED95E:
            fn_801B7CA8((cPlayer*)lbl_806E0C34);
            break;

        case 0x18F99186:
            fn_801B7CD4((cPlayer*)lbl_806E0C34);
            break;

        case 0xB631C31A:
            if (fn_801BE0B4(lbl_806E0C34))
            {
                ((Goalie*)lbl_806E0C34)->DoPassRelease();
            }
            break;

        case 0x25642360:
        case 0x35B0F74E:
            if (fn_801BE0B4(lbl_806E0C34))
            {
                fn_800EBBFC(0xB, 0x38DF70D6, 0, 0);
            }
            break;

        case 0xC21A0381:
            fn_801765C8((cFielder*)lbl_806E0C34, fn_800D1450(lbl_806E0C34),
                fn_801BE138(&gGameTweaks.m_unk14->mUnidentified414));
            fn_801BB20C(lbl_806E0C34);
            break;

        case 0x89E63F15:
            if (fn_80194660(lbl_806E0C34))
            {
                if (fn_801BE0E0(fn_800C2F64(lbl_806E0C34)))
                {
                    fn_801BE0C8(lbl_806E0C34, true);
                }
                else
                {
                    fn_801BE0D0(lbl_806E0C34, true);
                }
            }
            break;

        case 0x005B41CF:
            if (fn_80194660(lbl_806E0C34))
            {
                if (fn_801BE0E0(fn_800C2F64(lbl_806E0C34)))
                {
                    fn_801BE0D0(lbl_806E0C34, true);
                }
                else
                {
                    fn_801BE0C8(lbl_806E0C34, true);
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

extern "C" void fn_801B73B8(cPlayer* pCharacter)
{
    const char* groupName = "ball_impact";
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pCharacter->AttachEffect(pController);
    pController->SetPosition(g_pBall->m_v3Position);
    pController->SetVelocity(g_pBall->m_v3Velocity);
}

extern "C" void fn_801B74C8(cPlayer* pCharacter)
{
    const char* groupName = "ball_impact";
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pCharacter->AttachEffect(pController);
    pController->SetPosition(g_pBall->m_v3Position);
}

extern "C" void fn_801B75C8(cFielder* pCharacter,
    eBallShotEffectType eNewBallEffect, cPlayer* pPassTarget, bool bSilent,
    bool bParam5)
{
    EmissionController* pGlowControl = 0;
    int nRumble = 1;

    switch (eNewBallEffect)
    {
    case BALL_EFFECT_PERFECT_PASS:
    {
        cBall* pBall;
        fn_801BA034();
        fn_801BA358();
        static unsigned long uHash = nlStringLowerHash("skillshot_ball_meteor");
        pBall = g_pBall;
        pGlowControl = CreateBallEffect(uHash, pBall);
        g_pBall->InitiateBallBlur(eNewBallEffect, 0);
        PlayRumbleAction(2, pCharacter->GetGlobalPad());
        break;
    }
    case BALL_EFFECT_REGULAR_SHOT:
    {
        fn_801BA034();
        fn_801BA358();
        static unsigned long uHash = nlStringLowerHash("skillshot_ball_drybones");
        pGlowControl = CreateBallEffect(uHash, g_pBall);
        g_pBall->InitiateBallBlur(eNewBallEffect, 0);
        PlayRumbleAction(2, pCharacter->GetGlobalPad());
        break;
    }
    case BALL_EFFECT_ONETIMER_SHOT:
    {
        fn_801BA034();
        fn_801BA358();
        static unsigned long uHash = nlStringLowerHash("skillshot_ball_boo");
        pGlowControl = CreateBallEffect(uHash, g_pBall);
        PlayRumbleAction(2, pCharacter->GetGlobalPad());
        break;
    }
    case BALL_EFFECT_S2S_SHOT:
        if (pCharacter->m_eClassType == FIELDER && bParam5)
        {
            float fAmount = 0.0f;
            if (lbl_806E16F8)
            {
                if (fn_800155A0(g_pBall, 0) >= lbl_806DD1D4)
                {
                    fAmount = 1.0f;
                }
            }
            if (lbl_806DD1D8)
            {
                if (pCharacter->m_pShotMeter->m_fSpeedValue >= lbl_806DD1D0)
                {
                    fAmount = 1.0f;
                }
            }
            fn_80036A38(pCharacter, 0, fAmount);
        }
    case BALL_EFFECT_PERFECT_SHOT:
        if (fn_800155A0(g_pBall, 0) >= 4.0f)
        {
            nRumble = 2;
        }
    default:
        PlayRumbleAction(nRumble, pCharacter->GetGlobalPad());
        fn_800152B4(g_pBall);
        g_pBall->InitiateBallBlur(eNewBallEffect, pCharacter);
        break;
    }

    pCharacter->m_pTeam->GetOtherNet();

    if (pGlowControl != 0)
    {
        pGlowControl->SetPosition(g_pBall->m_v3Position);
        pGlowControl->SetVelocity(pCharacter->m_v3Velocity);
        SetBallUpdateCallback(pGlowControl);
    }
}

extern "C" void fn_801B79A4(const char* szEffectName, bool bReallyKill)
{
    EffectsGroup* pGroup
        = EmissionManager::Instance()->GetEffectsGroup(szEffectName);

    if (pGroup != 0 && fn_80014EA4(g_pBall, pGroup))
    {
        if (bReallyKill)
        {
            fn_802E8A2C(EmissionManager::Instance(), pGroup);
        }
        else
        {
            fn_802E83C4(EmissionManager::Instance(), pGroup);
        }
    }
}

extern "C" void fn_801B7A28(cBall* pBall)
{
    if (!pBall->m_bVisible)
    {
        fn_801BA358();
        fn_800189C4(pBall);
    }
    else
    {
        unsigned long uHash = fn_801B7B70(fn_800155A0(pBall, 0));
        if (uHash != pBall->m_CurrentGlowEffect)
        {
            if (uHash == 0)
            {
                fn_801BA358();
                fn_800189C4(pBall);
            }
            else
            {
                fn_801BA358();
                SetBallUpdateCallback(CreateBallEffect(uHash, g_pBall));
                g_pBall->m_CurrentGlowEffect = uHash;
            }
        }
    }
}

extern "C" unsigned long fn_801B7B70(float fLevel)
{
    static unsigned long uHash0 = nlStringLowerHash("ball_shot_windup_glow_0");
    static unsigned long uHash1 = nlStringLowerHash("ball_shot_windup_glow_1");
    static unsigned long uHash2 = nlStringLowerHash("ball_shot_windup_glow_2");
    static unsigned long uHash3 = nlStringLowerHash("ball_shot_windup_glow_3");
    static unsigned long uHashMax = nlStringLowerHash("ball_shot_windup_glow_max");

    if (fLevel < 1.0f)
    {
        return uHash0;
    }
    if (fLevel < 2.0f)
    {
        return uHash1;
    }
    if (fLevel < 3.0f)
    {
        return uHash2;
    }
    if (fLevel < 4.0f)
    {
        return uHash3;
    }
    return uHashMax;
}

static int lbl_806E173C;
static bool lbl_806E1740;

extern "C" void fn_801B7CA8(cPlayer* pPlayer)
{
    PlayRumbleAction(1, pPlayer->GetGlobalPad());
}

extern "C" void fn_801B7CD4(cPlayer* pPlayer)
{
    PlayRumbleAction(2, pPlayer->GetGlobalPad());
}

extern "C" void fn_801B7D00(cCharacter* pCharacter)
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
        pCharacter->m_aActualFacingDirection, 0.65f);
    v3Offset.z = 0.0f;

    nlVec3Add(v3Position, v3Position, v3Offset);
    pController->SetPosition(v3Position);
    SetZeroVelocity(pController);
}

extern "C" void fn_801B7E4C(const char* pName, cCharacter* pCharacter)
{
    EmissionController* pController = EmitGeneric(pCharacter, pName, 0);
    Function1<void, EmissionController&> update2(UpdateEmitterFromCharacter);
    pController->SetUpdateCallback(update2);
}

extern "C" void fn_801B7F8C(cPlayer* pCharacter)
{
    StopSound(fn_8001E168(pCharacter) ? 0xD73B11EC : 0x1CCDFC62, pCharacter);

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

    fn_800EDCE8(pCharacter);
    fn_800EBBFC(pCharacter->mUnidentified318,
        fn_8001E168(pCharacter) ? 0xD73B11EC : 0x1CCDFC62,
        "confused", pCharacter);
}

extern "C" bool fn_801B8164(cFielder* pFielder)
{
    unsigned long soundID = fn_8001E168(pFielder)
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

extern "C" void fn_801B81F8(cPlayer* pCharacter)
{
    if (pCharacter->m_eClassType == FIELDER)
    {
        StopSound(fn_8001E168(pCharacter) ? 0xFDC268FB : 0x1CCDFC62, pCharacter);
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
        fn_800EBBFC(pCharacter->mUnidentified318,
            fn_8001E168(pCharacter) ? 0xFDC268FB : 0x1CCDFC62,
            "dazed", pCharacter);
    }
    else
    {
        fn_800EBBFC(9, 0x8F82BB80, "dazed", pCharacter);
    }
}

bool KillDaze(cPlayer* player)
{
    if (player->m_eClassType == FIELDER)
    {
        unsigned long soundID = fn_8001E168(player)
                                  ? 0xFDC268FB
                                  : 0x1CCDFC62;
        StopSound(soundID, player);
    }
    else
    {
        StopSound(0x8F82BB80, player);
    }

    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("dazed");
    if (player->IsPlayingEffect(pGroup))
    {
        player->KillEffect(pGroup);
        return true;
    }
    return false;
}

extern "C" void fn_801B84B4(cPlayer* pCharacter)
{
    const char* groupName = "freeze";
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
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

    pGroup = EmissionManager::Instance()->GetEffectsGroup("freeze_ground");
    pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    pController->SetPosition(pCharacter->m_v3Position);
    pController->SetVelocity(v3Zero);
    PlayRumbleAction(1, pCharacter->GetGlobalPad());
    pCharacter->m_pEffectsTexturing = fxGetTexturing(eFXTex_Freeze);
}

extern "C" void fn_801B865C(cPlayer* pCharacter)
{
    pCharacter->KillEffect(EmissionManager::Instance()->GetEffectsGroup("freeze"));
    pCharacter->m_pEffectsTexturing = 0;
}

extern "C" void fn_801B86A4(cPlayer* pCharacter)
{
    pCharacter->KillEffect(EmissionManager::Instance()->GetEffectsGroup("freeze"));
    pCharacter->m_pEffectsTexturing = 0;

    const char* groupName = "unfreeze";
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
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
    PlayRumbleAction(1, pCharacter->GetGlobalPad());
}

extern "C" void fn_801B881C(cCharacter* pCharacter)
{
    const char* groupName = "yoshi_shell_break";
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pCharacter->AttachEffect(pController);
    pController->SetPosition(pCharacter->m_v3Position);
    pController->SetVelocity(v3Zero);
    {
        Function1<void, EmissionController&> update2(
            UpdateEmitterFromCharacter);
        pController->SetUpdateCallback(update2);
    }
}

extern "C" void fn_801B897C(cCharacter* pCharacter)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("peach_photo"), 0, true, 0);
    pController->SetPosition(pCharacter->m_v3Position);
    pController->SetVelocity(v3Zero);
}

extern "C" void fn_801B89F4(cFielder* pFielder)
{
    fn_801B7E4C("monty_deke_enter", pFielder);
}

extern "C" void fn_801B8B38(cPlayer* pPlayer)
{
    fn_801B7E4C("monty_deke_exit", pPlayer);
}

extern "C" void fn_801B8C7C(const nlVector3& v3Position)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("hammer_ground"), 3, true, 0);
    pController->SetPosition(v3Position);
    pController->SetVelocity(v3Zero);
}

static char s_szHammerGroundBig[] = "hammer_ground_big";

extern "C" void fn_801B8CF4(const nlVector3& v3Position)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("hammer_destroy_big"), 3, true, 0);
    pController->SetPosition(v3Position);
    pController->SetVelocity(v3Zero);
}

static char s_szGroundPound[] = "ground_pound";
static char s_szBowserJrGroundPound[] = "bowserjr_ground_pound";

static inline void EmitGroundPound(cCharacter* pCharacter)
{
    if (pCharacter->m_eCharacterClass == 7)
    {
        EmitCharacterEffect(pCharacter, s_szGroundPound);
    }
    else if (pCharacter->m_eCharacterClass == 9)
    {
        EmitCharacterEffect(pCharacter, s_szBowserJrGroundPound);
    }
}

extern "C" void fn_801B8D6C(cCharacter* pCharacter)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("monty_squish_enter"), 3, true, 0);
    pController->SetPosition(pCharacter->m_v3Position);
    pController->SetVelocity(v3Zero);
}

extern "C" void fn_801B8DE4(cCharacter* pCharacter)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("monty_squish_exit"), 3, true, 0);
    pController->SetPosition(pCharacter->m_v3Position);
    pController->SetVelocity(v3Zero);
}

extern "C" void fn_801B8E5C(cPlayer* pPlayer)
{
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("goalie_arm_in_ground");
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);

    nlVector3 v3Point;
    nlVec3Set(v3Point, lbl_806DD1DC, lbl_806DD1E0, 0.2f);
    GetWorldPoint(v3Point, v3Point, pPlayer->m_v3Position, pPlayer->m_aActualFacingDirection);
    pController->SetPosition(v3Point);
    pController->SetVelocity(v3Zero);
}

extern "C" void fn_801B8F04(cCharacter* pCharacter, const char* name)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup(name), 3, true, 0);
    pController->SetPosition(pCharacter->m_v3Position);
    pController->SetVelocity(v3Zero);
}

extern "C" void fn_801B8F7C(cCharacter* pCharacter, const char* name)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup(name), 3, true, 0);
    pController->SetPosition(pCharacter->m_v3Position);
    pController->SetVelocity(v3Zero);
}

extern "C" void fn_801B8FF4(cFielder* pFielder)
{
}

extern "C" void fn_801B8FF8(cFielder* pFielder)
{
    const char* groupName = "shyguy_bullet_start";
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pFielder->AttachEffect(pController);
    pController->SetPosition(g_pBall->m_v3Position);
}

extern "C" void fn_801B90F8(cFielder* pFielder)
{
    const char* groupName = "shyguy_bullet_shoot";
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pFielder->AttachEffect(pController);
    pController->SetPosition(pFielder->mUnidentified420->position);
}

extern "C" void fn_801B91F8(cFielder* pFielder)
{
    const char* groupName = "shyguy_bullet_end";
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pFielder->AttachEffect(pController);
    pController->SetPosition(pFielder->mUnidentified420->position);
}

extern "C" void fn_801B92F8(cCharacter* pCharacter)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("boo_deke_puff_start"), 3, true, 0);
    pController->SetPosition(pCharacter->m_v3Position);
    pController->SetVelocity(v3Zero);
}

extern "C" void fn_801B9370(cCharacter* pCharacter)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("boo_deke_puff_end"), 3, true, 0);
    pController->SetPosition(pCharacter->m_v3Position);
    pController->SetVelocity(v3Zero);
}

extern "C" void fn_801B93E8(cCharacter* pCharacter)
{
    pCharacter->EndEffect(EmissionManager::Instance()->GetEffectsGroup("electrocution"));
    pCharacter->m_pEffectsTexturing = 0;
    fn_8001C510(pCharacter, false);
}

static int lbl_806DD20C = 4;

extern "C" void fn_801B9440(EmissionController& ec)
{
    UpdateEmitterFromCharacterUnculled(ec);

    cCharacter* pCharacter = (cCharacter*)ec.m_uUserData;
    if (pCharacter->m_eClassType == FIELDER && fn_8003877C((cFielder*)pCharacter))
    {
        return;
    }

    if (lbl_806E173C == 0)
    {
        if (lbl_806E1740)
        {
            pCharacter->SetElectrocutionTextureEnabled(false);
        }
        else
        {
            pCharacter->SetElectrocutionTextureEnabled(true);
        }
        lbl_806E1740 = !lbl_806E1740;
        lbl_806E173C = nlRandom(lbl_806DD20C, &nlDefaultSeed);
    }
    else
    {
        lbl_806E173C--;
    }
}

extern "C" void fn_801B94EC(cCharacter* pCharacter, const nlVector3& v3Position,
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
    SetFreeUpdateCallback(pController, fn_801B9440);
    fn_8001C510(pCharacter, true);
    EmitElectricFenceCharacterEffect(v3Position, v3Normal, GetCharacterIndex(pCharacter));
}

extern "C" void fn_801B968C(cCharacter* pCharacter)
{
    const char* szName = "electrocution";
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(szName);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pCharacter->AttachEffect(pController);
    {
        Function1<void, EmissionController&> update2(fn_801B9440);
        pController->SetUpdateCallback(update2);
    }
    fn_8001C510(pCharacter, true);
}

extern "C" void fn_801B97DC(cFielder* pFielder)
{
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("bowser_smoke");
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetFreeUpdateCallback(pController, UpdateEmitterFromCharacterHead);
    pFielder->AttachEffect(pController);
    UpdateEmitterFromCharacterHead(*pController);
}

extern "C" void fn_801B98A0(cFielder* pFielder)
{
    const EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("bowser_smoke");
    if (pFielder->IsPlayingEffect(pGroup))
    {
        pFielder->EndEffect(pGroup);
    }
}

static nlVector3 lbl_80515720 = { 0.0f, 0.0f, 1.0f };

extern "C" void fn_801B9904(unsigned long uEffectHash)
{
    EmissionController* pControl = EmissionManager::Instance()->Create(
        fn_802E7D54(EmissionManager::Instance(), uEffectHash), 3, true, 0);
    nlVector3 v3Direction = lbl_80515720;

    pControl->SetVelocity(v3Direction);
    pControl->SetPosition(g_pBall->m_v3Position);
}

void EmitGoalieCatch(cPlayer* pPlayer, const char* name, bool bRumble)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup(name),
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

extern "C" void fn_801B9A98(cCharacter* pCharacter)
{
    const char* groupName = "landing_feet";
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pCharacter->AttachEffect(pController);
    pController->SetPosition(pCharacter->m_v3Position);
}

extern "C" void fn_801B9B94(cCharacter* pCharacter)
{
    const char* groupName = "toad_goal_hi_0_dust";
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pCharacter->AttachEffect(pController);
    pController->SetPosition(pCharacter->m_v3Position);
}

extern "C" bool fn_801B9C90(const char* szEffectName)
{
    if (!fn_80014EA4(g_pBall,
            EmissionManager::Instance()->GetEffectsGroup(szEffectName)))
    {
        SetBallUpdateCallback(
            CreateBallEffect(nlStringLowerHash(szEffectName), g_pBall));
        return true;
    }
    return false;
}

extern "C" bool fn_801B9DAC(const char* szEffectName)
{
    unsigned long uHash = nlStringLowerHash(szEffectName);

    fn_801BA358();

    SetBallUpdateCallback(CreateBallEffect(uHash, g_pBall));
    g_pBall->m_CurrentGlowEffect = uHash;
    return true;
}

static const char* s_szHeaderTarget = "header_target";
static const char* s_szHeaderTargetActive = "header_target_active";

extern "C" void fn_801B9EAC(cBall* pBall, nlVector3* pPosition, bool bActive)
{
    const char* szName = bActive ? s_szHeaderTargetActive : s_szHeaderTarget;

    EmissionController* pController = EmissionManager::Instance()->Create(
        fn_802E7D54(EmissionManager::Instance(), nlStringLowerHash(szName)),
        3, true, 0);
    pController->m_uUserData = (unsigned long)pBall;
    nlVector3 vel = lbl_80515478;
    pController->SetVelocity(vel);
    pController->m_fGround = 0.02f;
    pController->SetPosition(*pPosition);
    pController->SetVelocity(v3Zero);
    Function1<void, EmissionController&> update(UpdateEmitterFromBallLandingSpot);
    pController->SetUpdateCallback(update);
}

extern "C" void fn_801B9FD0(cBall* pBall, bool bActive)
{
    const char* szEffectName = bActive ? s_szHeaderTargetActive : s_szHeaderTarget;

    EmissionManager::Instance()->Destroy((unsigned long)pBall,
        EmissionManager::Instance()->GetEffectsGroup(szEffectName));
}

extern "C" void fn_801BA034()
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

    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), sHashBallShotWindup0));
    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), sHashBallShotWindupGround0));
    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), sHashBallShotWindup1));
    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), sHashBallShotWindupGround1));
    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), sHashBallShotWindup2));
    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), sHashBallShotWindupGround2));
    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), sHashBallShotWindup3));
    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), sHashBallShotWindupGround3));
    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), sHashBallShotWindupMax));
    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), sHashBallShotWindupGroundMax));
    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), sHashShootToScoreWindup));
    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), sHashBallStsWindup));
}

extern "C" void fn_801BA358()
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

    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), sHashBallShotWindupGlow0));
    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), sHashBallShotWindupGlow1));
    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), sHashBallShotWindupGlow2));
    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), sHashBallShotWindupGlow3));
    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), sHashBallShotWindupGlowMax));
    g_pBall->m_CurrentGlowEffect = 0;
}

extern "C" void fn_801BA4C8(const char* szName)
{
    fn_802E8A2C(EmissionManager::Instance(),
        fn_802E7D54(EmissionManager::Instance(), nlStringLowerHash(szName)));
}

extern "C" void fn_801BA510(cFielder* pFielder)
{
    const char* groupName = "mushroom";
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pFielder->AttachEffect(pController);
    {
        Function1<void, EmissionController&> update2(
            UpdateEmitterFromCharacter);
        pController->SetUpdateCallback(update2);
    }
    PlayRumbleAction(1, pFielder->GetGlobalPad());
}

void EmitMushroom(cFielder* pFielder, bool bParam)
{
    const char* groupName = "mushroom";
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
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
        0.0f,
        0);
    if (!bParam)
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

void EmitStar(cFielder* pFielder, bool bParam)
{
    PowerupBase::PlayPowerupSound(POWER_UP_STAR,
        PowerupBase::PWRUP_SOUND_ACTIVATE,
        pFielder->m_pPhysicsCharacter,
        0.0f,
        0);

    if (!bParam)
    {
        const char* groupName = "star";
        EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
        EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
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

extern "C" void fn_801BAA94(cCharacter* pCharacter)
{
    const char* groupName = "pull_head_out";
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
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
    PlayRumbleAction(1, ((cPlayer*)pCharacter)->GetGlobalPad());
}

extern "C" void fn_801BABEC(cPlayer* pPlayer)
{
    fn_801B7E4C("push_head_in", pPlayer);
}

extern "C" void fn_801BAD30(cCharacter* pCharacter)
{
    if (pCharacter->m_eClassType == FIELDER)
    {
        if (((cFielder*)pCharacter)->fn_8003E74C())
        {
            PlayRumbleAction(2, ((cPlayer*)pCharacter)->GetGlobalPad());

            const char* groupName = "landing_big";
            EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
            EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
            SetDefaultVelocity(pController);
            pController->m_fGround = 0.02f;
            SetPoseUpdateCallback(pController);
            pCharacter->AttachEffect(pController);
        }
        else
        {
            const char* groupName = "landing";
            EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
            EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
            SetDefaultVelocity(pController);
            pController->m_fGround = 0.02f;
            SetPoseUpdateCallback(pController);
            pCharacter->AttachEffect(pController);
        }
    }
}

extern "C" void fn_801BAF0C(cPlayer* pCharacter)
{
    EmissionController* pController = EmissionManager::Instance()->Create(
        EmissionManager::Instance()->GetEffectsGroup("tackle_impact"), 3, true, 0);
    pController->SetPosition(
        pCharacter->m_pPoseAccumulator->GetNodeMatrix(pCharacter->m_nBip01JointIndex_0xA4).GetTranslation());
    pController->SetVelocity(v3Zero);
}

extern "C" void fn_801BAF98(cFielder* pFielder)
{
    fn_801B7E4C("dk_super_charge", pFielder);
}

extern "C" void fn_801BB0DC(cFielder* pFielder)
{
    const EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("dk_super_charge");
    pFielder->EndEffect(pGroup);
}

extern "C" void fn_801BB120(cFielder* pFielder)
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

extern "C" void fn_801BB20C(cCharacter* pCharacter)
{
    const char* groupName = "bowser_explode";
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pCharacter->AttachEffect(pController);
    pController->SetPosition(pCharacter->m_v3Position);
    pController->SetVelocity(v3Zero);
}

extern "C" void fn_801BB318(cCharacter* pCharacter)
{
    fn_801B7E4C("slide_tackle_trail", pCharacter);
}

extern "C" void fn_801BB45C(cCharacter* pCharacter)
{
    if (pCharacter->m_eClassType == FIELDER && !((cFielder*)pCharacter)->fn_800344B0())
    {
        const char* groupName = "hit_trail";
        EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
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

        if (((cFielder*)pCharacter)->fn_8003E74C())
        {
            PlayRumbleAction(2, ((cPlayer*)pCharacter)->GetGlobalPad());
        }
    }
}

extern "C" void fn_801BB5DC(cFielder* pFielder, int nParam)
{
    const EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("slide_tackle_trail");
    if (nParam == 0)
    {
        pFielder->EndEffect(pGroup);
    }
    else
    {
        pFielder->KillEffect(pGroup);
    }
}

extern "C" void fn_801BB640(cFielder* pFielder, int nParam)
{
    const EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("hit_trail");
    if (nParam == 0)
    {
        pFielder->EndEffect(pGroup);
    }
    else
    {
        pFielder->KillEffect(pGroup);
    }
}

extern "C" void fn_801BB6A4(cCharacter* pCharacter, int nIcon)
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

    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(szIconName);
    EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    SetDefaultVelocity(pController);
    pController->m_fGround = 0.02f;
    SetPoseUpdateCallback(pController);
    pCharacter->AttachEffect(pController);

    nlVector3 v3Position = pCharacter->GetJointPosition(pCharacter->m_nHeadJointIndex);
    v3Position.z += 0.1f;
    pController->SetPosition(v3Position);
    pController->SetVelocity(pCharacter->m_v3Velocity);
}

extern "C" void fn_801BBE80(cCharacter* pCharacter)
{
    EmissionController* pController;

    if (pCharacter->m_eCharacterClass == 0)
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

extern "C" void fn_801BC094(cCharacter* pCharacter)
{
    EmissionController* pController;

    if (pCharacter->m_eCharacterClass == 0)
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

extern "C" void fn_801BC2A8(cCharacter* pCharacter, bool bRight)
{
    EmissionController* pController;

    if (bRight)
    {
        if (pCharacter->m_eCharacterClass == 0)
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
        if (pCharacter->m_eCharacterClass == 0)
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

    const nlVector3 v3Shake = { 0.1f, 0.08f, 0.0f };

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

    fn_800F026C(v3Shake, 30.0f, 1.0f);
}

extern "C" void fn_801BC6E4(cFielder* pFielder)
{
    fn_801B7E4C("koopa_shell_show", pFielder);
}

extern "C" void fn_801BC828(cCharacter* pCharacter)
{
    fn_801B7E4C("birdo_egg_show", pCharacter);
}

extern "C" void fn_801BC96C(const nlVector3& v3Position)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("koopa_shell_burst"), 3, true, 0);
    pController->SetPosition(v3Position);
    pController->SetVelocity(v3Zero);
}

extern "C" void fn_801BC9E4(const nlVector3& v3Position)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("birdo_egg_burst"), 3, true, 0);
    pController->SetPosition(v3Position);
    pController->SetVelocity(v3Zero);
}

extern "C" void fn_801BCA5C(const nlVector3& v3Position)
{
    EmissionController* pController = EmissionManager::Instance()->Create(EmissionManager::Instance()->GetEffectsGroup("hammer_destroy"), 3, true, 0);
    pController->SetPosition(v3Position);
    pController->SetVelocity(v3Zero);
}

extern "C" void fn_801BCAD4(cCharacter* pCharacter)
{
    if (!pCharacter->IsPlayingEffect(EmissionManager::Instance()->GetEffectsGroup("skillshot_hand_fire")))
    {
        const char* szName = "skillshot_hand_fire";
        EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(szName);
        EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
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

extern "C" void fn_801BCC38(cCharacter* pCharacter)
{
    const EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("skillshot_hand_fire");
    if (pCharacter->IsPlayingEffect(pGroup))
    {
        pCharacter->EndEffect(pGroup);
    }
}

extern "C" void fn_801BCC9C(cCharacter* pCharacter)
{
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("skillshot_player_on_fire");
    if (fn_8001E2C0(pCharacter, pGroup))
    {
        EmissionManager::Instance()->Destroy((unsigned long)pCharacter, pGroup);
    }

    if (!pCharacter->IsPlayingEffect(pGroup))
    {
        const char* szName = "skillshot_player_on_fire";
        EffectsGroup* pGroup2 = EmissionManager::Instance()->GetEffectsGroup(szName);
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

extern "C" void fn_801BCE2C(cCharacter* pCharacter)
{
    const EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("skillshot_player_on_fire");
    if (pCharacter->IsPlayingEffect(pGroup))
    {
        pCharacter->EndEffect(pGroup);
    }
}

extern "C" void fn_801BCE90(cCharacter* pCharacter)
{
    if (pCharacter->m_eAnimID == 0x50)
    {
        cPN_SAnimController* pAnimController = pCharacter->m_pCurrentAnimController;
        if (pAnimController->m_fTime
            < 15.0f / pAnimController->m_pSAnim->m_nNumKeys)
        {
            if (pCharacter->m_eCharacterClass == 0xB)
            {
                fn_801B7E4C("petey_deke", pCharacter);
            }
            else if (pCharacter->m_eCharacterClass == 0xC)
            {
                fn_801B7E4C("birdo_deke", pCharacter);
            }
        }
    }
}

extern "C" void fn_801BD144(cCharacter* pCharacter)
{
    if (pCharacter->m_eCharacterClass == 11)
    {
        EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("petey_deke");
        fn_802E83C4(EmissionManager::Instance(), pGroup);
    }
    else if (pCharacter->m_eCharacterClass == 12)
    {
        EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("birdo_deke");
        fn_802E83C4(EmissionManager::Instance(), pGroup);
    }
}

extern "C" void fn_801BD1C0(cFielder* pFielder)
{
    int eCharacterClass = pFielder->m_eCharacterClass;

    if (eCharacterClass == 7 || eCharacterClass == 9 || eCharacterClass == 1
        || eCharacterClass == 3 || eCharacterClass == 11
        || eCharacterClass == 12)
    {
        pFielder->muInvincibleStatus |= 1;
    }

    if (pFielder->m_eAnimID == 0x50)
    {
        cPN_SAnimController* pAnimController = pFielder->m_pCurrentAnimController;
        if (pAnimController->m_fTime
            < 15.0f / (float)pAnimController->m_pSAnim->m_nNumKeys)
        {
            if (pFielder->m_eCharacterClass == 11)
            {
                const char* groupName = "petey_deke";
                EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
                EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
                SetDefaultVelocity(pController);
                pController->m_fGround = 0.02f;
                {
                    Function1<void, EmissionController&> update(UpdateEmitterPoseFromCharacter);
                    pController->SetUpdateCallback(update);
                }
                pFielder->AttachEffect(pController);
                {
                    Function1<void, EmissionController&> update2(UpdateEmitterFromCharacter);
                    pController->SetUpdateCallback(update2);
                }
            }
            else if (pFielder->m_eCharacterClass == 12)
            {
                const char* groupName = "birdo_deke";
                EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
                EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
                SetDefaultVelocity(pController);
                pController->m_fGround = 0.02f;
                {
                    Function1<void, EmissionController&> update(UpdateEmitterPoseFromCharacter);
                    pController->SetUpdateCallback(update);
                }
                pFielder->AttachEffect(pController);
                {
                    Function1<void, EmissionController&> update2(UpdateEmitterFromCharacter);
                    pController->SetUpdateCallback(update2);
                }
            }
        }
    }

    if (pFielder->m_eCharacterClass == 13 || pFielder->m_eCharacterClass == 7
        || pFielder->m_eCharacterClass == 9)
    {
        pFielder->InitMovementFromAnim(0, v3Zero, 0.0f, false);
    }
}

extern "C" void fn_801BD4EC(cFielder* pFielder)
{
    fn_80038158(pFielder, 0);

    if (pFielder->m_eCharacterClass == 3)
    {
        EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("dk_deke");
        EmissionManager::Instance()->Destroy((unsigned long)g_pBall, pGroup);
    }
    else if (pFielder->m_eActionState == 1)
    {
        if (pFielder->m_eCharacterClass == 11)
        {
            EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("petey_deke");
            fn_802E83C4(EmissionManager::Instance(), pGroup);
        }
        else if (pFielder->m_eCharacterClass == 12)
        {
            EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("birdo_deke");
            fn_802E83C4(EmissionManager::Instance(), pGroup);
        }

        if (pFielder->m_eCharacterClass == 1)
        {
            pFielder->InitMovementFromAnim(0, v3Zero, 0.0f, false);
        }
        else if (pFielder->m_eCharacterClass == 7)
        {
            CharacterImpactEvent event;
            event.v3Position = pFielder->m_v3Position;
            event.fMagnitude = lbl_806DD1C0;
            event.pCharacter = pFielder;
            fn_800611F0(g_pGame, &event);
            EmitGroundPound(pFielder);
            fn_800EBBFC(pFielder->mUnidentified318, 0x5BF8E132, 0, 0);
        }
        else if (pFielder->m_eCharacterClass == 9)
        {
            CharacterImpactEvent event;
            event.v3Position = pFielder->m_v3Position;
            event.fMagnitude = lbl_806DD1C4;
            event.pCharacter = pFielder;
            fn_800611F0(g_pGame, &event);
            EmitGroundPound(pFielder);
            fn_800EBBFC(pFielder->mUnidentified318, 0x560BD5F9, 0, 0);
        }
        else if (pFielder->m_eCharacterClass == 13)
        {
            CharacterImpactEvent event;
            nlVector3 v3Offset;
            int nNodeIndex = pFielder->m_pPoseAccumulator->m_BaseSHierarchy->GetNodeIndexByID(
                nlStringLowerHash("bip01 R Prop"));
            event.v3Position = pFielder->GetJointPosition(nNodeIndex);
            event.v3Position.z = 0.0f;
            nlPolarToCartesian(v3Offset.x, v3Offset.y,
                pFielder->m_aActualFacingDirection, lbl_806DD1CC);
            v3Offset.z = 0.0f;
            nlVec3Add(event.v3Position, event.v3Position, v3Offset);
            event.fMagnitude = lbl_806DD1C8;
            event.pCharacter = pFielder;
            fn_80060FF4(g_pGame, &event);

            EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(s_szHammerGroundBig);
            EmissionController* pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
            pController->SetPosition(event.v3Position);
            pController->SetVelocity(v3Zero);
            fn_800EBBFC(pFielder->mUnidentified318, 0x560BD5F9, 0, 0);
        }
    }
}

extern "C" void fn_801BDC1C(const nlVector3& v3Position, const nlVector3& v3Direction,
    const nlVector3& v3Velocity)
{
    EmissionController* pController = EmissionManager::Instance()->Create(
        EmissionManager::Instance()->GetEffectsGroup("wind_dry"), 3, true, 0);
    pController->SetPosition(v3Position);
    pController->SetDirection(v3Direction);
    pController->SetVelocity(v3Velocity);
}

extern "C" void fn_801BDCB4(int bParam)
{
    if (bParam)
    {
        fn_802E8A2C(EmissionManager::Instance(),
            EmissionManager::Instance()->GetEffectsGroup("wind_dry"));
    }
    else
    {
        fn_802E83C4(EmissionManager::Instance(),
            EmissionManager::Instance()->GetEffectsGroup("wind_dry"));
    }
}

extern "C" void fn_801BDD24(const char* name, const nlVector3& v3Position, bool bParam)
{
    cBall* pBall = g_pBall;
    EmissionController* pController = EmissionManager::Instance()->Create(
        fn_802E7D54(EmissionManager::Instance(), nlStringLowerHash(name)), 3, true, 0);
    pController->m_uUserData = (unsigned long)pBall;
    nlVector3 vel = lbl_80515478;
    pController->SetVelocity(vel);
    pController->m_fGround = 0.02f;
    pController->SetPosition(v3Position);
    pController->SetVelocity(v3Zero);
}

extern "C" void fn_801BDDE0(cCharacter* pCharacter)
{
}

extern "C" void fn_801BDDE4()
{
    static unsigned long sHashWeatherLightningBall =
        nlStringLowerHash("weather_lightning_ball");

    cBall* pBall;
    EmissionController* pController;
    EffectsGroup* pGroup;

    pBall = g_pBall;
    pGroup = fn_802E7D54(EmissionManager::Instance(), sHashWeatherLightningBall);
    pController = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    pController->m_uUserData = (unsigned long)pBall;
    nlVector3 vel = lbl_80515478;
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

extern "C" void fn_801BDF0C(cCharacter* pCharacter)
{
    const char* groupName = "dk_deke";
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup(groupName);
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

extern "C" const char* fn_801BE09C(cCharacter* pCharacter)
{
    return pCharacter->m_szEffectsName;
}

extern "C" void fn_801BE0A4(cCharacter* pCharacter, bool bValue)
{
    *(bool*)((char*)pCharacter + 0x181) = bValue;
}

extern "C" void fn_801BE0AC(cCharacter* pCharacter, bool bValue)
{
    *(bool*)((char*)pCharacter + 0x182) = bValue;
}

extern "C" bool fn_801BE0B4(cCharacter* pCharacter)
{
    return pCharacter->m_eClassType == GOALIE;
}

extern "C" void fn_801BE0C8(cCharacter* pCharacter, bool bValue)
{
    *(bool*)((char*)pCharacter + 0x17E) = bValue;
}

extern "C" void fn_801BE0D0(cCharacter* pCharacter, bool bValue)
{
    *(bool*)((char*)pCharacter + 0x17F) = bValue;
}

extern "C" cSAnim* fn_801BE0D8(cPN_SAnimController* pController)
{
    return pController->m_pSAnim;
}

extern "C" bool fn_801BE0E0(cPN_SAnimController* pController)
{
    return pController->m_bMirror;
}

extern "C" bool fn_801BE0EC(cPlayer* pPlayer)
{
    return pPlayer->GetGlobalPad() != 0;
}

extern "C" cPlayer* fn_801BE118(cBall* pBall)
{
    return pBall->m_pOwner;
}

extern "C" void fn_801BE120(cBall* pBall, bool bVisible)
{
    pBall->m_bVisible = bVisible;
}

extern "C" bool fn_801BE128(Desire* pDesire)
{
    return pDesire->UnidentifiedIsActive();
}

extern "C" int fn_801BE130(cFielder* pFielder)
{
    return pFielder->m_eActionState;
}

extern "C" float fn_801BE138(TweakValueImpl_804F4DC8* pTweak)
{
    return *pTweak->m_pValue;
}

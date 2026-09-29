#ifndef GAME_CHARACTERTRIGGERS_H
#define GAME_CHARACTERTRIGGERS_H

enum eBallShotEffectType
{
    BALL_EFFECT_S2S_SUPER_SHOT = 0,
    BALL_EFFECT_S2S_SHOT = 1,
    BALL_EFFECT_PERFECT_SHOT = 2,
    BALL_EFFECT_PERFECT_PASS = 3,
    BALL_EFFECT_REGULAR_SHOT = 4,
    BALL_EFFECT_ONETIMER_SHOT = 5,
    BALL_EFFECT_CHIP_SHOT = 6,
    NUM_BALL_EFFECTS = 7,
};

class nlVector3;
class cCharacter;
class cPlayer;
class cFielder;
class cBall;
class Desire;
class TweakFloatBinding;
class cPN_SAnimController;
class EmissionController;
class cSAnim;
class EffectsGroup;

void CharacterTriggerHandler(cSAnim* pAnim, unsigned int uParam);
void GetAnimTriggerInfo(cCharacter* pCharacter, int animID,
    bool (*pInfoCallback)(float, float, unsigned long, float, void*), void* pUserData);
void EmitGoalieCatch(cPlayer* pPlayer, const char* szEffectName, bool bDoRumble);
void EmitMushroom(cFielder* pFielder, bool bReinitialize);
void KillMushroom(cFielder* pFielder);
void EmitStar(cFielder* pFielder, bool bReinitialize);
void KillStar(cFielder* pFielder);
bool KillDaze(cPlayer* pCharacter);
void EmitDaze(cPlayer* pCharacter);
void EmitBallImpact(cPlayer* pCharacter, bool bSilent);
void EmitBallPass(cPlayer* pCharacter);
void EmitHammerDestroyBig(const nlVector3& v3Position);

void EndElectrocution(cCharacter*);
void EmitBowserSmoke(cFielder*);
void KillSlideTackleTrail(cFielder*, int);
EmissionController* EmitGeneric(cCharacter* pCharacter, const char* baseName, const char* characterName);

extern "C" void fn_801BDDE0(bool bParam);

void EmitWind(const nlVector3& v3Position, const nlVector3& v3Direction,
    const nlVector3& v3Velocity);

void EmitLightning(const char* szEffectName, nlVector3 v3Position, bool bStrong);

void KillWind(bool bReallyKill);

// Shared functions and data from Game/CharacterTriggers.cpp.
unsigned long GetBallGlowEffectHash(float fCharge);
void EmitSmallRumble(cPlayer* pPlayer);
void EmitMediumRumble(cPlayer* pPlayer);
void EmitDivot(cCharacter* pCharacter);
void EmitFreeze(cPlayer* pCharacter);
void EmitUnFreeze(cPlayer* pCharacter);
void EmitMontyDekeEnter(cFielder* pFielder);
void EmitMontyDekeExit(cPlayer* pPlayer);
void EmitMontySquishEnter(cCharacter* pCharacter);
void EmitMontySquishExit(cCharacter* pCharacter);
void EmitGoalieArmInGround(cPlayer* pPlayer);
void EmitDekeEnter(cCharacter* pCharacter, const char* szEffectName);
void EmitDekeExit(cCharacter* pCharacter, const char* szEffectName);
extern "C" void fn_801B8FF4(cFielder* pFielder);
void EmitShyGuyBulletShoot(cFielder* pFielder);
void EmitBooDekePuffStart(cCharacter* pCharacter);
void EmitBooDekePuffEnd(cCharacter* pCharacter);
void ElectrocutionUpdateCallback(EmissionController& ec);
void CharacterElectrocutionEffect(cCharacter* pCharacter, const nlVector3& v3Position, const nlVector3& v3Normal);
void EmitLandingFeetTrigger(cCharacter* pCharacter);
void EmitToadGoalDustTrigger(cCharacter* pCharacter);
bool EmitBallGlow(const char* szEffectName);
void KillWindups();
void KillBallGlow();
void KillWindup(const char* szEffectName);
void EmitPullHeadOut(cCharacter* pPlayer);
void EmitPushHeadIn(cPlayer* pPlayer);
void EmitLanding(cCharacter* pCharacter);
void EmitDKSuperCharge(cFielder* pFielder);
void EmitDKSuperHit(cFielder* pFielder);
void EmitBowserExplode(cCharacter* pCharacter);
void EmitSlideTackleTrail(cCharacter* pCharacter);
void EmitHitTrail(cCharacter* pCharacter);
void KillHitTrail(cFielder* pFielder, int nKill);
void EmitSuperFootstep(cCharacter* pCharacter, bool bRight);
void EmitKoopaShellShow(cFielder* pFielder);
void EmitKoopaShellBurst(const nlVector3& v3Position);
void EmitDeke(cCharacter* pCharacter);
void KillDeke(cCharacter* pCharacter);
void BeginDeke(cFielder* pFielder);
void EndDeke(cFielder* pFielder);
void EmitLightningBall();
void EmitDKDeke(cCharacter* pCharacter);
const char* GetCharacterEffectsName(cCharacter* pCharacter);
extern "C" void fn_801BE0A4(cCharacter* pCharacter, bool bValue);
extern "C" void fn_801BE0AC(cCharacter* pCharacter, bool bValue);
bool IsCharacterGoalie(cCharacter* pCharacter);
void SetCharacterLeftPropAnimated(cCharacter* pCharacter, bool bAnimated);
void SetCharacterRightPropAnimated(cCharacter* pCharacter, bool bAnimated);
cSAnim* GetControllerSAnim(cPN_SAnimController* pController);
bool IsControllerMirrored(cPN_SAnimController* pController);
bool HasGlobalPad(cPlayer* pPlayer);
cPlayer* GetBallOwner(cBall* pBall);
void SetBallVisible(cBall* pBall, bool bVisible);
bool IsDesireActive(Desire* pDesire);
int GetFielderActionState(cFielder* pFielder);
float GetTweakFloatValue(TweakFloatBinding* pTweak);

void UpdateBallGlow(cBall* pBall);
void EmitShyGuyBulletStart(cFielder* pFielder);
void EmitShyGuyBulletEnd(cFielder* pFielder);
void EndBowserSmoke(cFielder* pFielder);
void EmitHeaderTarget(cBall* pBall, nlVector3* pPosition, bool bActive);
void KillHeaderTarget(cBall* pBall, bool bActive);
void CreateMushroomEffect(cFielder* pFielder);
void EmitTackleImpact(cPlayer* pCharacter);
void KillDKSuperCharge(cFielder* pFielder);
void EmitBirdoEggShow(cCharacter* pCharacter);
void EmitBirdoEggBurst(const nlVector3& v3Position);
void EmitSkillshotHandFire(cCharacter* pCharacter);
void KillSkillshotHandFire(cCharacter* pCharacter);
void EmitSkillshotPlayerOnFire(cCharacter* pCharacter);
void KillSkillshotPlayerOnFire(cCharacter* pCharacter);

void SetEffectsGroupFountainLife(EffectsGroup* group, float life);
void EmitBallShot(cFielder* pCharacter, eBallShotEffectType eNewBallEffect, cPlayer* pPassTarget, bool bSilent, bool bAwardPowerup);
void KillBallShot(const char* szEffectName, bool bReallyKill);
void EmitElectrocutionExplosion(const char* szEffectName, cCharacter* pCharacter);
void EmitConfused(cPlayer* pCharacter);
bool KillConfused(cFielder* pFielder);
void KillFreeze(cPlayer* pCharacter);
void EmitYoshiShellBreak(cCharacter* pCharacter);
void EmitPeachPhoto(cCharacter* pCharacter);
void EmitHammerGround(const nlVector3& v3Position);
void EmitElectrocution(cCharacter* pCharacter);
void EmitBallWindupTransition(unsigned long uEffectHash);
bool EmitWindupAtBall(const char* szEffectName);
void EmitPowerupIcon(cCharacter* pCharacter, int nIcon);
void EmitSuperGrow(cCharacter* pCharacter);
void EmitSuperShrink(cCharacter* pCharacter);
void EmitHammerDestroy(const nlVector3& v3Position);
extern "C" void fn_801BDF08(cCharacter* pCharacter);

#endif // GAME_CHARACTERTRIGGERS_H

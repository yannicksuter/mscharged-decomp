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

void CharacterTriggerHandler(cSAnim* arg0, unsigned int uParam);
void GetAnimTriggerInfo(cCharacter* pCharacter, int animIndex,
    bool (*callback)(float, float, unsigned long, float, void*), void* pData);
void EmitGoalieCatch(cPlayer* pPlayer, const char* name, bool bRumble);
void EmitMushroom(cFielder* pFielder, bool bParam);
void KillMushroom(cFielder* pFielder);
void EmitStar(cFielder* pFielder, bool bParam);
void KillStar(cFielder* pFielder);
bool KillDaze(cPlayer* player);
void EmitDaze(cPlayer* pCharacter);
extern "C" void fn_801B73B8(cPlayer* pCharacter, bool bParam);
extern "C" void fn_801B74C8(cPlayer* pCharacter);
extern "C" void fn_801B8CF4(const nlVector3& v3Position);

extern "C" void fn_801B93E8(cCharacter*);
extern "C" void fn_801B97DC(cFielder*);
extern "C" void fn_801BB5DC(cFielder*, int);
EmissionController* EmitGeneric(cCharacter* pCharacter, const char* baseName, const char* characterName);

extern "C" void fn_801BDDE0(bool bParam);

extern "C" void fn_801BDC1C(const nlVector3& v3Position, const nlVector3& v3Direction,
    const nlVector3& v3Velocity);

extern "C" void fn_801BDD24(const char* name, nlVector3 v3Position, bool bParam);

extern "C" void fn_801BDCB4(bool bParam);


// Shared functions and data from Game/CharacterTriggers.cpp.
extern "C" unsigned long fn_801B7B70(float fLevel);
extern "C" void fn_801B7CA8(cPlayer* pPlayer);
extern "C" void fn_801B7CD4(cPlayer* pPlayer);
extern "C" void fn_801B7D00(cCharacter* pCharacter);
extern "C" void fn_801B84B4(cPlayer* pCharacter);
extern "C" void fn_801B86A4(cPlayer* pCharacter);
extern "C" void fn_801B89F4(cFielder* pFielder);
extern "C" void fn_801B8B38(cPlayer* pPlayer);
extern "C" void fn_801B8D6C(cCharacter* pCharacter);
extern "C" void fn_801B8DE4(cCharacter* pCharacter);
extern "C" void fn_801B8E5C(cPlayer* pPlayer);
extern "C" void fn_801B8F04(cCharacter* pCharacter, const char* name);
extern "C" void fn_801B8F7C(cCharacter* pCharacter, const char* name);
extern "C" void fn_801B8FF4(cFielder* pFielder);
extern "C" void fn_801B90F8(cFielder* pFielder);
extern "C" void fn_801B92F8(cCharacter* pCharacter);
extern "C" void fn_801B9370(cCharacter* pCharacter);
extern "C" void fn_801B9440(EmissionController& ec);
extern "C" void fn_801B94EC(cCharacter* pCharacter, const nlVector3& v3Position, const nlVector3& v3Normal);
extern "C" void fn_801B9A98(cCharacter* pCharacter);
extern "C" void fn_801B9B94(cCharacter* pCharacter);
extern "C" bool fn_801B9DAC(const char* szEffectName);
extern "C" void fn_801BA034();
extern "C" void fn_801BA358();
extern "C" void fn_801BA4C8(const char* szName);
extern "C" void fn_801BAA94(cCharacter* pCharacter);
extern "C" void fn_801BABEC(cPlayer* pPlayer);
extern "C" void fn_801BAD30(cCharacter* pCharacter);
extern "C" void fn_801BAF98(cFielder* pFielder);
extern "C" void fn_801BB120(cFielder* pFielder);
extern "C" void fn_801BB20C(cCharacter* pCharacter);
extern "C" void fn_801BB318(cCharacter* pCharacter);
extern "C" void fn_801BB45C(cCharacter* pCharacter);
extern "C" void fn_801BB640(cFielder* pFielder, int nParam);
extern "C" void fn_801BC2A8(cCharacter* pCharacter, bool bRight);
extern "C" void fn_801BC6E4(cFielder* pFielder);
extern "C" void fn_801BC96C(const nlVector3& v3Position);
extern "C" void fn_801BCE90(cCharacter* pCharacter);
extern "C" void fn_801BD144(cCharacter* pCharacter);
extern "C" void fn_801BD1C0(cFielder* pFielder);
extern "C" void fn_801BD4EC(cFielder* pFielder);
extern "C" void fn_801BDDE4();
extern "C" void fn_801BDF0C(cCharacter* pCharacter);
extern "C" const char* fn_801BE09C(cCharacter* pCharacter);
extern "C" void fn_801BE0A4(cCharacter* pCharacter, bool bValue);
extern "C" void fn_801BE0AC(cCharacter* pCharacter, bool bValue);
extern "C" bool fn_801BE0B4(cCharacter* pCharacter);
extern "C" void fn_801BE0C8(cCharacter* pCharacter, bool bValue);
extern "C" void fn_801BE0D0(cCharacter* pCharacter, bool bValue);
extern "C" cSAnim* fn_801BE0D8(cPN_SAnimController* pController);
extern "C" bool fn_801BE0E0(cPN_SAnimController* pController);
extern "C" bool fn_801BE0EC(cPlayer* pPlayer);
extern "C" cPlayer* fn_801BE118(cBall* pBall);
extern "C" void fn_801BE120(cBall* pBall, bool bVisible);
extern "C" bool fn_801BE128(Desire* pDesire);
extern "C" int fn_801BE130(cFielder* pFielder);
extern "C" float fn_801BE138(TweakFloatBinding* pTweak);


extern "C" void fn_801B7A28(cBall* pBall);
extern "C" void fn_801B8FF8(cFielder* pFielder);
extern "C" void fn_801B91F8(cFielder* pFielder);
extern "C" void fn_801B98A0(cFielder* pFielder);
extern "C" void fn_801B9EAC(cBall* pBall, nlVector3* pPosition, bool bActive);
extern "C" void fn_801B9FD0(cBall* pBall, bool bActive);
extern "C" void fn_801BA510(cFielder* pFielder);
extern "C" void fn_801BAF0C(cPlayer* pCharacter);
extern "C" void fn_801BB0DC(cFielder* pFielder);
extern "C" void fn_801BC828(cCharacter* pCharacter);
extern "C" void fn_801BC9E4(const nlVector3& v3Position);
extern "C" void fn_801BCAD4(cCharacter* pCharacter);
extern "C" void fn_801BCC38(cCharacter* pCharacter);
extern "C" void fn_801BCC9C(cCharacter* pCharacter);
extern "C" void fn_801BCE2C(cCharacter* pCharacter);

#endif // GAME_CHARACTERTRIGGERS_H

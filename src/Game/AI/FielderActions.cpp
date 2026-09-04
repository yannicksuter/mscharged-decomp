#include "Game/AI/Fielder.h"

#include "Game/AI/FielderActions.h"
#include "Game/AI/Fuzzy.h"
#include "Game/AnimInventory.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/AI/HeadTrack.h"
#include "Game/AI/ShotMeter.h"
#include "Game/Goalie.h"
#include "Game/PoseAccumulator.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsColumn.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsObject.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Team.h"
#include "Game/TweakValue.h"
#include "unclassified/tu_80177498.h"
#include "Game/Ball.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/AI/AIPad.h"
#include "Game/AI/AiUtil.h"
#include "Game/CharacterTweaks.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/NetworkMessages.h"
#include "Game/SAnim.h"
#include "Game/SHierarchy.h"
#include "NL/nlString.h"
#include "Game/SAnim/pnSAnimController.h"
#include "NL/globalpad.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/DB/StatsTracker.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"
#include "NL/utility.h"
#include "unclassified/tu_801A0E64.h"
#include "unclassified/tu_801A5F10.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/ShootToScoreMeter.h"
#include "math.h"

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };
static const nlVector3 v3LaunchUp = { 0.0f, 0.0f, 5.0f };
static const nlVector3 v3ElectrocutionLaunch = { -5.0f, 0.0f, 5.0f };
static const nlVector3 v3Up = { 0.0f, 0.0f, 1.0f };

extern FuzzyVariant fvNotSet;

extern "C" unsigned int fn_800387CC(cFielder* pFielder);
extern "C" void fn_8002E3F8(cFielder* pFielder);
extern "C" void fn_8002E580(cFielder* pFielder);
extern "C" bool fn_8003E948(cFielder* pFielder);
extern "C" void fn_8003BA94(cFielder* pFielder, float fParam);
extern "C" void fn_8003B854(cFielder* pFielder);
extern "C" void fn_8003B790(cFielder* pFielder);
extern "C" void fn_8003B6CC(cFielder* pFielder);
extern "C" void fn_8003B2EC(cFielder* pFielder);
extern "C" void fn_8003B384(cFielder* pFielder);
extern "C" void fn_8003B190(cFielder* pFielder);
extern "C" void fn_8003B4B4(cFielder* pFielder);
extern "C" void fn_8003B41C(cFielder* pFielder);
extern "C" void fn_8003B664(cFielder* pFielder);
extern "C" void fn_8003B5FC(cFielder* pFielder);
extern "C" void fn_8003B254(cFielder* pFielder);
extern "C" void fn_8003A2D0(cFielder* pFielder, int nParam);
extern "C" float fn_8002CD2C(PlayerTweaks* pTweaks);
extern "C" float fn_8002CC44(PlayerTweaks* pTweaks);
extern "C" PlayerTweaks* fn_8003E6E4(cFielder* pFielder);
extern "C" float fn_8002BFA8(PlayerTweaks* pTweaks, float fParam);
extern "C" bool fn_8001E168(const cCharacter* pCharacter);
extern "C" void fn_800EDCE8(cPlayer* pPlayer);
extern "C" cFielder* fn_80096F54(cPlayer* pPlayer, bool bParam);
extern "C" bool fn_80035F34(cFielder* pFielder);
extern "C" bool fn_8003881C(cFielder* pFielder);
extern "C" void* fn_8002E1A4(cFielder* pFielder);
extern "C" void* fn_80319FC0(void* pParam, int nParam);
extern "C" void fn_80316968(void* pParam);
extern "C" float fn_8002E1B0(cFielder* pFielder);
extern "C" bool fn_8002D2C4(nlVector3* v3Position, float fParam, int nParam);
extern "C" const LooseBallContactAnimInfo* fn_80038230(cFielder* pFielder,
    const LooseBallContactAnimInfo* pAnimInfoList, int nNumAnims,
    unsigned short aFacingDirection, const nlVector3* v3Position,
    const nlVector3* v3Target, float fAngle);
extern "C" void fn_80036594(cPlayer* pAttacker, cFielder* pVictim, int nParam);
extern "C" void fn_80097358(cPlayer* pPlayer, float fParam);
extern "C" void fn_8009591C(cPlayer* pPlayer, bool bParam);
extern "C" void fn_800978E8(cPlayer* pPlayer, int nParam);
extern "C" void fn_80031A30(cFielder* pFielder, int nParam, float fParam);
extern bool lbl_806DB5A8;
extern "C" void fn_80139D1C(int nParam, void* pParam);
extern "C" void fn_8003A544(cFielder* pFielder);
extern "C" float fn_8002CFB0(PlayerTweaks* pTweaks);
extern "C" float fn_8002C328(PlayerTweaks* pTweaks);
extern "C" void fn_8005F03C(void* pParam, cFielder** ppFielder);
extern "C" void fn_8005CBF0(void* pParam);
extern "C" float fn_800155A0(cBall* pBall, int nParam);
extern "C" void fn_801B9C90(const char* pName);
extern "C" float fn_8002C5A4(PlayerTweaks* pTweaks);
extern "C" float fn_8002C180(PlayerTweaks* pTweaks);
extern "C" float fn_8002CF10(PlayerTweaks* pTweaks);
extern "C" float fn_8002C0AC(PlayerTweaks* pTweaks);
extern "C" void fn_800154FC(cBall* pBall, float fParam);
extern "C" void fn_801B75C8(cFielder* pFielder, int, int, int, int);
extern "C" void fn_800395C0(cFielder* pFielder);
extern "C" void fn_8019A270(void* pParam, cFielder* pFielder);
extern "C" float fn_8002C7E8(PlayerTweaks* pTweaks);
extern "C" void fn_801BA034();
extern "C" void fn_800EC12C(unsigned long uParam, void* pParam);
extern "C" void fn_8005CDD0(void* pParam);
extern "C" float fn_8002CF9C(PlayerTweaks* pTweaks);
extern "C" float fn_8002C730(PlayerTweaks* pTweaks);
extern "C" float fn_8002C678(PlayerTweaks* pTweaks);
extern "C" float fn_8002C6E8(PlayerTweaks* pTweaks);
extern "C" float fn_8002CFC4(PlayerTweaks* pTweaks);
extern "C" void fn_80097858(cPlayer* pPlayer, cPlayer* pPassTarget,
    int nParam1, int nParam2, int nParam3, int nParam4, float fParam1,
    float fParam2);
extern "C" float fn_8002CF88(PlayerTweaks* pTweaks);
extern "C" bool fn_8003886C(cFielder* pFielder);
extern "C" float fn_80030750(cFielder* pFielder);
extern "C" cFielder* fn_8003703C(cFielder* pFielder);
extern "C" float fn_8002D020(PlayerTweaks* pTweaks);
extern "C" float fn_8002D050(PlayerTweaks* pTweaks);
extern "C" bool fn_8003C180(cFielder* pFielder);
extern "C" float fn_800DBAB0(cFielder* pFielder);
extern "C" float fn_800A0508(cPlayer* pPlayer, int nParam1, int nParam2);
extern "C" void fn_80037AC8(cFielder* pFielder, int nParam);
extern "C" void fn_80035700(cFielder* pFielder);
extern "C" bool fn_8003D9BC(cFielder* pFielder);
extern "C" bool fn_8003E8A0(cFielder* pFielder);
extern "C" void fn_801B98A0(cFielder* pFielder);
extern "C" void fn_8002E340(cFielder* pFielder);
extern "C" void fn_8019AA00(void* pParam, cFielder* pFielder);
extern "C" void fn_801B8FF8(cFielder* pFielder);
extern "C" void fn_8003C560(cFielder* pFielder, int nParam1, int nParam2);
extern "C" void fn_801B90F8(cFielder* pFielder);
extern "C" void fn_80038158(cFielder* pFielder, int nParam);
extern "C" void fn_80039CF0(cFielder* pFielder, int nParam);
extern "C" void fn_80147F2C(void* pParam);
extern "C" bool fn_800167A8(cBall* pBall);
extern "C" float fn_80038970(
    cFielder* pFielder, nlVector3* pTarget, int nParam);
extern "C" float fn_8003C40C(cFielder* pFielder, int nParam);
extern "C" bool fn_8003E70C(cFielder* pFielder);
extern "C" float fn_8002C800(PlayerTweaks* pTweaks);
extern "C" float fn_8002CF24(PlayerTweaks* pTweaks);
extern "C" void fn_8005EBF8(void* pParam, void* pNode);
extern "C" void fn_8005ED64(void* pParam, void* pNode);
extern "C" bool fn_80014E20(cBall* pBall);
extern "C" bool fn_80036F88(cFielder* pFielder);
extern "C" void fn_8003D8A4(cFielder* pFielder, float fDeltaT);
extern "C" float fn_8002C8D4(PlayerTweaks* pTweaks);
extern "C" float fn_8002CE14(PlayerTweaks* pTweaks);
extern "C" float fn_8002BFB8(PlayerTweaks* pTweaks);
extern "C" void fn_8003B920(cFielder* pFielder);
extern "C" void fn_8003BE14(cFielder* pFielder, float fParam);
extern "C" void fn_8003A5C8(cFielder* pFielder);
extern "C" void fn_8003ADAC(cFielder* pFielder);
extern "C" void fn_8003B54C(cFielder* pFielder);
extern "C" void fn_8003B020(cFielder* pFielder);
extern "C" void fn_8003B0D8(cFielder* pFielder);
extern "C" bool fn_8003E74C(cFielder* pFielder);
extern "C" void fn_800ED92C(unsigned long soundID);
extern "C" float fn_800E02B8(cTeam* pTeam);
extern "C" bool fn_8003E99C(cFielder* pFielder);
extern "C" bool fn_8003E9F0(cFielder* pFielder);
extern "C" void fn_801B8FF4(cFielder* pFielder);
extern "C" float fn_800A9274(void* pParam);
extern "C" void fn_80080BFC(Goalie* pGoalie, float fDeltaT);
extern "C" void fn_801B7E4C(const char* pName, cFielder* pFielder);
extern "C" void fn_801B93E8(cFielder* pFielder);
extern "C" bool fn_800344B0(cFielder* pFielder);
extern "C" void fn_8002E718(cFielder* pFielder);
extern "C" void fn_8002E798(cFielder* pFielder);
extern "C" void fn_8002E39C(cFielder* pFielder);
extern "C" void fn_8002E2E4(cFielder* pFielder);
extern "C" void fn_801BB5DC(cFielder* pFielder, int nParam);
extern "C" void fn_801BB640(cFielder* pFielder, int nParam);
extern "C" void fn_8001458C(cBall* pBall);
extern "C" float fn_800A6388(cTeam* pTeam);
extern "C" cAIPad* fn_80007C3C(int nIndex);
extern "C" bool fn_80038918(cFielder* pFielder);

struct UnidentifiedTornado806E0C94
{
    /* 0x00 */ u8 mUnidentified00[0x0C];
    /* 0x0C */ bool mUnidentified0C;

    virtual void UnidentifiedVirtual08() = 0;
    virtual void UnidentifiedVirtual0C() = 0;
};
extern "C" UnidentifiedTornado806E0C94* fn_800AA060(
    void* pParam, int nParam);

extern BasicSlotPool<PlayerAttackData> lbl_80571960;

extern "C" bool fn_802B6BC8(const nlVector3* v3Start,
    const nlVector3* v3End, const nlVector3* v3A, const nlVector3* v3B,
    float* fOut1, float* fOut2);
extern "C" float fn_8003C300(cFielder* pFielder, float fSpeed);
extern "C" void fn_80331F9C(void* pPad, int nParam, int nParam2);
extern "C" bool fn_800EBC84(
    int, unsigned long, const void*, const void*, void*);
struct UnidentifiedActionTarget806E0C94
{
    /* 0x00 */ u8 mUnidentified00[0x14];
    /* 0x14 */ nlVector3 mUnidentified14;
};

extern "C" void fn_8005CA10(cGame* pGame);
extern "C" void fn_8005C830(cGame* pGame);
extern "C" float fn_8004F58C(void);
extern bool gbUseTurboCharging;
extern "C" void fn_8004F594(int category, const char* format, ...);
extern "C" void fn_8005F238(cGame* pGame, void* pEvent);
extern "C" void fn_800AA568(void* pParam);

class UnidentifiedHandler8011166C
{
public:
    virtual void UnidentifiedVirtual00();
    virtual void UnidentifiedVirtual04();
    virtual void UnidentifiedVirtual08();
    virtual void UnidentifiedVirtual0C();
    virtual void UnidentifiedVirtual10();
    virtual void UnidentifiedVirtual14();
    virtual void UnidentifiedVirtual18();
    virtual void UnidentifiedVirtual1C();
    virtual void UnidentifiedVirtual20();
    virtual void UnidentifiedVirtual24();
    virtual void UnidentifiedVirtual28();
    virtual void UnidentifiedVirtual2C();
    virtual void UnidentifiedVirtual30();
    virtual int UnidentifiedVirtual34();

    /* 0x00 vptr */
    /* 0x04 */ u8 mUnknown04[0x28];
    /* 0x2C */ float mUnidentified2C;
};
extern "C" UnidentifiedHandler8011166C* fn_8011166C(void);
extern "C" void fn_801BA4C8(const char* szName);
extern "C" int fn_8001C5E4(cFielder* pFielder, unsigned short aFacing,
    unsigned short aTarget, int nAnimID, float fParam);
extern "C" void* fn_8027267C(int nParam);
extern "C" void fn_802CE7F4(
    void* pParam, const nlVector3* v3Position, nlVector3* v3Out);
extern "C" float fn_800499EC(cFielder* pFielder, int nParam);
extern "C" float fn_80049CC0(cFielder* pFielder, int nParam);
extern "C" void fn_8005F434(cGame* pGame, void* pEvent);
extern "C" void fn_8005F630(cGame* pGame, void* pEvent);
extern "C" void fn_80111D7C(float fParam);

struct UnidentifiedOnlineState
{
    u8 mUnidentified000[4];
    bool mUnidentified004;
};
extern UnidentifiedOnlineState* lbl_806E2164;
extern "C" bool fn_80123314(void* pParam);
extern "C" void fn_80057FD8(cGame* pGame, bool bParam);
extern "C" bool fn_80332770(void);
extern "C" void fn_800AA3E8(void* pParam, int nParam);
extern "C" void fn_8005F82C(cGame* pGame, cFielder* pFielder);
extern void* lbl_806E10EC;
extern BaseGameSceneManager* lbl_806E1860;

struct UnidentifiedMegaStrikeScene
{
    /* 0x00 */ u8 mUnknown00[0x36];
    /* 0x36 */ bool mUnidentified36;
};



struct UnidentifiedMegaStrikeEvent
{
    /* 0x00 */ cFielder* pFielder;
    /* 0x04 */ float fMeterValue;
    /* 0x08 */ nlVector3 v3Position;
};

struct UnidentifiedSkillshotNode
{
    /* 0x0 */ cFielder* mUnidentified0;
    /* 0x4 */ void* mUnidentified4;
};
extern BasicSlotPool<UnidentifiedSkillshotNode> lbl_805712F8;

struct UnidentifiedWindupObject
{
    virtual ~UnidentifiedWindupObject();

    /* 0x04 */ u8 mUnidentified04[0x60];
    /* 0x64 */ cFielder* mUnidentified64;
};
extern UnidentifiedWindupObject* lbl_806E0C74;

extern "C" UnidentifiedWindupObject* fn_8000DDE8(
    UnidentifiedWindupObject* pObject, int nParam, const nlVector3* pPosition,
    const nlVector3* pTarget, float fParam);
extern "C" void fn_801B94EC(
    cFielder* pFielder, const nlVector3* pPosition, const nlVector3* pNormal);
extern "C" void fn_801B968C(cFielder* pFielder);
extern "C" bool fn_800EBBFC(int, unsigned long, const void*, void*);

static int gHitReactAnims[3][4] = {
    { 0x6A, 0x6D, 0x6C, 0x6B },
    { 0x6E, 0x71, 0x70, 0x6F },
    { 0x72, 0x75, 0x74, 0x73 },
};

static int gShellAttackReactAnims[4] = { 0x5F, 0x62, 0x61, 0x60 };

static int gPassAnims[4] = {
    0x25,
    0x28,
    0x27,
    0x26,
};

static unsigned short gHitReactFacingOffsets[4] = {
    0x8000, 0x4000, 0x0000, 0xC000,
};

extern float lbl_806E3610;
extern float lbl_806E35E0;
extern float lbl_806E35DC;
extern float lbl_806E3614;
extern float lbl_806E3618;
extern float lbl_806E361C;
extern float lbl_806E3620;
extern float lbl_806E3624;
extern float lbl_806E3540;
extern float lbl_806E35B0;
extern float lbl_806E35B4;
extern float lbl_806E355C;
extern float lbl_806E3570;
extern float lbl_806E35C8;
extern float lbl_806E35CC;
extern float lbl_806E35D4;
extern float lbl_806E3560;
extern float lbl_806E35EC;
extern float lbl_806E35F0;
extern float lbl_806E35F4;
extern float lbl_806E35FC;
extern float lbl_806E3600;
extern float lbl_806E3604;
extern float lbl_806E3608;
extern float lbl_806E360C;
extern float lbl_806E35F8;
extern float lbl_806E3578;
extern float lbl_806E35E4;
extern float lbl_806E35E8;
extern float lbl_806DB890;
extern float lbl_806DB89C;
extern float lbl_806DB8A0;
extern float lbl_806DB8A4;
extern float lbl_806DB8A8;
extern int lbl_806DB8AC;
extern int lbl_806DB8B0;
extern float lbl_806DB8B4;
extern float lbl_806DB8B8;
extern float lbl_806DB8BC;
extern float lbl_806DB8C0;
extern float lbl_806DB8C4;
extern float lbl_806DB8C8;
extern float lbl_806DB8CC;
extern float lbl_806DB8D0;
extern float lbl_806DB8D4;
extern float lbl_806DB8D8;
extern float lbl_806DB8DC;
extern float lbl_806DB8E0;
extern float lbl_806DB8E4;
extern float lbl_806DB8E8;
extern float lbl_806DB980;
extern float lbl_806DB984;
extern unsigned short g_IdleTurnCompletionDelta;
extern float lbl_806DB8EC;
extern float lbl_806DB8F0;
extern float lbl_806DB8F4;
extern float lbl_806DB988;
extern float lbl_806DB98C;
extern float lbl_806DB8F8;
extern float lbl_806DB920;
extern float lbl_806DB924;
extern float lbl_806DB928;
extern float lbl_806DB92C;
extern float lbl_806DB930;
extern float lbl_806DB934;
extern float lbl_806DB938;
extern float lbl_806DB93C;
extern float lbl_806DB940;
extern float lbl_806DB944;
extern float lbl_806DB948;
extern float lbl_806DB958;
extern float lbl_806DB95C;
extern float lbl_806DB960;
extern float lbl_806DB964;
extern float lbl_806DB968;
extern float lbl_806DB96C;
extern float lbl_806DB970;
extern float lbl_806DB974;
extern float lbl_806DB978;
extern float lbl_806E0C68;
extern float lbl_806E0C6C;
extern float lbl_806DB950;
extern float lbl_806DB954;
extern float lbl_806DB94C;
extern float lbl_806DB990;
extern float lbl_806DB894;
extern float lbl_806DB898;
extern float lbl_806DB8FC;
extern float lbl_806DB900;
extern float lbl_806DB904;
extern float lbl_806DB90C;
extern float lbl_806DB908;
extern float lbl_806DB910;
extern float lbl_806DB914;
extern float lbl_806DB918;
extern float lbl_806DB91C;
extern float lbl_806DB994;
extern float lbl_806DB9A0;
extern float lbl_806DB9A4;
extern float lbl_806DB9A8;
extern float lbl_806DB9AC;
extern float lbl_806DB9B0;
extern float lbl_806DB9B4;
extern float lbl_806DB998;
extern float lbl_806DB99C;

void cFielder::EndAction()
{
    if (m_tFireTimer.m_uPackedTime == 0)
    {
        SetAction(ACTION_NEED_ACTION);
    }
    else
    {
        fn_8004E11C(lbl_806DB8F8);
    }
}

void cFielder::fn_80043C18(float fDeltaT)
{
    switch (m_eAnimID)
    {
    case 0x7C:
    {
        float fSpin
            = 1.0f - fn_8003E6E4(this)->mUnidentified064;
        Unknown8(m_aActualFacingDirection
                + (u16)(s32)(5000.0f * (2.0f * fSpin + 1.0f)),
            false);

        SetFacingDirection(
            SeekDirection(m_aActualFacingDirection,
                m_aDesiredFacingDirection, fn_8002CF88(fn_8003E6E4(this)),
                fn_8002CF9C(fn_8003E6E4(this)), fDeltaT),
            true);

        if (!mUnidentified330.mUnidentified00)
        {
            float fTime = mUnidentified330.mUnidentified04 - fDeltaT;
            mUnidentified330.mUnidentified04 = fTime;
            if (fTime < 0.0f)
            {
                float fRadius = (float)(s32)(6.0f * (0.33f * fSpin + 1.0f));
                float fUpVelocity
                    = (float)(s32)(15.0f * (0.2f * fSpin + 1.0f));
                nlVector3 v3Velocity;
                MakeRandomDirection2D(v3Velocity, fRadius);
                v3Velocity.z = fUpVelocity;
                SetVelocity(v3Velocity);
                mUnidentified330.mUnidentified00 = true;
                m_v3Position.z += fDeltaT * m_v3Velocity.z;
            }
            else
            {
                float fBlend = 1.0f - fTime;
                UnidentifiedActionTarget806E0C94* pTarget
                    = (UnidentifiedActionTarget806E0C94*)
                          lbl_806E0C94->mUnidentified10E0;
                nlVector3 v3Position;
                v3Position.x = fTime * m_v3Position.x
                    + fBlend * pTarget->mUnidentified14.x;
                v3Position.y = fTime * m_v3Position.y
                    + fBlend * pTarget->mUnidentified14.y;
                v3Position.z = fTime * m_v3Position.z
                    + fBlend * pTarget->mUnidentified14.z;
                SetPosition(v3Position);
            }
        }
        else
        {
            nlVector3 v3Velocity = m_v3Velocity;
            v3Velocity.x *= 0.99f;
            v3Velocity.y *= 0.99f;
            v3Velocity.z = -30.0f * fDeltaT + v3Velocity.z;
            SetVelocity(v3Velocity);

            float fNewZ = fDeltaT * m_v3Velocity.z + m_v3Position.z;
            m_v3Position.z = fNewZ;
            if (fNewZ < 0.0f)
            {
                m_v3Position.z = 0.0f;
                m_v3Velocity.z = 0.0f;

                nlPolar polar;
                nlCartesianToPolar(polar, m_v3Velocity.x, m_v3Velocity.y);
                s16 sFacingDelta = polar.a - m_aActualFacingDirection;
                Unknown8(m_aActualFacingDirection, false);
                SetVelocity(v3Zero);
                SetAnimState(0x7D, true, 0.2f, false, false);
                InitMovementFromAnim(sFacingDelta, v3Zero, 0.15f, false);
            }
        }
        break;
    }
    case 0x7D:
        if (ShouldStartCrossBlend(4))
        {
            EndAction();
        }
        break;
    default:
        EndAction();
        break;
    }
}

void cFielder::fn_8004643C(float fDeltaT)
{
    if (mUnidentified34C > 0.0f)
    {
        if (m_v3Position.z > lbl_806E35E4)
        {
            m_v3Position.z += fDeltaT * m_v3Velocity.z;
        }
        mUnidentified34C -= fDeltaT;

        nlVector3 v3Velocity = m_v3Velocity;
        if (v3Velocity.z >= -15.0f)
        {
            v3Velocity.z += lbl_806E35E8 * fDeltaT;
        }
        SetVelocity(v3Velocity);

        if (mUnidentified34C <= 0.0f)
        {
            fn_80045930();
        }
    }
    else if (m_eAnimID == 0x7E)
    {
        nlVector3 v3Delta;
        v3Delta.y = v3Zero.y - m_v3Position.y;
        v3Delta.x = v3Zero.x - m_v3Position.x;
        v3Delta.z = v3Zero.z - m_v3Position.z;
        Unknown8(nlVector3ToAngle(v3Delta), false);

        SetFacingDirection(
            SeekDirection(m_aActualFacingDirection,
                m_aDesiredFacingDirection, fn_8002C0AC(fn_8003E6E4(this)),
                fn_8002CF10(fn_8003E6E4(this)), fDeltaT),
            true);

        nlPolarToCartesian(m_v3Velocity.x, m_v3Velocity.y,
            m_aActualFacingDirection,
            InterpolateRangeClamped(lbl_806DB8B4, lbl_806DB8B8, 0.0f, 4.0f,
                fn_800A6388(m_pTeam)));

        nlVector2 v3Distance = {
            m_v3Position.x - v3Zero.x,
            m_v3Position.y - v3Zero.y,
        };
        if (nlSqrt(v3Distance.x * v3Distance.x
                    + v3Distance.y * v3Distance.y,
                true)
            < lbl_806E35D4)
        {
            SetAnimState(0x7F, true, 0.2f, false, false);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);
        }
    }
    else
    {
        if (ShouldStartCrossBlend(4))
        {
            bool bHasPad = GetGlobalPad() != 0;
            if (!bHasPad)
            {
                bool bGiven = false;
                for (int i = 0; i < 4; i++)
                {
                    cFielder* pOther = m_pTeam->GetFielder(i);
                    if (pOther != this)
                    {
                        bool bOtherHasPad = pOther->GetGlobalPad() != 0;
                        if (bOtherHasPad
                            && (fn_800344B0(pOther) || fn_80038918(pOther)))
                        {
                            SetAIPad(pOther->m_pController);
                            m_bCanTestController = false;
                            pOther->SetAIPad(0);
                            bGiven = true;
                        }
                    }
                }


                if (!bGiven)
                {
                    for (int i = 0; i < 0x10; i++)
                    {
                        cAIPad* pPad = fn_80007C3C(i);
                        if (pPad != 0)
                        {
                            int mySide = m_pTeam->m_nSide;
                            short playingSide
                                = GameInfoManager::Instance()->GetPlayingSide(
                                    (u16)i);
                            if (playingSide == mySide)
                            {
                                bool bTaken = false;
                                for (int j = 0; j < 5; j++)
                                {
                                    cPlayer* pPlayer = m_pTeam->GetPlayer(j);
                                    bool bPlayerHasPad
                                        = pPlayer->GetGlobalPad() != 0;
                                    if (bPlayerHasPad
                                        && pPlayer->m_pController == pPad)
                                    {
                                        bTaken = true;
                                    }
                                }
                                if (!bTaken)
                                {
                                    SetAIPad(pPad);
                                }
                            }
                        }
                    }

                }
            }

            EndAction();
        }
    }
}

void cFielder::InitActionHit(cFielder* pTarget, unsigned short aDirection)
{
    if (!fn_8003886C(this))
    {
        float fSpeedScale = InterpolateRangeClamped(lbl_806DB894,
            lbl_806DB898, lbl_806E35B0, lbl_806E3560,
            fn_800A9274(lbl_806E0C94->mUnidentified10D8));
        float fStartTime
            = fSpeedScale * (fn_8002D020(fn_8003E6E4(this)) / lbl_806E35B4);
        float fEndTime
            = fSpeedScale * (fn_8002D050(fn_8003E6E4(this)) / lbl_806E35B4);
        float fTimeRange = fEndTime - fStartTime;
        float fMoveDistance = fn_80030750(this);

        if (pTarget == 0)
        {
            pTarget = fn_8003703C(this);
        }

        if (pTarget != 0 && !fn_8003E70C(this))
        {
            float distance = fMoveDistance / fTimeRange;

            float fMidTime = fTimeRange * 0.5f + fStartTime;

            nlVector3 targetVelocity = pTarget->m_v3Velocity;

            if (pTarget->m_eActionState == 1
                && pTarget->m_pCurrentAnimController->m_fTime < lbl_806E35EC)
            {
                targetVelocity = v3Zero;
            }

            int nInterceptResult = 0;

            nlVector3 interceptPos;
            interceptPos.z
                = fMidTime * targetVelocity.z + pTarget->m_v3Position.z;
            interceptPos.y
                = fMidTime * targetVelocity.y + pTarget->m_v3Position.y;
            interceptPos.x
                = fMidTime * targetVelocity.x + pTarget->m_v3Position.x;

            float fThisRadius = mUnidentified0A0;
            float fTargetRadius = pTarget->mUnidentified0A0;
            float fInterceptTimes[2];
            float combinedRadius
                = fn_8002BFA8(fn_8003E6E4(this), fThisRadius)
                + fn_8002BFA8(fn_8003E6E4(pTarget), fTargetRadius);
            CalcInterceptXY(m_v3Position, distance, combinedRadius,
                pTarget->m_v3Position, targetVelocity, nInterceptResult,
                fInterceptTimes);

            float fTime;

            if (nInterceptResult != 0)
            {
                if (nInterceptResult == 2)
                {
                    fTime = nlMinEquals(
                        fInterceptTimes[0], fInterceptTimes[1]);
                }
                else
                {
                    fTime = fInterceptTimes[0];
                }
            }
            else
            {
                fTime = 0.5f * fStartTime + 0.5f * fEndTime;
            }

            interceptPos.y
                = (fTime * targetVelocity.y) + pTarget->m_v3Position.y;
            interceptPos.x
                = (fTime * targetVelocity.x) + pTarget->m_v3Position.x;
            interceptPos.z
                = (fTime * targetVelocity.z) + pTarget->m_v3Position.z;

            nlVector3 v3Delta;
            v3Delta.x = interceptPos.x - m_v3Position.x;
            v3Delta.y = interceptPos.y - m_v3Position.y;
            v3Delta.z = interceptPos.z - m_v3Position.z;
            Unknown8(nlVector3ToAngle(v3Delta), false);
            SetFacingDirection(m_aDesiredFacingDirection, true);
        }
        else if (fn_8003E70C(this))
        {
            SetFacingDirection(m_aActualFacingDirection, true);
        }
        else
        {
            Unknown8(aDirection, false);
            SetFacingDirection(aDirection, true);
        }

        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction(ACTION_HIT);
        SetAnimState(0x67, true, 0.2f, false, false);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);
        m_pCurrentAnimController->m_fPlaybackSpeedScale = fSpeedScale;

        PlayerAttackData* pData;
        lbl_80571960.Allocate(pData);
        pData->pAttacker = this;
        bool bHasGlobalPad = GetGlobalPad() != 0;
        pData->nAttackerPadID
            = bHasGlobalPad ? GetGlobalPad()->fn_80332748() : -1;
        pData->pTarget = pTarget;
        pData->mUnidentified10 = false;
        fn_8005EBF8(lbl_806E0C94, pData);

        if (m_eCharacterClass == TOAD)
        {
            lbl_806E12C8
                ->fn_801743A8(6, this, m_v3Position, v3Zero,
                    lbl_806DB8FC, lbl_806DB900, lbl_806DB904)
                ->fn_80173B08(lbl_806E3578);
            fn_800EBBFC(mUnidentified318, 0xA9AF871E, 0, 0);
        }
    }
}

void cFielder::ActionHit(float fDeltaT)
{
    if (m_eCharacterClass == TOAD)
    {
        for (int i = 0; i < 0x3C; i++)
        {
            PhysicsPatch* pEffect = lbl_806E12C8->fn_801745B8(i);
            if (pEffect != 0 && pEffect->m_Type == 6)
            {
                cSHierarchy* pHierarchy = m_pPoseAccumulator->m_BaseSHierarchy;
                nlVector3 jointPos = GetJointPosition(
                    pHierarchy->GetNodeIndexByID(
                        nlStringLowerHash("bip01 Ponytail12")));
                pEffect->fn_801739A4(jointPos);
            }
        }
    }

    if (m_eCharacterClass == MYSTERY)
    {
        if (m_pTeam->GetCaptain()->m_eCharacterClass == MARIO
            || m_pTeam->GetOtherTeam()->GetCaptain()->m_eCharacterClass
                == MARIO)
        {
            cFielder* pCaptain = m_pTeam->GetCaptain();
            if (pCaptain->m_eCharacterClass != MARIO)
            {
                pCaptain = m_pTeam->GetOtherTeam()->GetCaptain();
            }

            nlVector3& rv3Position = m_v3Position;

            float fOffsetY;
            float fOffsetX;
            nlPolarToCartesian(
                fOffsetX, fOffsetY, m_aActualFacingDirection, lbl_806E35F0);

            nlVector3 v3Target;
            v3Target.y = fOffsetY + rv3Position.y;
            v3Target.x = fOffsetX + rv3Position.x;

            bool bBlocked = false;
            float fT1;
            float fT2;
            for (int i = 0; i < 0x14; i++)
            {
                PhysicsBox_80177498* pObject
                    = pCaptain->mUnidentified400->fn_801792C4(i);
                if (pObject != 0
                    && fn_802B6BC8(&rv3Position, &v3Target,
                        &pObject->mUnidentified038,
                        &pObject->mUnidentified044, &fT1, &fT2))
                {
                    bBlocked = true;
                }
            }

            if (bBlocked)
            {
                if (m_eMovementState != MOVEMENT_NONE)
                {
                    SetVelocity(v3Zero);
                    InitMovementNone(0.0f, 0.0f);
                }
            }
            else if (m_eMovementState != MOVEMENT_FROM_ANIM)
            {
                InitMovementFromAnim(0, v3Zero, 1.0f, false);
            }
        }
    }

    if (ShouldStartCrossBlend(4))
    {
        m_fActualSpeed = 0.0f;
        m_fDesiredSpeed = 0.0f;
        SetVelocity(v3Zero);
        InitMovementNone(0.0f, 0.0f);
        EndAction();
    }
}

bool cFielder::fn_800470B4(cFielder* pFielder, cPlayer* pAttacker)
{
    nlVector3 v3Delta;
    s16 nFacingDelta;
    u16 aAngle;

    v3Delta.y = pFielder->m_v3Position.y - pAttacker->m_v3Position.y;
    v3Delta.x = pFielder->m_v3Position.x - pAttacker->m_v3Position.x;
    v3Delta.z = pFielder->m_v3Position.z - pAttacker->m_v3Position.z;
    aAngle = nlVector3ToAngle(v3Delta);
    nFacingDelta = aAngle - pAttacker->m_aActualFacingDirection;

    float fIntensityA
        = fn_8003E6E4(pFielder)->mUnidentified064;
    float fIntensityB = 1.0f;
    if (pAttacker->m_eClassType == FIELDER)
    {
        fIntensityB
            = fn_8003E6E4((cFielder*)pAttacker)->mUnidentified064;
    }

    int nReact = 1;
    if (GameInfoManager::Instance()->IsRule0x8Equal1())
    {
        nReact = 2;
    }
    else if (fIntensityB < 0.0f && fIntensityA >= 0.0f)
    {
        nReact = 0;
    }
    else if (fIntensityA < 0.0f && fIntensityB >= 0.0f)
    {
        nReact = 2;
    }
    else if ((u16)abs_s16(nFacingDelta) < 0x4000)
    {
        nReact = 2;
    }

    if (pAttacker->m_eCharacterClass == MYSTERY
        && ((cFielder*)pAttacker)->m_eActionState == 1)
    {
        aAngle = pFielder->m_aActualFacingDirection;
    }

    return pFielder->fn_80047240(pAttacker, aAngle, nReact, false, false);
}

bool cFielder::fn_80047240(cPlayer* pAttacker, unsigned short aDirection,
    int nReact, bool bDoFrameLock, bool bBookPenalty)
{
    if (IsFallenDown() && m_tFireTimer.m_uPackedTime == 0)
    {
        return false;
    }

    fn_8002E3F8(this);

    mUnidentified360 = bDoFrameLock;

    if (m_pBall != 0)
    {
        ReleaseBall(0);
        ShootBallDueToContact(pAttacker->m_v3Velocity);

        if (bBookPenalty)
        {
            fn_80036594(pAttacker, this, 0);
        }
    }
    else if (bBookPenalty)
    {
        fn_80036594(pAttacker, this, 1);
    }

    if (!mUnidentified360)
    {
        fn_800EDCE8(this);

        unsigned long soundID;
        if (fn_8001E168(pAttacker))
        {
            bool bUnidentified = fn_8001E168(this);
            soundID = 0xE606A2;
            if (bUnidentified)
            {
                soundID = 0x3642C41B;
            }
        }
        else
        {
            bool bUnidentified = fn_8001E168(this);
            soundID = 0xBDD19FFF;
            if (bUnidentified)
            {
                soundID = 0xBD539FB8;
            }
        }
        fn_800EBBFC(mUnidentified318, soundID, 0, 0);
    }

    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction((eFielderActionState)5);

    s16 angleDiff
        = (s16)((u16)(aDirection + 0x8000) - m_aActualFacingDirection);
    u32 index = (u32)((s16)(angleDiff + 0x2000)) >> 14 & 3;

    bool bTrackStats = false;

    SetAnimState(gHitReactAnims[nReact][index], true, 0.2f, false, false);
    SetFacingDirection(
        (u16)(aDirection + gHitReactFacingOffsets[index]), true);

    InitMovementFromAnim(0, v3Zero, 1.0f, false);

    if (lbl_806E0C94->m_eGameState == 5
        || lbl_806E0C94->m_eGameState == 6)
    {
        bTrackStats = true;
    }
    if (bTrackStats)
    {
        StatsTracker::Instance()->TrackStat((ePlayerStats)0x12,
            pAttacker->m_pTeam->m_nSide, pAttacker->m_ID, 0, 0, 0, 0);
    }

    switch (nReact)
    {
    case 0:
        fn_80139D1C(2, GetGlobalPad());
        break;
    case 1:
        fn_80139D1C(3, GetGlobalPad());
        break;
    case 2:
        fn_80139D1C(4, GetGlobalPad());
        break;
    }

    m_fDesiredSpeed = 0.0f;
    return true;
}

void cFielder::fn_800474FC(float fDeltaT)
{
    if (m_pCurrentAnimController->TestFrameTrigger(lbl_806DB8EC)
        && mUnidentified360)
    {
        fn_8005CA10(lbl_806E0C94);
    }

    if (ShouldStartCrossBlend(4))
    {
        m_fActualSpeed = 0.0f;
        m_fDesiredSpeed = 0.0f;
        SetVelocity(v3Zero);
        InitMovementNone(0.0f, 0.0f);
        EndAction();
    }
}

void cFielder::InitActionIdleTurn(unsigned short desiredFacingDirection)
{
    Unknown8(desiredFacingDirection, false);
    SetAnimState(0, true, 0.2f, false, false);
    InitMovementNone(lbl_806E35F4, lbl_806E35F8);
    SetAction(ACTION_IDLE_TURN);
}

void cFielder::ActionIdleTurn(float fDeltaT)
{
    s16 angleDiff
        = (u16)abs_s16(m_aDesiredFacingDirection - m_aActualFacingDirection);

    if (angleDiff < g_IdleTurnCompletionDelta)
    {
        EndAction();
    }
}

void cFielder::InitActionLateOneTimerFromVolley()
{
    mActionShotVars.bIsChipShot = false;

    DoResetShotMeter(0.0f);

    ShotMeter* pShotMeter = m_pShotMeter;
    pShotMeter->CalcOneTimerValue(this, true);

    SetAction(ACTION_LATE_ONETIMER_FROM_VOLLEY);

    int LateOneTimerFromVolleyAnims[4] = {
        0x4A,
        0x4D,
        0x4C,
        0x4B,
    };

    s16 facingDelta = GetFacingDeltaToPosition(
        m_pTeam->GetOtherNet()->m_v3NetLocation);
    int index = (u16)(facingDelta + 0x2000) >> 14;

    int nAnimID;
    bool bMirror = false;
    if (index == 2)
    {
        bMirror = m_pCurrentAnimController->m_bMirror;
    }

    nAnimID = LateOneTimerFromVolleyAnims[index];

    s16 nTurnAdjust = 0;
    switch (nAnimID)
    {
    case 0x4C:
        nTurnAdjust = -0x8000;
        break;
    case 0x4D:
        nTurnAdjust = -0x4000;
        break;
    case 0x4B:
        nTurnAdjust = 0x4000;
        break;
    }

    s16 facingDelta2 = GetFacingDeltaToPosition(
        m_pTeam->GetOtherNet()->m_v3NetLocation);

    SetAnimState(nAnimID, true, 0.0f, false, bMirror);

    InitMovementFromAnim(
        (s16)(nTurnAdjust + facingDelta2), v3Zero, lbl_806E3540, false);

    bool bShotNormally = true;
    if (fn_8003C180(this))
    {
        fn_80035700(this);
        bShotNormally = false;
    }
    else
    {
        fn_80037AC8(this, 0);
        fn_8005C830(lbl_806E0C94);
    }

    fn_801B75C8(this, 2, 0, 0, bShotNormally);

    bool bUnidentified = fn_8001E168(this);
    unsigned long soundID = 0x1CEC5A02;
    if (bUnidentified)
    {
        soundID = 0xFDE0C69B;
    }
    fn_800EBBFC(mUnidentified318, soundID, 0, 0);
}

void cFielder::ActionLateOneTimerFromVolley(float fDeltaT)
{
    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

bool cFielder::DoCommonInitActionLooseBall(
    const nlVector3& rv3OneTimerTarget, bool bVolleyPass)
{
    nlVector3 v3SimulatedBallPos;
    nlVector3 v3IdleGroundContactOffset;
    nlVector3 v3IdleVolleyContactOffset;
    nlVector3 v3LeadGroundContactOffset;
    nlVector3 v3BallToSelf;
    nlVector3 v3ToTarget;
    nlVector3 v3ContactOffsetLocal;
    nlVector3 v3ContactOffsetWorld;
    nlVector3 v3TmpAdjustment;
    nlVector3 v3MoveAdjustment;
    float fBallDelta[2];
    float fGroundReachDelta[2];
    float fVolleyReachDelta[2];
    float fLeadReachDelta[2];
    float fPhysicsRadius;
    float fCos;
    float fSin;

    cSAnim* pIdleGroundContactAnim = m_pAnimInventory->GetAnim(
        GetOneTimerIdleGroundContactAnims()->nAnimID);
    float fMaxSimulatedTime
        = GetOneTimerIdleGroundContactAnims()->fAnimContactFrame
        / (float)pIdleGroundContactAnim->m_nNumKeys;

    float fSimulatedTime = 0.0f;

    FakeBallWorld::ResetBallIterator();

    const LooseBallContactAnimInfo* pAnimInfoList
        = GetOneTimerIdleGroundContactAnims();
    int nNumAnims = GetNumOneTimerIdleGroundContactAnims();

    GetJointPositionFuture(&v3IdleGroundContactOffset,
        pAnimInfoList->nAnimID, m_nBallJointIndex,
        pAnimInfoList->fAnimContactFrame
            / (float)pIdleGroundContactAnim->m_nNumKeys,
        true, true, true, true);

    cSAnim* pIdleVolleyContactAnim
        = m_pAnimInventory->GetAnim(pAnimInfoList->nAnimID);
    float fIdleVolleyTime
        = GetOneTimerIdleVolleyContactAnims()->fAnimContactFrame
        / (float)pIdleVolleyContactAnim->m_nNumKeys;
    GetJointPositionFuture(&v3IdleVolleyContactOffset,
        GetOneTimerIdleVolleyContactAnims()->nAnimID, m_nBallJointIndex,
        fIdleVolleyTime, true, true, true, true);

    cSAnim* pLeadGroundContactAnim
        = m_pAnimInventory->GetAnim(pAnimInfoList->nAnimID);
    float fLeadGroundTime
        = GetOneTimerLeadGroundContactAnims()->fAnimContactFrame
        / (float)pLeadGroundContactAnim->m_nNumKeys;
    GetJointPositionFuture(&v3LeadGroundContactOffset,
        GetOneTimerLeadGroundContactAnims()->nAnimID, m_nBallJointIndex,
        fLeadGroundTime, true, true, true, true);

    float fMaxCatchupSpeed = fn_8002E1B0(this);
    float fMinBallZ = lbl_806E35FC;
    bool bNoContactFound = true;
    bool bBallState5 = g_pBall->meBallState == 5;

    while (fSimulatedTime < fMaxSimulatedTime)
    {
        FakeBallWorld::GetNextBallPosition(v3SimulatedBallPos);
        fSimulatedTime += FixedUpdateTask::GetPhysicsUpdateTick();

        if (v3SimulatedBallPos.z < fMinBallZ)
        {
            fMinBallZ = v3SimulatedBallPos.z;
        }

        fBallDelta[1] = v3SimulatedBallPos.y - m_v3Position.y;
        fBallDelta[0] = v3SimulatedBallPos.x - m_v3Position.x;
        float fBallDistance = nlSqrt(fBallDelta[0] * fBallDelta[0]
                + fBallDelta[1] * fBallDelta[1],
            true);

        if (!bBallState5)
        {
            if (v3SimulatedBallPos.z
                < g_pBall->fn_80014F38(mUnidentified0A0))
            {
                float fAnimTime
                    = (pAnimInfoList->fAnimContactFrame / 30.0f)
                    / fSimulatedTime;
                fGroundReachDelta[1]
                    = v3IdleGroundContactOffset.y - m_v3Position.y;
                fGroundReachDelta[0]
                    = v3IdleGroundContactOffset.x - m_v3Position.x;
                float fCatchup = fBallDistance
                    - nlSqrt(fGroundReachDelta[0] * fGroundReachDelta[0]
                            + fGroundReachDelta[1] * fGroundReachDelta[1],
                        true);
                if (fCatchup > 0.0f)
                {
                    fCatchup /= fSimulatedTime;
                }
                bool bCanReach = fCatchup <= fMaxCatchupSpeed;
                if (fAnimTime > lbl_806DB8C4 && fAnimTime < lbl_806DB8C8
                    && bCanReach)
                {
                    bNoContactFound = false;
                    break;
                }
            }

            float fVolleyZDelta = (float)fabs(
                v3SimulatedBallPos.z - v3IdleVolleyContactOffset.z);
            if (fVolleyZDelta < g_pBall->fn_80014F38(mUnidentified0A0))
            {
                float fAnimTime
                    = (GetOneTimerIdleVolleyContactAnims()->fAnimContactFrame
                          / 30.0f)
                    / fSimulatedTime;
                fVolleyReachDelta[1]
                    = v3IdleVolleyContactOffset.y - m_v3Position.y;
                fVolleyReachDelta[0]
                    = v3IdleVolleyContactOffset.x - m_v3Position.x;
                float fCatchup = fBallDistance
                    - nlSqrt(fVolleyReachDelta[0] * fVolleyReachDelta[0]
                            + fVolleyReachDelta[1] * fVolleyReachDelta[1],
                        true);
                if (fCatchup > 0.0f)
                {
                    fCatchup /= fSimulatedTime;
                }
                bool bCanReach = fCatchup <= fMaxCatchupSpeed;
                if (fAnimTime > lbl_806DB8C4 && fAnimTime < lbl_806DB8C8
                    && bCanReach)
                {
                    pAnimInfoList = GetOneTimerIdleVolleyContactAnims();
                    nNumAnims = GetNumOneTimerIdleVolleyContactAnims();
                    bNoContactFound = false;
                    break;
                }
            }
        }

        float fLeadZDelta = (float)fabs(
            v3SimulatedBallPos.z - v3LeadGroundContactOffset.z);
        if (fLeadZDelta < g_pBall->fn_80014F38(mUnidentified0A0))
        {
            float fAnimTime
                = (GetOneTimerLeadGroundContactAnims()->fAnimContactFrame
                      / 30.0f)
                / fSimulatedTime;
            fLeadReachDelta[1]
                = v3LeadGroundContactOffset.y - m_v3Position.y;
            fLeadReachDelta[0]
                = v3LeadGroundContactOffset.x - m_v3Position.x;
            float fCatchup = fBallDistance
                - nlSqrt(fLeadReachDelta[0] * fLeadReachDelta[0]
                        + fLeadReachDelta[1] * fLeadReachDelta[1],
                    true);
            if (fCatchup > 0.0f)
            {
                fCatchup /= fSimulatedTime;
            }
            bool bCanReach = fCatchup <= fMaxCatchupSpeed * lbl_806DB8CC;
            if (fAnimTime > lbl_806DB8C4 && fAnimTime < lbl_806DB8C8
                && bCanReach)
            {
                pAnimInfoList = GetOneTimerLeadGroundContactAnims();
                nNumAnims = GetNumOneTimerLeadGroundContactAnims();
                bNoContactFound = false;
                break;
            }
        }

        if (fn_8002D2C4(&v3SimulatedBallPos, 0.0f, 0))
        {
            fSimulatedTime = fMaxSimulatedTime;
            break;
        }
    }

    if (bNoContactFound)
    {
        return false;
    }

    v3BallToSelf.y = m_v3Position.y - g_pBall->m_v3Position.y;
    v3BallToSelf.x = m_v3Position.x - g_pBall->m_v3Position.x;
    v3BallToSelf.z = m_v3Position.z - g_pBall->m_v3Position.z;

    const LooseBallContactAnimInfo* pBestBallContactAnimInfo
        = fn_80038230(this, pAnimInfoList, nNumAnims,
            m_aActualFacingDirection, &m_v3Position, &rv3OneTimerTarget,
            nlATan2f(v3BallToSelf.y, v3BallToSelf.x));

    v3ToTarget.y = rv3OneTimerTarget.y - m_v3Position.y;
    v3ToTarget.x = rv3OneTimerTarget.x - m_v3Position.x;
    v3ToTarget.z = rv3OneTimerTarget.z - m_v3Position.z;
    u32 aDesiredFacingDirection = nlVector3ToAngle(v3ToTarget);

    switch (pBestBallContactAnimInfo->nAnimID)
    {
    case 0x35:
    case 0x39:
    case 0x45:
        aDesiredFacingDirection += 0x4000;
        break;
    case 0x36:
    case 0x3A:
    case 0x46:
        aDesiredFacingDirection -= 0x4000;
        break;
    case 0x3B:
    case 0x47:
    case 0x49:
        aDesiredFacingDirection += 0x8000;
        break;
    }

    cSAnim* pBestContactAnim
        = m_pAnimInventory->GetAnim(pBestBallContactAnimInfo->nAnimID);
    s16 nFacingDelta
        = (s16)(aDesiredFacingDirection - m_aActualFacingDirection);

    GetJointPositionFuture(&v3ContactOffsetLocal,
        pBestBallContactAnimInfo->nAnimID, m_nBallJointIndex,
        pBestBallContactAnimInfo->fAnimContactFrame
            / (float)pBestContactAnim->m_nNumKeys,
        true, true, false, true);

    nlSinCos(&fSin, &fCos, aDesiredFacingDirection);

    v3ContactOffsetWorld.z = v3ContactOffsetLocal.z;

    mUnidentified368 = pBestBallContactAnimInfo->fAnimContactFrame
        / (float)pBestContactAnim->m_nNumKeys;

    const float fRotationCos = fCos;
    const float fContactOffsetX = v3ContactOffsetLocal.x;
    v3ContactOffsetWorld.x = (fContactOffsetX * fRotationCos)
        - (v3ContactOffsetLocal.y * fSin);
    v3ContactOffsetWorld.y = (v3ContactOffsetLocal.y * fRotationCos)
        + (fContactOffsetX * fSin);

    SetAnimState(pBestBallContactAnimInfo->nAnimID, false,
        mUnidentified368 * lbl_806DB990, false, false);

    m_pCurrentAnimController->m_fPlaybackSpeedScale
        = (pBestBallContactAnimInfo->fAnimContactFrame / 30.0f)
        / fSimulatedTime;

    nlVec3Sub(v3TmpAdjustment, v3SimulatedBallPos, v3ContactOffsetWorld);
    nlVec3Sub(v3MoveAdjustment, v3TmpAdjustment, m_v3Position);

    InitMovementFromAnim(nFacingDelta, v3MoveAdjustment,
        mUnidentified368 * lbl_806DB990, false);

    m_pPhysicsCharacter->m_pPlayerPlayerColumn->GetRadius(&fPhysicsRadius);

    float fMaxGoalX = cField::GetGoalLineX(1U) - 0.5f;
    float fNetWidth = cNet::m_fNetWidth;
    float fMaxGoalY = (0.5f * fNetWidth) + lbl_806E355C;
    float fMinGoalY
        = ((0.5f * fNetWidth) - fn_8004F58C()) - fPhysicsRadius;

    float fAbsX = (float)fabs(v3SimulatedBallPos.x);
    if ((fAbsX < fMaxGoalX)
        || ((float)fabs(v3SimulatedBallPos.y) > fMaxGoalY)
        || ((float)fabs(v3SimulatedBallPos.y) < fMinGoalY))
    {
        m_pPhysicsCharacter->m_CanCollideWithWall = false;
    }

    return true;
}

void cFielder::InitActionLooseBallPass(cFielder* pPassTarget, bool bVolleyPass)
{
    cFielder* finalPassTarget;
    if (pPassTarget != 0)
    {
        finalPassTarget = pPassTarget;
    }
    else
    {
        finalPassTarget = fn_80096F54(this, bVolleyPass);
    }

    mActionLooseBallPassVars.passTarget = finalPassTarget;

    if (finalPassTarget == 0)
    {
        if (DoCommonInitActionLooseBall(
                m_pTeam->GetOtherNet()->m_v3NetLocation, false))
        {
            InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet,
                fvNotSet);
            SetAction(ACTION_LOOSE_BALL_SHOT);
            mActionShotVars.bIsChipShot = false;
            SetNoPickUpTime(3.0f);

            bool bUnidentified = fn_8001E168(this);
            unsigned long soundID = 0x1CEC5A02;
            if (bUnidentified)
            {
                soundID = 0xFDE0C69B;
            }
            fn_800EBBFC(mUnidentified318, soundID, 0, 0);
        }
    }
    else if (DoCommonInitActionLooseBall(finalPassTarget->m_v3Position, true))
    {
        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction(ACTION_LOOSE_BALL_PASS);
        mActionShotVars.bIsChipShot = bVolleyPass;
        m_bCanTestController = false;
        SetNoPickUpTime(3.0f);
    }
}

void cFielder::fn_80048484(float fDeltaT)
{
    bool bIsChipShot = false;
    if (mActionShotVars.bIsChipShot || fn_80035F34(this))
    {
        bIsChipShot = true;
    }
    mActionShotVars.bIsChipShot = bIsChipShot;

    if (m_pCurrentAnimController->TestTrigger(mUnidentified368))
    {
        m_pCurrentAnimController->m_fPlaybackSpeedScale = 1.0f;
        m_pPhysicsCharacter->m_CanCollideWithWall = true;
    }

    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::InitActionLooseBallShot(bool bIsChipShot)
{
    if (DoCommonInitActionLooseBall(
            m_pTeam->GetOtherNet()->m_v3NetLocation, false))
    {
        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction(ACTION_LOOSE_BALL_SHOT);
        mActionShotVars.bIsChipShot = bIsChipShot;
        SetNoPickUpTime(lbl_806E35E0);

        bool bUnidentified = fn_8001E168(this);
        unsigned long soundID = 0x1CEC5A02;
        if (bUnidentified)
        {
            soundID = 0xFDE0C69B;
        }
        fn_800EBBFC(mUnidentified318, soundID, 0, 0);
    }
}

void cFielder::fn_800486DC(float fDeltaT)
{
    bool bIsChipShot = false;
    if (mActionShotVars.bIsChipShot || fn_80035F34(this))
    {
        bIsChipShot = true;
    }
    mActionShotVars.bIsChipShot = bIsChipShot;

    if (m_pCurrentAnimController->TestTrigger(mUnidentified368))
    {
        m_pCurrentAnimController->m_fPlaybackSpeedScale = 1.0f;
        m_pPhysicsCharacter->m_CanCollideWithWall = true;
    }

    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void fn_80048870(cFielder* pFielder)
{
    lbl_806DB5A8 = false;

    for (int i = 0; i < 2; i++)
    {
        cTeam* pTeam = g_pTeams[i];
        for (int j = 0; j < 4; j++)
        {
            cFielder* pOther = pTeam->GetFielder(j);
            if (pFielder != pOther && !fn_800387CC(pOther))
            {
                fn_800978E8(pOther, 0);
                fn_80031A30(pOther, 3, lbl_806E3600);
            }
        }
    }
}

void cFielder::fn_80048918()
{
    lbl_806DB5A8 = true;

    for (int i = 0; i < 2; i++)
    {
        cTeam* pTeam = g_pTeams[i];
        for (int j = 0; j < 4; j++)
        {
            cFielder* pFielder = pTeam->GetFielder(j);
            if (this != pFielder && fn_8003881C(pFielder)
                && lbl_806E0C94->mUnidentified030 == 0)
            {
                fn_80316968(fn_80319FC0(fn_8002E1A4(pFielder), 0x1D));
            }
        }
    }
}

void cFielder::fn_800489C0()
{
}

float cFielder::fn_800489C4()
{
    float fShooting = fn_8003E6E4(this)->fShooting;
    if (fShooting > 1.0f)
    {
        fShooting = 1.0f;
    }
    return InterpolateClamped(lbl_806DB948, lbl_806DB94C, fShooting);
}

float cFielder::fn_80048A08()
{
    float fShooting = fn_8003E6E4(this)->fShooting;
    if (fShooting > 1.0f)
    {
        fShooting = 1.0f;
    }
    return InterpolateClamped(lbl_806DB950, lbl_806DB954, fShooting);
}

void cFielder::InitActionMegaStrikeMeter(bool bParam)
{
    fn_8004F594(0x10, "InitActionMegaStrikeMeter at frame %d\n",
        fn_8011166C()->UnidentifiedVirtual34());

    mUnidentified390 = 0.0f;
    mUnidentified394 = 0.0f;
    mUnidentified398 = -1.0f;
    mUnidentified39C = -1.0f;
    mUnidentified3A0 = -1.0f;
    mUnidentified3A4 = -1.0f;
    mUnidentified3A8 = -1.0f;
    mUnidentified3AC = 0.0f;
    mUnidentified3B0 = 0.0f;
    mUnidentified3B4 = 0.0f;
    mUnidentified3B8 = false;
    mUnidentified3BC = 0.0f;
    mUnidentified3C0 = 0.0f;
    mUnidentified3C4 = 0.0f;
    mUnidentified3C8 = 0.0f;
    mUnidentified3CC = 0.0f;
    mUnidentified3D0 = 0.0f;
    mUnidentified3D4 = 0.0f;

    lbl_806E0C94->fn_80057FC0();

    mUnidentified478 = 0;

    if (m_pBall == 0)
    {
        m_pHeadTrack->m_bTrackOOI = true;

        if (m_eActionState != 0x12)
        {
            mUnidentified37C = 0;
            m_aActualMovementDirection = m_aActualFacingDirection;
            Unknown8(m_aActualFacingDirection, false);
            mUnidentified380 = false;
            m_fDesiredSpeed = m_fActualSpeed;
        }

        SetAction(ACTION_RUNNING);
        return;
    }

    if (mUnidentified3DC)
    {
        fn_8005001C(true);
    }

    bool bNearGoal = false;
    float fAbsX = (float)fabs(m_v3Position.x);
    if (fAbsX > cField::GetGoalLineX(1U) - lbl_806E3570)
    {
        if ((float)fabs(m_v3Position.y)
            < 0.5f * cNet::m_fNetWidth + 1.0f)
        {
            bNearGoal = true;
        }
    }

    bool bDidWindup = false;
    if (lbl_806E0C94->m_eGameState == 3 || bNearGoal)
    {
        fn_801BA4C8("ball_sts_windup");
        fn_8004B86C(false, false);
        bDidWindup = true;
    }

    if (!bDidWindup)
    {
        fn_801BA4C8("ball_sts_windup");
        SetAction(ACTION_SHOOT_TO_SCORE);

        if (m_eAnimID == 0x52)
        {
            SetAnimState(0x57, true, 0.2f, false, false);
        }
        else
        {
            SetAnimState(0x58, true, 0.2f, false, false);
        }

        nlVector3 v3NetLocation = m_pTeam->GetOtherNet()->m_v3NetLocation;

        InitMovementFromAnim(
            (s16)fn_8001C5E4(this, m_aActualFacingDirection,
                (u16)(s32)(10430.378f
                    * nlATan2f(v3NetLocation.y - m_v3Position.y,
                        v3NetLocation.x - m_v3Position.x)),
                m_eAnimID, 1.0f),
            v3Zero, lbl_806E35CC, false);

        if (bParam)
        {
            ShootToScoreMeter::instance.m_v3OriginalMeterPosition
                = m_v3Position;
            ShootToScoreMeter::instance.m_v3MeterPosition = m_v3Position;
            ShootToScoreMeter::instance.TurnOnMeter();
            fn_800EBBFC(0, 0xC4534945, 0, 0);
        }

        UnidentifiedMegaStrikeEvent event;
        event.pFielder = this;
        event.fMeterValue = mUnidentified3BC;
        nlVector3 v3Column;
        fn_802CE7F4(fn_8027267C(8), &m_v3Position, &v3Column);
        event.v3Position = v3Column;
        fn_8005F238(lbl_806E0C94, &event);

        fn_800AA568(lbl_806E0C94->mUnidentified10DC);

        fn_800978E8(this, 0);

        mUnidentified3B4 = lbl_806DB924;
        mUnidentified3AC = lbl_806DB924;
        mUnidentified3B8 = false;

        float fShooting = fn_8003E6E4(this)->fShooting;
        if (fShooting > 1.0f)
        {
            fShooting = 1.0f;
        }
        else if (fShooting < 0.0f)
        {
            fShooting = 0.0f;
        }

        float fSegmentA
            = InterpolateClamped(lbl_806DB95C, lbl_806DB958, fShooting);
        float fSegmentB
            = InterpolateClamped(lbl_806DB960, lbl_806DB964, fShooting);
        float fSegmentC
            = InterpolateClamped(lbl_806DB968, lbl_806DB96C, fShooting);
        float fSegmentD
            = InterpolateClamped(lbl_806E0C68, lbl_806E0C6C, fShooting);

        float fHalfA = fSegmentA * 0.5f;
        float fHalfB = fSegmentB * 0.5f;
        float fHalfD = fSegmentD * 0.5f;
        float fHalfC = fSegmentC * 0.5f;

        mUnidentified3C4 = lbl_806DB970 + fHalfA;
        ShootToScoreMeter::instance.fn_801B1004(mUnidentified3C4);
        ShootToScoreMeter::instance.fn_801B1024(fSegmentA);

        mUnidentified3C8 = fHalfB + (mUnidentified3C4 + fHalfA);
        ShootToScoreMeter::instance.fn_801B1044(mUnidentified3C8);
        ShootToScoreMeter::instance.fn_801B1064(fSegmentB);

        mUnidentified3CC = fHalfC + (mUnidentified3C8 + fHalfB);
        ShootToScoreMeter::instance.fn_801B1084(mUnidentified3CC);
        ShootToScoreMeter::instance.fn_801B10A4(fSegmentC);

        mUnidentified3D0 = fHalfD + (mUnidentified3CC + fHalfC);
        ShootToScoreMeter::instance.fn_801B10C4(mUnidentified3D0);
        ShootToScoreMeter::instance.fn_801B10E4(fSegmentD);

        mUnidentified3D4 = mUnidentified3D0;

        fn_800EC12C(0x5C8E379, this);
        fn_800EBBFC(0, 0x5C8E379, "Needle Left", this);
    }
}

void cFielder::fn_80048FB0(float fDeltaT, bool bButtonPressed, int nParam)
{
    if (bButtonPressed)
    {
        fn_8004F594(0x10, "Button pushed phase %d time %f\n",
            mUnidentified3B8, mUnidentified3AC);
    }

    if (bButtonPressed)
    {
        if (!mUnidentified3B8)
        {
            if (mUnidentified39C < 0.0f)
            {
                mUnidentified39C = mUnidentified3A8;
                DoMegaMeterFirstButtonPressEvent(nParam);
            }
        }
        else if (mUnidentified3A0 < 0.0f)
        {
            mUnidentified3A0 = mUnidentified3A8;
            DoMegaMeterSecondButtonPressEvent(nParam);
        }
    }

    mUnidentified3AC -= fDeltaT;
    if (mUnidentified3AC < 0.0f)
    {
        if (!mUnidentified3B8)
        {
            if (mUnidentified39C < 0.0f)
            {
                mUnidentified39C = 1.0f;
                DoMegaMeterFirstButtonPressEvent(nParam);
            }

            float fMidTime = lbl_806DB934 * 0.5f + lbl_806DB93C;
            if (mUnidentified3A8 <= fMidTime)
            {
                mUnidentified3B0 = InterpolateRangeClamped(lbl_806DB928,
                    0.0f, fMidTime, 0.0f, mUnidentified3A8);
            }
            else
            {
                mUnidentified3B0 = InterpolateRangeClamped(lbl_806DB930,
                    lbl_806DB92C, 1.0f, fMidTime, mUnidentified3A8);
            }

            mUnidentified3A4 = InterpolateClamped(lbl_806DB934,
                lbl_806DB938,
                fn_8003E6E4(this)->fShooting);
            float fSecondPhaseTime = InterpolateClamped(lbl_806DB940,
                lbl_806DB944,
                fn_8003E6E4(this)->fShooting);

            mUnidentified3B8 = true;
            mUnidentified3AC = mUnidentified3B0;
            ShootToScoreMeter::instance.SetGreenRegionWidth(
                mUnidentified3A4);
            ShootToScoreMeter::instance.fn_801B0FE4(fSecondPhaseTime);
            ShootToScoreMeter::instance.SetGreenBarPosition(lbl_806DB93C);
            ShootToScoreMeter::instance.mUnidentified2E = true;
        }
        else if (mUnidentified3A0 < 0.0f)
        {
            mUnidentified3A0 = 0.0f;
            DoMegaMeterSecondButtonPressEvent(nParam);
        }
    }

    if (mUnidentified3B8)
    {
        mUnidentified3A8
            = (mUnidentified39C * mUnidentified3AC) / mUnidentified3B0;
    }
    else
    {
        mUnidentified3A8 = 1.0f - mUnidentified3AC / mUnidentified3B4;
    }

    if (mUnidentified3A0 < 0.0f)
    {
        ShootToScoreMeter::instance.SetWhiteBarPosition(mUnidentified3A8);
    }
    else
    {
        ShootToScoreMeter::instance.SetWhiteBarPosition(mUnidentified3A0);
    }

    ShootToScoreMeter::instance.SetSavedWhiteBarPosition(mUnidentified39C);
}

void cFielder::fn_8004923C(float fDeltaT, bool bButtonPressed, int nParam)
{
    if (mUnidentified478 == 0)
    {
        if (nParam != 0)
        {
            UnidentifiedNetworkMessage_80126D84 message;
            if (fn_80123314(lbl_806E10EC))
            {
                fn_80057FD8(lbl_806E0C94, bButtonPressed);
            }
            fn_80048FB0(fDeltaT, bButtonPressed, nParam);
        }
        else if (fn_80123314(lbl_806E10EC)
            && lbl_806E0C94->mUnidentified0C0.mSize != 0)
        {
            fn_80048FB0(fDeltaT,
                lbl_806E0C94->mUnidentified0C0.UnidentifiedRemoveStart(),
                nParam);
        }
    }
    else if (fn_80123314(lbl_806E10EC) && nParam == 0
        && lbl_806E0C94->mUnidentified0C0.mSize != 0)
    {
        fn_8004F594(0x10,
            "Have unprocessed m_ReceivedMegaMeterQ %d in state %d.  "
            "Processing All Now.\n",
            lbl_806E0C94->mUnidentified0C0.mSize, mUnidentified478);

        while (lbl_806E0C94->mUnidentified0C0.mSize != 0)
        {
            fn_80048FB0(fDeltaT,
                lbl_806E0C94->mUnidentified0C0.UnidentifiedRemoveStart(),
                nParam);
        }
    }

    if (mUnidentified478 == 2)
    {
        if (!fn_80332770())
        {
            mUnidentified478 = 3;
        }
        else
        {
            float fEndTime = mUnidentified398 + lbl_806DB974;
            if (fn_8011166C()->mUnidentified2C >= fEndTime)
            {
                mUnidentified478 = 3;
            }
        }
    }
    else if (mUnidentified478 == 3)
    {
        ((UnidentifiedMegaStrikeScene*)lbl_806E1860->GetScene(
             (SceneList)0x64))
            ->mUnidentified36
            = true;
        SetAction((eFielderActionState)0xB);
        fn_80048870(0);

        m_pTeam->GetGoalie()->fn_8008E2D0();
        m_pTeam->GetOtherTeam()->GetGoalie()->fn_8008E2D0();

        g_pBall->mbBallFrozen = true;
        muInvincibleStatus |= 0x1F;

        m_pTeam->GetOtherTeam()->GetGoalie()->fn_8008EF58();

        fn_800AA3E8(lbl_806E0C94->mUnidentified10DC, 0);
        lbl_806E0C94->fn_80058704();
        lbl_806E0C94->mUnidentified03C = this;
        fn_8005F82C(lbl_806E0C94, this);
    }
}

void cFielder::DoMegaMeterFirstButtonPressEvent(int nParam)
{
    fn_8004F594(0x10, "DoMegaMeterFirstButtonPressEvent at time %f\n",
        mUnidentified3AC);

    ShootToScoreMeter::instance.mbShowSavedWhiteBar = true;

    mUnidentified3BC = (float)(s32)fn_800499EC(this, 0);

    fn_80139D1C(1, GetGlobalPad());

    UnidentifiedMegaStrikeEvent event;
    event.pFielder = this;
    event.fMeterValue = mUnidentified3BC;
    nlVector3 v3Column;
    fn_802CE7F4(fn_8027267C(8), &m_v3Position, &v3Column);
    event.v3Position = v3Column;
    fn_8005F434(lbl_806E0C94, &event);

    if (mUnidentified3AC >= 0.0f)
    {
        mUnidentified3AC = -1.0f;
    }

    fn_800EC12C(0x5C8E379, this);

    for (int i = 0; i < 2; i++)
    {
        cTeam* pTeam = g_pTeams[i];
        for (int j = 0; j < 5; j++)
        {
            cPlayer* pPlayer = pTeam->GetPlayer(j);
            if (pPlayer->GetGlobalPad() != 0)
            {
                fn_800EDCE8(pPlayer);
                fn_800EBBFC(0, 0xCC32C1A8, 0, 0);
            }
        }
    }

    fn_800EC12C(0xBF541A4C, this);
    fn_800EBBFC(0, 0xBF541A4C, "Needle Right", this);
}

void cFielder::DoMegaMeterSecondButtonPressEvent(int nParam)
{
    fn_8004F594(0x10, "DoMegaMeterSecondButtonPressEvent at time %f\n",
        mUnidentified3AC);

    ShootToScoreMeter::instance.mUnidentified2C = true;
    ShootToScoreMeter::instance.m_v3MeterPosition
        = ShootToScoreMeter::instance.m_v3OriginalMeterPosition;

    mUnidentified3C0 = fn_80049CC0(this, 0);

    for (int i = 0; i < 2; i++)
    {
        cTeam* pTeam = g_pTeams[i];
        for (int j = 0; j < 5; j++)
        {
            cPlayer* pPlayer = pTeam->GetPlayer(j);
            if (pPlayer->GetGlobalPad() != 0)
            {
                if (mUnidentified3C0 >= 1.0f)
                {
                    fn_80139D1C(3, pPlayer->GetGlobalPad());
                    fn_800EDCE8(pPlayer);
                    fn_800EBBFC(0, 0xD17A65BA, 0, 0);
                }
                else if (mUnidentified3C0 >= 0.0f)
                {
                    fn_80139D1C(2, pPlayer->GetGlobalPad());
                    fn_800EDCE8(pPlayer);
                    fn_800EBBFC(0, 0xCC2F680B, 0, 0);
                }
                else
                {
                    fn_80139D1C(1, pPlayer->GetGlobalPad());
                    fn_800EDCE8(pPlayer);
                    fn_800EBBFC(0, 0xCC36B742, 0, 0);
                }
            }
        }
    }

    UnidentifiedMegaStrikeEvent event;
    event.pFielder = this;
    event.fMeterValue = mUnidentified3C0;
    nlVector3 v3Column;
    fn_802CE7F4(fn_8027267C(8), &m_v3Position, &v3Column);
    event.v3Position = v3Column;
    fn_8005F630(lbl_806E0C94, &event);

    if (nParam != 0)
    {
        if (fn_80123314(lbl_806E10EC)
            && lbl_806E0C94->mUnidentified134.mSize > 0)
        {
            lbl_806E0C94->fn_80058180();
        }

        if (!lbl_806E2164->mUnidentified004)
        {
            lbl_806E0C94->fn_80059DEC(m_pTeam->m_nSide, m_ID,
                mUnidentified3BC, mUnidentified3C0);
        }

        mUnidentified478 = 1;
    }

    fn_800EC12C(0xBF541A4C, this);

    if (!fn_80332770())
    {
        fn_80111D7C(lbl_806DB978);
    }
}

extern "C" float fn_800499EC(cFielder* pFielder, int nParam)
{
    float fResult = pFielder->fn_800489C4();
    float fMeterMax = pFielder->fn_80048A08();
    float fMeterRange = fMeterMax - pFielder->fn_800489C4();

    float fShooting = fn_8003E6E4(pFielder)->fShooting;
    if (fShooting > 1.0f)
    {
        fShooting = 1.0f;
    }
    else if (fShooting < 0.0f)
    {
        fShooting = 0.0f;
    }

    float fSegmentA
        = InterpolateClamped(lbl_806DB95C, lbl_806DB958, fShooting);
    float fSegmentB
        = InterpolateClamped(lbl_806DB960, lbl_806DB964, fShooting);
    float fSegmentC
        = InterpolateClamped(lbl_806DB968, lbl_806DB96C, fShooting);
    float fSegmentD
        = InterpolateClamped(lbl_806E0C68, lbl_806E0C6C, fShooting);

    float fHalfA = fSegmentA / 2.0f;
    float fHalfB = fSegmentB / 2.0f;
    float fHalfC = fSegmentC / 2.0f;
    float fHalfD = fSegmentD / 2.0f;

    float fTime = pFielder->mUnidentified39C;
    if (nParam != 0)
    {
        fTime = pFielder->mUnidentified3A8;
    }

    if (fTime >= pFielder->mUnidentified3C4 - fHalfA)
    {
        if (fTime < pFielder->mUnidentified3C4 + fHalfA)
        {
            fResult = lbl_806E3578 * fMeterRange + fResult;
        }
        else if (fTime < pFielder->mUnidentified3C8 + fHalfB)
        {
            fResult = lbl_806E35EC * fMeterRange + fResult;
        }
        else if (fTime < pFielder->mUnidentified3CC + fHalfC)
        {
            fResult = pFielder->fn_80048A08();
        }
        else if (fTime < pFielder->mUnidentified3D0 + fHalfD)
        {
            fResult = pFielder->fn_80048A08();
        }
    }

    float fBias;
    if (fResult < 0.0f)
    {
        fBias = lbl_806E3608;
    }
    else
    {
        fBias = 0.5f;
    }
    return (float)(s32)(fResult + fBias);
}

extern "C" float fn_80049CC0(cFielder* pFielder, int nParam)
{
    float fNeedle = lbl_806E3604;
    if (pFielder->mUnidentified3B8)
    {
        float fTime = pFielder->mUnidentified3A0;
        if (nParam != 0)
        {
            fTime = pFielder->mUnidentified3A8;
        }
        float fDelta = fabsf(lbl_806DB93C - fTime);
        float fHalfWidth = fabsf(pFielder->mUnidentified3A4 / 2.0f);
        if (fDelta
            < InterpolateClamped(lbl_806DB940, lbl_806DB944,
                  fn_8003E6E4(pFielder)->fShooting)
                / 2.0f)
        {
            fDelta = 0.0f;
        }
        fNeedle = InterpolateRangeClamped(
            -1.0f, 1.0f, lbl_806E3570 * fHalfWidth, 0.0f, fDelta);
    }
    return fNeedle;
}

void cFielder::InitActionOneTimer(int animID, nlVector3& targetPos,
    float fAdjustEndTime, bool bIsChipShot, s16 nTurnAdjust)
{
    mActionShotVars.bIsChipShot = bIsChipShot;
    SetAction(ACTION_ONETIMER);
    mUnidentified368 = fAdjustEndTime;
    SetAnimState(animID, false, fAdjustEndTime * lbl_806DB990, false, false);

    nlVector3 v3MoveAdjustment;
    nlVec3Sub(v3MoveAdjustment, targetPos, m_v3Position);
    InitMovementFromAnim(nTurnAdjust, v3MoveAdjustment,
        fAdjustEndTime * lbl_806DB990, false);

    fn_800978E8(this, 0);

    bool bUnidentified = fn_8001E168(this);
    unsigned long soundID = 0x1CEC5A02;
    if (bUnidentified)
    {
        soundID = 0xFDE0C69B;
    }
    fn_800EBBFC(mUnidentified318, soundID, 0, 0);
}

void cFielder::fn_80049EA0(float fDeltaT)
{
    bool bIsChipShot = false;
    if (mActionShotVars.bIsChipShot || fn_80035F34(this))
    {
        bIsChipShot = true;
    }
    mActionShotVars.bIsChipShot = bIsChipShot;

    if (m_pCurrentAnimController->TestTrigger(mUnidentified368))
    {
        m_pCurrentAnimController->m_fPlaybackSpeedScale = 1.0f;
        m_pPhysicsCharacter->m_CanCollideWithWall = true;
    }

    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::InitActionOneTouchPassFromVolley(cPlayer* pPlayer, bool bParam)
{
    bool bIsChipShot = false;
    if (mActionShotVars.bIsChipShot || fn_80035F34(this))
    {
        bIsChipShot = true;
    }
    mActionShotVars.bIsChipShot = bIsChipShot;

    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_ONETOUCH_PASS_FROM_VOLLEY);

    int LateOneTimerFromVolleyAnims[4] = {
        0x4A,
        0x4D,
        0x4C,
        0x4B,
    };

    s16 facingDelta = GetFacingDeltaToPosition(pPlayer->m_v3Position);
    int index = (u16)(facingDelta + 0x2000) >> 14;

    bool bMirror = false;
    if (index == 2)
    {
        bMirror = m_pCurrentAnimController->m_bMirror;
    }

    int nAnimID = LateOneTimerFromVolleyAnims[index];

    s16 nTurnAdjust = 0;
    switch (nAnimID)
    {
    case 0x4C:
        nTurnAdjust = -0x8000;
        break;
    case 0x4D:
        nTurnAdjust = -0x4000;
        break;
    case 0x4B:
        nTurnAdjust = 0x4000;
        break;
    }

    s16 facingDelta2 = GetFacingDeltaToPosition(pPlayer->m_v3Position);

    SetAnimState(nAnimID, true, 0.0f, false, bMirror);

    InitMovementFromAnim(
        (s16)(nTurnAdjust + facingDelta2), v3Zero, lbl_806E3540, false);

    fn_80097858(this, pPlayer, bParam, true, true, bParam,
        fn_8002CFC4(fn_8003E6E4(this)), fn_8002C730(fn_8003E6E4(this)));

    mUnidentified371 = true;
}

void cFielder::ActionOneTouchPassFromVolley(float fDeltaT)
{
    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

bool cFielder::fn_8004A330(cFielder* pOther)
{
    if (!fn_8003C180(pOther))
    {
        float fOtherScore = fn_800DBAB0(pOther);

        bool bResult = false;
        float fOtherOpen = fn_800A0508(pOther, 0, 0);
        float fThisOpen = fn_800A0508(this, 0, 0);

        nlVector2 v2Delta = {
            pOther->m_v3Position.x - m_v3Position.x,
            pOther->m_v3Position.y - m_v3Position.y,
        };
        if (nlVec2LengthSquared(v2Delta) > lbl_806E360C
            && fOtherScore > lbl_806DB890 && fOtherOpen > fThisOpen)
        {
            bResult = true;
        }
        return bResult;
    }
    return false;
}

bool cFielder::InitActionPass(
    cPlayer* pPassTarget, bool bVolleyPass, int nParam, bool bParam)
{
    if (pPassTarget == 0)
    {
        return false;
    }

    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_PASS);

    s16 facingDelta = GetFacingDeltaToPosition(pPassTarget->m_v3Position);
    int index = (u16)(facingDelta + 0x2000) >> 14;

    if (m_eCharacterClass == HAMMERBROS)
    {
        SetAnimState(gPassAnims[index], false, lbl_806E35CC, false, false);
    }
    else
    {
        SetAnimState(gPassAnims[index], true, 0.2f, false, false);
    }

    InitMovementCoast();

    if (bVolleyPass)
    {
        nlVector3 delta;
        nlVec3Sub(delta, m_v3Position, pPassTarget->m_v3Position);
        float minDistSq = lbl_806E35C8;
        minDistSq *= minDistSq;
        float distSq = delta.GetLengthSq3D();

        if (distSq < minDistSq)
        {
            bVolleyPass = false;
        }
    }

    mActionShotVars.bIsChipShot = bVolleyPass;
    mUnidentified36C = pPassTarget;
    mUnidentified370 = nParam == 0;
    mUnidentified371 = bParam;
    return true;
}

void cFielder::ActionPass(float fDeltaT)
{
    if (m_pBall != 0 && m_pCurrentAnimController->TestFrameTrigger(1.0f))
    {
        float fA = fn_8002CFC4(fn_8003E6E4(this));
        float fB = fn_8002C730(fn_8003E6E4(this));
        if (!mActionShotVars.bIsChipShot)
        {
            fA = fn_8002C6E8(fn_8003E6E4(this));
            fB = fn_8002C678(fn_8003E6E4(this));
        }
        fn_80097858(this, mUnidentified36C, mActionShotVars.bIsChipShot,
            mUnidentified370, false, false, fA, fB);
    }

    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::InitActionSlideAttackReact(cPlayer* pAttacker, bool bSkipEvent)
{
    if (!IsFallenDown())
    {
        fn_8002E3F8(this);

        bool bHadBall = false;
        if (m_pBall != 0)
        {
            bHadBall = true;
            ReleaseBall(0);
        }

        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction(ACTION_SLIDE_ATTACK_REACT);

        s16 facingDelta;
        if (pAttacker != this)
        {
            facingDelta = GetFacingDeltaToPosition(pAttacker->m_v3Position);
        }
        else
        {
            facingDelta = (s16)(int)m_aActualFacingDirection;
        }

        static int SlideAttackReactAnims[4] = {
            0x5F,
            0x62,
            0x61,
            0x60,
        };

        SetAnimState(SlideAttackReactAnims[(u16)(facingDelta + 0x2000) >> 14],
            true, 0.2f, false, false);

        InitMovementFromAnim(0, v3Zero, 1.0f, false);

        fn_80139D1C(2, GetGlobalPad());

        if (pAttacker->m_eClassType == FIELDER && !bSkipEvent
            && pAttacker != this && bHadBall)
        {
            PlayerAttackData* pNode;
            lbl_80571960.Allocate(pNode);
            pNode->pAttacker = pAttacker;
            bool bHasPad = pAttacker->GetGlobalPad() != 0;
            pNode->nAttackerPadID
                = bHasPad ? pAttacker->GetGlobalPad()->fn_80332748() : -1;
            pNode->pTarget = 0;
            pNode->mUnidentified10 = true;
            fn_8005ED64(lbl_806E0C94, pNode);

            if (pAttacker->m_pBall != 0
                && GetStadiumUnknown0x10(GameInfoManager::Instance()->GetStadium()))
            {
                unsigned long soundID = 0xDCA472D;
                if (pAttacker->m_pTeam->m_nSide == 0)
                {
                    soundID = 0x902DA0E4;
                }
                fn_800ED92C(soundID);
            }
        }

        fn_80139D1C(1, pAttacker->GetGlobalPad());
        fn_800EBBFC(0, 0x57208DA, 0, 0);
        m_fDesiredSpeed = 0.0f;
    }
}

void cFielder::asmRunningWB(float fDeltaT)
{
    float fIdleToRunWBDesiredSpeed = 0.1f + fn_8002CE14(fn_8003E6E4(this));
    s16 nAbsActualToDesiredFacingDirection = (s16)(u16)abs_s16(
        (s16)(m_aDesiredFacingDirection - m_aActualFacingDirection));
    float fSpeedFactor = InterpolateRangeClamped(
        0.96f, 0.6f, 0.0f, 0.5f, fn_8003E6E4(this)->mUnidentified034);
    bool bFirstTime;

    do
    {
        bFirstTime = false;

        switch (m_eAnimID)
        {
        default:
        {
            if (mUnidentified385)
            {
                bool bAnimFinished = m_pCurrentAnimController->m_ePlayMode == PM_HOLD
                    && m_pCurrentAnimController->m_fTime == 1.0f;
                if (bAnimFinished
                    || m_fDesiredSpeed >= fIdleToRunWBDesiredSpeed)
                {
                    mUnidentified385 = false;
                }
            }

            if (mUnidentified385)
            {
                break;
            }

            fn_8003B920(this);
            bFirstTime = false;
            break;
        }

        case 0x10:
        case 0x11:
        case 0x12:
        {
            if (ShouldStartCrossBlend(0x17))
            {
                if (m_fDesiredSpeed <= fn_8002CE14(fn_8003E6E4(this)))
                {
                    fn_8003B920(this);
                }
                else
                {
                    m_fActualSpeed = fn_8002BFB8(fn_8003E6E4(this));
                    fn_8003BE14(this, 0.1f);
                }
            }
            break;
        }

        case 0x17:
        {
            m_aActualMovementDirection = m_aActualFacingDirection;

            if (ShouldStartCrossBlend(0xF))
            {
                fn_8003B920(this);
                m_fActualSpeed = 0.0f;
                break;
            }

            if (nAbsActualToDesiredFacingDirection >= 0x639C)
            {
                if (m_fActualSpeed
                    < fSpeedFactor * fn_8002C328(fn_8003E6E4(this)))
                {
                    fn_8003A5C8(this);
                }
                else
                {
                    fn_8003ADAC(this);
                }
                break;
            }

            if (m_fDesiredSpeed > fn_8002CE14(fn_8003E6E4(this)))
            {
                if (m_fActualSpeed
                    < fSpeedFactor * fn_8002C328(fn_8003E6E4(this)))
                {
                    fn_8003A5C8(this);
                }
            }
            break;
        }

        case 0xF:
        {
            if (m_fDesiredSpeed > 1.0f)
            {
                if (m_fActualSpeed
                    < fSpeedFactor * fn_8002C328(fn_8003E6E4(this)))
                {
                    fn_8003A5C8(this);
                }
                else
                {
                    fn_8003BE14(this, 0.1f);
                }
            }
            else if (m_fActualSpeed
                > 0.6f * fn_8002C328(fn_8003E6E4(this)))
            {
                fn_8003B54C(this);
            }
            else
            {
                m_fDesiredSpeed = 0.0f;
            }
            break;
        }

        case 9:
        {
            if (fn_8003E8A0(this) && mUnidentified3DC)
            {
                if (m_fDesiredSpeed
                    < fn_8002CE14(fn_8003E6E4(this)) - 0.15f)
                {
                    if (m_fActualSpeed
                        > 0.6f * fn_8002BFB8(fn_8003E6E4(this)))
                    {
                        fn_8003B54C(this);
                    }
                    else
                    {
                        fn_8003B920(this);
                    }
                }
                return;
            }

            if (fn_8003E9F0(this) && mUnidentified3DC)
            {
                return;
            }
        }

        case 0x14:
        {
            if (nAbsActualToDesiredFacingDirection >= 0x639C)
            {
                if (m_fActualSpeed
                    > 0.6f * fn_8002C328(fn_8003E6E4(this)))
                {
                    fn_8003ADAC(this);
                }
                else
                {
                    fn_8003A5C8(this);
                }
                break;
            }

            if (m_fDesiredSpeed < fn_8002CE14(fn_8003E6E4(this)) - 0.15f)
            {
                if (m_fActualSpeed
                    > 0.6f * fn_8002C328(fn_8003E6E4(this)))
                {
                    fn_8003B54C(this);
                }
                else
                {
                    fn_8003B920(this);
                }
                break;
            }

            if ((m_eAnimID != 9 && fn_8003E74C(this))
                || (m_eAnimID == 9 && !fn_8003E74C(this)))
            {
                fn_8003BE14(this, 0.1f);
            }
            break;
        }

        case 0x18:
        {
            bool bAnimFinished = m_pCurrentAnimController->m_ePlayMode == PM_HOLD
                && m_pCurrentAnimController->m_fTime == 1.0f;

            if (bAnimFinished)
            {
                if (m_fDesiredSpeed < fn_8002CE14(fn_8003E6E4(this)))
                {
                    if (mUnidentified384)
                    {
                        fn_8004B148();
                    }
                    else
                    {
                        fn_8003B020(this);
                    }
                }
                else
                {
                    fn_8003B0D8(this);
                }
            }
            break;
        }

        case 0x19:
        {
            if (ShouldStartCrossBlend(0xF))
            {
                if (mUnidentified384)
                {
                    fn_8004B148();
                }
                else if (m_fDesiredSpeed
                    >= fn_8002CE14(fn_8003E6E4(this)))
                {
                    m_fActualSpeed = fn_8002BFB8(fn_8003E6E4(this));
                    fn_8003BE14(this, 0.1f);
                }
                else
                {
                    fn_8003B54C(this);
                }
            }
            break;
        }

        case 0x1A:
        {
            if (ShouldStartCrossBlend(0x14))
            {
                fn_8003B920(this);
                m_fActualSpeed = 0.0f;
            }
            break;
        }
        }
    } while (bFirstTime);
}

void cFielder::asmRunning()
{
    fn_8002CE14(fn_8003E6E4(this));

    s16 nAbsActualToDesiredFacingDirection = (s16)(u16)abs_s16(
        (s16)(m_aDesiredFacingDirection - m_aActualFacingDirection));
    s16 nAbsActualToDesiredMovementDirection = (s16)(u16)abs_s16(
        (s16)(m_aDesiredMovementDirection - m_aActualMovementDirection));
    float fSpeedFactor = InterpolateRangeClamped(
        0.96f, 0.6f, 0.0f, 0.5f, fn_8003E6E4(this)->mUnidentified034);
    bool bFirstTime;

    do
    {
        bFirstTime = false;

        switch (m_eAnimID)
        {
        default:
        {
            fn_8003B854(this);
            bFirstTime = false;
            break;
        }

        case 1:
        case 2:
        case 3:
        {
            if (ShouldStartCrossBlend(0))
            {
                if (m_fDesiredSpeed <= fn_8002CE14(fn_8003E6E4(this)))
                {
                    fn_8003B854(this);
                }
                else
                {
                    m_fActualSpeed = fn_8002BFB8(fn_8003E6E4(this));
                    fn_8003BA94(this, 0.1f);
                }
            }
            break;
        }

        case 0x1B:
        {
            switch (mUnidentified37C)
            {
            case 1:
                fn_8003B790(this);
                break;
            case 2:
                fn_8003B6CC(this);
                break;
            case 0:
            case 3:
                if (m_fActualSpeed
                    > 0.6f * fn_8002BFB8(fn_8003E6E4(this)))
                {
                    if (nAbsActualToDesiredFacingDirection >= 0x3A98)
                    {
                        fn_8003B2EC(this);
                    }
                    else
                    {
                        fn_8003B384(this);
                    }
                }
                else
                {
                    fn_8003B854(this);
                }
                break;
            case 4:
                m_fDesiredSpeed = fn_8002CD2C(fn_8003E6E4(this));
                break;
            }
            break;
        }

        case 0x22:
        {
            if (ShouldStartCrossBlend(4))
            {
                fn_8003BA94(this, 0.1f);
            }
            break;
        }

        case 0x21:
        {
            if (ShouldStartCrossBlend(0x1B))
            {
                fn_8003B190(this);
            }
            break;
        }

        case 0x1E:
        {
            bool bAnimFinished = m_pCurrentAnimController->m_ePlayMode == PM_HOLD
                && m_pCurrentAnimController->m_fTime == 1.0f;

            if (bAnimFinished)
            {
                switch (mUnidentified37C)
                {
                case 0:
                case 1:
                case 2:
                case 4:
                    fn_8003B4B4(this);
                    break;
                case 3:
                {
                    int nIndex = (u16)(m_aDesiredFacingDirection
                                     - m_aActualFacingDirection + 0x2000)
                        >> 14;
                    if (nIndex != 0)
                    {
                        fn_8003A2D0(this, nIndex);
                    }
                    else
                    {
                        fn_8003B41C(this);
                    }
                    break;
                }
                }
            }
            break;
        }

        case 0x1F:
        {
            if (ShouldStartCrossBlend(4))
            {
                switch (mUnidentified37C)
                {
                case 1:
                case 2:
                case 4:
                    fn_8003B54C(this);
                    break;
                case 0:
                    fn_8003B854(this);
                    break;
                case 3:
                    fn_8003BA94(this, 0.1f);
                    break;
                }
            }
            break;
        }

        case 0x20:
        {
            if (ShouldStartCrossBlend(4))
            {
                switch (mUnidentified37C)
                {
                case 4:
                    fn_8003B190(this);
                    break;
                case 1:
                    fn_8003B790(this);
                    break;
                case 2:
                    fn_8003B6CC(this);
                    break;
                case 0:
                    fn_8003B854(this);
                    break;
                case 3:
                    fn_8003A2D0(this, -1);
                    break;
                }
            }
            break;
        }

        case 0x1C:
        {
            switch (mUnidentified37C)
            {
            case 0:
            case 1:
                if (m_fActualSpeed
                    > 0.6f * fn_8002BFB8(fn_8003E6E4(this)))
                {
                    fn_8003B664(this);
                }
                else
                {
                    fn_8003B854(this);
                }
                break;
            case 2:
                m_fDesiredSpeed = fn_8002CC44(fn_8003E6E4(this));
                break;
            case 3:
                fn_8003BA94(this, 0.1f);
                break;
            case 4:
                fn_8003B190(this);
                break;
            }
            break;
        }

        case 0x1D:
        {
            switch (mUnidentified37C)
            {
            case 0:
            case 2:
                if (m_fActualSpeed
                    > 0.6f * fn_8002BFB8(fn_8003E6E4(this)))
                {
                    fn_8003B5FC(this);
                }
                else
                {
                    fn_8003B854(this);
                }
                break;
            case 1:
                m_fDesiredSpeed = fn_8002CC44(fn_8003E6E4(this));
                break;
            case 3:
                fn_8003BA94(this, 0.1f);
                break;
            case 4:
                fn_8003B190(this);
                break;
            }
            break;
        }

        case 5:
        {
            m_aActualMovementDirection = m_aActualFacingDirection;

            switch (mUnidentified37C)
            {
            case 0:
                if (ShouldStartCrossBlend(0))
                {
                    fn_8003B854(this);
                }
                break;
            case 1:
                if (ShouldStartCrossBlend(0x1D))
                {
                    fn_8003B790(this);
                }
                break;
            case 2:
                if (ShouldStartCrossBlend(0x1C))
                {
                    fn_8003B6CC(this);
                }
                break;
            case 3:
                if (nAbsActualToDesiredFacingDirection >= 0x639C)
                {
                    if (m_fActualSpeed
                        < fSpeedFactor * fn_8002BFB8(fn_8003E6E4(this)))
                    {
                        fn_8003A2D0(this, -1);
                    }
                    else if (!m_tSwapFacingTimer.GetSeconds())
                    {
                        fn_8003ADAC(this);
                    }
                }
                else if (ShouldStartCrossBlend(0))
                {
                    fn_8003A2D0(this, -1);
                }
                break;
            case 4:
                if (ShouldStartCrossBlend(0))
                {
                    fn_8003B190(this);
                    m_fActualSpeed = 0.0f;
                }
                break;
            }
            break;
        }

        case 0x23:
        case 0x24:
        {
            if (ShouldStartCrossBlend(0))
            {
                fn_8003B854(this);
            }
            break;
        }

        case 0:
        {
            switch (mUnidentified37C)
            {
            case 0:
                m_fDesiredSpeed = 0.0f;
                if (m_fActualSpeed
                    > 0.6f * fn_8002BFB8(fn_8003E6E4(this)))
                {
                    fn_8003B54C(this);
                }
                break;
            case 1:
                fn_8003B790(this);
                break;
            case 2:
                fn_8003B6CC(this);
                break;
            case 3:
                if (m_fActualSpeed
                    < fSpeedFactor * fn_8002BFB8(fn_8003E6E4(this)))
                {
                    fn_8003A2D0(this, -1);
                }
                else
                {
                    fn_8003BA94(this, 0.1f);
                }
                break;
            case 4:
                fn_8003B190(this);
                break;
            }
            break;
        }

        case 9:
        {
            if (fn_8003E8A0(this) && mUnidentified3DC)
            {
                if (m_fDesiredSpeed
                    < fn_8002CE14(fn_8003E6E4(this)) - 0.15f)
                {
                    if (m_fActualSpeed
                        > 0.6f * fn_8002BFB8(fn_8003E6E4(this)))
                    {
                        fn_8003B54C(this);
                    }
                    else
                    {
                        fn_8003B854(this);
                    }
                }
                return;
            }

            if (fn_8003E9F0(this) && mUnidentified3DC)
            {
                return;
            }
        }

        case 4:
        {
            switch (mUnidentified37C)
            {
            case 0:
                if (m_fActualSpeed
                    > 0.6f * fn_8002BFB8(fn_8003E6E4(this)))
                {
                    fn_8003B54C(this);
                }
                else
                {
                    fn_8003B854(this);
                }
                break;
            case 3:
                if (nAbsActualToDesiredFacingDirection >= 0x639C)
                {
                    if (m_fActualSpeed
                        < fSpeedFactor * fn_8002BFB8(fn_8003E6E4(this)))
                    {
                        fn_8003A2D0(this, -1);
                    }
                    else if (!m_tSwapFacingTimer.GetSeconds())
                    {
                        fn_8003ADAC(this);
                    }
                }
                else if ((m_eAnimID != 9 && fn_8003E74C(this))
                    || (m_eAnimID == 9 && !fn_8003E74C(this)))
                {
                    fn_8003BA94(this, 0.1f);
                }
                break;
            case 1:
                fn_8003B790(this);
                break;
            case 2:
                fn_8003B6CC(this);
                break;
            case 4:
                if (m_fDesiredSpeed > fn_8002CE14(fn_8003E6E4(this)))
                {
                    if (nAbsActualToDesiredMovementDirection < 0x4000)
                    {
                        fn_8003B254(this);
                    }
                    else
                    {
                        fn_8003B54C(this);
                    }
                }
                else
                {
                    fn_8003B54C(this);
                }
                break;
            }
            break;
        }

        case 0xC:
        {
            bool bAnimFinished = m_pCurrentAnimController->m_ePlayMode == PM_HOLD
                && m_pCurrentAnimController->m_fTime == 1.0f;

            if (bAnimFinished)
            {
                if (m_fDesiredSpeed > fn_8002CE14(fn_8003E6E4(this)))
                {
                    fn_8003B0D8(this);
                }
                else
                {
                    fn_8003B020(this);
                }
            }
            break;
        }

        case 0xD:
        {
            if (ShouldStartCrossBlend(0))
            {
                if (m_fDesiredSpeed >= fn_8002CE14(fn_8003E6E4(this)))
                {
                    m_fActualSpeed = fn_8002BFB8(fn_8003E6E4(this));
                    fn_8003BA94(this, 0.1f);
                }
                else
                {
                    fn_8003B54C(this);
                }
            }
            break;
        }

        case 0xE:
        {
            if (ShouldStartCrossBlend(4))
            {
                fn_8003B854(this);
            }
            break;
        }
        }
    } while (bFirstTime);
}

bool cFielder::fn_800447C0(unsigned short aDirection)
{
    if (mUnidentified444 != 0)
    {
        return false;
    }

    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction((eFielderActionState)1);
    mUnidentified338 = 0;
    mUnidentified33A = false;
    mUnidentified33C = 2;

    if (fn_8003E99C(this))
    {
        if (mUnidentified3DC)
        {
            fn_8005001C(false);
        }
    }
    else if (fn_8003E9F0(this))
    {
        if (mUnidentified3DC)
        {
            fn_8005001C(false);
        }
    }
    else if (m_eCharacterClass == (eCharacterClass)0x13)
    {
        fn_801B8FF4(this);
    }

    s16 sFacingDelta;
    switch (m_eCharacterClass)
    {
    case (eCharacterClass)0x01:
    case (eCharacterClass)0x03:
    case (eCharacterClass)0x07:
    case (eCharacterClass)0x09:
    case (eCharacterClass)0x0B:
    case (eCharacterClass)0x0C:
    case (eCharacterClass)0x12:
        sFacingDelta = aDirection - m_aActualFacingDirection;
        SetAnimState(0x50, true, 0.2f, false, false);
        break;
    case (eCharacterClass)0x02:
    case (eCharacterClass)0x06:
    case (eCharacterClass)0x11:
        if (m_pController != 0
            && m_pController->GetMovementStickMagnitude() > 0.001f)
        {
            aDirection = m_pController->GetMovementStickDirection();
        }
        sFacingDelta = aDirection - m_aActualFacingDirection;
        SetAnimState(0x50, true, 0.2f, false, false);
        break;
    case (eCharacterClass)0x05:
    case (eCharacterClass)0x0A:
    case (eCharacterClass)0x0D:
    case (eCharacterClass)0x0F:
    case (eCharacterClass)0x10:
        sFacingDelta = aDirection - m_aActualFacingDirection;
        SetAnimState(0x50, true, 0.2f, false, false);
        break;
    default:
    {
        sFacingDelta = aDirection - m_aActualFacingDirection;
        if ((u16)abs_s16(sFacingDelta) < 0x2AAA)
        {
            SetAnimState(0x50, true, 0.2f, false, false);
        }
        else if ((u16)abs_s16(sFacingDelta) < 0x6AAA)
        {
            if (sFacingDelta < 0)
            {
                SetAnimState(0x4F, true, 0.2f, false, false);
                SetFacingDirection(aDirection + 0x4000, true);
            }
            else
            {
                SetAnimState(0x4E, true, 0.2f, false, false);
                SetFacingDirection(aDirection - 0x4000, true);
            }
            sFacingDelta = 0;
        }
        else
        {
            SetAnimState(0x51, true, 0.2f, false, false);
            SetFacingDirection(aDirection - 0x8000, true);
            sFacingDelta = 0;
        }
        break;
    }
    }

    if (fn_8003E70C(this))
    {
        Unknown8(m_aActualFacingDirection, false);
        SetFacingDirection(m_aDesiredFacingDirection, true);
    }

    m_fDesiredSpeed = 0.0f;
    InitMovementFromAnim(sFacingDelta, v3Zero, 0.1f, false);

    switch (m_eCharacterClass)
    {
    case (eCharacterClass)0x00:
    case (eCharacterClass)0x04:
    case (eCharacterClass)0x08:
    case (eCharacterClass)0x0E:
    case (eCharacterClass)0x13:
        m_pCurrentAnimController->m_fPlaybackSpeedScale
            = InterpolateRangeClamped(lbl_806DB988, lbl_806DB98C, 0.35f,
                0.25f, fn_800A9274(lbl_806E0C94->mUnidentified10D8));
        break;
    case (eCharacterClass)0x01:
    case (eCharacterClass)0x05:
    case (eCharacterClass)0x07:
    case (eCharacterClass)0x09:
    case (eCharacterClass)0x0D:
        SetFacingDirection(sFacingDelta + m_aActualFacingDirection, true);
        InitMovementDecelerateExponential(lbl_806DB8F4);
        break;
    }

    bool bUnidentified = fn_8001E168(this);
    unsigned long soundID = 0xDEA5F49B;
    if (bUnidentified)
    {
        soundID = 0xA91D4914;
    }
    fn_800EBBFC(mUnidentified318, soundID, 0, 0);

    bool bUnidentified2 = lbl_806E0C94->m_eGameState == 5
        || lbl_806E0C94->m_eGameState == 6;
    if (bUnidentified2)
    {
        StatsTracker::Instance()->TrackStat(
            (ePlayerStats)0x15, m_pTeam->m_nSide, m_ID, 0, 0, 0, 0);
    }

    return true;
}

void cFielder::fn_80044290(float fDeltaT)
{
    switch (m_eAnimID)
    {
    case 0x7C:
    {
        if (m_v3Velocity.z < 0.0f && m_v3Position.z <= 0.1f)
        {
            nlVector3 v3Position = m_v3Position;
            float fGoalLineX = (float)fabs(cField::GetGoalLineX(0U));
            float fSidelineY
                = (float)fabs(0.5f * (2.0f * cField::mv3FieldPosition.y));

            if (GameInfoManager::Instance()->GetStadium() == 0x0B
                && (((float)fabs(v3Position.x) - fGoalLineX > 0.0f
                        && (float)fabs(v3Position.x) - (5.0f + fGoalLineX)
                            < 0.0f)
                    || ((float)fabs(v3Position.y) - fSidelineY > 0.0f
                        && (float)fabs(v3Position.y) - (1.0f + fSidelineY)
                            < 0.0f)))
            {
                nlVector3 v3Velocity = m_v3Velocity;
                float fUpVelocity = v3Velocity.z;
                v3Velocity.z = 0.0f;
                float fSpeed = nlSqrt(v3Velocity.GetLengthSq3D(), true);
                if (fSpeed < 2.0f)
                {
                    if (fSpeed < 0.01f)
                    {
                        v3Velocity = m_v3Position;
                    }
                    float fRecipLength
                        = nlRecipSqrt(v3Velocity.GetLengthSq3D(), true);
                    nlVec3Scale(v3Velocity, v3Velocity, fRecipLength);
                    nlVec3Scale(v3Velocity, v3Velocity, 5.0f);
                }
                v3Velocity.z = (float)(-1.0 * fUpVelocity);
                if (v3Velocity.z < 1.0f)
                {
                    v3Velocity.z = -15.0f;
                }
                SetVelocity(v3Velocity);
            }
            else if ((float)fabs(v3Position.x) - (5.0f + fGoalLineX) > 0.0f
                     || (float)fabs(v3Position.y) - (1.0f + fSidelineY)
                         > 0.0f)
            {
                fn_80046244();
            }
            else
            {
                v3Position.z = 0.0f;
                SetPosition(v3Position);
                SetVelocity(v3Zero);
                SetAnimState(0x7D, true, 0.2f, false, false);
                InitMovementFromAnim(0, v3Zero, 1.0f, false);
            }
        }
        else
        {
            nlVector3 v3Velocity = m_v3Velocity;
            float fDamping = 1.0f - 0.5f * fDeltaT;
            v3Velocity.x *= fDamping;
            v3Velocity.y *= fDamping;
            v3Velocity.z = -30.0f * fDeltaT + v3Velocity.z;
            SetVelocity(v3Velocity);
            m_v3Position.z += v3Velocity.z * fDeltaT;

            Goalie* pGoalie = m_pTeam->GetOtherTeam()->GetGoalie();
            if (pGoalie->mGoalieActionState == (eGoalieActionState)0x0D
                && pGoalie->mpTarget == this)
            {
                fn_80080BFC(pGoalie, fDeltaT);
            }

            float fSpin = 1.0f
                - fn_8003E6E4(this)->mUnidentified064;
            Unknown8(m_aActualFacingDirection
                    + (u16)(s32)(5000.0f * (2.0f * fSpin + 1.0f)),
                false);

            SetFacingDirection(
                SeekDirection(m_aActualFacingDirection,
                    m_aDesiredFacingDirection,
                    fn_8002CF88(fn_8003E6E4(this)),
                    fn_8002CF9C(fn_8003E6E4(this)), fDeltaT),
                true);
        }
        break;
    }
    case 0x7D:
        if (ShouldStartCrossBlend(4))
        {
            EndAction();
        }
        break;
    }
}

void cFielder::fn_80044BEC(float fDeltaT)
{
    if (m_eMovementState == MOVEMENT_DECELERATE_EXPONENTIAL)
    {
        m_fDesiredSpeed = 0.0f;
    }

    if (GetGlobalPad() != 0)
    {
        if (m_pCurrentAnimController->m_fTime > lbl_806DB8F0)
        {
            mUnidentified33A = !fn_80036F88(this);
        }
        else
        {
            mUnidentified33A = false;
        }

        if (m_pBall != 0)
        {
            fn_8003D8A4(this, fDeltaT);
        }
    }
    else
    {
        switch (m_eCharacterClass)
        {
        case (eCharacterClass)0x05:
        case (eCharacterClass)0x0A:
        case (eCharacterClass)0x0F:
        {
            float fChance = fn_800E02B8(m_pTeam);
            if (nlRandomf(2.0f) < fChance)
            {
                if (m_pCurrentAnimController->TestFrameTrigger(5.0f))
                {
                    m_eLastPadAction = 0x1B;
                }
            }
            break;
        }
        }
    }

    if (ShouldStartCrossBlend(0x52))
    {
        EndAction();

        if (m_pBall == 0)
        {
            m_eLastPadAction = 0x32;
        }

        if (GetGlobalPad() != 0)
        {
            if (fn_8003D9BC(this))
            {
                m_eLastPadAction = 0x32;
            }
        }
    }
}

void cFielder::InitActionElectrocution(const nlVector3& wallPosition,
    const nlVector3& wallNormal, bool bParam)
{
    if (m_eActionState == ACTION_ELECTROCUTION)
    {
        return;
    }
    if (fn_800387CC(this) != true)
    {

        fn_8002E3F8(this);
        fn_8009750C();

        float fElectrocutionTime = 1.0f - m_pCurrentAnimController->m_fTime;
        fElectrocutionTime *= m_pCurrentAnimController->m_pSAnim->GetDuration();
        fElectrocutionTime += lbl_806DB920;

        InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction(ACTION_ELECTROCUTION);

        if (m_pBall != 0)
        {
            ReleaseBall(0);

            nlVector3 v3BallVelocity;
            nlVec3Set(v3BallVelocity, 0.4f * m_v3Velocity.x,
                0.5f * -m_v3Position.y, 6.0f);
            g_pBall->ShootRelease(v3BallVelocity, SPINTYPE_NONE);
            SetNoPickUpTime(0.5f);
        }

        SetFacingDirection(nlVector3ToAngle(wallNormal, 0x8000), true);
        SetAnimState(0x76, true, 0.2f, false, false);
        InitMovementNone(0.0f, 0.0f);

        nlVector3 jointPos = GetJointPosition(m_nBip01JointIndex_0xA4);

        nlVector3 futureJointPos;
        GetJointPositionFuture(&futureJointPos, 0, m_nBip01JointIndex_0xA4,
            0.0f, false, false, false, true);

        futureJointPos.z += 0.25f;
        jointPos.z = (jointPos.z >= futureJointPos.z) ? jointPos.z
                                                      : futureJointPos.z;

        float fNetWidth = cNet::m_fNetWidth;
        if ((float)fabs(jointPos.y) < 0.5f * fNetWidth)
        {
            float fAdjust = mUnidentified0A0;
            float fMaxY
                = fn_8002BFA8(fn_8003E6E4(this), fAdjust) + 0.5f * fNetWidth;
            float fMinY = -fMaxY;
            float fY = jointPos.y;
            fY = (fY >= fMinY) ? fY : fMinY;
            fY = (fY <= fMaxY) ? fY : fMaxY;
            jointPos.y = fY;
        }

        SetPosition(jointPos);

        mUnidentified340 = fElectrocutionTime;
        mUnidentified348 = false;
        if (fElectrocutionTime < 0.3f)
        {
            mUnidentified340 = 0.3f;
        }

        if (bParam)
        {
            nlVector3 v3EffectPos;
            v3EffectPos.x = wallPosition.x;
            v3EffectPos.y = wallPosition.y;
            v3EffectPos.z = jointPos.z;
            fn_801B94EC(this, &v3EffectPos, &wallNormal);
        }
        else
        {
            mUnidentified348 = true;
            fn_801B968C(this);
        }

        bool bUnidentified = fn_8001E168(this);
        unsigned long soundID = 0xBADF0EF9;
        if (bUnidentified)
        {
            soundID = 0x1602CA52;
        }
        fn_800EBBFC(mUnidentified318, soundID, 0, 0);
    }
}

void cFielder::fn_800451B0(const nlVector3& v3Position)
{
    if (m_eActionState == ACTION_ELECTROCUTION)
    {
        return;
    }
    if (fn_800387CC(this) != true)
    {

        if (m_pBall != 0)
        {
            ReleaseBall(0);

            float fSpread = 4.0f;
            nlVector3 v3BallVelocity;
            v3BallVelocity.x = nlRandomf(fSpread) - 0.5f * fSpread;
            v3BallVelocity.y = nlRandomf(fSpread) - 0.5f * fSpread;
            v3BallVelocity.z = nlRandomf(fSpread);
            g_pBall->ShootRelease(v3BallVelocity, SPINTYPE_NONE);
        }

        fn_8002E3F8(this);
        fn_8009750C();

        if (m_pBall == 0)
        {
            nlVector3 v3Direction;
            nlVec3Set(v3Direction,
                v3Position.x - m_v3Position.x,
                v3Position.y - m_v3Position.y,
                v3Position.z - m_v3Position.z);
            SetFacingDirection(nlVector3ToAngle(v3Direction), true);
        }

        InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction(ACTION_ELECTROCUTION);
        SetAnimState(0x79, true, 0.2f, false, false);
        InitMovementNone(0.0f, 0.0f);

        nlVector3 jointPos = GetJointPosition(m_nBip01JointIndex_0xA4);

        nlVector3 futureJointPos;
        GetJointPositionFuture(&futureJointPos, 0, m_nBip01JointIndex_0xA4,
            0.0f, false, false, false, true);

        futureJointPos.z += 0.05f;
        jointPos.z = (jointPos.z >= futureJointPos.z) ? jointPos.z
                                                      : futureJointPos.z;

        SetPosition(jointPos);

        mUnidentified340 = lbl_806DB994 + nlRandomf(lbl_806DB998);
        mUnidentified344 = lbl_806DB99C;
        mUnidentified348 = true;
        fn_801B968C(this);

        fn_80139D1C(4, GetGlobalPad());

        bool bUnidentified = fn_8001E168(this);
        unsigned long soundID = 0xBADF0EF9;
        if (bUnidentified)
        {
            soundID = 0x1602CA52;
        }
        fn_800EBBFC(mUnidentified318, soundID, 0, 0);
    }
}

void cFielder::fn_80043ADC()
{
    if (m_eActionState == (eFielderActionState)0
        || m_eActionState == (eFielderActionState)0x23
        || m_eActionState == ACTION_ELECTROCUTION)
    {
        return;
    }

    if (m_pBall != 0)
    {
        ReleaseBall(0);
        nlVector3 v3Velocity = v3LaunchUp;
        ShootBallDueToContact(v3Velocity);
    }

    mUnidentified330 = UnidentifiedFielderPair330(false, -1.0f);
    mUnidentified330.mUnidentified04 = 0.75f + nlRandomf(0.25f);

    nlVector3 v3Position = GetJointPosition(m_nBip01JointIndex_0xA4);
    SetPosition(v3Position);

    InitDesire(
        FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction((eFielderActionState)0);
    SetAnimState(0x7C, true, 0.2f, false, false);
    InitMovementCoast();
}

void cFielder::fn_80044148(const nlVector3& v3Velocity)
{
    if (m_eActionState == (eFielderActionState)0
        || m_eActionState == (eFielderActionState)0x23)
    {
        return;
    }
    if (m_pBall != 0)
    {
        ReleaseBall(0);
        g_pBall->SetVelocity(m_v3Velocity, SPINTYPE_NONE, 0);
    }

    InitDesire(
        FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction((eFielderActionState)0x23);
    SetAnimState(0x7C, false, 0.0333333f, false, false);
    SetVelocity(v3Velocity);
    InitMovementCoast();
    fn_8003C560(this, 1, 0);
    mUnidentified178 = 1.0f;

    if (GameInfoManager::Instance()->GetStadium() == 0x0B)
    {
        m_pPhysicsCharacter->m_CanCollideWithGoalLine = 0;
        m_pPhysicsCharacter->m_CanCollideWithWall = 0;
    }

    bool bUnidentified = fn_8001E168(this);
    unsigned long soundID = 0x1CF82176;
    if (bUnidentified)
    {
        soundID = 0xFDEC8E0F;
    }
    fn_800EBBFC(mUnidentified318, soundID, 0, 0);
}

void cFielder::ActionElectrocution(float dt)
{
    switch (m_eAnimID)
    {
    case 0x76:
    case 0x79:
    {
        mUnidentified340 -= dt;
        mUnidentified344 -= dt;

        if (m_eAnimID == 0x79 && mUnidentified344 > 0.0f)
        {
            nlVector3 v3Position = m_v3Position;
            float fShake
                = fn_8003E6E4(this)->mUnidentified064;
            float fRise = lbl_806DB9A0
                * ((1.0f - fShake) * nlRandomf(0.5f) + 0.5f);
            v3Position.z += fRise * dt;
            SetPosition(v3Position);
        }

        if (mUnidentified340 <= 0.0f)
        {
            if (m_eAnimID == 0x76)
            {
                SetAnimState(0x77, true, 0.2f, false, false);

                nlVector3 launchVelocity = v3ElectrocutionLaunch;
                float cosAngle;
                float sinAngle;
                nlSinCos(&sinAngle, &cosAngle, m_aActualFacingDirection);

                nlVector3 velocity;
                velocity.x = launchVelocity.x * cosAngle
                    - launchVelocity.y * sinAngle;
                velocity.y = launchVelocity.y * cosAngle
                    + launchVelocity.x * sinAngle;
                velocity.z = launchVelocity.z;
                SetVelocity(velocity);

                if (!mUnidentified348)
                {
                    fn_801B7E4C("electrocution_explosion", this);
                }
            }
            else
            {
                SetAnimState(0x7A, true, 0.2f, false, false);
            }

            fn_801B93E8(this);
            InitMovementCoast();
            fn_80139D1C(1, GetGlobalPad());
        }
        break;
    }
    case 0x77:
    case 0x7A:
    {
        nlVector3 velocity = m_v3Velocity;
        velocity.x *= 0.99f;
        velocity.y *= 0.99f;
        velocity.z = -30.0f * dt + velocity.z;
        SetVelocity(velocity);

        m_v3Position.z += dt * m_v3Velocity.z;
        if (m_v3Position.z < 0.0f)
        {
            m_v3Position.z = 0.0f;
            m_v3Velocity.z = 0.0f;

            if (m_eAnimID == 0x77)
            {
                SetAnimState(0x78, true, 0.2f, false, false);
            }
            else
            {
                SetAnimState(0x7B, true, 0.2f, false, false);
            }
            InitMovementFromAnim(0, v3Zero, 0.0f, false);
            fn_801B968C(this);
            fn_80139D1C(1, GetGlobalPad());
        }
        else
        {
            if (m_pCurrentAnimController->TestTrigger(lbl_806DB9A4))
            {
                fn_801B968C(this);
            }
            if (m_pCurrentAnimController->TestTrigger(lbl_806DB9A8))
            {
                fn_801B93E8(this);
            }
        }
        break;
    }
    case 0x78:
    case 0x7B:
    {
        if (m_pCurrentAnimController->TestTrigger(lbl_806DB9AC))
        {
            fn_801B93E8(this);
            fn_80139D1C(1, GetGlobalPad());
        }
        if (m_pCurrentAnimController->TestTrigger(lbl_806DB9B0))
        {
            fn_801B968C(this);
            fn_80139D1C(1, GetGlobalPad());
        }
        if (m_pCurrentAnimController->TestTrigger(lbl_806DB9B4))
        {
            fn_801B93E8(this);
            fn_80139D1C(1, GetGlobalPad());
        }

        if (ShouldStartCrossBlend(4))
        {
            EndAction();
        }
        break;
    }
    }
}

void cFielder::fn_80045930()
{
    m_v3Position.z = 0.0f;
    m_v3Velocity.z = 0.0f;

    nlVector3 v3Direction;
    if (!g_pBall->m_pPhysicsBall->mbUseWindForce)
    {
        UnidentifiedTornado806E0C94* pObject
            = fn_800AA060(lbl_806E0C94->mUnidentified10DC, 2);
        if (pObject != 0 && !pObject->mUnidentified0C)
        {
            pObject->UnidentifiedVirtual0C();
            v3Direction = g_pBall->m_pPhysicsBall->mv3WindForce;
        }
        else
        {
            nlVec3Set(v3Direction, 0.0f, 1.0f, 0.0f);
        }
    }
    else
    {
        v3Direction = g_pBall->m_pPhysicsBall->mv3WindForce;
    }

    nlVector3 v3Position = { 0.0f, 0.0f, 0.0f };
    v3Position.x = nlRandomf(4.0f) - 2.0f;
    v3Position.y = nlRandomf(4.0f) - 2.0f;

    float fRecipLength = nlRecipSqrt(v3Direction.GetLengthSq3D(), true);
    nlVec3Scale(v3Direction, v3Direction, fRecipLength);
    nlVec3Scale(v3Direction, v3Direction, 40.0f);
    nlVec3Sub(v3Position, v3Position, v3Direction);
    SetPosition(v3Position);

    SetAnimState(0x7E, true, 0.2f, false, false);
    InitMovementCoast();
}

void cFielder::fn_80045AEC(PhysicsObject* pObject)
{
    if (!fn_800344B0(this))
    {
        if (m_pBall != 0)
        {
            ReleaseBall(0);
            g_pBall->ShootRelease(m_v3Velocity, SPINTYPE_NONE);
        }

        fn_8002E718(this);
        fn_8002E798(this);
        fn_8002E3F8(this);
        fn_8002E39C(this);
        fn_8002E2E4(this);

        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction((eFielderActionState)0x18);
        SetAnimState(0x7C, false, 0.2f, false, false);
        InitMovementCoast();
        SetVelocity(v3Zero);

        mUnidentified34C = lbl_806DB90C;
        mUnidentified350 = pObject->GetPosition();

        fn_801BB5DC(this, 1);
        fn_801BB640(this, 1);
        fn_801BA034();

        bool bHasPad = GetGlobalPad() != 0;
        if (bHasPad)
        {
            fn_8009591C(this, false);
        }

        bool bUnidentified = fn_8001E168(this);
        unsigned long soundID = 0x1CF82176;
        if (bUnidentified)
        {
            soundID = 0xFDEC8E0F;
        }
        fn_800EBBFC(mUnidentified318, soundID, 0, 0);
    }
}

void cFielder::fn_80045C74(float fDeltaT)
{
    if (mUnidentified34C > 0.0f)
    {
        mUnidentified34C -= fDeltaT;

        int nSpinStep = (u16)(lbl_806DB918
            * (lbl_806DB914
                    * InterpolateRangeClamped(
                        0.0f, 1.0f, 0.0f, lbl_806DB910, m_v3Velocity.z)
                + 1.0f));
        Unknown8(m_aActualFacingDirection + nSpinStep, false);

        SetFacingDirection(
            SeekDirection(m_aActualFacingDirection,
                m_aDesiredFacingDirection, fn_8002CF88(fn_8003E6E4(this)),
                fn_8002CF9C(fn_8003E6E4(this)), fDeltaT),
            true);

        float fT = FMIN((float)fabs(mUnidentified34C - lbl_806DB90C)
                / (lbl_806DB90C * lbl_806DB908),
            1.0f);

        nlVector3 v3NewPosition;
        v3NewPosition.x
            = (1.0f - fT) * m_v3Position.x + fT * mUnidentified350.x;
        v3NewPosition.y
            = (1.0f - fT) * m_v3Position.y + fT * mUnidentified350.y;
        v3NewPosition.z
            = (1.0f - fT) * m_v3Position.z + fT * mUnidentified350.z;
        v3NewPosition.z = m_v3Position.z;
        SetPosition(v3NewPosition);

        m_v3Position.z += fDeltaT * m_v3Velocity.z;
        if (m_v3Position.z >= 50.0f)
        {
            m_v3Position.z = 50.0f;
        }

        float fNewVelocityZ = SeekSpeedExponential(
            m_v3Velocity.z, lbl_806DB910, lbl_806DB91C, fDeltaT);
        nlVector3 v3NewVelocity = v3Zero;
        v3NewVelocity.z = fNewVelocityZ;
        SetVelocity(v3NewVelocity);

        if (mUnidentified34C <= 0.0f)
        {
            fn_80045930();
        }
    }
    else if (m_eAnimID == 0x7E)
    {
        nlVector3 v3Delta;
        v3Delta.y = v3Zero.y - m_v3Position.y;
        v3Delta.x = v3Zero.x - m_v3Position.x;
        v3Delta.z = v3Zero.z - m_v3Position.z;
        Unknown8(nlVector3ToAngle(v3Delta), false);

        SetFacingDirection(
            SeekDirection(m_aActualFacingDirection,
                m_aDesiredFacingDirection, fn_8002C0AC(fn_8003E6E4(this)),
                fn_8002CF10(fn_8003E6E4(this)), fDeltaT),
            true);

        nlPolarToCartesian(m_v3Velocity.x, m_v3Velocity.y,
            m_aActualFacingDirection,
            InterpolateRangeClamped(lbl_806DB8B4, lbl_806DB8B8, 0.0f, 4.0f,
                fn_800A6388(m_pTeam)));

        nlVector2 v3Distance = {
            m_v3Position.x - v3Zero.x,
            m_v3Position.y - v3Zero.y,
        };
        if (nlSqrt(v3Distance.x * v3Distance.x
                    + v3Distance.y * v3Distance.y,
                true)
            < lbl_806E35D4)
        {
            SetAnimState(0x7F, true, 0.2f, false, false);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);
        }
    }
    else
    {
        if (ShouldStartCrossBlend(4))
        {
            bool bHasPad = GetGlobalPad() != 0;
            if (!bHasPad)
            {
                bool bGiven = false;
                for (int i = 0; i < 4; i++)
                {
                    cFielder* pOther = m_pTeam->GetFielder(i);
                    if (pOther != this)
                    {
                        bool bOtherHasPad = pOther->GetGlobalPad() != 0;
                        if (bOtherHasPad
                            && (fn_800344B0(pOther) || fn_80038918(pOther)))
                        {
                            SetAIPad(pOther->m_pController);
                            m_bCanTestController = false;
                            pOther->SetAIPad(0);
                            bGiven = true;
                        }
                    }
                }

                if (!bGiven)
                {
                    for (int i = 0; i < 0x10; i++)
                    {
                        cAIPad* pPad = fn_80007C3C(i);
                        if (pPad != 0)
                        {
                            int mySide = m_pTeam->m_nSide;
                            short playingSide
                                = GameInfoManager::Instance()->GetPlayingSide(
                                    (u16)i);
                            if (playingSide == mySide)
                            {
                                bool bTaken = false;
                                for (int j = 0; j < 5; j++)
                                {
                                    cPlayer* pPlayer = m_pTeam->GetPlayer(j);
                                    bool bPlayerHasPad
                                        = pPlayer->GetGlobalPad() != 0;
                                    if (bPlayerHasPad
                                        && pPlayer->m_pController == pPad)
                                    {
                                        bTaken = true;
                                    }
                                }
                                if (!bTaken)
                                {
                                    SetAIPad(pPad);
                                }
                            }
                        }
                    }
                }
            }

            EndAction();
        }
    }
}

void cFielder::fn_80046244()
{
    if (!fn_800344B0(this))
    {
        if (m_pBall != 0)
        {
            ReleaseBall(0);
            g_pBall->ShootRelease(m_v3Velocity, SPINTYPE_NONE);
            fn_8001458C(g_pBall);
        }

        fn_8002E718(this);
        fn_8002E798(this);
        fn_8002E3F8(this);
        fn_8002E39C(this);
        fn_8002E2E4(this);

        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction((eFielderActionState)3);
        SetAnimState(0x7C, false, 0.2f, false, false);
        InitMovementCoast();

        nlVector3 v3Velocity = m_v3Velocity;
        v3Velocity.y += AIsgn(m_v3Position.y);
        v3Velocity.z += lbl_806E35DC;
        SetVelocity(v3Velocity);

        mUnidentified17C = false;

        mUnidentified34C = InterpolateRangeClamped(lbl_806DB8BC,
            lbl_806DB8C0, lbl_806E35E0, 0.0f, fn_800A6388(m_pTeam));

        fn_801BB5DC(this, 1);
        fn_801BB640(this, 1);
        fn_801BA034();

        bool bHasPad = GetGlobalPad() != 0;
        if (bHasPad)
        {
            fn_8009591C(this, false);
        }

        if (GameInfoManager::Instance()->GetStadium() == 0x0B)
        {
            m_pPhysicsCharacter->m_CanCollideWithGoalLine = 0;
            m_pPhysicsCharacter->m_CanCollideWithWall = 0;
        }

        bool bUnidentified = fn_8001E168(this);
        unsigned long soundID = 0x1CF82176;
        if (bUnidentified)
        {
            soundID = 0xFDEC8E0F;
        }
        fn_800EBBFC(mUnidentified318, soundID, 0, 0);
    }
}

void cFielder::InitActionPostWhistle()
{
    SetAction(ACTION_POST_WHISTLE);
    SetAnimState(0, false, 0.0f, false, false);
    InitMovementNone(0.0f, 0.0f);
    Unknown8(m_aActualFacingDirection, false);
    m_fActualSpeed = 0.0f;
    SetVelocity(v3Zero);
}

void cFielder::ActionPostWhistle(float fDeltaT)
{
}

void cFielder::InitActionBombReact(const nlVector3& v3BombPosition,
    float fRadius)
{
    fn_8002E3F8(this);
    fn_8009750C();

    if (g_pBall->m_pOwner == this)
    {
        ReleaseBall(0);
        ShootBallDueToContact((unsigned short)(s32)nlRandomf(lbl_806E3610));
    }

    float fDistance
        = nlSqrt(CalculateDistanceSquared(m_v3Position, v3BombPosition), true)
        - fRadius;

    if (fDistance > 1.0f && !IsFallenDown())
    {
        InitActionBombHitReact(v3BombPosition);
    }
    else
    {
        if (!IsFallenDown())
        {
            bool bUnidentified = fn_8001E168(this);
            unsigned long soundID = 0x00E606A2;
            if (bUnidentified)
            {
                soundID = 0x3642C41B;
            }
            fn_800EBBFC(mUnidentified318, soundID, 0, 0);
        }

        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction(ACTION_BOMB_REACT);

        s16 facingDelta = GetFacingDeltaToPosition(v3BombPosition);
        u16 absFacingDelta = (u16)abs_s16(facingDelta);

        if (absFacingDelta < 0x4000)
        {
            SetAnimState(0x65, true, 0.2f, false, false);
        }
        else
        {
            SetAnimState(0x66, true, 0.2f, false, false);
        }

        InitMovementFromAnim(0, v3Zero, 1.0f, false);
        m_fDesiredSpeed = 0.0f;
    }
}

void cFielder::InitActionBombHitReact(const nlVector3& v3BombPosition)
{
    fn_8002E3F8(this);
    fn_8009750C();

    mUnidentified360 = false;

    if (!IsFallenDown())
    {
        bool bUnidentified = fn_8001E168(this);
        unsigned long soundID = 0x00E606A2;
        if (bUnidentified)
        {
            soundID = 0x3642C41B;
        }
        fn_800EBBFC(mUnidentified318, soundID, 0, 0);
    }

    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_HIT_REACT);

    u32 index = (((u16)((u16)GetFacingDeltaToPosition(v3BombPosition)) >> 14) & 3);
    SetAnimState(gHitReactAnims[2][index], true, 0.2f, false, false);

    float fDX = m_v3Position.x - v3BombPosition.x;
    float fDY = m_v3Position.y - v3BombPosition.y;
    float angleRad = nlATan2f(fDY, fDX);
    u16 targetAngle = (u16)(s32)(10430.378f * angleRad);
    SetFacingDirection(targetAngle + gHitReactFacingOffsets[index], true);

    InitMovementFromAnim(0, v3Zero, 1.0f, false);

    m_fDesiredSpeed = 0.0f;
}

void cFielder::InitActionBananaReact(const nlVector3& fDeltaT)
{
    u16 angleDiff
        = (u16)abs_s16(m_aActualFacingDirection - m_aActualMovementDirection);
    if (angleDiff < 0x4000)
    {
        SetAnimState(0x63, true, 0.2f, false, false);
    }
    else
    {
        SetAnimState(0x64, true, 0.2f, false, false);
    }

    if (g_pBall->m_pOwner == this)
    {
        ReleaseBall(0);
        ShootBallDueToContact(m_aActualFacingDirection);
    }

    fn_80139D1C(2, GetGlobalPad());

    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_BANANA_REACT);

    InitMovementFromAnim(0, v3Zero, 1.0f, false);

    m_fDesiredSpeed = 0.0f;
}

void cFielder::InitActionShellReact(const nlVector3& v3CollisionLocation,
    const nlVector3& v3CollisionVelocity)
{
    if (g_pBall->m_pOwner == this)
    {
        ReleaseBall(0);

        if (nlSqrt(v3CollisionVelocity.GetLengthSq3D(), true) > 0.05f)
        {
            ShootBallDueToContact(v3CollisionVelocity);
        }
        else
        {
            ShootBallDueToContact(m_aActualFacingDirection);
        }
    }

    fn_8002E580(this);
    fn_8009750C();

    fn_80139D1C(2, GetGlobalPad());

    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_SHELL_REACT);

    s16 facingDelta = GetFacingDeltaToPosition(v3CollisionLocation);
    facingDelta += 0x2000;
    SetAnimState(gShellAttackReactAnims[(u16)facingDelta >> 14], true, 0.2f,
        false, false);

    InitMovementFromAnim(0, v3Zero, 1.0f, false);

    m_fDesiredSpeed = 0.0f;
}

void cFielder::InitActionRunning()
{
    m_pHeadTrack->m_bTrackOOI = true;

    if (m_eActionState != ACTION_RUNNING)
    {
        mUnidentified37C = 0;
        m_aActualMovementDirection = m_aActualFacingDirection;
        Unknown8(m_aActualFacingDirection, false);
        m_fDesiredSpeed = m_fActualSpeed;
        mUnidentified380 = false;
    }

    SetAction(ACTION_RUNNING);
}

void cFielder::ActionRunning(float dt)
{
    if (m_pBall != 0)
    {
        SetAction(ACTION_RUNNING_WB);
        mUnidentified385 = false;
        mUnidentified384 = false;
        mUnidentified37C = 0;
        m_aActualMovementDirection = m_aActualFacingDirection;
        mActionShotVars.bIsChipShot = false;
        mUnidentified374 = UnidentifiedFielderPair374();
    }
    else
    {
        if (fn_8003E948(this) && mUnidentified3DC)
        {
            if (m_eAnimID != 4)
            {
                fn_8003BA94(this, 0.1f);
            }
        }
        else
        {
            asmRunning();
        }

        if (CanPickupBall(g_pBall, false))
        {
            PickupBall(g_pBall);
            SetAction(ACTION_RUNNING_WB);
            mUnidentified385 = false;
            mUnidentified384 = false;
            mUnidentified37C = 0;
            m_aActualMovementDirection = m_aActualFacingDirection;
            mActionShotVars.bIsChipShot = false;
            mUnidentified374 = UnidentifiedFielderPair374();
        }
    }
}

void cFielder::InitActionRunningWB(bool bWaitForAnimToFinish)
{
    SetAction(ACTION_RUNNING_WB);
    mUnidentified385 = bWaitForAnimToFinish;
    mUnidentified384 = false;
    mUnidentified37C = 0;
    m_aActualMovementDirection = m_aActualFacingDirection;
    mActionShotVars.bIsChipShot = false;
    mUnidentified374 = UnidentifiedFielderPair374();
}

void cFielder::ActionRunningWB(float dt)
{
    if (m_pBall == 0)
    {
        EndAction();
    }
    else
    {
        nlVector3 v3Target;
        nlPolarToCartesian(
            v3Target.x, v3Target.y, m_aActualFacingDirection, 2.0f);
        v3Target.z = 0.0f;
        nlVec3Add(v3Target, v3Target, m_v3Position);
        asmRunningWB(dt);
    }
}

void cFielder::fn_8004B658()
{
    if (m_pBall == 0)
    {
        InitActionRunning();
    }
    else
    {
        SetAction(ACTION_UNKNOWN_30);
        DoResetShotMeter(0.0f);
        fn_8003A544(this);
        InitMovementRunningNoTurn(0.0f, fn_8002CFB0(fn_8003E6E4(this)));
        m_fDesiredSpeed = 0.0f;
        if (m_fActualSpeed > fn_8002C328(fn_8003E6E4(this)))
        {
            m_fActualSpeed = fn_8002C328(fn_8003E6E4(this));
        }

        cFielder* pFielder = this;
        fn_8005F03C(lbl_806E0C94, &pFielder);
        fn_8005CBF0(lbl_806E0C94);

        fn_800EBBFC(0, 0x900862AC, "Windup", this);

        if (fn_800155A0(g_pBall, 0) < 1.0f)
        {
            fn_801B9C90("ball_shot_windup_0");
            fn_801B9C90("ball_shot_windup_ground_0");
        }
        else if (fn_800155A0(g_pBall, 0) < 2.0f)
        {
            fn_801B9C90("ball_shot_windup_1");
            fn_801B9C90("ball_shot_windup_ground_1");
        }
        else if (fn_800155A0(g_pBall, 0) < lbl_806E35E0)
        {
            fn_801B9C90("ball_shot_windup_2");
            fn_801B9C90("ball_shot_windup_ground_2");
        }
        else if (fn_800155A0(g_pBall, 0) < 4.0f)
        {
            fn_801B9C90("ball_shot_windup_3");
            fn_801B9C90("ball_shot_windup_ground_3");
        }
        else
        {
            fn_801B9C90("ball_shot_windup_max");
            fn_801B9C90("ball_shot_windup_ground_max");
        }
    }
}

void cFielder::fn_8004B148()
{
    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction((eFielderActionState)0x13);

    u16 aDirection = m_aActualFacingDirection;
    if (GetGlobalPad() != 0)
    {
        if (m_pController->GetMovementStickMagnitude() > 0.01f)
        {
            aDirection = m_pController->GetMovementStickDirection();
        }
    }

    if (m_fActualSpeed < fn_8002C5A4(fn_8003E6E4(this)))
    {
        float fMinSpeed = fn_8002C5A4(fn_8003E6E4(this));
        m_fActualSpeed = fMinSpeed;
        m_fDesiredSpeed = fMinSpeed;
    }

    nlVector3 v3Velocity;
    nlPolarToCartesian(v3Velocity.x, v3Velocity.y, aDirection,
        fn_8003C300(this, m_fActualSpeed));
    v3Velocity.z = 0.0f;
    SetVelocity(v3Velocity);
    InitMovementCoast();

    if (GetGlobalPad() != 0)
    {
        mUnidentified374.mUnidentified04
            = InterpolateRangeClamped(lbl_806DB8D8, lbl_806DB8DC, 0.2f,
                lbl_806DB8E0,
                (float)mUnidentified374.mUnidentified00
                    * FixedUpdateTask::GetPhysicsUpdateTick());
        fn_80331F9C(GetGlobalPad(), 0x17, 1);
    }
    else
    {
        mUnidentified374.mUnidentified04 = lbl_806DB8D8;
    }

    SetAnimState(0x25, true, 0.2f, false, false);
    fn_800EBC84(0, 0x874F86F2, g_pBall->mUnidentifiedEC, 0, 0);
}

void cFielder::fn_8004B2E4(float fDeltaT)
{
    if (m_pBall != 0 && m_pCurrentAnimController->TestFrameTrigger(1.0f))
    {
        float fSpeed = m_fActualSpeed + mUnidentified374.mUnidentified04;
        if (fSpeed < lbl_806DB8E8)
        {
            fSpeed = lbl_806DB8E8;
        }

        nlVector3 v3BallVelocity;
        nlPolarToCartesian(v3BallVelocity.x, v3BallVelocity.y,
            m_aActualFacingDirection, fSpeed);
        v3BallVelocity.z = InterpolateRangeClamped(lbl_806DB8D0,
            lbl_806DB8D4, lbl_806DB8D8, lbl_806DB8DC,
            mUnidentified374.mUnidentified04);

        nlVector3 v3Spin;
        nlVector3 v3UpCopy = v3Up;
        nlVec3CrossProduct(v3Spin, v3BallVelocity, v3UpCopy);
        float fScale = lbl_806DB8E4 / nlSqrt(v3Spin.GetLengthSq3D(), true);
        nlVec3Scale(v3Spin, fScale);

        ReleaseBall(0);
        g_pBall->SetVelocity(v3BallVelocity, SPINTYPE_PARAMETER, &v3Spin);
        fn_80015C38(g_pBall, 0);
        g_pBall->SetVelocity(v3BallVelocity, SPINTYPE_PARAMETER, &v3Spin);
        SetNoPickUpTime(0.25f);

        if (gbUseTurboCharging != 0)
        {
            float fValue = fn_8003E6E4(this)->mUnidentified034;
            float fFraction
                = InterpolateRangeClamped(0.0f, 1.0f, 0.5f, 1.0f, fValue);
            float fCharge = Interpolate(lbl_806DB980, lbl_806DB984, fFraction);
            fn_800154FC(g_pBall, fCharge + fn_800155A0(g_pBall, 0));
        }

        InitMovementRunning(fn_8002C0AC(fn_8003E6E4(this)),
            fn_8002CF10(fn_8003E6E4(this)), fn_8002C180(fn_8003E6E4(this)),
            0.0f);
        InitDesire(
            (eFielderDesireState)0x14, 0.5f, -1.0f, fvNotSet, fvNotSet);
        fn_801B75C8(this, 0, 0, 0, 0);
    }

    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

bool cFielder::fn_8004B86C(bool bIsChipShot, bool bParam)
{
    if (m_pBall == 0)
    {
        InitActionRunning();
        return false;
    }
    else
    {
        if (bParam)
        {
            DoResetShotMeter(0.0f);
            m_pShotMeter->CalcOneTimerValue(this, false);
        }
        else
        {
            m_pShotMeter->ShotReleased(this);
        }

        if (m_pShotMeter->m_eShotMeterState == SHOT_METER_STS_RELEASED)
        {
            g_pBall->m_uGoalType = 2;
            if (m_eCharacterClass == (eCharacterClass)0x10)
            {
                fn_800395C0(this);
            }
            else if (m_eCharacterClass == (eCharacterClass)0x0D
                     || m_eCharacterClass == (eCharacterClass)0x12
                     || m_eCharacterClass + 0 == (eCharacterClass)0x13)
            {
                fn_8004E438();
                return true;
            }
            else if (m_eCharacterClass == (eCharacterClass)0x0E)
            {
                if (lbl_806E1608->mUnidentified02C != 0)
                {
                    fn_801A6344(lbl_806E1608->mUnidentified02C, this);
                }
            }
            else if (m_eCharacterClass == (eCharacterClass)0x0C)
            {
                if (lbl_806E1608->mUnidentified028 != 0)
                {
                    fn_8019A270(lbl_806E1608->mUnidentified028, this);
                }
            }
        }

        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction(ACTION_UNKNOWN_15);
        mActionShotVars.bIsChipShot = bIsChipShot;

        switch (m_eAnimID)
        {
        case 0x52:
            SetAnimState(0x54, true, 0.2f, false, false);
            break;
        case 0x53:
            SetAnimState(0x55, true, 0.2f, false, false);
            break;
        default:
        {
            s16 facingDelta = GetFacingDeltaToPosition(
                m_pTeam->GetOtherNet()->m_v3NetLocation);
            if (facingDelta < 0)
            {
                SetAnimState(0x55, true, 0.2f, false, false);
            }
            else
            {
                SetAnimState(0x54, true, 0.2f, false, false);
            }
            break;
        }
        }

        float fSpeed = m_fActualSpeed;
        InitMovementRunningNoTurn(0.0f, fSpeed / fn_8002C7E8(fn_8003E6E4(this)));
        m_fDesiredSpeed = 0.0f;

        nlVector3 v3NetPos = m_pTeam->GetOtherNet()->m_v3NetLocation;
        nlVector3 v3Delta;
        nlVec3Sub(v3Delta, v3NetPos, m_v3Position);
        Unknown8(nlVector3ToAngle(v3Delta), false);
        return true;
    }
}

void cFielder::InitActionSlideAttack(
    cFielder* pTarget, int nParam, float fTime)
{
    if (!fn_8003886C(this))
    {
        nlVector3 v3Velocity = m_v3Velocity;

        SetAction(ACTION_SLIDE_ATTACK);
        SetAnimState(0x5E, true, 0.2f, false, false);
        InitMovementRunning(0.0f, 0.0f, fn_8002C180(fn_8003E6E4(this)),
            fn_8002CF24(fn_8003E6E4(this)));
        m_tSlideAttackTimer.SetSeconds(fn_8002C800(fn_8003E6E4(this)));

        mUnidentified388 = 0;
        mUnidentified38C = false;
        mUnidentified38D = false;

        nlVector3 v3Target;
        if (fTime < 0.0f)
        {
            fTime = fn_80038970(this, &v3Target, nParam);
        }
        else
        {
            nlVector3 v3TargetPosition;
            nlVector3 v3TargetVelocity;
            if (pTarget != 0)
            {
                v3TargetPosition = pTarget->m_v3Position;
                v3TargetVelocity = pTarget->m_v3Velocity;
            }
            else
            {
                v3TargetPosition = g_pBall->m_v3Position;
                v3TargetVelocity = g_pBall->m_v3Velocity;
            }

            v3Target.x = v3TargetPosition.x + v3TargetVelocity.x * fTime;
            v3Target.y = v3TargetPosition.y + v3TargetVelocity.y * fTime;
            v3Target.z = 0.0f;
        }

        float fSpeed = fn_8003C40C(this, nParam);
        if (fn_8003E70C(this))
        {
            nlPolarToCartesian(
                v3Velocity.x, v3Velocity.y, m_aActualFacingDirection, fSpeed);
        }
        else
        {
            nlVector3 v3BallDelta;
            nlVec3Sub(v3BallDelta, m_v3Position, g_pBall->m_v3Position);
            float fAdjust = mUnidentified0A0;
            float fBallDistance
                = nlSqrt(v3BallDelta.GetLengthSq3D(), true);

            if (fBallDistance
                < 0.1f + fn_8002BFA8(fn_8003E6E4(this), fAdjust))
            {
                float fLengthSq = v3Velocity.GetLengthSq3D();
                if (fLengthSq > lbl_806E361C)
                {
                    float fRecipLength = nlRecipSqrt(fLengthSq, true);
                    nlVec3Scale(v3Velocity, fRecipLength);
                    nlVec3Scale(v3Velocity, fSpeed);
                }
                else
                {
                    nlPolarToCartesian(v3Velocity.x, v3Velocity.y,
                        m_aActualFacingDirection, fSpeed);
                }
            }
            else
            {
                if (0.0f == fTime)
                {
                    nlPolarToCartesian(v3Velocity.x, v3Velocity.y,
                        m_aActualFacingDirection, fSpeed);
                }
                else
                {
                    v3Velocity.x = v3Target.x - m_v3Position.x;
                    v3Velocity.y = v3Target.y - m_v3Position.y;
                    v3Velocity.z = 0.0f;
                    float fLengthSq = v3Velocity.GetLengthSq3D();
                    if (fLengthSq > lbl_806E361C)
                    {
                        float fRecipLength = nlRecipSqrt(fLengthSq, true);
                        nlVec3Scale(v3Velocity, fRecipLength);
                        nlVec3Scale(v3Velocity, fSpeed);
                    }
                    else
                    {
                        nlPolarToCartesian(v3Velocity.x, v3Velocity.y,
                            m_aActualFacingDirection, fSpeed);
                    }
                }

                nlPolar polar;
                nlCartesianToPolar(polar, v3Velocity.x, v3Velocity.y);
                Unknown8(polar.a, false);
                SetFacingDirection(m_aDesiredFacingDirection, true);
            }
        }

        v3Velocity.z = 0.0f;
        SetVelocity(v3Velocity);

        nlPolar polar;
        nlCartesianToPolar(polar, v3Velocity.x, v3Velocity.y);
        float fFinalSpeed = polar.r;
        m_fDesiredSpeed = fFinalSpeed;
        m_fActualSpeed = fFinalSpeed;

        fn_800EC12C(0x2AE03886, this);
        fn_800EBBFC(0, 0x2AE03886, "SlideAttack", this);

        PlayerAttackData* pNode;
        lbl_80571960.Allocate(pNode);
        pNode->pAttacker = this;
        bool bHasPad = GetGlobalPad() != 0;
        pNode->nAttackerPadID = bHasPad ? GetGlobalPad()->fn_80332748() : -1;
        pNode->pTarget = 0;
        pNode->mUnidentified10 = true;
        fn_8005EBF8(lbl_806E0C94, pNode);
    }
}

void cFielder::fn_8004BF58(eFielderActionState eNewAction)
{
    if (lbl_806E0C74 != 0)
    {
        delete lbl_806E0C74;
        lbl_806E0C74 = 0;
    }

    fn_801BA034();
    fn_800EC12C(0x900862AC, this);

    if (eNewAction != ACTION_UNKNOWN_15)
    {
        m_pShotMeter->Abort();
        if (m_eCharacterClass == (eCharacterClass)0x10
            && eNewAction != (eFielderActionState)1)
        {
            mUnidentified178 = 1.0f;
        }
    }
    else
    {
        if (m_eCharacterClass == (eCharacterClass)0x10
            && m_pShotMeter->m_eShotMeterState != SHOT_METER_STS_RELEASED)
        {
            mUnidentified178 = 1.0f;
        }
    }

    fn_8005CDD0(lbl_806E0C94);
}

void cFielder::fn_8004BB80(float fDeltaT)
{
    fn_80139D1C(1, GetGlobalPad());

    nlVector3 v3NetPos = m_pTeam->GetOtherNet()->m_v3NetLocation;
    nlVector3 v3Delta;
    nlVec3Sub(v3Delta, v3NetPos, m_v3Position);
    m_aDesiredFacingDirection = nlVector3ToAngle(v3Delta);

    SetFacingDirection(
        SeekDirection(m_aActualFacingDirection, m_aDesiredFacingDirection,
            fn_8002CF88(fn_8003E6E4(this)), fn_8002CF9C(fn_8003E6E4(this)),
            fDeltaT),
        false);

    float fChargeTime = m_pShotMeter->m_fTime;
    fChargeTime = fn_8002C7E8(fn_8003E6E4(this)) - fChargeTime;
    if (fChargeTime < 0.01f)
    {
        fChargeTime = 0.01f;
    }

    InitMovementRunningNoTurn(0.0f, m_fActualSpeed / fChargeTime);
    m_fDesiredSpeed = 0.0f;

    if (fChargeTime < 0.5f && lbl_806E0C74 == 0)
    {
        if (AIsgn(m_v3Position.x)
            == AIsgn(m_pTeam->GetOtherNet()->m_v3NetLocation.x))
        {
            nlVector3 v3Dir;
            nlVec3Sub(v3Dir, m_v3Position,
                m_pTeam->GetOtherNet()->m_v3NetLocation);
            float fRecipLength = nlRecipSqrt(v3Dir.GetLengthSq3D(), true);
            nlVec3Scale(v3Dir, v3Dir, fRecipLength);
            nlVec3ScaleAdd(v3Dir, 2.0f, v3Dir, m_v3Position);

            UnidentifiedWindupObject* pObject
                = (UnidentifiedWindupObject*)nlMalloc(0x68, 8, false);
            if (pObject != 0)
            {
                pObject = fn_8000DDE8(pObject, 2, &v3Dir,
                    &m_pTeam->GetOtherNet()->m_v3NetLocation, lbl_806E35DC);
            }
            lbl_806E0C74 = pObject;
            lbl_806E0C74->mUnidentified64 = this;
        }
    }

    if (!fn_8003886C(this))
    {
        m_pShotMeter->Update(fDeltaT);
    }

    if (m_eCharacterClass == (eCharacterClass)0x10 && m_pBall != 0)
    {
        ShotMeter* pShotMeter = m_pShotMeter;
        float fWindow = 0.1f;
        float fReleaseTime = pShotMeter->mUnidentified00C;
        float fTime = pShotMeter->m_fTime;
        if (fTime > fReleaseTime - fWindow)
        {
            mUnidentified178 = InterpolateRangeClamped(
                mUnidentified178, 0.0f, fWindow, 0.0f, fReleaseTime - fTime);
        }
    }

    if (m_pBall == 0)
    {
        EndAction();
    }
}

void cFielder::fn_8004C02C(float fDeltaT)
{
    if (m_pCurrentAnimController->m_fTime
        <= lbl_806E3614 / (float)m_pCurrentAnimController->m_pSAnim->m_nNumKeys)
    {
        nlVector3 v3NetPos = m_pTeam->GetOtherNet()->m_v3NetLocation;
        nlVector3 v3Delta;
        nlVec3Sub(v3Delta, v3NetPos, m_v3Position);
        m_aDesiredFacingDirection = nlVector3ToAngle(v3Delta);

        SetFacingDirection(
            SeekDirection(m_aActualFacingDirection,
                m_aDesiredFacingDirection, lbl_806E3618,
                fn_8002CF9C(fn_8003E6E4(this)), fDeltaT),
            false);
    }

    if (m_pBall != 0
        && m_pCurrentAnimController->TestFrameTrigger(lbl_806E3614))
    {
        if (!fn_8003C180(this))
        {
            fn_80037AC8(this, 0);

            if (g_pBall->meBallState == 8)
            {
                if (m_eCharacterClass == (eCharacterClass)0x11)
                {
                    fn_801B75C8(this, 4, 0, 0, 1);
                }
                else if (m_eCharacterClass == (eCharacterClass)0x0F)
                {
                    fn_801B75C8(this, 3, 0, 0, 1);
                }
                else if (m_eCharacterClass == (eCharacterClass)0x10)
                {
                    fn_801B75C8(this, 5, 0, 0, 1);
                }
            }
            else
            {
                if (m_tBallPossessionTimer.GetSeconds() < 0.1f
                    || m_pShotMeter->m_fSpeedValue < 0.1f)
                {
                    fn_801B75C8(this, 2, 0, 0, 1);
                }
                else
                {
                    fn_801B75C8(this, 1, 0, 0, 1);
                }
            }
        }
        else
        {
            fn_80035700(this);
            fn_801B75C8(this, 1, 0, 0, 0);
        }

        InitMovementFromAnim(0, v3Zero, 1.0f, false);
    }

    if (ShouldStartCrossBlend(4))
    {
        m_pHeadTrack->m_bTrackOOI = false;
        EndAction();
    }
}

void cFielder::fn_8004C88C(float fDeltaT)
{
    if (!mUnidentified38C && mUnidentified388 == 0 && fn_8003E6FC())
    {
        nlVector3 v3Delta;
        v3Delta.y = g_pBall->m_v3Position.y - m_v3Position.y;
        v3Delta.x = g_pBall->m_v3Position.x - m_v3Position.x;
        float fLength = nlSqrt(
            v3Delta.x * v3Delta.x + v3Delta.y * v3Delta.y, true);
        float fRecipLength = 1.0f
            / nlSqrt(v3Delta.x * v3Delta.x + v3Delta.y * v3Delta.y, true);

        float fDir[2];
        fDir[1] = fRecipLength * v3Delta.y;
        fDir[0] = fRecipLength * v3Delta.x;
        v3Delta.y = lbl_806E3620 * fDir[1];
        v3Delta.x = lbl_806E3620 * fDir[0];

        float fVelX2 = m_v3Velocity.x * m_v3Velocity.x;
        float fVelY2 = m_v3Velocity.y * m_v3Velocity.y;
        float fSpeed = nlSqrt(fVelX2 + fVelY2, true);

        float fBlend[2];
        fBlend[1] = v3Delta.y + m_v3Velocity.y;
        fBlend[0] = v3Delta.x + m_v3Velocity.x;
        float fRecipBlend = 1.0f
            / nlSqrt(fBlend[0] * fBlend[0] + fBlend[1] * fBlend[1], true);
        float fBlendN[2];
        fBlendN[1] = fRecipBlend * fBlend[1];
        fBlendN[0] = fRecipBlend * fBlend[0];

        if (fBlendN[0] * fDir[0] + fBlendN[1] * fDir[1] >= 0.0f)
        {
            nlVector3 v3NewVelocity;
            v3NewVelocity.z = 0.0f;
            fBlend[1] = fSpeed * fBlendN[1];
            fBlend[0] = fSpeed * fBlendN[0];
            v3NewVelocity.y = fBlend[1];
            v3NewVelocity.x = fBlend[0];

            nlPolar polar;
            nlCartesianToPolar(polar, fBlend[0], fBlend[1]);
            Unknown8(polar.a, false);
            SetFacingDirection(polar.a, true);
            SetVelocity(v3NewVelocity);
        }
    }

    if (g_pBall->m_pOwner == 0 && !fn_80014E20(g_pBall)
        && g_pBall->m_tLightningTimer.m_uPackedTime == 0
        && !(g_pBall->m_tNoPickupTimer.m_uPackedTime != 0
            && g_pBall->m_pPrevOwner != 0
            && g_pBall->m_pPrevOwner->m_eClassType == GOALIE))
    {
        bool bCanPickup = CanPickupBall(g_pBall, true);
        bool bTouched = false;
        if (mUnidentified38C || bCanPickup)
        {
            bTouched = true;
        }
        mUnidentified38C = bTouched;

        if (bTouched && bCanPickup)
        {
            bool bAirborne = g_pBall->meBallState == 5
                || g_pBall->meBallState == 3;
            bool bStolen = bAirborne && g_pBall->m_pPassTarget != 0;

            if (bStolen && g_pBall->m_pPrevOwner != 0
                && g_pBall->m_pPrevOwner->m_eClassType == FIELDER
                && !IsOnSameTeam(g_pBall->m_pPrevOwner))
            {
                PlayerAttackData* pNode;
                lbl_80571960.Allocate(pNode);
                pNode->pAttacker = this;
                bool bHasPad = GetGlobalPad() != 0;
                pNode->nAttackerPadID
                    = bHasPad ? GetGlobalPad()->fn_80332748() : -1;
                pNode->pTarget = 0;
                pNode->mUnidentified10 = true;
                fn_8005ED64(lbl_806E0C94, pNode);

                if (m_pBall != 0)
                {
                    if (GetStadiumUnknown0x10(
                            GameInfoManager::Instance()->GetStadium()))
                    {
                        unsigned long soundID = 0x0DCA472D;
                        if (m_pTeam->m_nSide == 0)
                        {
                            soundID = 0x902EA0E4;
                        }
                        fn_800ED92C(soundID);
                    }
                }
            }

            PickupBall(g_pBall);
        }
    }

    if (mUnidentified38C)
    {
        if (GetGlobalPad() != 0 && !mUnidentified38D)
        {
            mUnidentified38D = fn_80036F88(this) == 0;
        }

        if (m_pBall != 0)
        {
            fn_8003D8A4(this, fDeltaT);
        }
    }

    switch (mUnidentified388)
    {
    case 0:
    {
        nlVector3 v3Velocity = m_v3Velocity;
        nlPolarToCartesian(v3Velocity.x, v3Velocity.y,
            m_aActualFacingDirection, m_fActualSpeed);
        v3Velocity.z = 0.0f;
        SetVelocity(v3Velocity);

        bool bUnidentified = false;
        if (GetGlobalPad() != 0 && mUnidentified38D && m_pBall != 0)
        {
            if (!fn_80036F88(this))
            {
                bUnidentified = true;
            }
        }

        if (m_tSlideAttackTimer.m_uPackedTime == 0 || bUnidentified)
        {
            fn_80139D1C(1, GetGlobalPad());
            mUnidentified388 = 1;
            m_tSlideAttackTimer.SetSeconds(fn_8002C8D4(fn_8003E6E4(this)));
        }
        break;
    }
    case 1:
    {
        float fDecelTime = m_tSlideAttackTimer.GetSeconds();
        if (fDecelTime < 0.01f)
        {
            fDecelTime = 0.01f;
        }

        float fTargetSpeed = 1.0f;
        if (m_pBall != 0)
        {
            fTargetSpeed = fn_8002CE14(fn_8003E6E4(this));
        }

        InitMovementRunningNoTurn(
            0.0f, (m_fActualSpeed - fTargetSpeed) / fDecelTime);
        m_fDesiredSpeed = fTargetSpeed;

        if (m_tSlideAttackTimer.m_uPackedTime == 0)
        {
            fn_8004D238();
        }
        break;
    }
    }

    bool bUnidentified2 = lbl_806E0C94->m_eGameState == 5
        || lbl_806E0C94->m_eGameState == 6;
    if (!bUnidentified2)
    {
        fn_8004D238();
    }
}

void cFielder::fn_8004D238()
{
    if (m_pBall != 0)
    {
        if (!fn_8003D9BC(this))
        {
            EndDesire();
            EndAction();
        }
    }
    else
    {
        EndDesire();
        EndAction();
    }

    m_eLastPadAction = 0x32;
}

void cFielder::fn_8004D480(const nlVector3& v3CollisionVelocity)
{
    fn_8002E3F8(this);
    fn_8009750C();

    if (IsFallenDown() && m_eActionState == (eFielderActionState)0x1C)
    {
        SetAnimState(0x56, true, 0.2f, false, false);
        cPN_SAnimController* pController = m_pCurrentAnimController;
        pController->m_fPrevTime = pController->m_fTime;
        pController->m_fTime = 0.05f;
        pController->m_bLooped = 0;
    }
    else
    {
        if (g_pBall->m_pOwner == this)
        {
            ReleaseBall(0);
            ShootBallDueToContact(v3CollisionVelocity);
        }

        InitDesire(
            FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
        SetAction((eFielderActionState)0x1C);
        SetAnimState(0x56, true, 0.2f, false, false);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);

        nlPolar polar;
        nlCartesianToPolar(
            polar, v3CollisionVelocity.x, v3CollisionVelocity.y);
        SetFacingDirection(polar.a, true);
        m_aActualMovementDirection = polar.a;
        m_fDesiredSpeed = 0.0f;

        fn_80139D1C(2, GetGlobalPad());

        if (fn_8003E8A0(this))
        {
            fn_801B98A0(this);
        }
    }
}

void cFielder::ActionSlideAttackReact(float fDeltaT)
{
    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::ActionBombReact(float fDeltaT)
{
    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::ActionSTSHitReact(float fDeltaT)
{
    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::ActionShellReact(float fDeltaT)
{
    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::ActionBananaReact(float fDeltaT)
{
    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::InitActionReceivePass(int animID, nlVector3& v3TargetPos,
    short sDirectionSeekSpeed, float fAdjustEndTime)
{
    mUnidentified368 = fAdjustEndTime;
    SetAction(ACTION_RECEIVE_PASS);
    SetAnimState(animID, false, fAdjustEndTime * lbl_806DB990, false, false);

    nlVector3 v3Direction;
    nlVec3Set(v3Direction,
        v3TargetPos.x - m_v3Position.x,
        v3TargetPos.y - m_v3Position.y,
        v3TargetPos.z - m_v3Position.z);

    InitMovementFromAnim(sDirectionSeekSpeed, v3Direction,
        fAdjustEndTime * lbl_806DB990, false);
    fn_800978E8(this, 0);
}

void cFielder::fn_8004E228()
{
    if (m_tFireTimer.m_uPackedTime == 0)
    {
        mUnidentified3D8 = 0;
        mUnidentified3DA = 0;
        EndAction();
    }
    else
    {
        int nTarget = 0x32;
        int nLimit = lbl_806DB8AC;
        if (mUnidentified3DA < -nLimit)
        {
            nTarget = 0x4B;
        }
        else if (mUnidentified3DA > nLimit)
        {
            nTarget = 0x19;
        }
        else if (mUnidentified3DA * mUnidentified3D8 > 0)
        {
            int nSign = -0x19;
            if (mUnidentified3DA > 0)
            {
                nSign = 0x19;
            }
            nTarget = nSign + 0x32;
        }

        s16 nRandom = (s16)nlRandom(0x64);
        mUnidentified3D8 = mUnidentified3D8 + (nTarget - nRandom) / 15;
        if (mUnidentified3D8 > lbl_806DB8B0)
        {
            mUnidentified3D8 = lbl_806DB8B0;
        }
        else if (mUnidentified3D8 < -lbl_806DB8B0)
        {
            mUnidentified3D8 = -lbl_806DB8B0;
        }
        mUnidentified3DA = mUnidentified3DA + mUnidentified3D8;
        SetFacingDirection(mUnidentified3DA + m_aActualFacingDirection, true);
    }
}

void cFielder::fn_8004E438()
{
    fn_8002E340(this);
    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_UNKNOWN_32);

    nlVector3 v3NetPos = m_pTeam->GetOtherNet()->m_v3NetLocation;
    nlVector3 v3Delta;
    nlVec3Sub(v3Delta, v3NetPos, m_v3Position);
    Unknown8(nlVector3ToAngle(v3Delta), false);
    SetFacingDirection(m_aDesiredFacingDirection, true);

    if (m_eCharacterClass == (eCharacterClass)0x0D)
    {
        SetAnimState(0x81, true, 0.2f, false, false);
        InitMovementFromAnim(0, v3Zero, 0.0f, false);
        fn_800EBC84(mUnidentified318, 0x3D267BDF, g_pBall->mUnidentifiedEC,
            "Skillshot", this);
    }
    else if (m_eCharacterClass == (eCharacterClass)0x12)
    {
        SetAnimState(0x81, true, 0.2f, false, false);
        InitMovementFromAnim(0, v3Zero, 0.0f, false);
        fn_800EBC84(mUnidentified318, 0x3D267BDF, g_pBall->mUnidentifiedEC,
            "Skillshot", this);
    }
    else if (m_eCharacterClass == (eCharacterClass)0x13)
    {
        SetAnimState(0x81, true, 0.2f, false, false);
        fn_8019AA00(mUnidentified420, this);
        InitMovementFromAnim(0, v3Zero, 0.0f, false);
        fn_801B8FF8(this);
        fn_800EBBFC(mUnidentified318, 0x1D6C8D56, 0, 0);
    }

    bool bUnidentified = lbl_806E0C94->m_eGameState == 5
        || lbl_806E0C94->m_eGameState == 6;
    if (bUnidentified)
    {
        StatsTracker::Instance()->TrackStat(
            (ePlayerStats)4, m_pTeam->m_nSide, m_ID, 1, 0, 0, 0);
    }
}

void cFielder::ActionSquishReact(float fDeltaT)
{
    if (ShouldStartCrossBlend(0x52))
    {
        EndAction();
    }
}

void cFielder::fn_8004E6B4()
{
    if (m_eActionState == ACTION_UNKNOWN_32)
    {
        HammerObject* pProjectile = lbl_806E1608->fn_801AA3AC(-1);
        if (pProjectile != 0)
        {
            fn_801A1B54(pProjectile, this);
            fn_801A1170(
                pProjectile, &GetJointPosition(m_nLeftHandJointIndex));

            float fDistance = lbl_806DB89C
                + lbl_806DB8A0
                    * (float)(pProjectile->_020 % 5);

            nlVector3 v3Target;
            v3Target.x = fDistance * m_m4WorldMatrix.e2[0][0]
                + m_v3Position.x;
            v3Target.y = fDistance * m_m4WorldMatrix.e2[0][1]
                + m_v3Position.y;
            v3Target.z = fDistance * m_m4WorldMatrix.e2[0][2]
                + m_v3Position.z;

            nlVector3 v3Delta;
            nlVec3Sub(v3Delta, v3Target, m_v3Position);

            float fSpeed = pProjectile->_028->m_gravity;
            float fGravity = fn_801A1168(pProjectile)->z;


            float fHeight
                = lbl_806DB8A4
                * nlSqrt(v3Delta.x * v3Delta.x + v3Delta.y * v3Delta.y,
                    true);

            int nNumRoots;
            float fX1, fX2;
            SolveQuadratic(
                0.5f * fSpeed, fHeight, fGravity, nNumRoots, fX1, fX2);

            float fRoot;
            if (fX1 > 0.0f)
            {
                fRoot = fX1;
            }
            else if (fX2 > 0.0f)
            {
                fRoot = fX2;
            }
            else
            {
                fRoot = fX2;
            }
            float fScale = 1.0f / fRoot;
            v3Delta.x *= fScale;
            v3Delta.y *= fScale;
            v3Delta.z = fHeight;
            fn_801A117C(pProjectile, &v3Delta);
        }
    }
}

void cFielder::fn_8004E8B8()
{
    if (m_eActionState == ACTION_UNKNOWN_32)
    {
        muInvincibleStatus |= 1;
        if (m_eCharacterClass == (eCharacterClass)0x13)
        {
            fn_801B90F8(this);
            fn_800EBC84(mUnidentified318, 0x3D267BDF,
                g_pBall->mUnidentifiedEC, "Skillshot", this);
        }
    }
}

void cFielder::fn_8004EA9C()
{
    if (m_eActionState == ACTION_UNKNOWN_32)
    {
        fn_80038158(this, 0);
    }
}

void cFielder::fn_8004E92C()
{
    bool bUnidentified = g_pGame->IsGameplayOrOvertime();
    if (bUnidentified)
    {
        if (m_pBall != 0 && m_eActionState == ACTION_UNKNOWN_32)
        {
            DoResetShotMeter(0.0f);
            m_pShotMeter->CalcOneTimerValue(this, false);

            if (m_eCharacterClass == (eCharacterClass)0x12)
            {
                g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;

                float fBallX = (float)fabs(g_pBall->m_v3Position.x);
                float fRadius = g_pBall->m_pPhysicsBall->GetRadius();
                float fGoalLineX = cField::GetGoalLineX(1U);
                if (fBallX < fGoalLineX + fRadius)
                {
                    fn_80037AC8(this, 0);
                }
                else
                {
                    ReleaseBall(0);
                }
                fn_801B75C8(this, 1, 0, 0, 0);
            }
            else if (m_eCharacterClass == (eCharacterClass)0x0D)
            {
                fn_80037AC8(this, 0);
                fn_801B75C8(this, 1, 0, 0, 0);
            }
        }
    }
    else
    {
        if (m_pBall != 0)
        {
            ReleaseBall(0);
        }
    }
}

void cFielder::fn_8004EAB4(float fDeltaT)
{
    if (m_eCharacterClass == (eCharacterClass)0x13)
    {
        mUnidentified420->mUnidentified10 = GetJointPosition(m_nBallJointIndex);
        mUnidentified420->mUnidentified1C = m_v3Velocity;
    }

    if (ShouldStartCrossBlend(4))
    {
        EndAction();
    }
}

void cFielder::fn_8004EC40()
{
    if (!mbTangible)
    {
        fn_80039CF0(this, 0);
    }

    if (m_eCharacterClass == (eCharacterClass)0x12)
    {
        fn_80038158(this, 0);
        g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;
    }
    else if (m_eCharacterClass == (eCharacterClass)0x13)
    {
        fn_80038158(this, 0);

        if (mUnidentified420->mUnidentified38)
        {
            UnidentifiedSkillshotNode* pNode;
            lbl_805712F8.Allocate(pNode);
            pNode->mUnidentified0 = this;
            pNode->mUnidentified4 = mUnidentified420;
            fn_80147F2C(pNode);
        }

        SetVelocity(v3Zero);
        mUnidentified054 = v3Zero;
    }

    fn_800EC12C(0x3D267BDF, this);
}

void cFielder::fn_8004ED64()
{
    fn_8002E340(this);
    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction((eFielderActionState)0x21);
    InitMovementCoast();

    mUnidentified410 = m_v3Position;
    mUnidentified41C = true;

    g_pBall->m_tNoPickupTimer.SetSeconds(0.5f);
    SetNoPickUpTime(0.5f);

    Goalie* pGoalie = m_pTeam->GetOtherTeam()->GetGoalie();
    pGoalie->m_pPhysicsCharacter->m_CanCollideWithBall = 0;
    fn_8003C560(this, 0, 1);

    g_pBall->m_pPhysicsBall->mbCanCollidePlayer = false;
    g_pBall->m_pPhysicsBall->mbCanCollideGoalie = false;
}

void cFielder::fn_8004EE48(float fDeltaT)
{
    if (mUnidentified41C)
    {
        float fGoalLineX = cField::GetGoalLineX(1U) - 0.5f;

        nlVector3 v3BallPos = g_pBall->m_v3Position;
        float fAbsX = (float)fabs(v3BallPos.x);

        nlVector3 v3Delta;
        v3Delta.y = mUnidentified410.y - v3BallPos.y;
        v3Delta.x = mUnidentified410.x - v3BallPos.x;
        float fDistSq
            = v3Delta.x * v3Delta.x + v3Delta.y * v3Delta.y;

        if (fn_800167A8(g_pBall)
            && !(fDistSq > lbl_806DB8A8 * lbl_806DB8A8)
            && !(fAbsX > fGoalLineX))
        {
            return;
        }

        mUnidentified41C = false;
        SetAnimState(0x81, true, 0.2f, false, false);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);
        fn_801B75C8(this, 1, 0, 0, 0);
        SetNoPickUpTime(0.2f);

        Goalie* pGoalie = m_pTeam->GetOtherTeam()->GetGoalie();
        pGoalie->m_pPhysicsCharacter->m_CanCollideWithBall = 1;

        g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;
        g_pBall->m_pPhysicsBall->mbCanCollidePlayer = true;
    }
    else
    {
        nlVector3 v3Position = m_v3Position;
        v3Position.z
            *= 1.0f - m_pCurrentAnimController->m_fTime;
        if (v3Position.z < 0.0f)
        {
            v3Position.z = 0.0f;
        }
        SetPosition(v3Position);

        if (m_pCurrentAnimController->TestTrigger(0.05f))
        {
            fn_80039CF0(this, 0);
        }

        if (ShouldStartCrossBlend(0))
        {
            if (0.0f != v3Position.z)
            {
                v3Position.z = 0.0f;
                SetPosition(v3Position);
            }
            EndAction();
        }
    }
}

void cFielder::fn_8004F180()
{
    nlVector3 v3Position = m_v3Position;
    v3Position.z = 0.0f;
    SetPosition(v3Position);

    mUnidentified17C = true;
    fn_80039CF0(this, 0);
    fn_80038158(this, 0);
    mUnidentified424 = false;
}

void cFielder::fn_8004F204()
{
    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_UNKNOWN_34);
    SetAnimState(0, false, 0.0f, false, false);
    InitMovementNone(0.0f, 0.0f);

    nlVector3 v3Position = m_v3Position;
    v3Position.z = lbl_806E3624;
    SetPosition(v3Position);
    SetVelocity(v3Zero);

    fn_8003C560(this, 1, 0);

    mUnidentified17C = false;
    mUnidentified178 = 1.0f;
    SetNoPickUpTime(1.0f);
    mUnidentified424 = false;
}

void cFielder::fn_8004F2FC(float fDeltaT)
{
    if (mUnidentified424)
    {
        switch (m_eAnimID)
        {
        case 0x7C:
        {
            if (m_v3Velocity.z < 0.0f && m_v3Position.z <= 0.1f)
            {
                nlVector3 v3Position = m_v3Position;
                v3Position.z = 0.0f;
                SetPosition(v3Position);
                SetVelocity(v3Zero);
                SetAnimState(0x7D, true, 0.2f, false, false);
                InitMovementFromAnim(0, v3Zero, 1.0f, false);
            }
            else
            {
                nlVector3 v3Velocity = m_v3Velocity;
                float fDamping = 1.0f - 0.5f * fDeltaT;
                v3Velocity.x *= fDamping;
                v3Velocity.y *= fDamping;
                v3Velocity.z = -30.0f * fDeltaT + v3Velocity.z;
                SetVelocity(v3Velocity);
                m_v3Position.z += v3Velocity.z * fDeltaT;
            }
            break;
        }
        case 0x50:
        case 0x7D:
            if (ShouldStartCrossBlend(4))
            {
                fn_8003C560(this, 1, 0);
                EndAction();
            }
            break;
        }
    }
}

void cFielder::InitActionWait()
{
    SetAction(ACTION_WAIT);
    SetAnimState(0, true, 0.2f, false, false);
    InitMovementNone(0.0f, 0.0f);
    Unknown8(m_aActualFacingDirection, false);
}

void cFielder::ActionWait(float fDeltaT)
{
}

void cFielder::fn_8004E11C(float fParam)
{
    Unknown8(m_aActualFacingDirection, false);
    fn_80097358(this, fParam);
    InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
    SetAction(ACTION_UNKNOWN_31);
    SetAnimState(0x80, false, 0.3f, false, false);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    m_pCurrentAnimController->m_fPlaybackSpeedScale = 1.5f;
    mUnidentified3D8 = 0;
    mUnidentified3DA = 0;

    bool bHasPad = GetGlobalPad() != 0;
    if (bHasPad == true)
    {
        fn_8009591C(this, false);
    }
}

extern "C" float fn_8004F58C(void)
{
    return cNet::m_fNetPostRadius;
}

extern "C" void fn_8004F594(int category, const char* format, ...)
{
}

bool gbUseTurboCharging = true;

extern const char* lbl_806E1E90;

static TweakValueBoolImpl_804F4538 s_UseTurboChargingTweak(
    "Game/Gameplay/Charging/Turbo", "gbUseTurboCharging",
    &gbUseTurboCharging, true);

u16 g_IdleTurnCompletionDelta = (u16)(65536.0f / 36.0f);

// The original template identities of these common weak pool statics are
// not yet known.
struct UnidentifiedPoolEntry14
{
    u8 mUnknown[0x14];
};

struct UnidentifiedPoolEntry40
{
    u8 mUnknown[0x40];
};

template <typename T>
struct UnidentifiedPoolStorage
{
    static SlotPool<UnidentifiedPoolEntry14> pool14;
    static SlotPool<UnidentifiedPoolEntry40> pool40;
};

template <typename T>
SlotPool<UnidentifiedPoolEntry14> UnidentifiedPoolStorage<T>::pool14(
    0x10, 0x10);
template <typename T>
SlotPool<UnidentifiedPoolEntry40> UnidentifiedPoolStorage<T>::pool40(
    0x10, 0x10);

// The original type identity of this common weak static is not yet known.
struct UnidentifiedStaticState
{
    UnidentifiedStaticState()
        : value(0)
    {
    }

    void* value;
};

template <typename T>
struct UnidentifiedStaticStorage
{
    static UnidentifiedStaticState state;
};

template <typename T>
UnidentifiedStaticState UnidentifiedStaticStorage<T>::state;

struct UnidentifiedStaticTag;

template struct UnidentifiedPoolStorage<UnidentifiedStaticTag>;
template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;

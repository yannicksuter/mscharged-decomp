#include <stddef.h>
#include <math.h>

#include "Game/Ball.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/DesireReceivePass.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/ShotMeter.h"
#include "Game/BallTrail.h"
#include "Game/CharacterTweaks.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/Drawable/DrawableObj.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Event.h"
#include "Game/Field.h"
#include "Game/FixedUpdateTask.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/Goalie.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/ObjectBlur.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Player.h"
#include "Game/PoseAccumulator.h"
#include "Game/Render/NPCManager.h"
#include "Game/SHierarchy.h"
#include "Game/Sys/audio.h"
#include "Game/Team.h"
#include "NL/nlAVLTree.h"
#include "NL/nlMain.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/utility.h"
#include "unclassified/tu_80199E84.h"
#include "unclassified/tu_801A5F10.h"
#include "Game/DB/StadiumInfo.h"

struct DebugFieldType
{
    unsigned short size;
    unsigned short unknown;
    void* writer;
};

struct UnidentifiedBallRuntime
{
    u8 mUnidentified00[0x04];
    nlVector3* m_pPosition;
    u8 mUnidentified08[0x14];
    union
    {
        unsigned int mUnidentified1C;
        struct
        {
            unsigned int mUnidentified1C00 : 18;
            unsigned int mUnidentified1CFlag : 1;
            unsigned int mUnidentified1C13 : 13;
        } mUnidentified1CBits;
    };
    u8 mUnidentified20[0x24];
    unsigned int mUnidentified44;
};

struct UnidentifiedGameState
{
    u8 mUnidentified00[0x28];
    u32 mUnidentified28;
    u32 mUnidentified2C;
    u32 mUnidentified30;
    u8 mUnidentified34[0x0C];
    bool mUnidentified40;
};

typedef nlAVLTree<unsigned int, UnidentifiedEventBase*,
    DefaultKeyCompare<unsigned int> >
    UnidentifiedEventRegistry;

class UnidentifiedTypedEvent0 : public UnidentifiedEventBase
{
public:
    UnidentifiedTypedEvent0(const char* name, int length)
        : UnidentifiedEventBase(name, length)
    {
    }

    virtual ~UnidentifiedTypedEvent0() { }
    virtual void Disconnect(void*) = 0;
    virtual void Add(Function<FnVoidVoid>&, unsigned int, int) = 0;
};

template <typename P1, typename P2>
class UnidentifiedTypedEvent2 : public UnidentifiedEventBase
{
public:
    UnidentifiedTypedEvent2(const char* name, int length)
        : UnidentifiedEventBase(name, length)
    {
    }

    virtual ~UnidentifiedTypedEvent2() { }
    virtual void Disconnect(void*) = 0;
    virtual void Add(Function2<void, P1, P2>&, unsigned int, int) = 0;
};

template <typename P1, typename P2>
struct UnidentifiedListener2 : public UnidentifiedConnection
{
    Function2<void, P1, P2> callback;
};

template <typename P1, typename P2>
class UnidentifiedEvent2View : public UnidentifiedEventBase
{
    typedef UnidentifiedListener2<P1, P2> Listener;
    typedef DLListEntry<Listener> ListenerEntry;

public:
    void UnidentifiedDeliver(P1 p1, P2 p2)
    {
        nlDLListIterator<Listener> iterator = mListeners.Begin();
        while (iterator.hasNext())
        {
            Listener* listener = &*iterator;
            ListenerEntry* currentEntry = iterator.CurrentEntry();
            this->mCurrentConnection = listener;

            if ((listener->mFlags >> 31) != 0)
            {
                listener->callback(p1, p2);
                iterator = mListeners.Begin();
                iterator.m_Curr = currentEntry;
            }

            iterator.next();
            if (((listener->mFlags >> 29) & 1) != 0)
            {
                ListenerEntry* entry = mListeners.Begin(
                    (ListenerEntry*)((char*)listener - 8)).CurrentEntry();
                nlDLRingRemove(&mListeners.m_Head, entry);
                if (entry != NULL)
                {
                    entry->entry.~Listener();
                }
                mListeners.m_Allocator.DeleteEntry(entry);
            }
        }
        this->mCurrentConnection = 0;
    }

private:
    DLListContainerBase<Listener, BasicSlotPool<ListenerEntry> > mListeners;
};

extern "C" DebugFieldType lbl_80533C98[];
extern "C" LiveBallTrail lbl_8056B518[];
extern "C" unsigned int lbl_806E0C10;
extern "C" UnidentifiedEventRegistry* lbl_806E1D90;
extern "C" void fn_8001847C(cBall*, bool);
extern "C" float fn_8002BE64(PlayerTweaks*);
extern "C" float fn_8002BFA8(PlayerTweaks*, float);
extern "C" bool fn_8002D92C(nlVector3*, bool, float);
extern "C" void fn_80031A30(cFielder*, int, float);
extern "C" void fn_80035544(cFielder*);
extern "C" void fn_80036594(cFielder*, cFielder*, int);
extern "C" bool fn_80038538(cFielder*);
extern "C" bool fn_80038660(cFielder*);
extern "C" bool fn_8003886C(cFielder*);
extern "C" void fn_8003CAA8(cFielder*, cPlayer*);
extern "C" PlayerTweaks* fn_8003E6E4(cFielder*);
extern "C" bool fn_8003E74C(cFielder*);
extern "C" float fn_800DEFD4(cFielder*);
extern "C" void fn_800156F8(cBall*, cPlayer*);
extern "C" void fn_80017448(cBall*, float);
extern "C" void fn_80017F18(cBall*);
extern "C" void fn_8001929C();
extern "C" void fn_800193A0(void*);
extern "C" void fn_800194A4(void*);
extern "C" void fn_800195D8();
extern "C" void fn_800196FC(void*);
extern "C" void fn_80019718(void*);
extern "C" void fn_80019814(void*);
extern "C" void fn_80019910(PhysicsPatch*);
extern "C" void fn_80019F10(void*);
extern "C" void fn_8001A00C(void*);
extern "C" void fn_8001A108(int, int);
extern "C" void fn_8001AA0C(LiveBallTrail*, bool);
extern "C" void fn_8019A434(State_80199E84*, bool);
extern "C" void fn_801BDDE4();
extern "C" void fn_801BDF08(int);
extern "C" void fn_80097358(cPlayer*, float);
extern "C" void fn_801B79A4(const char*, int);
extern "C" void fn_801B7A28(cBall*);
extern "C" void fn_801B9904(unsigned long);
extern "C" void fn_801B9EAC(cBall*, nlVector3*, bool);
extern "C" void fn_801B9FD0(cBall*, bool);
extern "C" void fn_80139D1C(int, cGlobalPad*);
extern "C" void fn_802ECC54(void*, void*);
extern "C" void fn_802B5370(
    nlQuaternion&, const nlVector3&, unsigned short);
extern "C" DrawableObject* fn_8027638C(unsigned int);
extern "C" DrawableObject* fn_8027725C(unsigned long);
extern "C" UnidentifiedBallRuntime* fn_802ECB68(void*);
extern "C" unsigned short fn_80338EBC(DebugWriteCache*, const char*);
extern "C" void fn_80338F78(DebugWriteCache*);
extern "C" void fn_80338F88(
    DebugWriteCache*, int, unsigned short, unsigned int, const char*);
extern "C" void* fn_8033930C(
    DebugWriteCache*, unsigned short, void*, unsigned int);
extern "C" void fn_80339450(
    DebugWriteCache*, unsigned short, void*, void*);

float Exp(float);

cBall* g_pBall = NULL;
unsigned char lbl_806E0BC4;
float lbl_806E0BC8;
unsigned char lbl_806E0BCC;
float lbl_806E0BD0;
float lbl_806E0BD4;
float lbl_806E0BD8;
bool lbl_806E0BDC;

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };
static const char szPerfectPassBallBlurTexture[]
    = "global/perfectpassstreak";
static const char sUnidentifiedPerfectPassBallBlurTexture2[]
    = "global/perfectpass2streak";
static const char sUnidentifiedPerfectPassBallBlurTexture3[]
    = "global/perfectpass3streak";
static const char sUnidentifiedPerfectPassBallBlurTexture4[]
    = "global/perfectpass4streak";
static const char sUnidentifiedShootToScoreBallBlurTexture0[]
    = "global/bowsershoottoscorestreak";
static const char sUnidentifiedShootToScoreBallBlurTexture1[]
    = "global/bowserjrshoottoscorestreak";
static const char szDaisyShootToScoreBallBlurTexture[]
    = "global/daisyshoottoscorestreak";
static const char szDonkeyKongShootToScoreBallBlurTexture[]
    = "global/dkshoottoscorestreak";
static const char sUnidentifiedShootToScoreBallBlurTexture2[]
    = "global/diddykongshoottoscorestreak";
static const char szLuigiShootToScoreBallBlurTexture[]
    = "global/luigishoottoscorestreak";
static const char szMarioShootToScoreBallBlurTexture[]
    = "global/marioshoottoscorestreak";
static const char szPeachShootToScoreBallBlurTexture[]
    = "global/peachshoottoscorestreak";
static const char sUnidentifiedShootToScoreBallBlurTexture3[]
    = "global/peteyshoottoscorestreak";
static const char szWaluigiShootToScoreBallBlurTexture[]
    = "global/washoottoscorestreak";
static const char szWarioShootToScoreBallBlurTexture[]
    = "global/warioshoottoscorestreak";
static const char szYoshiShootToScoreBallBlurTexture[]
    = "global/yoshishoottoscorestreak";
extern const nlVector3 lbl_804DBE30;
static const nlVector3 lbl_804DBE48 = { 0.0f, 1.0f, 0.0f };
static nlMatrix3 m3Ident
    = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };

static float lbl_806DB514 = 2.5f;
static float fShootToScoreBallBlurWidth = 2.2f;
static int nShootToScoreBallBlurLength = 150;
extern float lbl_806DB518;
extern float lbl_806DB51C;
extern float lbl_806DB520;
extern float lbl_806DB524;
extern float lbl_806DB528;
extern float lbl_806DB52C;
extern float lbl_806DB530;
extern float lbl_806DB534;
extern float lbl_806DB538;
extern float lbl_806DB53C;
extern float lbl_806DB540;
extern unsigned char lbl_806DB544;
extern unsigned char lbl_806DB500;
extern float lbl_806DB504;
extern float lbl_806DB508;
extern float lbl_806DB50C;
extern float lbl_806DB510;
extern float lbl_806DB548;
extern float lbl_806DB54C;
extern float lbl_806DB558;
extern float lbl_806DB55C;
extern float lbl_806DB560;
extern float lbl_806DB564;
extern float lbl_806DB568;
extern float lbl_806DB56C;
extern float lbl_806DB578;
extern float lbl_806DB57C;
extern float lbl_806DB580;
extern float lbl_806DB584;
extern float lbl_806DB588;
extern float lbl_806DB58C;
extern float lbl_806DB590;
extern float lbl_806DB594;
extern float lbl_806DB598;
extern float lbl_806DB59C;
extern float lbl_806DB5A0;
extern float lbl_806DB5A4;
extern bool lbl_806DB5A8;
extern float lbl_806DB5AC;
extern float lbl_806DB5B0;
extern "C" EffectsGroup* fn_802E7CDC(
    EmissionManager*, const char*);
extern "C" EmissionController* fn_802E7FE4(
    EmissionManager*, EffectsGroup*, int, bool, bool);
extern "C" void fn_801BE950(EmissionController&);
extern float lbl_806E31C0;
extern float lbl_806E31C4;
extern float lbl_806E31C8;
extern float lbl_806E31CC;
extern float lbl_806E31D0;
extern float lbl_806E31D4;
extern float lbl_806E31D8;
extern float lbl_806E31DC;
extern float lbl_806E31E0;
static unsigned short lbl_806DB5C0 = 0xFFFF;

cBall::cBall()
    : m_tLightningTimer(0.0f)
    , m_tShotTimer(0.0f)
    , m_tNoPickupTimer(0.0f)
    , m_tPassTargetTimer(0.0f)
    , mtStuckInRiotTimer(0.0f)
    , mtNoChargeLossTimer(0.0f)
    , mtShotClockTimer(0.0f)
{
    m_bVisible = true;
    m_bBallPathChangeCount = 0;
    m_bBallDeflectCount = 0;
    m_fTotalPassTime = 0.0f;
    m_uGoalType = 4;
    m_uVoiceID = 0;
    m_CurrentGlowEffect = 0;
    mfChargeValue = 0.0f;
    mfSkillShotTime = 0.0f;
    meBallState = 0;
    mePrevBallState = 0;
    m_pOwner = NULL;
    m_pPrevOwner = NULL;
    m_pLastTouch = NULL;
    m_pPassTarget = NULL;
    m_pShooter = NULL;
    mpDamageTarget = NULL;
    m_iConsecutiveVolleyPasses = 0;

    m_tNoPickupTimer.SetSeconds(0.0f);
    m_tLightningTimer.SetSeconds(0.0f);
    m_tShotTimer.SetSeconds(0.0f);
    m_tPassTargetTimer.SetSeconds(0.0f);
    mtNoChargeLossTimer.SetSeconds(0.0f);
    mtStuckInRiotTimer.SetSeconds(0.0f);
    mtShotClockTimer.SetSeconds(0.0f);

    mnShotClockTeam = -1;
    mbStuckInRiotDone = false;
    mbBallOnFire = false;
    mbBallFrozen = false;

    m_v3Position.x = 0.0f;
    m_v3Position.y = 0.0f;
    m_v3Position.z = 0.18f;
    m_v3PrevPosition = m_v3Position;
    m_v3PassIntercept.x = 0.0f;
    m_v3PassIntercept.y = 0.0f;
    m_v3PassIntercept.z = 0.0f;
    m_qOrientation.z = 0.0f;
    m_qOrientation.y = 0.0f;
    m_qOrientation.x = 0.0f;
    m_qOrientation.w = 1.0f;
    m_v3Velocity.x = 0.0f;
    m_v3Velocity.y = 0.0f;
    m_v3Velocity.z = 0.0f;
    m_v3ShotTarget.x = 0.0f;
    m_v3ShotTarget.y = 0.0f;
    m_v3ShotTarget.z = 0.0f;
    m_v3ShotOrigin.x = 0.0f;
    m_v3ShotOrigin.y = 0.0f;
    m_v3ShotOrigin.z = 0.0f;

    m_pBlurHandler = NULL;
    mUnidentifiedF0 = 0;
    m_pDrawableBall = fn_8027725C(nlStringHash("gameplay/ball"));

    m_pPhysicsBall = new (8, false) PhysicsAIBall(0.18f);
    m_pPhysicsBall->m_pAIBall = this;
    m_pPhysicsBall->SetPosition(
        m_v3Position, PhysicsObject::WORLD_COORDINATES);
    m_v3ShotOrigin = m_v3Position;
    m_pPhysicsBall->SetLinearVelocity(m_v3Velocity);
    m_pPhysicsBall->SetAngularVelocity(v3Zero);

    mUnidentifiedEC = fn_802ECB68(lbl_806E201C);
    mUnidentifiedEC->mUnidentified44 |= 0x00800000;
    mUnidentifiedEC->m_pPosition = &m_v3Position;
    mUnidentifiedEC->mUnidentified1C |= 0x8000;
}

cBall::~cBall()
{
    fn_80015C38(this, 0);
    ClearBallEffects();

    if (lbl_806E0BCC || GameInfoManager::Instance()->IsRule0x4Equal5())
    {
        mfChargeValue = 4.0f;
    }
    else
    {
        mfChargeValue = 0.0f;
    }

    float fMaxCharge = lbl_806DB510 * 4.0f;
    if (mfChargeValue >= fMaxCharge)
    {
        mfChargeValue = fMaxCharge;
    }
    else if (mfChargeValue < 0.0f)
    {
        mfChargeValue = 0.0f;
    }

    fn_801B7A28(this);
    fn_800EC12C(mUnidentifiedF0, this);
    mUnidentifiedF0 = 0;
    fn_802ECC54(lbl_806E201C, mUnidentifiedEC);
    delete m_pPhysicsBall;
}

void cBall::ClearOwner()
{
    m_pPrevOwner = m_pOwner;
    m_pOwner->fn_80096CDC(NULL);
    m_pOwner = NULL;
    m_pPhysicsBall->EnableCollisions();
    m_v3PrevPosition = m_v3Position;
    m_pPhysicsBall->GetPosition(&m_v3Position);
    m_pPhysicsBall->GetLinearVelocity(&m_v3Velocity);
    ++m_bBallPathChangeCount;
}

void cBall::ClearBallEffects()
{
    if (m_pBlurHandler != NULL)
    {
        m_pBlurHandler->Die(0.25f);
        m_pBlurHandler = NULL;
    }
    fn_801B79A4("skillshot_ball_meteor", 0);
    fn_801B79A4("skillshot_ball_drybones", 0);
    fn_801B79A4("skillshot_ball_boo", 0);
}

void cBall::CollideWithCharacterCallback(
    cPlayer* pCharacter, const nlVector3& v3PreBallVelocity)
{
    bool bCanDamage;
    switch (meBallState)
    {
    case 6:
        bCanDamage = nlSqrt(m_v3Velocity.GetLengthSq3D(), true)
            > lbl_806DB578;
        break;
    case 8:
        bCanDamage = true;
        break;
    default:
        bCanDamage = false;
        break;
    }

    if (bCanDamage && pCharacter->m_eClassType == FIELDER)
    {
        cFielder* pCharacterFielder = (cFielder*)pCharacter;
        nlVector3 v3BallDirection;
        nlVec3Sub(v3BallDirection, m_v3Position,
            m_pPrevOwner->m_v3Position);
        unsigned short aBallDirection
            = (unsigned short)(int)(lbl_806E31C4
                * nlATan2f(v3BallDirection.y, v3BallDirection.x));
        bool bReactToHit = true;
        bool bDeflectBall = true;

        bool bInvincible = !fn_8003886C(pCharacterFielder)
            && (pCharacterFielder->muInvincibleStatus & 4) != 0;
        if (!bInvincible)
        {
            if (pCharacter->m_pBall != NULL)
            {
                pCharacter->ReleaseBall(0);
            }

            cPlayer* pShooter = m_pShooter;
            bool bLightningBall = m_tLightningTimer.m_uPackedTime != 0
                && meBallState == 8 && pShooter != NULL;
            if (bLightningBall
                && pShooter->m_eCharacterClass == (eCharacterClass)0xF)
            {
                fn_80097358(pCharacter, lbl_806E31C8);
            }
            else if (bLightningBall
                && pShooter->m_eCharacterClass == (eCharacterClass)0x11)
            {
                fn_800156F8(this, pShooter);
                pCharacterFielder->fn_800451B0(pShooter->m_v3Position);
                if (GetStadiumUnknown0x10(
                        GameInfoManager::Instance()->GetStadium()))
                {
                    unsigned long soundID = 0xCE269987;
                    if (pCharacter->m_pTeam->m_nSide == 0)
                    {
                        soundID = 0x5089F33E;
                    }
                    fn_800ED92C(soundID);
                }
                bReactToHit = false;
                bDeflectBall = false;
            }
            else if (mbBallOnFire)
            {
                fn_80097358(pCharacter, lbl_806E31C8);
            }

            if (bReactToHit)
            {
                int nReact = 0;
                float fSpeed
                    = nlSqrt(m_v3Velocity.GetLengthSq3D(), true);
                if (fSpeed > lbl_806DB580)
                {
                    nReact = 2;
                }
                else if (fSpeed > lbl_806DB57C)
                {
                    nReact = 1;
                }
                pCharacterFielder->fn_80047240(m_pPrevOwner,
                    aBallDirection, nReact, false, false);
            }
        }
        else
        {
            pCharacter->SetNoPickUpTime(lbl_806E31CC);
        }

        if (bDeflectBall)
        {
            if (m_pOwner != NULL)
            {
                fn_80015C38(this, 2);
            }
            else
            {
                bool bPassTarget = (meBallState == 5
                                       || meBallState == 3)
                    && m_pPassTarget != NULL;
                if (bPassTarget
                    && fn_800DEFD4((cFielder*)m_pPassTarget))
                {
                    cFielder* pFielder;
                    if (m_pPassTarget != NULL
                        && m_pPassTarget->m_eClassType == FIELDER)
                    {
                        pFielder = (cFielder*)m_pPassTarget;
                    }
                    else
                    {
                        pFielder = NULL;
                    }
                    DesireReceivePass* pReceivePass
                        = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
                    if (pReceivePass == NULL
                        || !pReceivePass->UnidentifiedIsActive()
                        || pReceivePass->meDesireSubState != 0)
                    {
                        fn_80015C38(this, 0);
                    }
                }
                else
                {
                    fn_80015C38(this, 0);
                }
            }

            m_tNoPickupTimer.SetSeconds(0.0f);

            nlVector3 v3Velocity;
            if (nlVec3DotProduct(v3BallDirection, m_v3Velocity) > 0.0f)
            {
                nlVec3Scale(v3Velocity, m_v3Velocity, lbl_806E31D0);
                nlVector3 v3CharacterToBall;
                nlVec3Sub(v3CharacterToBall, m_v3Position,
                    pCharacter->m_v3Position);
                if (nlVec3DotProduct(v3CharacterToBall, v3Velocity) < 0.0f)
                {
                    m_v3Position = m_v3PrevPosition;
                    m_pPhysicsBall->SetPosition(m_v3PrevPosition,
                        PhysicsObject::WORLD_COORDINATES);
                    m_pPhysicsBall->SetRotation(m3Ident);
                    FakeBallWorld::InvalidateBallCache();
                    ++m_bBallPathChangeCount;
                }
            }
            else
            {
                nlVec3Scale(v3Velocity, m_v3Velocity, lbl_806E31D4);
            }

            v3Velocity.z += lbl_806E31C0 + nlRandomf(lbl_806E31D8);
            v3Velocity.y += lbl_806E31DC + nlRandomf(lbl_806E31E0);

            nlVector3 v3AngularVelocity;
            m_pPhysicsBall->GetAngularVelocity(&v3AngularVelocity);
            SetVelocity(v3Velocity, SPINTYPE_PARAMETER,
                &v3AngularVelocity);
        }
    }

    if (meBallState == 4 && pCharacter->m_eClassType == FIELDER)
    {
        cFielder* pFielder = (cFielder*)pCharacter;
        if (pFielder->m_eActionState < ACTION_LOOSE_BALL_PASS
            || pFielder->m_eActionState > ACTION_LOOSE_BALL_SHOT)
        {
            if (m_pOwner != NULL)
            {
                fn_80015C38(this, 2);
            }
            else
            {
                bool bPassTarget = (meBallState == 5
                                       || meBallState == 3)
                    && m_pPassTarget != NULL;
                if (bPassTarget
                    && fn_800DEFD4((cFielder*)m_pPassTarget))
                {
                    cFielder* pPassTarget;
                    if (m_pPassTarget != NULL
                        && m_pPassTarget->m_eClassType == FIELDER)
                    {
                        pPassTarget = (cFielder*)m_pPassTarget;
                    }
                    else
                    {
                        pPassTarget = NULL;
                    }
                    DesireReceivePass* pReceivePass
                        = (DesireReceivePass*)fn_8002E08C(
                            pPassTarget, 22);
                    if (pReceivePass == NULL
                        || !pReceivePass->UnidentifiedIsActive()
                        || pReceivePass->meDesireSubState != 0)
                    {
                        fn_80015C38(this, 0);
                    }
                }
                else
                {
                    fn_80015C38(this, 0);
                }
            }
        }
    }

    bool bPassTarget = (meBallState == 5 || meBallState == 3)
        && m_pPassTarget != NULL;
    if (m_tShotTimer.m_uPackedTime != 0 || bPassTarget)
    {
        if (pCharacter->m_eClassType == FIELDER)
        {
            m_uGoalType = 3;
        }

        if (m_pOwner != NULL)
        {
            fn_80015C38(this, 2);
        }
        else
        {
            bool bHasPassTarget = (meBallState == 5
                                      || meBallState == 3)
                && m_pPassTarget != NULL;
            if (bHasPassTarget
                && fn_800DEFD4((cFielder*)m_pPassTarget))
            {
                cFielder* pFielder;
                if (m_pPassTarget != NULL
                    && m_pPassTarget->m_eClassType == FIELDER)
                {
                    pFielder = (cFielder*)m_pPassTarget;
                }
                else
                {
                    pFielder = NULL;
                }
                DesireReceivePass* pReceivePass
                    = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
                if (pReceivePass == NULL
                    || !pReceivePass->UnidentifiedIsActive()
                    || pReceivePass->meDesireSubState != 0)
                {
                    fn_80015C38(this, 0);
                }
            }
            else
            {
                fn_80015C38(this, 0);
            }
        }
    }

    cFielder* pOwnerFielder = (cFielder*)m_pOwner;
    if (pOwnerFielder == NULL
        || pOwnerFielder->m_eClassType != FIELDER)
    {
        pOwnerFielder = NULL;
    }

    if (pOwnerFielder != NULL && pOwnerFielder != pCharacter
        && pCharacter->m_eClassType == FIELDER)
    {
        cFielder* pCharacterFielder = (cFielder*)pCharacter;
        fn_8003CAA8(pOwnerFielder, pCharacter);

        if (!pCharacterFielder->IsOnSameTeam(pOwnerFielder))
        {
            if (fn_80038660(pCharacterFielder))
            {
                nlVector3 v3ContactLocation
                    = pCharacter->m_v3Position;
                nlVector3 v3PhysicsRadialSpot;
                float fRadius = fn_8002BFA8(
                    fn_8003E6E4(pCharacterFielder),
                    pCharacter->mUnidentified0A0);
                nlPolarToCartesian(v3PhysicsRadialSpot.x,
                    v3PhysicsRadialSpot.y,
                    pCharacter->m_aActualFacingDirection, fRadius);
                v3PhysicsRadialSpot.z = 0.0f;
                nlVec3Add(v3ContactLocation, v3ContactLocation,
                    v3PhysicsRadialSpot);

                s16 nHitterContactLocationFacingDelta
                    = pCharacter->GetFacingDeltaToPosition(
                        v3ContactLocation);
                u16 absFacingDelta
                    = nHitterContactLocationFacingDelta < 0
                    ? -nHitterContactLocationFacingDelta
                    : nHitterContactLocationFacingDelta;
                if (absFacingDelta < 0x2000)
                {
                    if (fn_80038660(pOwnerFielder))
                    {
                        s16 nHitteeContactLocationFacingDelta
                            = pOwnerFielder->GetFacingDeltaToPosition(
                                v3ContactLocation);
                        u16 absOwnerFacingDelta
                            = nHitteeContactLocationFacingDelta < 0
                            ? -nHitteeContactLocationFacingDelta
                            : nHitteeContactLocationFacingDelta;
                        if (absOwnerFacingDelta < 0x2000)
                        {
                            if (fn_8003E6E4(pOwnerFielder)
                                    ->mUnidentified064
                                < fn_8003E6E4(pCharacterFielder)
                                      ->mUnidentified064)
                            {
                                pOwnerFielder->InitActionSlideAttackReact(
                                    pCharacterFielder, false);
                                fn_80035544(pCharacterFielder);
                                pCharacterFielder->PickupBall(g_pBall);
                            }
                            else if (fn_8003E6E4(pOwnerFielder)
                                         ->mUnidentified064
                                > fn_8003E6E4(pCharacterFielder)
                                      ->mUnidentified064)
                            {
                                pCharacterFielder
                                    ->InitActionSlideAttackReact(
                                        pOwnerFielder, false);
                                fn_80035544(pOwnerFielder);
                            }
                            else if (pOwnerFielder->m_fActualSpeed
                                < pCharacterFielder->m_fActualSpeed)
                            {
                                pOwnerFielder->InitActionSlideAttackReact(
                                    pCharacterFielder, false);
                                fn_80035544(pCharacterFielder);
                                pCharacterFielder->PickupBall(g_pBall);
                            }
                            else
                            {
                                pCharacterFielder
                                    ->InitActionSlideAttackReact(
                                        pOwnerFielder, false);
                                fn_80035544(pOwnerFielder);
                            }
                        }
                        else
                        {
                            pOwnerFielder->InitActionSlideAttackReact(
                                pCharacterFielder, false);
                            fn_80036594(
                                pCharacterFielder, pOwnerFielder, 2);
                            fn_80035544(pCharacterFielder);
                            if (pCharacterFielder->CanPickupBall(
                                    g_pBall, false))
                            {
                                pCharacterFielder->PickupBall(g_pBall);
                            }
                        }
                    }
                    else
                    {
                        pOwnerFielder->InitActionSlideAttackReact(
                            pCharacterFielder, false);
                        fn_80036594(
                            pCharacterFielder, pOwnerFielder, 2);
                        fn_80035544(pCharacterFielder);
                        if (pCharacterFielder->CanPickupBall(
                                g_pBall, false))
                        {
                            pCharacterFielder->PickupBall(g_pBall);
                        }
                    }
                }
            }
            else if (fn_80038660(pOwnerFielder)
                && !fn_80038538(pCharacterFielder))
            {
                pCharacterFielder->InitActionSlideAttackReact(
                    pOwnerFielder, false);
                fn_80035544(pOwnerFielder);
            }
        }
        else
        {
            if (fn_80038660(pOwnerFielder)
                && !fn_8003E74C(pOwnerFielder))
            {
                bool bInvincible = !fn_8003886C(pOwnerFielder)
                    && (pOwnerFielder->muInvincibleStatus & 1) != 0;
                if (!bInvincible)
                {
                    pOwnerFielder->fn_8004D238();
                }
            }
            if (fn_80038660(pCharacterFielder)
                && !fn_8003E74C(pCharacterFielder))
            {
                bool bInvincible = !fn_8003886C(pCharacterFielder)
                    && (pCharacterFielder->muInvincibleStatus & 1) != 0;
                if (!bInvincible)
                {
                    pCharacterFielder->fn_8004D238();
                }
            }
        }
    }

    if (m_pOwner == NULL)
    {
        m_pLastTouch = pCharacter;
        FakeBallWorld::InvalidateBallCache();
        ++m_bBallDeflectCount;
    }

    ++m_bBallPathChangeCount;

    if (pCharacter->m_eClassType == FIELDER)
    {
        m_v3ShotOrigin = m_v3Position;
    }
}

static inline void fn_80014494Impl(cBall* pBall)
{
    if (pBall->m_pOwner != NULL)
    {
        fn_80015C38(pBall, 2);
        return;
    }

    bool bPassTarget = (pBall->meBallState == 5
                           || pBall->meBallState == 3)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        if (fn_800DEFD4((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
            if (pReceivePass != NULL
                && pReceivePass->UnidentifiedIsActive()
                && pReceivePass->meDesireSubState == 0)
            {
                return;
            }

            fn_80015C38(pBall, 0);
            return;
        }
    }

    fn_80015C38(pBall, 0);
}

extern "C" void fn_80014494(cBall* pBall)
{
    fn_80014494Impl(pBall);
}

extern "C" void fn_8001458C(cBall* pBall)
{
    if (pBall->meBallState != 10)
    {
        fn_80015C38(pBall, 10);
    }
}

extern "C" void fn_800145A4(cBall* pBall)
{
    if (pBall->m_tNoPickupTimer.m_uPackedTime != 0)
    {
        return;
    }

    if (pBall->meBallState == 4)
    {
        return;
    }

    if (pBall->m_pOwner != NULL)
    {
        fn_80015C38(pBall, 2);
        return;
    }

    bool bPassTarget = (pBall->meBallState == 5
                           || pBall->meBallState == 3)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        if (fn_800DEFD4((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
            if (pReceivePass != NULL
                && pReceivePass->UnidentifiedIsActive()
                && pReceivePass->meDesireSubState == 0)
            {
                return;
            }

            fn_80015C38(pBall, 0);
            return;
        }
    }

    fn_80015C38(pBall, 0);
}

static inline float clampAbove(float minVal, float x)
{
    if (minVal >= x)
    {
        return minVal;
    }
    return x;
}

void cBall::PostPhysicsUpdate(float fDeltaT)
{
    m_v3PrevPosition = m_v3Position;
    m_pPhysicsBall->GetPosition(&m_v3Position);
    m_pPhysicsBall->GetLinearVelocity(&m_v3Velocity);

    bool bCanDamage;
    switch (meBallState)
    {
    case 6:
        if (nlSqrt(m_v3Velocity.GetLengthSq3D(), true)
            > lbl_806DB578)
        {
            bCanDamage = true;
        }
        else
        {
            bCanDamage = false;
        }
        break;
    case 8:
        bCanDamage = true;
        break;
    default:
        bCanDamage = false;
        break;
    }

    if (bCanDamage && mpDamageTarget != NULL)
    {
        nlVector3 v3HitSpot;
        nlVector3 v3CurPos;
        nlVector3 targetDelta;
        nlVector3 currentDelta;
        nlVector3 v3PrevPos;
        float fPercent;
        nlVector3 v3BallVel;
        float fPrevZVel;

        v3HitSpot = mpDamageTarget->GetJointPosition(
            mpDamageTarget->m_pPoseAccumulator->m_BaseSHierarchy
                ->m_nPelvisNodeIndex);
        v3HitSpot.z = clampAbove(0.3f, v3HitSpot.z + 0.05f);

        v3CurPos = m_v3Position;
        v3PrevPos = m_v3PrevPosition;

        if (v3CurPos.z < 0.3f)
        {
            v3CurPos.z = 0.3f;
        }

        if (v3PrevPos.z < 0.3f)
        {
            v3PrevPos.z = 0.3f;
        }

        nlVec3Set(targetDelta, v3HitSpot.x - v3PrevPos.x,
            v3HitSpot.y - v3PrevPos.y, v3HitSpot.z - v3PrevPos.z);
        nlVec3Set(currentDelta, v3CurPos.x - v3PrevPos.x,
            v3CurPos.y - v3PrevPos.y, v3CurPos.z - v3PrevPos.z);

        float targetDist = nlSqrt(targetDelta.GetLengthSq3D(), true);
        float currentDist = nlSqrt(currentDelta.GetLengthSq3D(), true);

        fPercent = 0.5f;
        if (targetDist < currentDist)
        {
            nlVec3Scale(currentDelta, targetDist / targetDist);
        }
        else
        {
            nlVec3Scale(targetDelta, currentDist / targetDist);
        }

        if (targetDist < 5.0f)
        {
            fPercent += 0.5f * (1.0f - targetDist / 5.0f);
        }

        nlVecLerp(currentDelta, currentDelta, targetDelta, fPercent);
        nlVec3Add(v3CurPos, v3PrevPos, currentDelta);

        m_v3Position = v3CurPos;
        m_pPhysicsBall->SetPosition(
            v3CurPos, PhysicsObject::WORLD_COORDINATES);
        m_pPhysicsBall->SetRotation(m3Ident);

        FakeBallWorld::InvalidateBallCache();
        m_bBallPathChangeCount = m_bBallPathChangeCount + 1;

        fPrevZVel = m_v3Velocity.z;
        const nlVector3& ballVelocity = m_v3Velocity;
        float distanceSq = currentDelta.GetLengthSq3D();
        float projectedScale
            = nlVec3DotProduct(ballVelocity, currentDelta) / distanceSq;
        nlVec3Scale(v3BallVel, currentDelta, projectedScale);
        v3BallVel.z = fPrevZVel;

        float speedSq = v3BallVel.GetLengthSq3D();
        if (speedSq < 400.0f)
        {
            float speed = nlSqrt(speedSq, true);
            nlVec3Scale(v3BallVel, 20.0f / speed);
        }

        if (v3CurPos.z < 0.4f && v3BallVel.z < 0.0f)
        {
            v3BallVel.z = 0.0f;
        }

        m_v3Velocity = v3BallVel;
        m_pPhysicsBall->SetLinearVelocity(v3BallVel);
    }

    UpdateOrientation(fDeltaT);

    bool bUnidentified = m_tLightningTimer.m_uPackedTime != 0
        && meBallState == 8 && m_pShooter != NULL
        && m_pShooter->m_eCharacterClass == (eCharacterClass)0x10;
    if (bUnidentified)
    {
        cFielder* pFielder = (cFielder*)m_pShooter;
        if (pFielder->m_eActionState == (eFielderActionState)0x21
            && pFielder->mUnidentified41C)
        {
            nlVector3 v3JointPosition = pFielder->GetJointPosition(
                pFielder->m_nBip01JointIndex_0xA4);
            nlVector3 v3Delta;
            v3Delta.x = v3JointPosition.x - pFielder->m_v3Position.x;
            v3Delta.y = v3JointPosition.y - pFielder->m_v3Position.y;
            float fDistance
                = nlSqrt(v3Delta.x * v3Delta.x + v3Delta.y * v3Delta.y,
                    true);
            float fHeight
                = pFielder->m_v3Position.z - v3JointPosition.z;

            nlVector3 v3Velocity = m_v3Velocity;
            pFielder->SetFacingDirection(
                (unsigned short)(int)(10430.378f
                    * nlATan2f(v3Velocity.y, v3Velocity.x)),
                true);
            pFielder->SetVelocity(v3Velocity);

            v3Velocity.z = 0.0f;
            float fRecipLength
                = nlRecipSqrt(v3Velocity.GetLengthSq3D(), true);
            nlVec3Scale(v3Velocity, v3Velocity, fRecipLength);

            nlVector3 v3Position;
            nlVec3ScaleAdd(
                v3Position, -fDistance, v3Velocity, m_v3Position);
            v3Position.z += fHeight;
            if (v3Position.z < 0.0f)
            {
                v3Position.z = 0.0f;
            }
            pFielder->SetPosition(v3Position);
        }
    }

    if (m_pBlurHandler != NULL)
    {
        m_pBlurHandler->AddViewOrientedPoint(
            m_v3Position, m_v3Velocity);
    }

    KoopaShellObject* pKoopaShell = lbl_806E1608->mUnidentified02C;
    if (pKoopaShell != NULL && pKoopaShell->mVisible)
    {
        pKoopaShell->mVelocity = m_v3Velocity;
        fn_801A65D0(pKoopaShell, m_v3Position);
    }

    State_80199E84* pState = lbl_806E1608->mUnidentified028;
    if (pState != NULL && pState->visible)
    {
        pState->unknown_40 = m_v3Velocity;
        fn_8019A248(pState, m_v3Position);
    }
}

static inline bool fn_80014D38Impl(cBall* pBall)
{
    bool bPassLockedIn = false;
    bool bPassTarget = (pBall->meBallState == 5
                           || pBall->meBallState == 3)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        cPlayer* pPassTarget = pBall->m_pPassTarget;
        cFielder* pFielder;
        if (pPassTarget != NULL
            && pPassTarget->m_eClassType == FIELDER)
        {
            pFielder = (cFielder*)pPassTarget;
        }
        else
        {
            pFielder = NULL;
        }
        DesireReceivePass* pReceivePass
            = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
        if (pReceivePass != NULL
            && pReceivePass->UnidentifiedIsActive())
        {
            bPassLockedIn = pReceivePass->meDesireSubState != 0;
        }
    }

    if (bPassLockedIn != pBall->m_pPhysicsBall->mbPassLockedIn)
    {
        pBall->m_pPhysicsBall->mbPassLockedIn = bPassLockedIn;
        FakeBallWorld::InvalidateBallCache();
        ++pBall->m_bBallPathChangeCount;
    }
    return bPassLockedIn;
}

extern "C" bool fn_80014D38(cBall* pBall)
{
    return fn_80014D38Impl(pBall);
}

extern "C" bool fn_80014E20(cBall* pBall)
{
    switch (pBall->meBallState)
    {
    case 6:
        return nlSqrt(pBall->m_v3Velocity.GetLengthSq3D(), true)
            > lbl_806DB578;
    case 8:
        return true;
    default:
        return false;
    }
}

extern "C" bool fn_80014EA4(
    cBall* pBall, const EffectsGroup* pEffectsGroup)
{
    return EmissionManager::Instance()->IsPlaying(
        (unsigned long)pBall, pEffectsGroup);
}

void cBall::InitiateBallBlur(
    eBallShotEffectType effectType, cPlayer* pPlayer)
{
    if (m_pBlurHandler != NULL)
    {
        BlurManager::DestroyHandler(m_pBlurHandler, 0.1f);
        m_pBlurHandler = NULL;
    }

    switch (effectType)
    {
    case BALL_EFFECT_PERFECT_PASS:
    case BALL_EFFECT_REGULAR_SHOT:
    case BALL_EFFECT_ONETIMER_SHOT:
    case BALL_EFFECT_CHIP_SHOT:
        break;
    default:
        if (mfChargeValue >= 1.0f)
        {
            char textureName[64] = "";
            int nLength;
            if (mfChargeValue < 2.0f)
            {
                nlStrNCpy(textureName,
                    szPerfectPassBallBlurTexture,
                    sizeof(textureName));
                nLength = 8;
            }
            else if (mfChargeValue < 3.0f)
            {
                nlStrNCpy(textureName,
                    sUnidentifiedPerfectPassBallBlurTexture2,
                    sizeof(textureName));
                nLength = 12;
            }
            else if (mfChargeValue < 4.0f)
            {
                nlStrNCpy(textureName,
                    sUnidentifiedPerfectPassBallBlurTexture3,
                    sizeof(textureName));
                nLength = 18;
            }
            else
            {
                nlStrNCpy(textureName,
                    sUnidentifiedPerfectPassBallBlurTexture4,
                    sizeof(textureName));
                nLength = 24;
            }

            m_pBlurHandler = BlurManager::GetNewHandler(
                textureName, 0.18f, nLength, true);
        }
        break;
    }
}

extern "C" void fn_800152B4(cBall* pBall)
{
    static unsigned long sUnidentified0
        = nlStringLowerHash("ball_shot_windup_trans_0_1");
    static unsigned long sUnidentified1
        = nlStringLowerHash("ball_shot_windup_trans_1_2");
    static unsigned long sUnidentified2
        = nlStringLowerHash("ball_shot_windup_trans_2_3");
    static unsigned long sUnidentified3
        = nlStringLowerHash("ball_shot_windup_trans_3_max");
    static unsigned long sUnidentified4
        = nlStringLowerHash("ball_shot_windup_trans_max");

    if (pBall->mfChargeValue < 1.0f)
    {
        fn_801B9904(sUnidentified0);
    }
    else if (pBall->mfChargeValue < 2.0f)
    {
        fn_801B9904(sUnidentified1);
    }
    else if (pBall->mfChargeValue < 3.0f)
    {
        fn_801B9904(sUnidentified2);
    }
    else if (pBall->mfChargeValue < 4.0f)
    {
        fn_801B9904(sUnidentified3);
    }
    else
    {
        fn_801B9904(sUnidentified4);
    }
}

extern "C" void fn_800153FC(cBall* pBall, bool bParam)
{
    if (pBall->mfChargeValue >= 0.0f && !bParam)
    {
        fn_800152B4(pBall);
    }

    if (pBall->m_pBlurHandler != NULL)
    {
        pBall->m_pBlurHandler->Die(0.25f);
        pBall->m_pBlurHandler = NULL;
    }
    fn_801B79A4("skillshot_ball_meteor", 0);
    fn_801B79A4("skillshot_ball_drybones", 0);
    fn_801B79A4("skillshot_ball_boo", 0);

    if (lbl_806E0BCC || GameInfoManager::Instance()->IsRule0x4Equal5())
    {
        pBall->mfChargeValue = 4.0f;
    }
    else
    {
        pBall->mfChargeValue = 0.0f;
    }

    float fMaxCharge = lbl_806DB510 * 4.0f;
    float fValue = pBall->mfChargeValue;
    if (fValue >= fMaxCharge)
    {
        pBall->mfChargeValue = fMaxCharge;
    }
    else if (fValue < 0.0f)
    {
        pBall->mfChargeValue = 0.0f;
    }

    fn_801B7A28(pBall);
}

extern "C" void fn_800154FC(cBall* pBall, float fParam)
{
    if (lbl_806E0BCC || GameInfoManager::Instance()->IsRule0x4Equal5())
    {
        pBall->mfChargeValue = 4.0f;
    }
    else
    {
        pBall->mfChargeValue = fParam;
    }

    float fMaxCharge = lbl_806DB510 * 4.0f;
    float fValue = pBall->mfChargeValue;
    if (fValue >= fMaxCharge)
    {
        pBall->mfChargeValue = fMaxCharge;
    }
    else if (fValue < 0.0f)
    {
        pBall->mfChargeValue = 0.0f;
    }

    fn_801B7A28(pBall);
}

extern "C" float fn_800156A8(cBall* pBall)
{
    float fParam = pBall->mfChargeValue - 1.0f;
    float fResult = 0.0f;
    if (fParam > 0.0f)
    {
        fResult = fParam / 3.0f;
    }

    fResult = fResult >= 0.0f ? fResult : 0.0f;
    return fResult <= 1.0f ? fResult : 1.0f;
}

static inline void ShootAtFastImpl(cBall* pBall, nlVector3& v3Vel,
    const nlVector3& v3Target, float fDesiredTime)
{
    float k = lbl_806DB584 * pBall->m_pPhysicsBall->mfBallAirResistance;
    float g = lbl_806DB588 * pBall->m_pPhysicsBall->m_gravity;
    float eToTheNegativeKT = Exp(-k * fDesiredTime);
    float kSquaredOverOneMinusEToTheNegativeKT
        = (k * k) / (1.0f - eToTheNegativeKT);
    float oneOverK = 1.0f / k;

    v3Vel.x = kSquaredOverOneMinusEToTheNegativeKT
        * (oneOverK * (v3Target.x - pBall->m_v3Position.x));
    v3Vel.y = kSquaredOverOneMinusEToTheNegativeKT
        * (oneOverK * (v3Target.y - pBall->m_v3Position.y));
    v3Vel.z = kSquaredOverOneMinusEToTheNegativeKT
            * (oneOverK * (v3Target.z - pBall->m_v3Position.z
                              - g * fDesiredTime / k))
        + g / k;
}

extern "C" void fn_800156F8(cBall*, cPlayer* pShooter)
{
    g_pBall->m_pPhysicsBall->fn_8013FE14();

    nlVector3 v3Position = lbl_804DBE30;
    v3Position.x *= AIsgn(g_pBall->m_v3Position.x);

    eCharacterClass eClass = CHARACTER_CLASS_INVALID;
    if (pShooter != NULL)
    {
        eClass = pShooter->m_eCharacterClass;
        v3Position = pShooter->m_v3Position;
    }

    float fTimeScale;
    float fOriginalX = v3Position.x;
    switch (eClass)
    {
    case CHARACTER_CLASS_INVALID:
    default:
        v3Position.x += nlRandomf(lbl_806DB538)
            - 0.5f * lbl_806DB538;
        v3Position.y += nlRandomf(lbl_806DB538)
            - 0.5f * lbl_806DB538;
        fTimeScale = lbl_806DB518;
        break;
    case (eCharacterClass)14:
        v3Position.x += nlRandomf(lbl_806DB52C)
            - 0.5f * lbl_806DB52C;
        v3Position.y += nlRandomf(lbl_806DB52C)
            - 0.5f * lbl_806DB52C;
        fTimeScale = lbl_806DB51C;
        break;
    case MYSTERY:
        v3Position.x += nlRandomf(lbl_806DB530)
            - 0.5f * lbl_806DB530;
        v3Position.y += nlRandomf(lbl_806DB530)
            - 0.5f * lbl_806DB530;
        fTimeScale = lbl_806DB520;
        break;
    case (eCharacterClass)17:
        v3Position.x += nlRandomf(lbl_806DB528)
            - 0.5f * lbl_806DB528;
        v3Position.y += nlRandomf(lbl_806DB528)
            - 0.5f * lbl_806DB528;
        fTimeScale = lbl_806DB518;
        break;
    case (eCharacterClass)19:
        v3Position.x += nlRandomf(lbl_806DB534)
            - 0.5f * lbl_806DB534;
        v3Position.y += nlRandomf(lbl_806DB534)
            - 0.5f * lbl_806DB534;
        fTimeScale = lbl_806DB524;
        break;
    }

    if (v3Position.x < 0.0f)
    {
        v3Position.x = nlMinEquals(
            nlMaxEquals(v3Position.x, g_pBall->m_v3Position.x),
            fOriginalX);
    }
    else
    {
        v3Position.x = nlMinEquals(
            nlMaxEquals(v3Position.x, fOriginalX),
            g_pBall->m_v3Position.x);
    }

    cField::FixOutOfBoundsPosition(v3Position, 0.2f, true);

    nlVector3 v3Velocity;
    nlVector2 v2Delta;
    v2Delta.x = v3Position.x - g_pBall->m_v3Position.x;
    v2Delta.y = v3Position.y - g_pBall->m_v3Position.y;
    float fDistance = nlSqrt(
        v2Delta.x * v2Delta.x + v2Delta.y * v2Delta.y, true);
    float fDesiredTime = fTimeScale * fDistance;
    if (eClass == (eCharacterClass)19)
    {
        fDesiredTime = fTimeScale;
    }
    else if (fDesiredTime < lbl_806DB53C)
    {
        fDesiredTime = lbl_806DB53C;
    }
    else if (fDesiredTime > lbl_806DB540)
    {
        fDesiredTime = lbl_806DB540;
    }

    ShootAtFastImpl(g_pBall, v3Velocity, v3Position, fDesiredTime);

    eSpinType spinType;
    if (nlRandom(100) > 50)
    {
        spinType = SPINTYPE_FORWARD;
    }
    else
    {
        spinType = SPINTYPE_BACK;
    }
    g_pBall->SetVelocity(v3Velocity, spinType, NULL);
    g_pBall->m_tNoPickupTimer.SetSeconds(0.15f);
    PhysicsBall* pPhysicsBall = g_pBall->m_pPhysicsBall;
    pPhysicsBall->mbUseMagnusEffect = false;
    pPhysicsBall->mfChargeBonus = 0.0f;
    fn_80015C38(g_pBall, 4);
}

static inline void fn_80015B38Impl(cBall* pBall, bool bParam)
{
    if (pBall->m_pOwner != NULL)
    {
        fn_80015C38(pBall, 2);
        return;
    }

    if (!bParam)
    {
        bool bPassTarget = (pBall->meBallState == 5
                               || pBall->meBallState == 3)
            && pBall->m_pPassTarget != NULL;
        if (bPassTarget && fn_800DEFD4((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
            if (pReceivePass != NULL && pReceivePass->UnidentifiedIsActive()
                && pReceivePass->meDesireSubState == 0)
            {
                return;
            }

            fn_80015C38(pBall, 0);
            return;
        }
    }

    fn_80015C38(pBall, 0);
}

extern "C" void fn_80015B38(cBall* pBall, bool bParam)
{
    fn_80015B38Impl(pBall, bParam);
}

static inline void ClearBallStateTargets(cBall* pBall)
{
    pBall->m_tLightningTimer.m_unk0
        = pBall->m_tLightningTimer.m_uPackedTime != 0;
    pBall->m_tLightningTimer.m_uPackedTime = 0;
    pBall->mpDamageTarget = NULL;
    if (pBall->m_pPassTarget != NULL)
    {
        pBall->m_pPassTarget = NULL;
    }

    pBall->m_tPassTargetTimer.m_unk0
        = pBall->m_tPassTargetTimer.m_uPackedTime != 0;
    pBall->m_tPassTargetTimer.m_uPackedTime = 0;
    pBall->m_v3PassIntercept.x = 0.0f;
    pBall->m_v3PassIntercept.y = 0.0f;
    pBall->m_v3PassIntercept.z = 0.0f;
    pBall->m_fTotalPassTime = 0.0f;
    if (pBall->m_uVoiceID != 0)
    {
        pBall->m_uVoiceID = 0;
    }
}

static inline void UpdateBallShotClock(cBall* pBall)
{
    if (pBall->m_pOwner != NULL)
    {
        if (!pBall->m_pOwner->IsOnSameTeam(pBall->m_pPrevOwner))
        {
            pBall->mtShotClockTimer.SetSeconds(lbl_806DB558);
            pBall->mnShotClockTeam = pBall->m_pOwner->m_pTeam->m_nSide;
        }
    }
    else
    {
        pBall->mnShotClockTeam = -1;
        pBall->mtShotClockTimer.SetSeconds(0.0f);
    }
}

extern "C" void fn_80015C38(cBall* pBall, int nBallState)
{
    int previousState = pBall->meBallState;
    UnidentifiedEvent2View<int, int>* event
        = (UnidentifiedEvent2View<int, int>*)
            &g_pGame->mUnidentified49C.mEvent14;
    event->UnidentifiedDeliver(previousState, nBallState);

    if (pBall->meBallState == 9)
    {
        pBall->m_pPhysicsBall->m_gravity = -22.5f;
        fn_801BDF08(0);
        pBall->m_tShotTimer.m_unk0
            = pBall->m_tShotTimer.m_uPackedTime != 0;
        pBall->m_tShotTimer.m_uPackedTime = 0;
    }

    if ((pBall->meBallState == 6 || pBall->meBallState == 7)
        && nBallState != 6 && nBallState != 7 && nBallState != 10)
    {
        fn_800152B4(pBall);
        pBall->m_pPhysicsBall->fn_80140C30();
    }
    else if (pBall->meBallState == 1 && nBallState != 10)
    {
        pBall->m_pPhysicsBall->fn_80140C30();
    }
    else if (pBall->meBallState == 10)
    {
        pBall->m_pPhysicsBall->fn_80140C30();
    }

    switch (nBallState)
    {
    case 2:
        UpdateBallShotClock(pBall);
        ClearBallStateTargets(pBall);
        if (pBall->m_pBlurHandler != NULL)
        {
            pBall->m_pBlurHandler->Die(0.25f);
            pBall->m_pBlurHandler = NULL;
        }
        break;
    case 10:
        UpdateBallShotClock(pBall);
        ClearBallStateTargets(pBall);
        break;
    case 9:
    {
        fn_801BDDE4();
        pBall->SetVelocity(v3Zero, SPINTYPE_NONE, NULL);
        pBall->m_pPhysicsBall->m_gravity = 0.0f;
        pBall->m_tShotTimer.SetSeconds(0.5f);

        fn_800154FC(pBall, 4.0f);

        if (pBall->m_pBlurHandler != NULL)
        {
            pBall->m_pBlurHandler->Die(0.25f);
            pBall->m_pBlurHandler = NULL;
        }
    }
    case 0:
        UpdateBallShotClock(pBall);
        ClearBallStateTargets(pBall);
        break;
    case 4:
        ClearBallStateTargets(pBall);
        pBall->InitiateBallBlur((eBallShotEffectType)0, NULL);
        break;
    case 1:
        UpdateBallShotClock(pBall);
        if (pBall->m_pPrevOwner != NULL
            && pBall->m_pPrevOwner->m_eClassType == FIELDER)
        {
            pBall->m_pPhysicsBall->mfBallAirResistance
                = pBall->m_pPhysicsBall->fn_80140C3C() * 15.0f;
        }
        break;
    case 6:
        UpdateBallShotClock(pBall);
        if (pBall->m_pPrevOwner != NULL
            && pBall->m_pPrevOwner->m_eClassType == FIELDER)
        {
            float resistance = pBall->m_pPhysicsBall->fn_80140C3C();
            PlayerTweaks* tweaks
                = fn_8003E6E4((cFielder*)pBall->m_pPrevOwner);
            pBall->m_pPhysicsBall->mfBallAirResistance
                = resistance * Interpolate(24.0f, 1.0f,
                    (float)tweaks->fShooting);
        }
        break;
    case 7:
        UpdateBallShotClock(pBall);
        break;
    case 3:
    case 5:
    case 8:
    default:
        break;
    }

    if (nBallState == 2 || nBallState == 4)
    {
        fn_800EBC84(0, 0xBF92BBAF,
            (XSoundOwner_802ED74C*)pBall->mUnidentifiedEC, NULL, NULL);
    }

    if (pBall->m_pPrevOwner != NULL
        && pBall->m_pPrevOwner->m_eClassType == FIELDER)
    {
        fn_800EC12C(0x65321E47, pBall);
        if (nBallState == 7)
        {
            fn_800EBC84(0, 0xDE8FC45D,
                (XSoundOwner_802ED74C*)pBall->mUnidentifiedEC, NULL, NULL);
        }
        else if (nBallState == 6)
        {
            fn_800EBC84(0, 0xDE8FC45D,
                (XSoundOwner_802ED74C*)pBall->mUnidentifiedEC, NULL, NULL);
            if (pBall->mfChargeValue >= 1.0f
                && pBall->mfChargeValue < 2.0f)
            {
                fn_800EBC84(0, 0xDE8FC45E,
                    (XSoundOwner_802ED74C*)pBall->mUnidentifiedEC, NULL,
                    NULL);
            }
            else if (pBall->mfChargeValue >= 2.0f
                && pBall->mfChargeValue < 3.0f)
            {
                fn_800EBC84(0, 0xDE8FC45F,
                    (XSoundOwner_802ED74C*)pBall->mUnidentifiedEC, NULL,
                    NULL);
            }
            else if (pBall->mfChargeValue >= 3.0f
                && pBall->mfChargeValue < 4.0f)
            {
                fn_800EBC84(0, 0xDE8FC460,
                    (XSoundOwner_802ED74C*)pBall->mUnidentifiedEC, NULL,
                    NULL);
            }
            else if (pBall->mfChargeValue >= 4.0f)
            {
                fn_800EBC84(0, 0xDE8FC461,
                    (XSoundOwner_802ED74C*)pBall->mUnidentifiedEC, NULL,
                    NULL);
            }
        }
        else if (nBallState == 5 || nBallState == 3 || nBallState == 1)
        {
            fn_800EBC84(0, 0x875086F2,
                (XSoundOwner_802ED74C*)pBall->mUnidentifiedEC, NULL, NULL);
            if (pBall->mfChargeValue >= 1.0f
                && pBall->mfChargeValue < 2.0f)
            {
                fn_800EBC84(0, 0x71406564,
                    (XSoundOwner_802ED74C*)pBall->mUnidentifiedEC, NULL,
                    NULL);
            }
            else if (pBall->mfChargeValue >= 2.0f
                && pBall->mfChargeValue < 3.0f)
            {
                fn_800EBC84(0, 0x71406565,
                    (XSoundOwner_802ED74C*)pBall->mUnidentifiedEC, NULL,
                    NULL);
            }
            else if (pBall->mfChargeValue >= 3.0f
                && pBall->mfChargeValue < 4.0f)
            {
                fn_800EBC84(0, 0x71406566,
                    (XSoundOwner_802ED74C*)pBall->mUnidentifiedEC, NULL,
                    NULL);
            }
            else if (pBall->mfChargeValue >= 4.0f)
            {
                fn_800EBC84(0, 0x71406567,
                    (XSoundOwner_802ED74C*)pBall->mUnidentifiedEC, NULL,
                    NULL);
            }

            if (nBallState == 5)
            {
                fn_800EBC84(0, 0x65321E47,
                    (XSoundOwner_802ED74C*)pBall->mUnidentifiedEC,
                    "Volley Pass", pBall);
            }
        }
        else if (nBallState == 8)
        {
            fn_800EBC84(pBall->m_pPrevOwner->mUnidentified318,
                0x3D267BDF,
                (XSoundOwner_802ED74C*)pBall->mUnidentifiedEC, NULL, NULL);
        }
    }

    pBall->mePrevBallState = pBall->meBallState;
    pBall->meBallState = nBallState;
}

void cBall::ClearBallBlur()
{
    if (m_pBlurHandler != NULL)
    {
        m_pBlurHandler->Die(0.25f);
        m_pBlurHandler = NULL;
    }
}

extern "C" bool fn_80016768(cBall* pBall)
{
    return pBall->m_tLightningTimer.m_uPackedTime != 0
        && pBall->meBallState == 8 && pBall->m_pShooter != NULL
        && pBall->m_pShooter->m_eCharacterClass
        == (eCharacterClass)0x11;
}

extern "C" bool fn_800167A8(cBall* pBall)
{
    return pBall->m_tLightningTimer.m_uPackedTime != 0
        && pBall->meBallState == 8 && pBall->m_pShooter != NULL
        && pBall->m_pShooter->m_eCharacterClass
        == (eCharacterClass)0x10;
}

extern "C" bool fn_800167E8(cBall* pBall)
{
    return pBall->m_tLightningTimer.m_uPackedTime != 0;
}

void cBall::SetOwner(cPlayer* pOwner)
{
    m_pOwner = pOwner;
    pOwner->fn_80096CDC(this);
    m_pLastTouch = pOwner;
    fn_80015C38(this, 2);

    if (pOwner->m_eClassType != GOALIE)
    {
        g_pGame->SetPotentialScorer(pOwner);
    }

    PhysicsBall* pPhysicsBall = m_pPhysicsBall;
    pPhysicsBall->mbUseMagnusEffect = false;
    pPhysicsBall->mfChargeBonus = 0.0f;
}

void cBall::SetPosition(const nlVector3& pos)
{
    m_v3Position = pos;
    m_pPhysicsBall->SetPosition(pos, PhysicsObject::WORLD_COORDINATES);
    m_pPhysicsBall->SetRotation(m3Ident);
    FakeBallWorld::InvalidateBallCache();
    ++m_bBallPathChangeCount;
}

void cBall::SetVelocity(const nlVector3& velocity, eSpinType spin,
    const nlVector3* pAngularVelocity)
{
    nlVector3 v3AngVel;
    float fSpinRand;

    m_v3Velocity = velocity;
    m_pPhysicsBall->SetLinearVelocity(velocity);

    if (spin == SPINTYPE_NONE)
    {
        v3AngVel.x = 0.0f;
        v3AngVel.y = 0.0f;
        v3AngVel.z = 0.0f;
    }
    else if (spin == SPINTYPE_FORWARD)
    {
        fSpinRand = lbl_806DB58C + nlRandomf(2.0f);

        nlVector3 v3Up = { 0.0f, 0.0f, 0.0f };
        v3Up.z = fSpinRand;

        nlVec3CrossProductAlt(v3AngVel, v3Up, velocity);
        nlVec3Set(v3AngVel, v3AngVel.z, v3AngVel.y, v3AngVel.x);
    }
    else if (spin == SPINTYPE_BACK)
    {
        fSpinRand = lbl_806DB590 + nlRandomf(2.0f);

        nlVector3 v3Up = { 0.0f, 0.0f, 0.0f };
        v3Up.z = fSpinRand;

        nlVec3CrossProductAlt(v3AngVel, v3Up, velocity);
        nlVec3Set(v3AngVel, v3AngVel.z, v3AngVel.y, v3AngVel.x);
    }
    else if (spin == SPINTYPE_ROLLING)
    {
        m_pPhysicsBall->CalcAngularFromLinearVelocity(v3AngVel);
        nlVec3Set(v3AngVel, 0.92f * v3AngVel.x,
            0.92f * v3AngVel.y, 0.92f * v3AngVel.z);
    }
    else if (spin == SPINTYPE_PARAMETER)
    {
        v3AngVel = *pAngularVelocity;
    }

    m_pPhysicsBall->SetAngularVelocity(v3AngVel);
    m_pPhysicsBall->SetUseAngularVelocity(true);
    m_pPhysicsBall->SetRotation(m3Ident);
    FakeBallWorld::InvalidateBallCache();
    m_bBallPathChangeCount = m_bBallPathChangeCount + 1;
    fn_80014D38Impl(this);
    m_v3ShotOrigin = m_v3Position;
}

void cBall::Shoot(cPlayer* pShooter, const nlVector3& v3Dir,
    const nlVector3& v3Spin, eSpinType spinType, int nBallState, bool bParam6)
{
    nlVector3 v3PredPos;
    nlVector3 v3PredVel;
    nlVector3 v3ToDir;
    nlVector3 v3FromDir;
    nlQuaternion qRot;
    nlVector3 v3Unidentified;

    SetVelocity(v3Dir, spinType, &v3Spin);
    m_tNoPickupTimer.SetSeconds(0.1f);
    m_tLightningTimer.SetSeconds(1.5f);
    fn_80015C38(this, nBallState);
    m_pShooter = pShooter;

    if (bParam6)
    {
        m_pPhysicsBall->fn_80140C30();
    }

    if (m_pPhysicsBall->mbUseMagnusEffect)
    {
        nlVec3Sub(v3Unidentified, m_v3Position, m_v3ShotTarget);
        float fDist = nlSqrt(v3Unidentified.GetLengthSq3D(), true);

        fn_8016F06C();
        FakeBallWorld::GetPredictedPosAtDistance(
            fDist, v3PredPos, v3PredVel, true);

        nlVec3Sub(v3ToDir, m_v3ShotTarget, m_v3Position);
        nlVec3Sub(v3FromDir, v3PredPos, m_v3Position);

        GetRotationBetweenVectors(qRot, v3FromDir, v3ToDir);
        RotateVector(m_v3Velocity, v3Dir, qRot);

        if (m_v3Velocity.z < 1.0f && m_v3Position.z < 1.0f)
        {
            m_v3Velocity.z = 1.0f;
        }

        float fSidelineY = cField::GetSidelineY(1) - 0.5f;
        if (m_v3Position.y > fSidelineY && m_v3Velocity.y > -0.1f)
        {
            m_v3Velocity.y = -0.1f;
        }
        else if (m_v3Position.y < -cField::GetSidelineY(1) + 0.5f
            && m_v3Velocity.y < 0.1f)
        {
            m_v3Velocity.y = 0.1f;
        }

        m_pPhysicsBall->SetLinearVelocity(m_v3Velocity);
        FakeBallWorld::InvalidateBallCache();
    }

    if (!g_pGame->mUnidentified040)
    {
        Goalie* pGoalie = m_pPrevOwner->m_pTeam->GetOtherTeam()->GetGoalie();
        pGoalie->InitActionSaveSetup(true);
    }
}

extern "C" void fn_80016DF8(cBall* pBall, cPlayer* pPlayer,
    nlVector3* pVelocity, int nSpinType, bool bVolleyPass, bool bParam)
{
    if (bVolleyPass && pBall->mePrevBallState == 5)
    {
        ++pBall->m_iConsecutiveVolleyPasses;
    }
    else
    {
        pBall->m_iConsecutiveVolleyPasses = 0;
    }

    int nReleaseReason = bVolleyPass ? 5 : 3;
    if (pPlayer->m_pBall != NULL)
    {
        pPlayer->ReleaseBall(nReleaseReason);
    }
    else
    {
        fn_80015C38(pBall, nReleaseReason);
    }

    pBall->SetVelocity(*pVelocity, (eSpinType)nSpinType, NULL);
    pBall->m_tNoPickupTimer.SetSeconds(0.1f);
    PhysicsBall* pPhysicsBall = pBall->m_pPhysicsBall;
    pPhysicsBall->mbUseMagnusEffect = false;
    pPhysicsBall->mfChargeBonus = 0.0f;

    if (lbl_806DB500 && pPlayer->m_eClassType == FIELDER)
    {
        float fValue = fn_8002BE64(fn_8003E6E4((cFielder*)pPlayer));
        float fPercent = InterpolateRangeClamped(
            0.0f, 1.0f, 0.5f, 1.0f, fValue);
        float fCharge = Interpolate(lbl_806DB504, lbl_806DB508, fPercent);
        float fCurrentCharge = pBall->mfChargeValue;
        if (lbl_806E0BCC
            || GameInfoManager::Instance()->IsRule0x4Equal5())
        {
            pBall->mfChargeValue = 4.0f;
        }
        else
        {
            pBall->mfChargeValue = fCharge + fCurrentCharge;
        }

        float fMaxCharge = lbl_806DB510 * 4.0f;
        fValue = pBall->mfChargeValue;
        if (fValue >= fMaxCharge)
        {
            pBall->mfChargeValue = fMaxCharge;
        }
        else if (fValue < 0.0f)
        {
            pBall->mfChargeValue = 0.0f;
        }

        fn_801B7A28(pBall);
    }
}

void cBall::ShootRelease(const nlVector3& v3Velocity, eSpinType SpinType)
{
    SetVelocity(v3Velocity, SpinType, NULL);
    m_tNoPickupTimer.SetSeconds(0.1f);
    PhysicsBall* pPhysicsBall = m_pPhysicsBall;
    pPhysicsBall->mbUseMagnusEffect = false;
    pPhysicsBall->mfChargeBonus = 0.0f;
}

void cBall::ShootAtFast(nlVector3& v3Vel, const nlVector3& v3Target,
    float fDesiredTime)
{
    ShootAtFastImpl(this, v3Vel, v3Target, fDesiredTime);
}

extern "C" void fn_80017114(cBall* pBall)
{
    if (nlAbs(pBall->m_v3Position.y) - lbl_806DB56C < 0.0f)
    {
        fn_80014494Impl(pBall);
        return;
    }

    if (pBall->m_v3Position.z < 0.36f)
    {
        nlVector3 v3Position = pBall->m_v3Position;
        v3Position.z = 0.36f;
        pBall->SetPosition(v3Position);
    }

    nlVector3 v3Position = pBall->m_v3Position;
    v3Position.y = AIsgn(v3Position.y) * lbl_806DB56C;
    fn_8002D92C(&v3Position, true, 3.0f);

    nlVector3 v3Direction;
    nlVec3Sub(v3Direction, v3Position, pBall->m_v3Position);
    nlVec3Scale(v3Direction,
        nlRecipSqrt(v3Direction.GetLengthSq3D(), true));
    nlVec3Scale(v3Direction, lbl_806DB560);

    float fSpeed = nlGetLength3D(pBall->m_v3Velocity.x,
        pBall->m_v3Velocity.y, pBall->m_v3Velocity.z);
    if (fSpeed < lbl_806DB564)
    {
        fSpeed = lbl_806DB564;
    }
    else if (fSpeed > lbl_806DB568)
    {
        fSpeed = lbl_806DB568;
    }

    nlVector3 v3Velocity;
    nlVec3Add(v3Velocity, pBall->m_v3Velocity, v3Direction);
    nlVec3Scale(v3Velocity,
        nlRecipSqrt(v3Velocity.GetLengthSq3D(), true));
    nlVec3Scale(v3Velocity, fSpeed);

    nlVector3 v3AngularVelocity;
    pBall->m_pPhysicsBall->GetAngularVelocity(&v3AngularVelocity);
    pBall->SetVelocity(
        v3Velocity, SPINTYPE_PARAMETER, &v3AngularVelocity);
}

static inline cFielder* GetOwnerFielderImpl(cBall* pBall)
{
    cPlayer* player = pBall->m_pOwner;
    if ((player != NULL) && (player->m_eClassType == FIELDER))
    {
        return (cFielder*)player;
    }
    return NULL;
}

static inline cFielder* GetPassTargetFielderImpl(const cBall* pBall)
{
    cPlayer* player = pBall->m_pPassTarget;
    if ((player != NULL) && (player->m_eClassType == FIELDER))
    {
        return (cFielder*)player;
    }
    return NULL;
}

extern "C" void fn_80017448(cBall* pBall, float fDeltaT)
{
    bool bIsGameplay = g_pGame->IsGameplayOrOvertime();

    if (bIsGameplay)
    {
        if (pBall->meBallState == 10)
        {
            fn_80017114(pBall);
        }

        pBall->m_tNoPickupTimer.Countdown(fDeltaT, 0.0f);

        if (pBall->m_tShotTimer.m_uPackedTime != 0
            && pBall->m_tShotTimer.Countdown(fDeltaT, 0.0f))
        {
            if (pBall->meBallState == 9)
            {
                fn_80014494(pBall);
            }
            else
            {
                fn_8001847C(pBall, false);
            }
        }

        if (pBall->m_tLightningTimer.m_uPackedTime != 0
            && pBall->m_tLightningTimer.Countdown(fDeltaT, 0.0f))
        {
            fn_80014494(pBall);
        }

        if (pBall->m_tPassTargetTimer.m_uPackedTime != 0
            && pBall->m_tPassTargetTimer.Countdown(fDeltaT, 0.0f))
        {
            pBall->m_fTotalPassTime = 0.0f;
        }

        if (pBall->mtNoChargeLossTimer.m_uPackedTime != 0
            && pBall->mtNoChargeLossTimer.Countdown(fDeltaT, 0.0f))
        {
            pBall->mbStuckInRiotDone = true;
        }

        if (pBall->mtStuckInRiotTimer.m_uPackedTime != 0)
        {
            pBall->mtStuckInRiotTimer.Countdown(fDeltaT, 0.0f);
        }

        if (pBall->mtShotClockTimer.m_uPackedTime != 0
            && pBall->mtShotClockTimer.Countdown(fDeltaT, 0.0f)
            && lbl_806E0BDC)
        {
            cFielder* pFielder = NULL;
            if (GetOwnerFielderImpl(pBall) != NULL)
            {
                pFielder = GetOwnerFielderImpl(pBall);
            }
            else if (GetPassTargetFielderImpl(pBall) != NULL)
            {
                pFielder = GetPassTargetFielderImpl(pBall);
            }
            if (pFielder != NULL)
            {
                fn_80031A30(pFielder, 1, lbl_806DB55C);
            }
        }
    }
}

static bool sUnidentifiedUpdateActive;

void cBall::Update(float fDeltaT)
{
    if (mbBallFrozen)
    {
        SetPosition(m_v3PrevPosition);
    }
    else
    {
        fn_80017448(this, fDeltaT);
        fn_80017F18(this);

        bool bUnidentified = true;
        static Timer tUnidentifiedUpdateTimer(0.33f);

        bool bIsGameplay = g_pGame->IsGameplayOrOvertime();

        if (bIsGameplay)
        {
            if (lbl_806DB5A8
                && meBallState != 6
                && meBallState != 7
                && meBallState != 8
                && meBallState != 9
                && meBallState != 2
                && meBallState != 10)
            {
                if (sUnidentifiedUpdateActive)
                {
                    bUnidentified = m_v3Position.z < 0.4f;
                }
                else if (m_v3Position.z > lbl_806DB5A4
                    && tUnidentifiedUpdateTimer.Countdown(fDeltaT, 0.0f))
                {
                    nlVector3 v3Unidentified;
                    PredictLandingSpotAndTime(v3Unidentified,
                        NULL, NULL, lbl_806DB5A4);
                    fn_801B9EAC(this, &v3Unidentified, false);
                    sUnidentifiedUpdateActive = true;
                    bUnidentified = false;
                }
            }

            if (sUnidentifiedUpdateActive && bUnidentified)
            {
                fn_801B9FD0(this, false);
                sUnidentifiedUpdateActive = false;
                tUnidentifiedUpdateTimer.SetSeconds(lbl_806DB5AC);
            }
        }
        else
        {
            fn_801B9FD0(this, false);
            sUnidentifiedUpdateActive = false;
            tUnidentifiedUpdateTimer.SetSeconds(lbl_806DB5B0);
        }

        if (meBallState != 5)
        {
            m_iConsecutiveVolleyPasses = 0;
        }

        mUnidentifiedEC->mUnidentified1CBits.mUnidentified1CFlag
            = !g_pGame->IsGameplayOrOvertime();
    }
}

static inline void CalcBallRotationFromVelocity(
    nlQuaternion& qOrientationDelta, const nlVector3& v3Velocity,
    float fDeltaT)
{
    qOrientationDelta.z = 0.0f;
    qOrientationDelta.y = 0.0f;
    qOrientationDelta.x = 0.0f;
    qOrientationDelta.w = 1.0f;

    float fVel = nlSqrt(v3Velocity.GetLengthSq3D(), true);
    if (fVel > 0.0001f)
    {
        nlVector3 v3NormalizedVelocity = v3Velocity;
        nlVector3 v3Up;
        nlVector3 v3RotationAxis;

        nlVec3Set(v3Up, 0.0f, 0.0f, 1.0f);

        v3NormalizedVelocity.x /= fVel;
        v3NormalizedVelocity.y /= fVel;
        v3NormalizedVelocity.z /= fVel;

        float fAxisX;
        float fAxisY;
        float fAxisZ;

        fAxisX = v3Up.y * v3NormalizedVelocity.z
            - v3Up.z * v3NormalizedVelocity.y;
        fAxisY = -v3Up.x * v3NormalizedVelocity.z
            + v3Up.z * v3NormalizedVelocity.x;
        fAxisZ = v3Up.x * v3NormalizedVelocity.y
            - v3Up.y * v3NormalizedVelocity.x;
        nlVec3Set(v3RotationAxis, fAxisX, fAxisY, fAxisZ);

        fn_802B5370(qOrientationDelta, v3RotationAxis,
            (unsigned short)(int)(10430.378f
                * (fDeltaT * (fVel / 0.18f))));
    }
}

void cBall::UpdateOrientation(float fDeltaT)
{
    nlQuaternion qOrientationDelta;
    nlVector3 v3AngVel;
    float fInvAng;
    nlQuaternion qNewOrientation;

    if (m_pOwner == NULL)
    {
        u8 bUseAngularVel = 0;
        if (m_pPhysicsBall->mbUseAngularVel != 0
            || m_pPhysicsBall->mfSpinTimer > 0.0f)
        {
            bUseAngularVel = 1;
        }

        if (bUseAngularVel != 0)
        {
            m_pPhysicsBall->GetAngularVelocity(&v3AngVel);

            float fAng = nlSqrt(v3AngVel.x * v3AngVel.x
                    + v3AngVel.y * v3AngVel.y
                    + v3AngVel.z * v3AngVel.z,
                true);
            if (fAng > 0.01f)
            {
                fInvAng = 1.0f / fAng;
                nlVec3Scale(v3AngVel, fInvAng);
                fn_802B5370(qOrientationDelta, v3AngVel,
                    (unsigned short)(int)(10430.378f
                        * (fAng * fDeltaT)));
            }
            else
            {
                qOrientationDelta.z = 0.0f;
                qOrientationDelta.y = 0.0f;
                qOrientationDelta.x = 0.0f;
                qOrientationDelta.w = 1.0f;
            }
        }
        else
        {
            CalcBallRotationFromVelocity(
                qOrientationDelta, m_v3Velocity, fDeltaT);
        }
    }
    else
    {
        m_pPhysicsBall->SetUseAngularVelocity(false);

        switch (m_pOwner->m_eBallRotationMode)
        {
        case BRM_ANIMATED:
            m_pOwner->GetAnimatedBallOrientation(m_qOrientation);
            return;
        case BRM_MATCH_VELOCITY:
        {
            CalcBallRotationFromVelocity(
                qOrientationDelta, m_v3Velocity, fDeltaT);
            break;
        }
        }
    }

    nlMultQuat(qNewOrientation, qOrientationDelta, m_qOrientation);
    nlQuatNormalize(m_qOrientation, qNewOrientation);
}

void cBall::WarpTo(const nlVector3& toPos)
{
    m_v3Position = toPos;
    m_pPhysicsBall->SetPosition(toPos, PhysicsObject::WORLD_COORDINATES);
    m_pPhysicsBall->SetRotation(m3Ident);
    FakeBallWorld::InvalidateBallCache();
    m_bBallPathChangeCount = m_bBallPathChangeCount + 1;
    m_v3PrevPosition = toPos;
}

void cBall::SetPassTarget(
    cPlayer* passTargetPlayer, const nlVector3& pos, bool bVolley)
{
    m_pPassTarget = passTargetPlayer;
    m_v3PassIntercept = pos;
}

void cBall::SetPassTargetTimer(float seconds)
{
    m_tPassTargetTimer.SetSeconds(seconds);
    if (m_fTotalPassTime == 0.0f)
    {
        m_fTotalPassTime = seconds;
    }
}

void cBall::KillBlurHandler()
{
    if (m_pBlurHandler != NULL)
    {
        m_pBlurHandler->Die(0.f);
        m_pBlurHandler = NULL;
    }
}

extern "C" void fn_800180F4(
    cBall* pBall, nlVector3* pPosition, float fTime)
{
    float k = pBall->m_pPhysicsBall->mfBallAirResistance * lbl_806DB584;
    float g = lbl_806DB588 * pBall->m_pPhysicsBall->m_gravity;
    float eToTheNegativeKT = Exp(-k * fTime);
    float oneOverK = 1.0f / k;
    float oneMinusEToTheNegativeKTOverK
        = oneOverK * (1.0f - eToTheNegativeKT);

    pPosition->x = pBall->m_v3Position.x
        + pBall->m_v3Velocity.x * oneMinusEToTheNegativeKTOverK;
    pPosition->y = pBall->m_v3Position.y
        + pBall->m_v3Velocity.y * oneMinusEToTheNegativeKTOverK;
    pPosition->z = pBall->m_v3Position.z + fTime * g / k
        + oneOverK * oneMinusEToTheNegativeKTOverK
            * (k * pBall->m_v3Velocity.z - g);
    pPosition->z = nlMaxEquals(0.0f, pPosition->z);
}

float cBall::PredictLandingSpotAndTime(nlVector3& v3Dest,
    int* pNumSolutions, float* pTimes, float fHeight)
{
    float fTime = 0.0f;

    if (!nlNear(fHeight, 0.0f)
        || !nlNear(m_v3Position.z, 0.18f)
        || (m_v3Position.z <= fHeight && m_v3Velocity.z <= 0.0f))
    {
        int numSolutions;
        float times[2];

        SolveQuadratic(0.5f * m_pPhysicsBall->m_gravity, m_v3Velocity.z,
            m_v3Position.z - fHeight,
            numSolutions, times[0], times[1]);

        if (pNumSolutions != NULL && pTimes != NULL)
        {
            *pNumSolutions = 0;
            float* root = times;
            for (int i = 0; i < numSolutions; i++)
            {
                if (*root > 0.0f)
                {
                    pTimes[*pNumSolutions] = *root;
                    (*pNumSolutions)++;
                }
                root++;
            }
            return 0.0f;
        }

        if (numSolutions == 2)
        {
            float solution1 = times[1];
            fTime = times[0];
            fTime = (fTime >= solution1) ? fTime : solution1;
        }
        else if (numSolutions == 1)
        {
            fTime = times[0];
        }
        else
        {
            return -10000.0f;
        }

        float k = m_pPhysicsBall->mfBallAirResistance * lbl_806DB584;
        float g = lbl_806DB588 * m_pPhysicsBall->m_gravity;
        float eToTheNegativeKT = Exp(-k * fTime);
        float oneOverK = 1.0f / k;
        float oneMinusEToTheNegativeKTOverK
            = oneOverK * (1.0f - eToTheNegativeKT);

        v3Dest.x = m_v3Position.x
            + m_v3Velocity.x * oneMinusEToTheNegativeKTOverK;
        v3Dest.y = m_v3Position.y
            + m_v3Velocity.y * oneMinusEToTheNegativeKTOverK;
        v3Dest.z = m_v3Position.z + fTime * g / k
            + oneOverK * oneMinusEToTheNegativeKTOverK
                * (k * m_v3Velocity.z - g);
        v3Dest.z = nlMaxEquals(0.18f, v3Dest.z);
        cField::FixOutOfBoundsPosition(v3Dest, 0.18f, true);
    }
    else
    {
        v3Dest = m_v3Position;
    }

    return fTime;
}

extern "C" void fn_8001847C(cBall* pBall, bool bParam)
{
    nlVector3 v3Pos = { 0.0f, 0.0f, 0.18f };
    if (g_pTeams[0]->m_nScore == 0 && g_pTeams[1]->m_nScore == 0)
    {
        v3Pos.z = lbl_806E0BC8;
    }

    float fUnidentified = 0.0f;
    if (lbl_806DB544)
    {
        fUnidentified = pBall->mfChargeValue;
    }

    if (pBall->m_pOwner != NULL)
    {
        pBall->m_pOwner->ReleaseBall(0);
    }

    pBall->m_pPhysicsBall->Unknown0();
    fn_80015B38Impl(pBall, false);

    pBall->m_bVisible = true;
    pBall->m_bBallPathChangeCount = 0;
    pBall->m_bBallDeflectCount = 0;
    pBall->m_fTotalPassTime = 0.0f;
    pBall->m_uGoalType = 4;
    pBall->m_uVoiceID = 0;
    pBall->m_CurrentGlowEffect = 0;
    pBall->mfChargeValue = 0.0f;
    pBall->mfSkillShotTime = 0.0f;
    pBall->meBallState = 0;
    pBall->mePrevBallState = 0;
    pBall->m_pOwner = NULL;
    pBall->m_pPrevOwner = NULL;
    pBall->m_pLastTouch = NULL;
    pBall->m_pPassTarget = NULL;
    pBall->m_pShooter = NULL;
    pBall->mpDamageTarget = NULL;
    pBall->m_iConsecutiveVolleyPasses = 0;

    pBall->m_tNoPickupTimer.SetSeconds(0.0f);
    pBall->m_tLightningTimer.SetSeconds(0.0f);
    pBall->m_tShotTimer.SetSeconds(0.0f);
    pBall->m_tPassTargetTimer.SetSeconds(0.0f);
    pBall->mtNoChargeLossTimer.SetSeconds(0.0f);
    pBall->mtStuckInRiotTimer.SetSeconds(0.0f);
    pBall->mtShotClockTimer.SetSeconds(0.0f);

    pBall->m_v3Position.x = 0.0f;
    pBall->m_v3Position.y = 0.0f;
    pBall->m_v3Position.z = 0.18f;
    pBall->mnShotClockTeam = -1;
    pBall->mbStuckInRiotDone = false;
    pBall->mbBallOnFire = false;
    pBall->mbBallFrozen = false;
    pBall->m_v3PrevPosition = pBall->m_v3Position;
    pBall->m_v3PassIntercept.x = 0.0f;
    pBall->m_v3PassIntercept.y = 0.0f;
    pBall->m_v3PassIntercept.z = 0.0f;
    pBall->m_qOrientation.z = 0.0f;
    pBall->m_qOrientation.y = 0.0f;
    pBall->m_qOrientation.x = 0.0f;
    pBall->m_qOrientation.w = 1.0f;
    pBall->m_v3Velocity.x = 0.0f;
    pBall->m_v3Velocity.y = 0.0f;
    pBall->m_v3Velocity.z = 0.0f;
    pBall->m_v3ShotTarget.x = 0.0f;
    pBall->m_v3ShotTarget.y = 0.0f;
    pBall->m_v3ShotTarget.z = 0.0f;
    pBall->m_v3ShotOrigin.x = 0.0f;
    pBall->m_v3ShotOrigin.y = 0.0f;
    pBall->m_v3ShotOrigin.z = 0.0f;

    pBall->WarpTo(v3Pos);
    pBall->ClearBallEffects();
    fn_800154FC(pBall, 0.0f);

    if (pBall->m_pOwner != NULL)
    {
        if (!pBall->m_pOwner->IsOnSameTeam(pBall->m_pPrevOwner))
        {
            pBall->mtShotClockTimer.SetSeconds(lbl_806DB558);
            pBall->mnShotClockTeam = pBall->m_pOwner->m_pTeam->m_nSide;
        }
    }
    else
    {
        pBall->mnShotClockTeam = -1;
        pBall->mtShotClockTimer.SetSeconds(0.0f);
    }

    if (lbl_806DB544 && bParam)
    {
        fn_800154FC(pBall, fUnidentified);
    }
    else if (lbl_806E0BC4)
    {
        fn_800154FC(pBall, nlRandomf(4.0f));
    }

    fn_801B9FD0(pBall, false);
}

extern "C" void fn_800189C4(cBall* pBall)
{
    fn_800EC12C(pBall->mUnidentifiedF0, pBall);
    pBall->mUnidentifiedF0 = 0;
}

void cBall::SyncLog(void* context, DebugWriteCache* cache)
{
    if (lbl_806DB5C0 == 0xFFFF)
    {
        lbl_806DB5C0 = fn_80338EBC(cache, "DetBall");
        fn_80338F88(cache, 16, lbl_80533C98[16].size, 0,
            "m_bVisible");
        fn_80338F88(cache, 9, lbl_80533C98[9].size,
            (u8*)&m_bBallPathChangeCount - (u8*)this,
            "m_bBallPathChangeCount");
        fn_80338F88(cache, 9, lbl_80533C98[9].size,
            (u8*)&m_bBallDeflectCount - (u8*)this,
            "m_bBallDeflectCount");
        fn_80338F88(cache, 20, lbl_80533C98[20].size,
            (u8*)&m_tShotTimer - (u8*)this, "m_tShotTimer");
        fn_80338F88(cache, 20, lbl_80533C98[20].size,
            (u8*)&m_tLightningTimer - (u8*)this,
            "m_tLightningTimer");
        fn_80338F88(cache, 20, lbl_80533C98[20].size,
            (u8*)&m_tNoPickupTimer - (u8*)this,
            "m_tNoPickupTimer");
        fn_80338F88(cache, 20, lbl_80533C98[20].size,
            (u8*)&m_tPassTargetTimer - (u8*)this,
            "m_tPassTargetTimer");
        fn_80338F88(cache, 20, lbl_80533C98[20].size,
            (u8*)&mtNoChargeLossTimer - (u8*)this,
            "mtNoChargeLossTimer");
        fn_80338F88(cache, 20, lbl_80533C98[20].size,
            (u8*)&mtStuckInRiotTimer - (u8*)this,
            "mtStuckInRiotTimer");
        fn_80338F88(cache, 20, lbl_80533C98[20].size,
            (u8*)&mtShotClockTimer - (u8*)this,
            "mtShotClockTimer");
        fn_80338F88(cache, 8, lbl_80533C98[8].size,
            (u8*)&mnShotClockTeam - (u8*)this, "mnShotClockTeam");
        fn_80338F88(cache, 16, lbl_80533C98[16].size,
            (u8*)&mbStuckInRiotDone - (u8*)this,
            "mbStuckInRiotDone");
        fn_80338F88(cache, 16, lbl_80533C98[16].size,
            (u8*)&mbBallOnFire - (u8*)this, "mbBallOnFire");
        fn_80338F88(cache, 16, lbl_80533C98[16].size,
            (u8*)&mbBallFrozen - (u8*)this, "mbBallFrozen");
        fn_80338F88(cache, 17, lbl_80533C98[17].size,
            (u8*)&m_fTotalPassTime - (u8*)this, "m_fTotalPassTime");
        fn_80338F88(cache, 8, lbl_80533C98[8].size,
            (u8*)&m_iConsecutiveVolleyPasses - (u8*)this,
            "m_iConsecutiveVolleyPasses");
        fn_80338F88(cache, 22, lbl_80533C98[22].size,
            (u8*)&m_v3Position - (u8*)this, "m_v3Position");
        fn_80338F88(cache, 22, lbl_80533C98[22].size,
            (u8*)&m_v3PrevPosition - (u8*)this, "m_v3PrevPosition");
        fn_80338F88(cache, 22, lbl_80533C98[22].size,
            (u8*)&m_v3Velocity - (u8*)this, "m_v3Velocity");
        fn_80338F88(cache, 22, lbl_80533C98[22].size,
            (u8*)&m_v3PassIntercept - (u8*)this, "m_v3PassIntercept");
        fn_80338F88(cache, 24, lbl_80533C98[24].size,
            (u8*)&m_qOrientation - (u8*)this, "m_qOrientation");
        fn_80338F88(cache, 22, lbl_80533C98[22].size,
            (u8*)&m_v3ShotTarget - (u8*)this, "m_v3ShotTarget");
        fn_80338F88(cache, 22, lbl_80533C98[22].size,
            (u8*)&m_v3ShotOrigin - (u8*)this, "m_v3ShotOrigin");
        fn_80338F88(cache, 2, lbl_80533C98[2].size,
            (u8*)&m_uGoalType - (u8*)this, "m_uGoalType");
        fn_80338F88(cache, 2, lbl_80533C98[2].size,
            (u8*)&m_uVoiceID - (u8*)this, "m_uVoiceID");
        fn_80338F88(cache, 2, lbl_80533C98[2].size,
            (u8*)&m_CurrentGlowEffect - (u8*)this,
            "m_CurrentGlowEffect");
        fn_80338F88(cache, 17, lbl_80533C98[17].size,
            (u8*)&mfChargeValue - (u8*)this, "mfChargeValue");
        fn_80338F88(cache, 17, lbl_80533C98[17].size,
            (u8*)&mfSkillShotTime - (u8*)this, "mfSkillShotTime");
        fn_80338F88(cache, 14, lbl_80533C98[14].size,
            (u8*)&meBallState - (u8*)this, "meBallState");
        fn_80338F88(cache, 14, lbl_80533C98[14].size,
            (u8*)&mePrevBallState - (u8*)this, "mePrevBallState");
        fn_80338F88(cache, 15, lbl_80533C98[15].size,
            (u8*)&m_pOwner - (u8*)this, "m_pOwner");
        fn_80338F88(cache, 15, lbl_80533C98[15].size,
            (u8*)&m_pPrevOwner - (u8*)this, "m_pPrevOwner");
        fn_80338F88(cache, 15, lbl_80533C98[15].size,
            (u8*)&m_pLastTouch - (u8*)this, "m_pLastTouch");
        fn_80338F88(cache, 15, lbl_80533C98[15].size,
            (u8*)&m_pPassTarget - (u8*)this, "m_pPassTarget");
        fn_80338F88(cache, 15, lbl_80533C98[15].size,
            (u8*)&m_pShooter - (u8*)this, "m_pShooter");
        fn_80338F88(cache, 15, lbl_80533C98[15].size,
            (u8*)&mpDamageTarget - (u8*)this, "mpDamageTarget");
        fn_80338F78(cache);
    }

    cBall* copy = (cBall*)fn_8033930C(
        cache, lbl_806DB5C0, this, offsetof(cBall, m_pBlurHandler));
    if (copy != NULL)
    {
        *(int*)&copy->m_pOwner
            = m_pOwner == NULL ? -1 : m_pOwner->mUnidentified120;
        *(int*)&copy->m_pPrevOwner
            = m_pPrevOwner == NULL ? -1 : m_pPrevOwner->mUnidentified120;
        *(int*)&copy->m_pLastTouch
            = m_pLastTouch == NULL ? -1 : m_pLastTouch->mUnidentified120;
        *(int*)&copy->m_pPassTarget
            = m_pPassTarget == NULL ? -1 : m_pPassTarget->mUnidentified120;
        *(int*)&copy->m_pShooter
            = m_pShooter == NULL ? -1 : m_pShooter->mUnidentified120;
        *(int*)&copy->mpDamageTarget
            = mpDamageTarget == NULL ? -1 : mpDamageTarget->mUnidentified120;
        fn_80339450(cache, lbl_806DB5C0, copy, context);
    }
}

void cBall::fn_8001A898(RunningChecksum* runningChecksum)
{
    runningChecksum->ChecksumData(&m_v3Position, sizeof(m_v3Position));
    runningChecksum->ChecksumData(&m_v3Velocity, sizeof(m_v3Velocity));
    runningChecksum->ChecksumData(&m_qOrientation, sizeof(m_qOrientation));
    runningChecksum->ChecksumData(&meBallState, sizeof(meBallState));
    runningChecksum->ChecksumData(&mfChargeValue, sizeof(mfChargeValue));
    runningChecksum->ChecksumData(
        &mfSkillShotTime, sizeof(mfSkillShotTime));
}

nlVector3* cBall::GetAIVelocity() const
{
    cPlayer* temp_r4 = m_pOwner;
    if (temp_r4 != NULL)
    {
        return &(temp_r4->m_v3Velocity);
    }
    return (nlVector3*)&(m_v3Velocity);
}

nlVector3* cBall::GetDrawablePosition() const
{
    const nlMatrix4& mtx = m_pDrawableBall->GetWorldMatrix();
    return (nlVector3*)&(mtx.e2[3][0]);
}

float cBall::fn_80014F38(float fScale) const
{
    return fScale * (0.18f * lbl_806DB514);
}

cFielder* cBall::GetOwnerFielder()
{
    return GetOwnerFielderImpl(this);
}

extern "C" void fn_80017F18(cBall* pBall)
{
    if (pBall->mtStuckInRiotTimer.m_uPackedTime != 0)
    {
        return;
    }

    bool bLoseCharge = false;
    switch (pBall->meBallState)
    {
    case 0:
    case 1:
    case 4:
        bLoseCharge = true;
        break;
    case 2:
        if (pBall->GetOwnerFielder() != NULL)
        {
            cFielder* pOwnerFielder = pBall->GetOwnerFielder();
            bool bIsShotActive = true;
            eShotMeterState state
                = pOwnerFielder->m_pShotMeter->m_eShotMeterState;
            if (state != SHOT_METER_ACTIVE
                && state != SHOT_METER_STS_ACTIVE)
            {
                bIsShotActive = false;
            }
            if (!bIsShotActive)
            {
                bLoseCharge = true;
            }
        }
        else
        {
            bLoseCharge = true;
        }
        break;
    }

    if (!bLoseCharge)
    {
        return;
    }

    float fValue = pBall->mfChargeValue;
    float fChargeLoss = g_fSimulationTick / lbl_806DB50C / 2.0f;
    if (lbl_806E0BCC || GameInfoManager::Instance()->IsRule0x4Equal5())
    {
        pBall->mfChargeValue = 4.0f;
    }
    else
    {
        pBall->mfChargeValue = fValue - fChargeLoss;
    }

    float fMaxCharge = lbl_806DB510 * 4.0f;
    fValue = pBall->mfChargeValue;
    if (fValue >= fMaxCharge)
    {
        pBall->mfChargeValue = fMaxCharge;
    }
    else if (fValue < 0.0f)
    {
        pBall->mfChargeValue = 0.0f;
    }

    fn_801B7A28(pBall);
}

extern "C" float fn_800155A0(cBall* pBall, int nParam)
{
    if (nParam != 0)
    {
        if (!pBall->m_bVisible)
        {
            return 0.0f;
        }

        if (pBall->GetOwnerFielder() != NULL
            && pBall->GetOwnerFielder()->m_eCharacterClass == MYSTERY)
        {
            return 0.0f;
        }

        if (pBall->GetOwnerFielder() != NULL
            && pBall->GetOwnerFielder()->m_eCharacterClass
                == (eCharacterClass)0x13
            && pBall->GetOwnerFielder()->m_eActionState == ACTION_UNKNOWN_32)
        {
            return 0.0f;
        }
    }

    return pBall->mfChargeValue;
}

cPlayer* cBall::GetOwnerGoalie()
{
    cPlayer* player = m_pOwner;
    if ((player == NULL) || (player->m_eClassType != GOALIE))
    {
        return NULL;
    }
    return player;
}

cFielder* cBall::GetPassTargetFielder() const
{
    return GetPassTargetFielderImpl(this);
}

bool cBall::GetInNet(int& nSide)
{
    UnidentifiedGameState* gameState
        = (UnidentifiedGameState*)lbl_806E0C94;
    if (gameState->mUnidentified40 == 0)
    {
        if (m_pPhysicsBall->mbIsInsideNet)
        {
            float fDirection = g_pTeams[0]->m_pNet->m_fDirection;
            nSide = !(m_v3Position.x * fDirection > 1.0f);
            return true;
        }
    }
    else if (gameState->mUnidentified2C > gameState->mUnidentified28
             && gameState->mUnidentified30 != 0)
    {
        float fDirection = g_pTeams[0]->m_pNet->m_fDirection;
        nSide = !(m_v3Position.x * fDirection > 1.0f);
        return true;
    }

    return false;
}

template <typename T>
static inline void UnidentifiedRegisterEventCallback(
    const char* name, void (*callback)(T*))
{
    Function<T*> function(callback);
    unsigned int hash = fn_802B289C(name, -1);
    UnidentifiedEventBase** foundEvent = 0;
    lbl_806E1D90->Find(hash, &foundEvent, 0);
    UnidentifiedEventBase* event
        = foundEvent != 0 ? *foundEvent : 0;
    ((UnidentifiedTypedEvent<T>*)event)->Add(function, 0, -1);
}

static inline void UnidentifiedRegisterEventCallback(
    const char* name, void (*callback)())
{
    Function<FnVoidVoid> function(callback);
    unsigned int hash = fn_802B289C(name, -1);
    UnidentifiedEventBase** foundEvent = 0;
    lbl_806E1D90->Find(hash, &foundEvent, 0);
    UnidentifiedEventBase* event
        = foundEvent != 0 ? *foundEvent : 0;
    ((UnidentifiedTypedEvent0*)event)->Add(function, 0, -1);
}

template <typename P1, typename P2>
static inline void UnidentifiedRegisterEventCallback(
    const char* name, void (*callback)(P1, P2))
{
    Function2<void, P1, P2> function(callback);
    unsigned int hash = fn_802B289C(name, -1);
    UnidentifiedEventBase** foundEvent = 0;
    lbl_806E1D90->Find(hash, &foundEvent, 0);
    UnidentifiedEventBase* event
        = foundEvent != 0 ? *foundEvent : 0;
    ((UnidentifiedTypedEvent2<P1, P2>*)event)->Add(function, 0, -1);
}

extern "C" void fn_80018A00()
{
    UnidentifiedRegisterEventCallback("BallFall", fn_800196FC);
    UnidentifiedRegisterEventCallback(
        "BallStateChange", fn_8001A108);
    UnidentifiedRegisterEventCallback("ResetEffects", fn_800193A0);
    UnidentifiedRegisterEventCallback("Kickoff", fn_800195D8);
    UnidentifiedRegisterEventCallback(
        "GetReadyForKickoff", fn_800194A4);
    UnidentifiedRegisterEventCallback("GameOver", fn_8001929C);
    UnidentifiedRegisterEventCallback(
        "CollisionBallTronWall", fn_80019718);
    UnidentifiedRegisterEventCallback(
        "CollisionEggBall", fn_80019814);
    UnidentifiedRegisterEventCallback(
        "CollisionDebrisBall", fn_80019F10);
    UnidentifiedRegisterEventCallback(
        "CollisionPatchBall", fn_80019910);
    UnidentifiedRegisterEventCallback(
        "CollisionThwompBall", fn_8001A00C);

    lbl_806E0C10 = 0;
    unsigned int i = 0;
    LiveBallTrail* pBallTrail = lbl_8056B518;
    for (; i < 10; ++i)
    {
        pBallTrail->visible = false;
        if (!pBallTrail->visible)
        {
            EmissionManager::Instance()->Destroy(
                (unsigned long)pBallTrail, NULL);
            if (pBallTrail->mUnidentified038 != NULL)
            {
                pBallTrail->mUnidentified038->Die(lbl_806DB54C);
                pBallTrail->mUnidentified038 = NULL;
            }
        }
        ++pBallTrail;
    }
}

extern "C" void fn_8001929C()
{
    cBall* pBall = g_pBall;
    if (pBall == NULL)
    {
        return;
    }

    if (pBall->m_pOwner != NULL)
    {
        fn_80015C38(pBall, 2);
        return;
    }

    bool bPassTarget = (pBall->meBallState == 5
                           || pBall->meBallState == 3)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        if (fn_800DEFD4((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
            if (pReceivePass != NULL
                && pReceivePass->UnidentifiedIsActive()
                && pReceivePass->meDesireSubState == 0)
            {
                return;
            }

            fn_80015C38(pBall, 0);
            return;
        }
    }

    fn_80015C38(pBall, 0);
}

extern "C" void fn_800193A0(void*)
{
    cBall* pBall = g_pBall;
    if (pBall == NULL)
    {
        return;
    }

    if (pBall->m_pOwner != NULL)
    {
        fn_80015C38(pBall, 2);
        return;
    }

    bool bPassTarget = (pBall->meBallState == 5
                           || pBall->meBallState == 3)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        if (fn_800DEFD4((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
            if (pReceivePass != NULL
                && pReceivePass->UnidentifiedIsActive()
                && pReceivePass->meDesireSubState == 0)
            {
                return;
            }

            fn_80015C38(pBall, 0);
            return;
        }
    }

    fn_80015C38(pBall, 0);
}

extern "C" void fn_800194A4(void*)
{
    if (g_pBall == NULL)
    {
        return;
    }

    cFielder* pCaptain
        = g_pTeams[g_pGame->mUnidentified024]->GetCaptain();
    cFielder* pOtherCaptain
        = pCaptain->m_pTeam->GetOtherTeam()->GetCaptain();

    if (pCaptain->GetGlobalPad() != NULL)
    {
        fn_800EDCE8(pCaptain);
        fn_800EBBFC(0, 0xCC32C1A8, NULL, NULL);
        fn_80139D1C(1, pCaptain->GetGlobalPad());
    }

    if (pOtherCaptain->GetGlobalPad() != NULL)
    {
        fn_800EDCE8(pOtherCaptain);
        fn_800EBBFC(0, 0xCC32C1A8, NULL, NULL);
        fn_80139D1C(1, pOtherCaptain->GetGlobalPad());
    }

    if ((g_pTeams[0]->m_nScore > 0 || g_pTeams[1]->m_nScore > 0)
        && pOtherCaptain->CanReceivePass())
    {
        pOtherCaptain->PickupBall(g_pBall);
    }
}

extern "C" void fn_800195D8()
{
    if (g_pBall == NULL)
    {
        return;
    }
    if (g_pTeams[0]->m_nScore != 0)
    {
        return;
    }
    if (g_pTeams[1]->m_nScore != 0)
    {
        return;
    }
    if (g_pBall->m_pOwner != NULL)
    {
        return;
    }

    float fUnidentified0
        = nlRandomf(lbl_806DB548 * lbl_806E0BD8)
        + lbl_806DB548 * (1.0f - lbl_806E0BD8);
    float fUnidentified1
        = nlRandomf(lbl_806E0BD0 * lbl_806E0BD8)
        + lbl_806E0BD0 * (1.0f - lbl_806E0BD8);

    nlVector3 v3Velocity = { fUnidentified0, 0.0f, 0.0f };
    v3Velocity.y
        = 0.5f * fUnidentified1 - nlRandomf(fUnidentified1);
    v3Velocity.x = 0.0f;
    v3Velocity.z
        = 0.75f * lbl_806E0BD4
        + nlRandomf(0.25f * lbl_806E0BD4);

    g_pBall->SetVelocity(v3Velocity, SPINTYPE_NONE, NULL);
}

extern "C" void fn_800196FC(void*)
{
    cBall* pBall = g_pBall;
    if (pBall->meBallState != 10)
    {
        fn_80015C38(pBall, 10);
    }
}

extern "C" void fn_80019718(void*)
{
    cBall* pBall = g_pBall;
    if (pBall->m_pOwner != NULL)
    {
        fn_80015C38(pBall, 2);
        return;
    }

    bool bPassTarget = (pBall->meBallState == 5
                           || pBall->meBallState == 3)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        if (fn_800DEFD4((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
            if (pReceivePass != NULL
                && pReceivePass->UnidentifiedIsActive()
                && pReceivePass->meDesireSubState == 0)
            {
                return;
            }

            fn_80015C38(pBall, 0);
            return;
        }
    }

    fn_80015C38(pBall, 0);
}

extern "C" void fn_80019814(void*)
{
    cBall* pBall = g_pBall;
    if (pBall->m_pOwner != NULL)
    {
        fn_80015C38(pBall, 2);
        return;
    }

    bool bPassTarget = (pBall->meBallState == 5
                           || pBall->meBallState == 3)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        if (fn_800DEFD4((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
            if (pReceivePass != NULL
                && pReceivePass->UnidentifiedIsActive()
                && pReceivePass->meDesireSubState == 0)
            {
                return;
            }

            fn_80015C38(pBall, 0);
            return;
        }
    }

    fn_80015C38(pBall, 0);
}

extern "C" void fn_80019910(PhysicsPatch* pPatch)
{
    if (pPatch->m_Type == 1 && g_pBall->m_pOwner == NULL
        && !fn_800167A8(g_pBall))
    {
        fn_80015B38(g_pBall, false);

        nlVector3 v3Velocity;
        float fLengthSquared = pPatch->m_Velocity.GetLengthSq3D();
        if (fLengthSquared > 4.0f)
        {
            nlVec3Scale(v3Velocity, pPatch->m_Velocity,
                nlRecipSqrt(fLengthSquared, true));
            v3Velocity.z = nlRandomf(0.3f);
            nlVec3Scale(v3Velocity, 4.0f + nlRandomf(5.0f));
        }
        else
        {
            MakeRandomDirection2D(v3Velocity, 2.0f + nlRandomf(3.0f));
            v3Velocity.z = 5.0f + nlRandomf(5.0f);
        }

        g_pBall->SetVelocity(v3Velocity, SPINTYPE_NONE, NULL);
        g_pBall->m_tNoPickupTimer.SetSeconds(0.1f);
        g_pBall->m_pPhysicsBall->mbUseMagnusEffect = false;
        g_pBall->m_pPhysicsBall->mfChargeBonus = 0.0f;
    }
    else if (pPatch->m_Type == 10)
    {
        fn_800154FC(g_pBall, 4.0f);
    }

    if (pPatch->GetPosition().z != 0.0f)
    {
        return;
    }
    if (g_pBall->m_pPhysicsBall->GetPosition().z < 0.207f)
    {
        int nBallState = g_pBall->meBallState;
        if (nBallState >= 6)
        {
            if (nBallState < 9)
            {
                return;
            }
        }
        else if (nBallState == 2)
        {
            return;
        }
    }
    else
    {
        return;
    }

    int nPatchType = pPatch->m_Type;
    UnidentifiedPhysicsPatchInfo_80510BF0* pPatchInfo
        = fn_80174ED4(&nPatchType);
    if (pPatchInfo->mUnidentified18 != 0.0f)
    {
        nlVector3 v3Force;
        g_pBall->m_pPhysicsBall->GetLinearVelocity(&v3Force);
        nlVec3Scale(
            v3Force, -10.0f * pPatchInfo->mUnidentified18);
        g_pBall->m_pPhysicsBall->AddForceAtCentreOfMass(v3Force);
    }

    if (pPatch->m_Type != 9)
    {
        return;
    }
    if (g_pBall->m_pOwner != NULL)
    {
        return;
    }
    if (fn_800167A8(g_pBall))
    {
        return;
    }

    fn_80015B38(g_pBall, false);
    fn_800154FC(g_pBall, 4.0f);

    nlVector3 v3Velocity;
    g_pBall->m_pPhysicsBall->GetLinearVelocity(&v3Velocity);
    v3Velocity.z = 0.0f;
    float fLengthSquared = v3Velocity.GetLengthSq3D();
    if (fLengthSquared < lbl_806DB5A0 * lbl_806DB5A0)
    {
        MakeRandomDirection2D(v3Velocity, lbl_806DB59C);
    }
    else
    {
        float fLength = nlSqrt(fLengthSquared, true);
        if (fLength > lbl_806DB59C)
        {
            fLength = lbl_806DB59C;
        }
        nlVec3Scale(v3Velocity,
            nlRecipSqrt(v3Velocity.GetLengthSq3D(), false));
        nlVec3Scale(v3Velocity, fLength * lbl_806DB594);
    }

    v3Velocity.z = lbl_806DB598;
    g_pBall->SetVelocity(v3Velocity, SPINTYPE_NONE, NULL);
}

extern "C" void fn_80019F10(void*)
{
    cBall* pBall = g_pBall;
    if (pBall->m_pOwner != NULL)
    {
        fn_80015C38(pBall, 2);
        return;
    }

    bool bPassTarget = (pBall->meBallState == 5
                           || pBall->meBallState == 3)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        if (fn_800DEFD4((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
            if (pReceivePass != NULL
                && pReceivePass->UnidentifiedIsActive()
                && pReceivePass->meDesireSubState == 0)
            {
                return;
            }

            fn_80015C38(pBall, 0);
            return;
        }
    }

    fn_80015C38(pBall, 0);
}

extern "C" void fn_8001A00C(void*)
{
    cBall* pBall = g_pBall;
    if (pBall->m_pOwner != NULL)
    {
        fn_80015C38(pBall, 2);
        return;
    }

    bool bPassTarget = (pBall->meBallState == 5
                           || pBall->meBallState == 3)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        if (fn_800DEFD4((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
            if (pReceivePass != NULL
                && pReceivePass->UnidentifiedIsActive()
                && pReceivePass->meDesireSubState == 0)
            {
                return;
            }

            fn_80015C38(pBall, 0);
            return;
        }
    }

    fn_80015C38(pBall, 0);
}

extern "C" void fn_8001A108(int previousState, int currentState)
{
    if (previousState == 8 && currentState != 8)
    {
        g_pBall->mfSkillShotTime = 0.0f;

        cBall* pBall = g_pBall;
        if (fn_80016768(pBall))
        {
            if (lbl_806E0BCC
                || GameInfoManager::Instance()->IsRule0x4Equal5())
            {
                pBall->mfChargeValue = 4.0f;
            }
            else
            {
                pBall->mfChargeValue = 4.0f;
            }

            float fMaxChargeScale = 4.0f;
            float fMaxCharge = lbl_806DB510 * fMaxChargeScale;
            if (pBall->mfChargeValue >= fMaxCharge)
            {
                pBall->mfChargeValue = fMaxCharge;
            }
            else if (pBall->mfChargeValue < 0.0f)
            {
                pBall->mfChargeValue = 0.0f;
            }

            fn_801B7A28(pBall);
        }

        KoopaShellObject* pKoopaShell
            = lbl_806E1608->mUnidentified02C;
        if (pKoopaShell != NULL && pKoopaShell->mVisible)
        {
            fn_801A64A4(pKoopaShell, false);
        }

        State_80199E84* pState = lbl_806E1608->mUnidentified028;
        if (pState != NULL && pState->visible)
        {
            fn_8019A434(pState, false);
        }

        g_pBall->m_pPhysicsBall->mbCanCollidePlayer = true;
        g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;
        g_pBall->m_bVisible = true;
        g_pBall->m_pPhysicsBall->fn_8013FE14();

        if (g_pBall->mbBallOnFire)
        {
            g_pBall->mbBallOnFire = false;
            fn_801B79A4("skillshot_ball_meteor", 0);
        }

        g_pBall->ClearBallEffects();
    }
}

LiveBallTrail::LiveBallTrail()
{
    drawable = NULL;
    mUnidentified038 = NULL;
    visible = false;
    orientation.z = 0.0f;
    orientation.y = 0.0f;
    orientation.x = 0.0f;
    orientation.w = 1.0f;
    position = v3Zero;
    velocity = v3Zero;
    mUnidentified028 = v3Zero;
}

LiveBallTrail::~LiveBallTrail()
{
    if (mUnidentified038 != NULL)
    {
        mUnidentified038->Die(lbl_806DB54C);
        mUnidentified038 = NULL;
    }
}

extern "C" LiveBallTrail* fn_8001B284(unsigned int nIndex)
{
    return &lbl_8056B518[nIndex];
}

extern "C" void fn_8001AA0C(LiveBallTrail* pBallTrail, bool bParam)
{
    pBallTrail->visible = bParam;
    if (!pBallTrail->visible)
    {
        EmissionManager::Instance()->Destroy(
            (unsigned long)pBallTrail, NULL);
        if (pBallTrail->mUnidentified038 != NULL)
        {
            pBallTrail->mUnidentified038->Die(lbl_806DB54C);
            pBallTrail->mUnidentified038 = NULL;
        }
    }
}

extern "C" void fn_8001AA6C(LiveBallTrail* pBallTrail, float fParam)
{
    nlVec3ScaleAdd(pBallTrail->position, fParam,
        pBallTrail->velocity, pBallTrail->position);

    if (pBallTrail->mUnidentified038 != NULL)
    {
        nlVector3 v3Position;
        float fVelocityLengthSq
            = pBallTrail->velocity.GetLengthSq3D();
        if (fVelocityLengthSq > 0.1f)
        {
            float fRecipLength
                = nlRecipSqrt(fVelocityLengthSq, true);
            nlVec3Scale(v3Position,
                pBallTrail->velocity, fRecipLength);
            nlVec3Scale(v3Position, 0.36f);
            nlVec3Add(
                v3Position, v3Position, pBallTrail->position);
        }
        else
        {
            v3Position = pBallTrail->position;
        }

        nlVector3 v3Up = lbl_804DBE48;
        if (pBallTrail->velocity.GetLengthSq3D() < 0.1f)
        {
            v3Up = v3Zero;
        }
        pBallTrail->mUnidentified038->AddViewOrientedPoint(
            v3Position, v3Up);
    }

    if ((float)fabs(pBallTrail->position.x) > 0.02f
        || (float)fabs(pBallTrail->position.y) > 0.02f
        || (float)fabs(pBallTrail->position.z) > 0.02f)
    {
        pBallTrail->visible = false;
        pBallTrail->position = v3Zero;
        pBallTrail->velocity = v3Zero;
    }

    nlVector3 v3Rotation;
    nlVec3Scale(
        v3Rotation, pBallTrail->mUnidentified028, fParam);
    nlQuaternion qOrientation;
    qOrientation.x = pBallTrail->orientation.x
        + 0.5f * (v3Rotation.x * pBallTrail->orientation.w
                     + v3Rotation.y * pBallTrail->orientation.z
                     - v3Rotation.z * pBallTrail->orientation.y);
    qOrientation.y = pBallTrail->orientation.y
        + 0.5f * (v3Rotation.z * pBallTrail->orientation.x
                     + v3Rotation.y * pBallTrail->orientation.w
                     - v3Rotation.x * pBallTrail->orientation.z);
    qOrientation.z = pBallTrail->orientation.z
        + 0.5f * (v3Rotation.x * pBallTrail->orientation.y
                     + v3Rotation.z * pBallTrail->orientation.w
                     - v3Rotation.y * pBallTrail->orientation.x);
    qOrientation.w = pBallTrail->orientation.w
        - 0.5f * (v3Rotation.x * pBallTrail->orientation.x
                     + v3Rotation.y * pBallTrail->orientation.y
                     + v3Rotation.z * pBallTrail->orientation.z);
    nlQuatNormalize(pBallTrail->orientation, qOrientation);
}

extern "C" void fn_8001AD24(
    LiveBallTrail* pBallTrail, cFielder* pFielder)
{
    char effectName[64];
    char textureName[64];

    nlSNPrintf(effectName, sizeof(effectName),
        "%s_megastrike_home_3_gameplay",
        pFielder->mUnidentified11C->mName);

    EffectsGroup* pEffectsGroup = fn_802E7CDC(
        EmissionManager::Instance(), effectName);
    EmissionController* pController = fn_802E7FE4(
        EmissionManager::Instance(), pEffectsGroup, 0, true, false);
    pController->m_fGround = 0.02f;
    pController->SetPosition(pBallTrail->position);
    pController->m_uUserData = (u32)pBallTrail;
    pController->SetUpdateCallback(
        Function1<void, EmissionController&>(fn_801BE950));

    pEffectsGroup = fn_802E7CDC(
        EmissionManager::Instance(), "megastrike_ball_launch");
    pController = fn_802E7FE4(
        EmissionManager::Instance(), pEffectsGroup, 0, true, false);
    pController->SetPosition(pBallTrail->position);
    pController->SetVelocity(v3Zero);

    if (pBallTrail->mUnidentified038 != NULL)
    {
        pBallTrail->mUnidentified038->Die(lbl_806DB54C);
        pBallTrail->mUnidentified038 = NULL;
    }

    switch (pFielder->m_eCharacterClass)
    {
    case (eCharacterClass)1:
        nlStrNCpy(textureName,
            sUnidentifiedShootToScoreBallBlurTexture0,
            sizeof(textureName));
        break;
    case (eCharacterClass)9:
        nlStrNCpy(textureName,
            sUnidentifiedShootToScoreBallBlurTexture1,
            sizeof(textureName));
        break;
    case (eCharacterClass)2:
        nlStrNCpy(textureName,
            szDaisyShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case (eCharacterClass)3:
        nlStrNCpy(textureName,
            szDonkeyKongShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case (eCharacterClass)10:
        nlStrNCpy(textureName,
            sUnidentifiedShootToScoreBallBlurTexture2,
            sizeof(textureName));
        break;
    case (eCharacterClass)4:
        nlStrNCpy(textureName,
            szLuigiShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case (eCharacterClass)0:
        nlStrNCpy(textureName,
            szMarioShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case (eCharacterClass)5:
        nlStrNCpy(textureName,
            szPeachShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case (eCharacterClass)11:
        nlStrNCpy(textureName,
            sUnidentifiedShootToScoreBallBlurTexture3,
            sizeof(textureName));
        break;
    case (eCharacterClass)6:
        nlStrNCpy(textureName,
            szWaluigiShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case (eCharacterClass)7:
        nlStrNCpy(textureName,
            szWarioShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    case (eCharacterClass)8:
        nlStrNCpy(textureName,
            szYoshiShootToScoreBallBlurTexture,
            sizeof(textureName));
        break;
    }

    pBallTrail->mUnidentified038 = BlurManager::GetNewHandler(
        textureName,
        0.18f * fShootToScoreBallBlurWidth * 0.925f,
        nShootToScoreBallBlurLength,
        true);
}

extern "C" void fn_8001B298(float fParam)
{
    LiveBallTrail* pBallTrail = lbl_8056B518;
    for (unsigned int i = 0; i < lbl_806E0C10; ++i)
    {
        if (pBallTrail->visible)
        {
            fn_8001AA6C(pBallTrail, fParam);
        }
        ++pBallTrail;
    }
}

extern "C" unsigned int fn_8001B30C()
{
    return lbl_806E0C10;
}

extern "C" void fn_8001B314(unsigned int nNumTrails)
{
    lbl_806E0C10 = nNumTrails;
    nlVector3 v3Unidentified = v3Zero;

    unsigned int i = 0;
    for (; i < nNumTrails; ++i)
    {
        LiveBallTrail* pBallTrail = &lbl_8056B518[i];
        fn_8001AA0C(pBallTrail, false);
        pBallTrail->position = v3Unidentified;
        pBallTrail->velocity = v3Unidentified;
        pBallTrail->drawable = fn_8027638C(i);
    }

    for (; i < 10; ++i)
    {
        LiveBallTrail* pBallTrail = &lbl_8056B518[i];
        fn_8001AA0C(pBallTrail, false);
    }
}

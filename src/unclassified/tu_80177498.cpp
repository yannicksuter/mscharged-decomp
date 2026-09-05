#include "unclassified/tu_80177498.h"

#include "Game/AI/Fielder.h"
#include "Game/Ball.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Event.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Team.h"
#include "NL/nlAVLTree.h"
#include "NL/nlMemory.h"
#include "NL/platvmath.h"

typedef nlAVLTree<unsigned int, UnidentifiedEventBase*,
    DefaultKeyCompare<unsigned int> >
    UnidentifiedEventRegistry_80177498;

extern "C" UnidentifiedEventRegistry_80177498* lbl_806E1D90;
extern "C" bool fn_802B6BC8(const nlVector3*, const nlVector3*,
    const nlVector3*, const nlVector3*, float*, float*);
extern "C" WaluigiWallGeometry_80177498* fn_8000DDE8(void*, int,
    const nlVector3*, const nlVector3*, float);
extern "C" void fn_8014A180(void*);
extern "C" void fn_802E8A2C(EmissionManager*, EffectsGroup*);
extern "C" EffectsGroup* fn_802E7CDC(EmissionManager*, const char*);
extern "C" EmissionController* fn_802E7FE4(
    EmissionManager*, EffectsGroup*, int, bool, bool);

extern "C" void fn_80179390(EmissionController&, int);
extern "C" void fn_801793D8(EmissionController&);
extern "C" void fn_80179490(cFielder*);
extern "C" void fn_801794A4(cFielder*);
extern "C" void fn_80179580(cFielder*);
extern "C" void fn_80179670(void*);

extern char lbl_80511054[];
extern char lbl_8051106C[];
extern char lbl_80511080[];
extern char lbl_80511090[];
extern char lbl_805110A8[];

float lbl_806DCB10 = 1.0f;
float lbl_806DCB14 = 1.2f;
float lbl_806DCB18 = 16.5f;
float lbl_806DCB1C = 4.2f;
float lbl_806DCB20 = 0.5f;
float lbl_806DCB24 = 0.5f;
float lbl_806DCB28 = 0.5f;
float lbl_806DCB2C = 0.5f;
float lbl_806DCB30[2] = { 0.75f, 0.0f };

unsigned int lbl_806E12E8;
unsigned int lbl_806E12EC;
bool lbl_806E12F0;

static const float sWallThickness = 0.1f;
static const float sOne = 1.0f;
static const float sAngleToRadians = 0.0000958738f;
static const float sMinimumLengthSquared = 0.1f;
static const float sHalf = 0.5f;
static const float sMaximumSegmentAge = 5.0f;
static const float sWallLifetime = 25.0f;

inline PhysicsBox_80177498::PhysicsBox_80177498(
    cFielder* pParam, float fParam1, float fParam2)
    : PhysicsBox(g_CollisionSpace, 0, sWallThickness, fParam1, fParam2)
    , mUnidentified050(pParam)
    , mUnidentified054(0)
    , mUnidentified058(sOne)
    , mUnidentified05C(0.0f)
    , mUnidentified060(++lbl_806E12EC)
    , mUnidentified064(false)
    , mUnidentified068(0)
{
    nlMatrix3 mRotation;
    nlMakeRotationMatrixZ(
        mRotation, sAngleToRadians * pParam->m_aActualFacingDirection);

    mUnidentified038.x
        = pParam->m_v3Position.x + lbl_806DCB1C * mRotation.m11;
    mUnidentified038.y
        = pParam->m_v3Position.y + lbl_806DCB1C * mRotation.m12;
    mUnidentified038.z
        = pParam->m_v3Position.z + lbl_806DCB1C * mRotation.m13;
    mUnidentified044.x
        = pParam->m_v3Position.x - lbl_806DCB1C * mRotation.m11;
    mUnidentified044.y
        = pParam->m_v3Position.y - lbl_806DCB1C * mRotation.m12;
    mUnidentified044.z
        = pParam->m_v3Position.z - lbl_806DCB1C * mRotation.m13;

    nlVector3 v3Position;
    nlVec3WeightedSum(v3Position, sHalf, mUnidentified038,
        sHalf, mUnidentified044);
    v3Position.z = sHalf * fParam2;
    SetPosition(v3Position, WORLD_COORDINATES);
    SetRotation(mRotation, WORLD_COORDINATES);
    SetCategory(0x4000);
    SetCollide(0x1B062);

    void* pStorage = nlMalloc(0x68, 8, false);
    if (pStorage != 0)
    {
        mUnidentified068 = fn_8000DDE8(pStorage, 4,
            &mUnidentified038, &mUnidentified044, lbl_806DCB18);
    }
}

PhysicsBox_80177498::~PhysicsBox_80177498()
{
    delete mUnidentified068;
    if (mUnidentified054 != 0)
    {
        mUnidentified054->m_uUserData = 0;
    }
}

bool PhysicsBox_80177498::SetContactInfo(
    dContact* pContact, PhysicsObject*, bool bFirst)
{
    if (bFirst)
    {
        SetDefaultContactInfo(pContact);
    }
    pContact->surface.bounce = 0.15f;
    pContact->surface.mu = 0.005f;
    pContact->surface.bounce_vel = 0.0f;
    return true;
}

void PhysicsBox_80177498::PreCollide()
{
    if (g_pBall == 0)
    {
        return;
    }

    nlVector3 v3Start = g_pBall->m_v3Position;
    nlVector3 v3End;
    float fTick = FixedUpdateTask::GetPhysicsUpdateTick();
    nlVec3ScaleAdd(v3End, fTick, g_pBall->m_v3Velocity, v3Start);

    float fBallTime;
    float fWallTime;
    if (fn_802B6BC8(&v3Start, &v3End, &mUnidentified038,
            &mUnidentified044, &fBallTime, &fWallTime))
    {
        nlVector3 v3Position;
        nlVecLerp(v3Position, v3Start, v3End, fBallTime);
        g_pBall->SetPosition(v3Position);

        nlVector3 v3Normal;
        v3Normal.x = mUnidentified038.y - mUnidentified044.y;
        v3Normal.y = mUnidentified044.x - mUnidentified038.x;
        v3Normal.z = 0.0f;
        float fNormalScale = nlRecipSqrt(
            v3Normal.x * v3Normal.x + v3Normal.y * v3Normal.y, true);
        v3Normal.x *= fNormalScale;
        v3Normal.y *= fNormalScale;

        nlVector3 v3Velocity = g_pBall->m_v3Velocity;
        float fAlongNormal = nlVec3DotProduct(v3Velocity, v3Normal);
        v3Velocity.x -= 2.0f * fAlongNormal * v3Normal.x;
        v3Velocity.y -= 2.0f * fAlongNormal * v3Normal.y;
        g_pBall->SetVelocity(v3Velocity, SPINTYPE_NONE, 0);
    }
}

void PhysicsBox_80177498::fn_80177E44(const nlVector3& v3Param)
{
    mUnidentified044 = v3Param;

    nlVector3 v3Position;
    nlVec3WeightedSum(v3Position, sHalf, mUnidentified038,
        sHalf, mUnidentified044);

    float afSides[3];
    GetSides(afSides);
    v3Position.z = sHalf * afSides[2];
    SetPosition(v3Position, WORLD_COORDINATES);

    nlVector3 v3Direction;
    nlVec3Sub(v3Direction, mUnidentified044, mUnidentified038);
    float fLength = nlGetLength2D(v3Direction.x, v3Direction.y);
    if (fLength > 0.001f)
    {
        v3Direction.x /= fLength;
        v3Direction.y /= fLength;
    }
    else
    {
        v3Direction.x = 1.0f;
        v3Direction.y = 0.0f;
        v3Direction.z = 0.0f;
    }

    SetSides(fLength, afSides[1], afSides[2]);
    nlMatrix3 mRotation;
    mRotation.m11 = v3Direction.x;
    mRotation.m12 = v3Direction.y;
    mRotation.m13 = 0.0f;
    mRotation.m21 = -v3Direction.y;
    mRotation.m22 = v3Direction.x;
    mRotation.m23 = 0.0f;
    mRotation.m31 = 0.0f;
    mRotation.m32 = 0.0f;
    mRotation.m33 = 1.0f;
    SetRotation(mRotation, WORLD_COORDINATES);

    if (mUnidentified054 != 0)
    {
        mUnidentified054->SetPosition(v3Position);
    }
}

void PhysicsBox_80177498::fn_80178170(float fParam)
{
    mUnidentified058 -= fParam;
    if (mUnidentified058 <= 0.0f)
    {
        mUnidentified058 = 0.0f;
        mUnidentified064 = true;
    }
}

ContactType PhysicsBox_80177498::fn_8017819C(cFielder* pParam)
{
    if (pParam == mUnidentified050)
    {
        return NO_CONTACT;
    }

    fn_80178170(lbl_806DCB20);
    return ONE_WAY_CONTACT_OTHER;
}

ContactType PhysicsBox_80177498::Contact(
    PhysicsObject* pOther, dContact*, int)
{
    int nType = pOther->GetObjectType();
    switch (nType)
    {
    case 4:
        if (pOther->m_parentObject != 0)
        {
            PhysicsCharacter* pCharacter
                = (PhysicsCharacter*)pOther->m_parentObject;
            return fn_8017819C((cFielder*)pCharacter->m_pAICharacter);
        }
        return NO_CONTACT;
    case 16:
    case 24:
    case 26:
    case 33:
    case 34:
        fn_80178170(lbl_806DCB24);
        return ONE_WAY_CONTACT_OTHER;
    case 20:
    case 21:
    case 32:
        fn_80178170(lbl_806DCB28);
        return NO_CONTACT;
    case 27:
    case 28:
        fn_80178170(lbl_806DCB2C);
        return NO_CONTACT;
    default:
        return NO_CONTACT;
    }
}

char lbl_80511054[] = "waluigi_super_ability";
char lbl_8051106C[] = "WaluigiWallStart";
char lbl_80511080[] = "WaluigiWallEnd";
char lbl_80511090[] = "WaluigiWallAbort";
char lbl_805110A8[] = "MegastrikeStart";

WaluigiWallManager_80178400::WaluigiWallManager_80178400()
    : mUnidentified050(0)
    , mUnidentified054(0)
    , mUnidentified058(0)
    , mUnidentified05C(false)
    , mUnidentified060(0.0f)
    , mUnidentified064(0)
    , mUnidentified068(0)
    , mUnidentified06C(0)
    , mUnidentified070(0)
{
    {
        Function<void*> callback((void (*)(void*))fn_80179490);
        UnidentifiedEventBase** ppEvent;
        unsigned int uHash;
        uHash = fn_802B289C(lbl_8051106C, -1);
        ppEvent = 0;
        lbl_806E1D90->Find(uHash, &ppEvent, 0);
        UnidentifiedEventBase* pEvent = ppEvent != 0 ? *ppEvent : 0;
        ((UnidentifiedTypedEvent<void>*)pEvent)
            ->Add(callback, (unsigned int)&mUnidentified064, -1);
    }
    {
        Function<void*> callback((void (*)(void*))fn_801794A4);
        UnidentifiedEventBase** ppEvent;
        unsigned int uHash;
        uHash = fn_802B289C(lbl_80511080, -1);
        ppEvent = 0;
        lbl_806E1D90->Find(uHash, &ppEvent, 0);
        UnidentifiedEventBase* pEvent = ppEvent != 0 ? *ppEvent : 0;
        ((UnidentifiedTypedEvent<void>*)pEvent)
            ->Add(callback, (unsigned int)&mUnidentified068, -1);
    }
    {
        Function<void*> callback((void (*)(void*))fn_80179580);
        UnidentifiedEventBase** ppEvent;
        unsigned int uHash;
        uHash = fn_802B289C(lbl_80511090, -1);
        ppEvent = 0;
        lbl_806E1D90->Find(uHash, &ppEvent, 0);
        UnidentifiedEventBase* pEvent = ppEvent != 0 ? *ppEvent : 0;
        ((UnidentifiedTypedEvent<void>*)pEvent)
            ->Add(callback, (unsigned int)&mUnidentified06C, -1);
    }
    {
        Function<void*> callback(fn_80179670);
        UnidentifiedEventBase** ppEvent;
        unsigned int uHash;
        uHash = fn_802B289C(lbl_805110A8, -1);
        ppEvent = 0;
        lbl_806E1D90->Find(uHash, &ppEvent, 0);
        UnidentifiedEventBase* pEvent = ppEvent != 0 ? *ppEvent : 0;
        ((UnidentifiedTypedEvent<void>*)pEvent)
            ->Add(callback, (unsigned int)&mUnidentified070, -1);
    }

    for (unsigned int i = 0; i < 20; ++i)
    {
        mUnidentified000[i] = 0;
    }
    ++lbl_806E12E8;
}

WaluigiWallManager_80178400::~WaluigiWallManager_80178400()
{
    --lbl_806E12E8;
    for (unsigned int i = 0; i < 20; ++i)
    {
        if (mUnidentified000[i] != 0)
        {
            delete mUnidentified000[i];
            mUnidentified000[i] = 0;
        }
    }
    EffectsGroup* pGroup = fn_802E7CDC(
        EmissionManager::Instance(), lbl_80511054);
    fn_802E8A2C(EmissionManager::Instance(), pGroup);
    mUnidentified050 = 0;
    mUnidentified054 = 0;

    if (lbl_806E12E8 == 0)
    {
        PhysicsBox_80177498::pool.FreeBlocks();
        lbl_806E12EC = 0;
    }

    UnidentifiedDisconnectOwner(&mUnidentified070);
    UnidentifiedDisconnectOwner(&mUnidentified06C);
    UnidentifiedDisconnectOwner(&mUnidentified068);
    UnidentifiedDisconnectOwner(&mUnidentified064);
}

PhysicsBox_80177498* WaluigiWallManager_80178400::fn_80178968(
    cFielder* pParam, float fParam1, float fParam2)
{
    int nIndex;
    for (nIndex = 0; nIndex < 20; ++nIndex)
    {
        if (mUnidentified000[nIndex] == 0)
        {
            break;
        }
    }
    if (nIndex == 20)
    {
        return 0;
    }

    PhysicsBox_80177498* pObject
        = new PhysicsBox_80177498(pParam, fParam1, fParam2);
    mUnidentified000[nIndex] = pObject;
    if (mUnidentified050 != 0)
    {
        mUnidentified054 = mUnidentified050;
    }
    mUnidentified050 = pObject;
    mUnidentified058 = pParam;

    if (!mUnidentified05C)
    {
        EmissionManager* pManager = EmissionManager::Instance();
        EffectsGroup* pGroup = fn_802E7CDC(pManager, lbl_80511054);
        if (pGroup != 0)
        {
            EmissionController* pController
                = fn_802E7FE4(pManager, pGroup, 3, true, false);
            if (pController != 0)
            {
                pController->SetPosition(pParam->m_v3Position);
                pController->SetVelocity(pParam->m_v3Velocity);
                pController->m_uUserData = (unsigned long)pObject;
                pController->SetUpdateCallback(
                    Function1<void, EmissionController&>(fn_801793D8));
                pObject->mUnidentified054 = pController;
                mUnidentified05C = true;
            }
        }
    }
    return pObject;
}

void WaluigiWallManager_80178400::fn_80178D0C()
{
    for (unsigned int i = 0; i < 20; ++i)
    {
        if (mUnidentified000[i] != 0)
        {
            delete mUnidentified000[i];
            mUnidentified000[i] = 0;
        }
    }
    EffectsGroup* pGroup = fn_802E7CDC(
        EmissionManager::Instance(), lbl_80511054);
    fn_802E8A2C(EmissionManager::Instance(), pGroup);
    mUnidentified050 = 0;
    mUnidentified054 = 0;
}

void WaluigiWallManager_80178400::fn_80178DBC(float fDeltaT)
{
    if (mUnidentified060 > 0.0f)
    {
        mUnidentified060 -= fDeltaT;
    }

    PhysicsBox_80177498* pNewest = 0;
    for (int i = 0; i < 20; ++i)
    {
        PhysicsBox_80177498* pObject = mUnidentified000[i];
        if (pObject == 0)
        {
            continue;
        }

        if (pObject != mUnidentified050)
        {
            pObject->mUnidentified05C += fDeltaT;
        }
        pObject->fn_80178170(fDeltaT / sWallLifetime);

        if (lbl_806E12F0
            && pObject->mUnidentified05C >= sMaximumSegmentAge)
        {
            nlVector3 v3End;
            nlVecLerp(v3End, pObject->mUnidentified044,
                pObject->mUnidentified038, lbl_806DCB18 * fDeltaT);
            pObject->fn_80177E44(v3End);
        }

        if (pObject->mUnidentified064)
        {
            if (mUnidentified050 == pObject)
            {
                mUnidentified050 = 0;
            }
            if (mUnidentified054 == pObject)
            {
                mUnidentified054 = 0;
            }
            delete pObject;
            mUnidentified000[i] = 0;
            continue;
        }

        if (pNewest == 0
            || pNewest->mUnidentified060 < pObject->mUnidentified060)
        {
            pNewest = pObject;
        }
    }

    if (!lbl_806E12F0 && pNewest != 0 && pNewest != mUnidentified050
        && mUnidentified060 <= 0.0f
        && pNewest->mUnidentified05C >= sMaximumSegmentAge)
    {
        pNewest->fn_80177E44(pNewest->mUnidentified038);
    }
}

PhysicsBox_80177498* WaluigiWallManager_80178400::fn_801792C4(
    int nIndex)
{
    return mUnidentified000[nIndex];
}

PhysicsBox_80177498* WaluigiWallManager_80178400::fn_801792D0(
    unsigned int uParam)
{
    for (int i = 0; i < 20; ++i)
    {
        PhysicsBox_80177498* pObject = mUnidentified000[i];
        if (pObject != 0 && uParam == pObject->mUnidentified060)
        {
            return pObject;
        }
    }
    return 0;
}

extern "C" void fn_80179390(
    EmissionController& rController, int nParam)
{
    if (!rController.m_GlView && nParam == 2)
    {
        PhysicsBox_80177498* pObject
            = (PhysicsBox_80177498*)rController.m_uUserData;
        if (pObject != 0)
        {
            pObject->mUnidentified054 = 0;
        }
    }
}

extern "C" void fn_801793D8(EmissionController& rController)
{
    PhysicsBox_80177498* pObject
        = (PhysicsBox_80177498*)rController.m_uUserData;
    if (pObject != 0 && pObject->mUnidentified050 != 0)
    {
        rController.SetPosition(pObject->mUnidentified050->m_v3Position);
        rController.SetVelocity(pObject->mUnidentified050->m_v3Velocity);
    }
}

extern "C" void fn_80179490(cFielder* pParam)
{
    pParam->mUnidentified400->fn_80178968(
        pParam, lbl_806DCB10, lbl_806DCB14);
}

extern "C" void fn_801794A4(cFielder* pParam)
{
    WaluigiWallManager_80178400* pManager = pParam->mUnidentified400;
    PhysicsBox_80177498* pObject = pManager->mUnidentified050;
    if (pObject != 0)
    {
        EffectsGroup* pGroup = fn_802E7CDC(
            EmissionManager::Instance(), lbl_80511054);
        if (EmissionManager::Instance()->IsPlaying(
                (unsigned long)pObject, pGroup))
        {
            EmissionManager::Instance()->Kill(
                (unsigned long)pObject, pGroup);
        }
        pManager->mUnidentified05C = false;
        pObject->mUnidentified05C = 0.0f;

        nlVector3 v3Direction;
        nlVec3Sub(v3Direction,
            pObject->mUnidentified038, pObject->mUnidentified044);
        if (v3Direction.GetLengthSq2D() < sMinimumLengthSquared)
        {
            pObject->mUnidentified064 = true;
        }
        else
        {
            pManager->mUnidentified054 = pObject;
        }
        pManager->mUnidentified050 = 0;
    }
}

extern "C" void fn_80179580(cFielder* pParam)
{
    WaluigiWallManager_80178400* pManager = pParam->mUnidentified400;
    PhysicsBox_80177498* pObject = pManager->mUnidentified050;
    if (pObject != 0)
    {
        EffectsGroup* pGroup = fn_802E7CDC(
            EmissionManager::Instance(), lbl_80511054);
        if (EmissionManager::Instance()->IsPlaying(
                (unsigned long)pObject, pGroup))
        {
            EmissionManager::Instance()->Kill(
                (unsigned long)pObject, pGroup);
        }
        pManager->mUnidentified05C = false;
        pObject->mUnidentified05C = 0.0f;

        nlVector3 v3Direction;
        nlVec3Sub(v3Direction,
            pObject->mUnidentified038, pObject->mUnidentified044);
        if (v3Direction.GetLengthSq2D() < sMinimumLengthSquared)
        {
            pObject->mUnidentified064 = true;
        }
        else
        {
            pManager->mUnidentified054 = pObject;
        }
        pManager->mUnidentified050 = 0;
        fn_8014A180(pParam);
    }
}

extern "C" void fn_80179670(void*)
{
    for (int nTeam = 0; nTeam < 2; ++nTeam)
    {
        cTeam* pTeam = g_pTeams[nTeam];
        if (pTeam == 0)
        {
            continue;
        }
        for (int nFielder = 0; nFielder < 4; ++nFielder)
        {
            cFielder* pFielder = pTeam->GetFielder(nFielder);
            if (pFielder != 0 && pFielder->mUnidentified400 != 0)
            {
                pFielder->mUnidentified400->fn_80178D0C();
            }
        }
    }
}

SlotPool<PhysicsBox_80177498> PhysicsBox_80177498::pool(16, 16);

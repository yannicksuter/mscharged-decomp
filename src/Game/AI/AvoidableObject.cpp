#include "Game/AI/AvoidableObject.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Desire.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/Game.h"
#include "Game/GameTweaks.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Physics/PhysicsSphere_801798A8.h"
#include "Game/Player.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/ChainChomp.h"
#include "Game/Team.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlList.h"
#include "NL/nlMath.h"
#include "unclassified/tu_801B535C.h"

extern cTeam* g_pCurrentlyUpdatingTeam;

extern "C" void fn_8000F594(AvoidableObject* pObject);
extern "C" float fn_80030750(cFielder*);
extern "C" bool fn_8003E8A0(cFielder*);
extern "C" bool fn_8003E948(cFielder*);
extern "C" bool fn_8003E99C(cFielder*);
extern "C" float fn_8002BFA8(PlayerTweaks*, float);
extern "C" float fn_800D6E54(cFielder*);
extern "C" float fn_800D9EC4(cPlayer*);
extern "C" float fn_800DEAB4(cFielder*);
extern "C" float fn_800DED80(cFielder*);
extern "C" void fn_802B5CC0(
    nlVector4& out, const nlVector2& point, const nlVector2& normal);
extern "C" void fn_802B5D10(
    nlVector4& out, const nlVector3& point, const nlVector3& normal);
extern "C" float fn_802B5DD0(const nlVector2& point, const nlVector4& plane);

// The desire queried by the fielder strength rule carries its target fielder
// at 0xB8; the concrete desire class is not reconstructed yet.
struct UnidentifiedDesire_8000D62C
{
    /* 0x00 */ u8 mUnidentified000[0x8];
    /* 0x08 */ bool mUnidentifiedActive;
    /* 0x09 */ u8 mUnidentified009[0xAF];
    /* 0xB8 */ cFielder* mUnidentified0B8;
};

static const nlVector2 v2Zero = { 0.0f, 0.0f };
static const nlVector2 sUnidentifiedStrengthExtrema = { 0.5f, 1.0f };

float sUnidentifiedTweaks[7][7] = {
    { 1.0f, 1.0f, 3.0f, 0.0f, 5.0f, 6.5f, 5.5f },
    { 2.0f, 4.0f, 8.0f, 0.0f, 6.0f, 10.0f, 15.0f },
    { 3.0f, 3.0f, 5.0f, 0.0f, 4.0f, 6.5f, 5.0f },
    { 3.0f, 2.0f, 4.0f, 0.0f, 3.0f, 10.0f, 10.0f },
    { 3.0f, 2.0f, 4.0f, 0.0f, 3.0f, 6.0f, 5.0f },
    { 4.0f, 1.5f, 5.0f, 0.0f, 3.0f, 8.0f, 8.0f },
    { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
};

static float sBananaAvoidanceStrengthScale = 0.5f;

nlList<AvoidableObject> gAvoidableObjects(0, 0);
int gNextAvoidableObjectId;

static inline int AvoidableEnumToIndex(eAvoidableThings avoidable)
{
    if (avoidable == AVOID_EVERYTHING)
    {
        return 7;
    }
    for (int i = 0; i < NUM_AVOIDABLES; i++)
    {
        if ((1 << i) & (int)avoidable)
        {
            return i;
        }
    }
    return -1;
}

AvoidableObject::AvoidableObject(int type)
{
    int nIndex = AvoidableEnumToIndex((eAvoidableThings)type);
    mTweaks = sUnidentifiedTweaks[nIndex];
    mType = type;
    nlListAddEnd(&gAvoidableObjects.m_pStart, &gAvoidableObjects.m_pEnd, this);
    mUnidentified008 = gNextAvoidableObjectId;
    gNextAvoidableObjectId++;
}

AvoidableObject::~AvoidableObject()
{
    fn_8000F594(this);
    nlListRemoveElement(&gAvoidableObjects.m_pStart, this, &gAvoidableObjects.m_pEnd);
}

int GetAvoidableIndex(eAvoidableThings avoidable)
{
    return AvoidableEnumToIndex(avoidable);
}

int GetAvoidableMask(int index)
{
    int avoidable = 1 << index;
    if (index == 7)
    {
        avoidable = AVOID_EVERYTHING;
    }
    return avoidable;
}

bool AvoidableObject::UnidentifiedVirtual20(
    AvoidableObject* other, float range)
{
    const nlVector3& v3OtherPos = other->GetPosition();
    const nlVector3& v3Pos = GetPosition();
    nlVector2 v2Diff;
    v2Diff.x = v3Pos.x - v3OtherPos.x;
    v2Diff.y = v3Pos.y - v3OtherPos.y;
    float fDist = nlVec2Length(v2Diff);
    float fOtherRadius = other->GetRadius();
    float fRadius = GetRadius();
    float fGap = fDist - (fRadius + fOtherRadius);
    if (range <= 0.0f)
    {
        range = other->mTweaks[2];
    }
    return fGap <= range;
}

bool AvoidableObject::UnidentifiedVirtual1C(
    const nlVector3& target, nlVector3& point, nlVector3& dir)
{
    bool bInside = false;
    const nlVector3& v3Pos = GetPosition();
    nlVec3Sub(dir, target, v3Pos);
    dir.z = 0.0f;
    float fRadius = GetRadius();
    float fLengthSq = dir.GetLengthSq3D();
    if (fLengthSq < fRadius * fRadius)
    {
        bInside = true;
    }
    if (nlNear(fLengthSq, 0.0f))
    {
        nlPolar polar = { 0, 1.0f };
        polar.a = nlRandom(0xFFFF);
        nlPolarToCartesian(dir, polar);
    }
    else
    {
        float fScale = nlRecipSqrt(dir.GetLengthSq3D(), true);
        nlVec3Scale(dir, fScale);
    }
    const nlVector3& v3Origin = GetPosition();
    float fScale = GetRadius();
    nlVec2Set(*(nlVector2*)&point, fScale * dir.x + v3Origin.x,
        fScale * dir.y + v3Origin.y);
    point.z = 0.0f;
    return bInside;
}

const nlVector3& AvoidableFielder::GetPosition()
{
    return m_pFielder->mUnidentified024.m_v3Position;
}

const nlVector3& AvoidableFielder::GetVelocity()
{
    return m_pFielder->mUnidentified024.m_v3Velocity;
}

float AvoidableFielder::GetRadius()
{
    float fRadius = 0.0f;
    if (m_pFielder->fn_8003EA6C())
    {
        fRadius = gNPCManager->mUnidentified024->mUnidentified2C->GetRadius();
    }
    else
    {
        m_pFielder->m_pPhysicsCharacter->GetRadius(&fRadius);
    }
    return fRadius;
}

float AvoidableFielder::UnidentifiedVirtual18()
{
    float fRadius;
    if (fn_8003E8A0(m_pFielder))
    {
        fRadius = 11.0f;
    }
    else if (m_pFielder->fn_8003E9F0())
    {
        fRadius = 11.0f;
    }
    else
    {
        float fTime = m_pFielder->mUnidentified024.m_fPlayerScale;
        float fValue = fn_8002BFA8(m_pFielder->GetTweaks(), fTime);
        fRadius = fn_80030750(m_pFielder) - fValue;
    }
    if (m_pFielder->fn_8003EA6C())
    {
        fRadius += 2.0f;
    }
    return fRadius;
}

bool AvoidableFielder::UnidentifiedVirtual20(
    AvoidableObject* other, float range)
{
    int nIndex = m_pFielder->mUnidentified120;
    int otherType = other->mType;
    cPlayer* pOther = 0;
    int nOtherIndex = -1;
    if (otherType == AVOID_FIELDERS)
    {
        pOther = ((AvoidableFielder*)other)->m_pFielder;
        nOtherIndex = pOther->mUnidentified120;
    }
    else if (otherType == AVOID_GOALIES)
    {
        pOther = ((AvoidableGoalie*)other)->m_pPlayer;
        nOtherIndex = pOther->mUnidentified120;
    }
    if (range <= 0.0f)
    {
        range = other->mTweaks[2];
    }
    if (nOtherIndex >= 0)
    {
        float fDist = g_pGame->fn_8005B748(nIndex, nOtherIndex);
        float fOtherRadius = other->GetRadius();
        float fRadius = GetRadius();
        float fGap = fDist - (fRadius + fOtherRadius);
        if (!m_pFielder->IsOnSameTeam(pOther))
        {
            fGap -= other->UnidentifiedVirtual18();
        }
        return fGap <= range;
    }
    else if (otherType == AVOID_UNIDENTIFIED_08)
    {
        return other->UnidentifiedVirtual20(this, range);
    }
    else
    {
        return AvoidableObject::UnidentifiedVirtual20(other, range);
    }
}

float AvoidableFielder::UnidentifiedVirtual28(
    AvoidableObject* other)
{
    float fSkill = fn_800A636C(g_pCurrentlyUpdatingTeam)->Off_Avoidance->GetValue();
    float fMin = sUnidentifiedStrengthExtrema.x;
    float fMax = sUnidentifiedStrengthExtrema.y;
    float fStrength = InterpolateClamped(fMin, fMax, fSkill);
    if (m_pFielder->m_pBall != 0)
    {
        fStrength *= 1.5f;
    }

    switch (other->mType)
    {
    case AVOID_GOALIES:
        if (m_pFielder->IsOnSameTeam(
                ((AvoidableGoalie*)other)->m_pPlayer))
        {
            fStrength *= 0.5f;
        }
        m_pFielder->fn_8003EA6C();
        break;
    case AVOID_UNIDENTIFIED_08:
    {
        AvoidablePolygon* pPolygon
            = (AvoidablePolygon*)other;
        if (pPolygon->mUnidentified014 == 4)
        {
            if (fn_8003E948(m_pFielder) && m_pFielder->m_pBall == 0
                && fn_800DED80(m_pFielder) > 0.7f)
            {
                fStrength = 0.0f;
            }
            else
            {
                fStrength *= 1.3f;
            }
        }
        else if (pPolygon->mUnidentified014 == 1 && m_pFielder->fn_8003EA6C())
        {
            fStrength *= 0.3f;
        }
        else if (pPolygon->mUnidentified014 != 3)
        {
            if (fn_800DED80(m_pFielder))
            {
                fStrength *= 0.0f;
            }
        }
        break;
    }
    case AVOID_FIELDERS:
    {
        cFielder* pOther
            = ((AvoidableFielder*)other)->m_pFielder;
        if (fn_800DED80(m_pFielder) && !fn_800D9EC4(pOther))
        {
            fStrength *= 0.2f;
        }
        else
        {
            if (m_pFielder->fn_800306F4(pOther))
            {
                fStrength *= 0.4f;
            }
            float fValue = fn_800D6E54(pOther);
            if (pOther->IsOnSameTeam(m_pFielder) && fValue >= 0.7f)
            {
                fStrength *= 2.0f;
            }
        }
        if (fn_8003E8A0(m_pFielder) || m_pFielder->fn_8003E9F0()
            || fn_8003E99C(m_pFielder))
        {
            cFielder* pTarget = 0;
            UnidentifiedDesire_8000D62C* pDesire
                = (UnidentifiedDesire_8000D62C*)fn_8002E08C(m_pFielder, 12);
            if (pDesire != 0 && pDesire->mUnidentifiedActive)
            {
                pTarget = pDesire->mUnidentified0B8;
            }
            if (pTarget == pOther)
            {
                fStrength *= 0.0f;
            }
            else
            {
                fStrength *= 0.2f;
            }
        }
        break;
    }
    case AVOID_UNIDENTIFIED_20:
    {
        switch (((AvoidablePatch*)other)->m_pPatch->m_Type)
        {
        case 1:
        case 8:
        case 9:
        case 10:
            fStrength *= 1.5f;
            break;
        }
        break;
    }
    case AVOID_POWERUPS:
    {
        PowerupBase* pPowerup
            = ((AvoidablePowerup*)other)->m_pPowerup;
        if (pPowerup != 0 && pPowerup->m_eType == POWER_UP_BANANA)
        {
            fStrength *= sBananaAvoidanceStrengthScale;
        }
        break;
    }
    }
    return fStrength;
}

float AvoidableFielder::UnidentifiedVirtual24(
    AvoidableObject* other)
{
    switch (other->mType)
    {
    case AVOID_FIELDERS:
    {
        bool bIgnore = false;
        cFielder* pFielder = m_pFielder;
        cFielder* pOther
            = ((AvoidableFielder*)other)->m_pFielder;
        if (!pFielder->IsStuck() && (pFielder->muInvincibleStatus & 1))
        {
            bIgnore = true;
        }
        if (bIgnore || pOther->fn_800344B0() || pOther->IsShattered()
            || (!m_pFielder->IsOnSameTeam(pOther)
                && (m_pFielder->fn_8003E7F8() || m_pFielder->fn_8003E84C())))
        {
            return 0.0f;
        }
        break;
    }
    case AVOID_POWERUPS:
    {
        PowerupBase* pPowerup
            = ((AvoidablePowerup*)other)->m_pPowerup;
        cFielder* pFielder = m_pFielder;
        bool bIgnore = false;
        if (!pFielder->IsStuck() && (pFielder->muInvincibleStatus & 8))
        {
            bIgnore = true;
        }
        if (bIgnore)
        {
            return 0.0f;
        }
        if (pPowerup != 0 && pPowerup->mtNoHitTimer.GetSeconds() > 0.0f
            && fn_800DEAB4(m_pFielder) && pPowerup->m_pThrower == m_pFielder)
        {
            return 0.0f;
        }
        break;
    }
    }

    float fWeight = other->mTweaks[0];
    switch (other->mType)
    {
    case AVOID_FIELDERS:
    {
        cFielder* pOther
            = ((AvoidableFielder*)other)->m_pFielder;
        if (fn_8003E8A0(pOther) || pOther->fn_8003E9F0())
        {
            fWeight *= 2.5f;
        }
        if (m_pFielder->IsOnSameTeam(pOther))
        {
            fWeight *= 0.8f;
        }
        if (pOther->fn_8003EA6C())
        {
            fWeight *= 3.0f;
        }
        break;
    }
    case AVOID_GOALIES:
        if (m_pFielder->IsOnSameTeam(
                ((AvoidableGoalie*)other)->m_pPlayer))
        {
            fWeight *= 0.8f;
        }
        else
        {
            fWeight *= 1.5f;
        }
        m_pFielder->fn_8003EA6C();
        break;
    case AVOID_UNIDENTIFIED_20:
        switch (((AvoidablePatch*)other)->m_pPatch->m_Type)
        {
        case 4:
            if (m_pFielder->mUnidentified024.m_eCharacterClass == YOSHI)
            {
                fWeight = 0.0f;
            }
            break;
        case 0:
            if (m_pFielder->mUnidentified024.m_eCharacterClass == PEACH)
            {
                fWeight = 0.0f;
            }
            break;
        case 2:
            if (m_pFielder->mUnidentified024.m_eCharacterClass == WARIO)
            {
                fWeight = 0.0f;
            }
            break;
        }
        break;
    }
    return fWeight;
}

const nlVector3& AvoidableGoalie::GetPosition()
{
    return m_pPlayer->mUnidentified024.m_v3Position;
}

const nlVector3& AvoidableGoalie::GetVelocity()
{
    return m_pPlayer->mUnidentified024.m_v3Velocity;
}

float AvoidableGoalie::GetRadius()
{
    float fRadius;
    m_pPlayer->m_pPhysicsCharacter->GetRadius(&fRadius);
    return fRadius;
}

const nlVector3& AvoidablePowerup::GetPosition()
{
    if (m_pPowerup != 0)
    {
        return m_pPowerup->m_v3Position;
    }
    if (m_pChainChomp != 0)
    {
        return m_pChainChomp->mv3Position;
    }
    return v3Zero;
}

const nlVector3& AvoidablePowerup::GetVelocity()
{
    if (m_pPowerup != 0)
    {
        return m_pPowerup->m_v3Velocity;
    }
    if (m_pChainChomp != 0)
    {
        return m_pChainChomp->mv3Velocity;
    }
    return v3Zero;
}

float AvoidablePowerup::GetRadius()
{
    float fRadius = 0.0f;
    if (m_pPowerup != 0)
    {
        return m_pPowerup->GetRadius();
    }
    if (m_pChainChomp != 0)
    {
        return m_pChainChomp->mpPhysObj->GetRadius();
    }
    return fRadius;
}

bool AvoidablePowerup::UnidentifiedVirtual2C()
{
    if (m_pPowerup != 0 && m_pPowerup->m_eType == POWER_UP_BANANA)
    {
        return false;
    }
    return true;
}

const nlVector3& AvoidablePoint::GetPosition()
{
    return mPosition;
}

const nlVector3& AvoidablePoint::GetVelocity()
{
    return v3Zero;
}

float AvoidablePoint::GetRadius()
{
    return mRadius;
}

const nlVector3& AvoidablePatch::GetPosition()
{
    return m_pPatch->GetPosition();
}

const nlVector3& AvoidablePatch::GetVelocity()
{
    if (m_pPatch->m_Type == 10)
    {
        float fSpeed = m_pPatch->m_PathSpeed;
        nlVec3Scale(mUnidentified018, m_pPatch->fn_80173CCC(), fSpeed);
        return mUnidentified018;
    }
    return m_pPatch->m_Velocity;
}

float AvoidablePatch::GetRadius()
{
    float fRadius = m_pPatch->GetRadius();
    if (m_pPatch->m_Type == 10)
    {
        fRadius *= 3.0f;
    }
    return fRadius;
}

bool AvoidablePatch::UnidentifiedVirtual2C()
{
    if (m_pPatch->m_Type == 8 || m_pPatch->m_Type == 10)
    {
        return true;
    }
    return false;
}

static inline void InitPolygon(AvoidablePolygon* pPolygon)
{
    for (int i = 0; i < 4; i++)
    {
        pPolygon->mPoints[i] = v2Zero;
        pPolygon->mNormals[i] = v2Zero;
    }
    pPolygon->mUnidentified064 = 0;
}

static inline void BuildPolygon(AvoidablePolygon* pPolygon,
    const nlVector2& a, const nlVector2& b, float fHalfWidth)
{
    // Retail zero-initializes one more stack vector here that is never read.
    nlVector2 v2Temp = v2Zero;
    nlVec2Sub(pPolygon->mNormals[1], b, a);
    float fScale = nlRecipSqrt(nlVec2LengthSquared(pPolygon->mNormals[1]), true);
    nlVec2Set(pPolygon->mNormals[1], fScale * pPolygon->mNormals[1].x,
        fScale * pPolygon->mNormals[1].y);
    nlVec2Set(pPolygon->mNormals[3], -pPolygon->mNormals[1].x,
        -pPolygon->mNormals[1].y);
    {
        float fCos;
        float fSin;
        nlSinCos(&fSin, &fCos, 0x4000);
        pPolygon->mNormals[0].x
            = pPolygon->mNormals[1].x * fCos - pPolygon->mNormals[1].y * fSin;
        pPolygon->mNormals[0].y
            = pPolygon->mNormals[1].y * fCos + pPolygon->mNormals[1].x * fSin;
    }
    {
        float fCos;
        float fSin;
        nlSinCos(&fSin, &fCos, 0x4000);
        pPolygon->mNormals[2].x
            = pPolygon->mNormals[3].x * fCos - pPolygon->mNormals[3].y * fSin;
        pPolygon->mNormals[2].y
            = pPolygon->mNormals[3].y * fCos + pPolygon->mNormals[3].x * fSin;
    }
    float fExtent = 0.5f * fHalfWidth;
    nlVec2Set(pPolygon->mPoints[0], fExtent * pPolygon->mNormals[0].x + a.x,
        fExtent * pPolygon->mNormals[0].y + a.y);
    nlVec2Set(pPolygon->mPoints[1], fExtent * pPolygon->mNormals[0].x + b.x,
        fExtent * pPolygon->mNormals[0].y + b.y);
    nlVec2Set(pPolygon->mPoints[2], fExtent * pPolygon->mNormals[2].x + b.x,
        fExtent * pPolygon->mNormals[2].y + b.y);
    nlVec2Set(pPolygon->mPoints[3], fExtent * pPolygon->mNormals[2].x + a.x,
        fExtent * pPolygon->mNormals[2].y + a.y);
}

AvoidablePolygon::AvoidablePolygon(
    int mode, const nlVector3& a, const nlVector3& b, float width)
    : AvoidableObject(AVOID_UNIDENTIFIED_08)
{
    InitPolygon(this);
    mUnidentified014 = mode;
    BuildPolygon(this, *(const nlVector2*)&a, *(const nlVector2*)&b, width);
}

AvoidablePolygon::AvoidablePolygon(
    int mode, const nlVector3& center, float length, float width)
    : AvoidableObject(AVOID_UNIDENTIFIED_08)
{
    nlVector2 a;
    nlVector2 b;
    InitPolygon(this);
    mUnidentified014 = mode;
    nlVec2Set(a, center.x, center.y - 0.5f * width);
    nlVec2Set(b, center.x, center.y + 0.5f * width);
    BuildPolygon(this, a, b, length);
}

AvoidablePolygon::~AvoidablePolygon()
{
}

const nlVector3& AvoidablePolygon::GetPosition()
{
    *(nlVector2*)&mCenter = v2Zero;
    for (int i = 0; i < 4; i++)
    {
        nlVec2Set(*(nlVector2*)&mCenter, mCenter.x + mPoints[i].x,
            mCenter.y + mPoints[i].y);
    }
    nlVec2Set(*(nlVector2*)&mCenter, 0.25f * mCenter.x, 0.25f * mCenter.y);
    mCenter.z = 0.0f;
    return mCenter;
}

bool AvoidablePolygon::UnidentifiedVirtual1C(
    const nlVector3& target, nlVector3& point, nlVector3& dir)
{
    int aFront[2] = { -1, -1 };
    int* pFront = aFront;
    float fThreshold = 0.0f;
    float fMinDist = 10000000000.0f;
    int nClosest = -1;
    nlVector2 aEdge[2];
    nlVector4 line;
    bool bInside;
    int i;
    int nFront = 0;

    for (i = 0; i < 4; i++)
    {
        fn_802B5CC0(line, mPoints[i], mNormals[i]);
        float fDist = fn_802B5DD0(*(const nlVector2*)&target, line);
        bool bFront = fDist - fThreshold > 0.0001f || nlNear(fDist, fThreshold);
        int nSide = 2;
        if (bFront)
        {
            nSide = 1;
        }
        if (nSide == 1)
        {
            *pFront++ = i;
            nFront++;
        }
        float fAbs = nlAbs(fDist);
        if (fAbs < fMinDist)
        {
            fMinDist = fAbs;
            nClosest = i;
        }
    }

    if (nFront == 0)
    {
        bInside = true;
        if (nClosest < 3)
        {
            aEdge[0] = mPoints[nClosest];
            aEdge[1] = mPoints[nClosest + 1];
        }
        else
        {
            aEdge[0] = mPoints[nClosest];
            aEdge[1] = mPoints[0];
        }
        *(nlVector2*)&point = GetClosestPointOnLineABFromPointC(
            aEdge[0], aEdge[1], *(const nlVector2*)&target);
        *(nlVector2*)&dir = mNormals[nClosest];
    }
    else
    {
        dir = v3Zero;
        i = 0;
        if (nFront > 0)
        {
            for (; i < nFront; i++)
            {
                nlVec2Set(*(nlVector2*)&dir,
                    dir.x + (1.0f / (float)nFront) * mNormals[aFront[i]].x,
                    dir.y + (1.0f / (float)nFront) * mNormals[aFront[i]].y);
            }
        }
        if (nFront == 2)
        {
            dir.z = 0.0f;
            float fScale = nlRecipSqrt(dir.GetLengthSq3D(), true);
            nlVec3Scale(dir, fScale);
        }
        if (aFront[0] < 3)
        {
            aEdge[0] = mPoints[aFront[0]];
            aEdge[1] = mPoints[aFront[0] + 1];
        }
        else
        {
            aEdge[0] = mPoints[aFront[0]];
            aEdge[1] = mPoints[0];
        }
        *(nlVector2*)&point = GetClosestPointOnLineABFromPointC(
            aEdge[0], aEdge[1], *(const nlVector2*)&target);
        bInside = false;
    }
    dir.z = 0.0f;
    point.z = 0.0f;
    return bInside;
}

bool AvoidablePolygon::UnidentifiedVirtual20(
    AvoidableObject* other, float range)
{
    nlVector3 v3Point;
    nlVector3 v3Dir;
    nlVector4 line;
    bool bInside = UnidentifiedVirtual1C(other->GetPosition(), v3Point, v3Dir);
    if (range <= 0.0f)
    {
        range = other->mTweaks[2];
    }
    if (!bInside)
    {
        fn_802B5D10(line, v3Point, v3Dir);
        float fDist
            = fn_802B5DD0(*(const nlVector2*)&other->GetPosition(), line);
        float fRadius = other->GetRadius();
        return fDist - fRadius <= range;
    }
    return bInside;
}


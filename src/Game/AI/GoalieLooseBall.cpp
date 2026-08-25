#include "Game/AI/GoalieLooseBall.h"

#include "Game/CharacterTriggers.h"
#include "NL/nlMath.h"

LooseBallInfo LooseBallAnims::mLooseBallKickInfo[3];
LooseBallInfo LooseBallAnims::mTrapBallInfo;
LooseBallInfo LooseBallAnims::mAttackSTSInfo;
LooseBallInfo LooseBallAnims::mUnknownD0BC;
LooseBallInfo LooseBallAnims::mDiveBallInfo;
LooseBallInfo* LooseBallAnims::mLooseBallDesperationInfo[4];

LooseBallInfo* LooseBallAnims::mpLooseBallInfo = 0;
unsigned int LooseBallAnims::muNumLooseBallAnims = 0;

static LooseBallInfo gCatches[9];
static const int gLooseBallAnims[9] = {
    0x31,
    0x7F,
    0x88,
    0x30,
    0x87,
    0x82,
    0x84,
    0x81,
    0x83,
};
static const int gLooseBallKickAnims[3] = { 0x2, 0x3, 0x4 };

static bool LooseBallCallback(float fTime, float fDuration,
    unsigned long uEventID, float, void* pUserData)
{
    if ((uEventID + 0x307c0000U) == 0xe7cd)
    {
        LooseBallInfo* info = (LooseBallInfo*)pUserData;
        info->mfPickupTime = fTime;
        info->mfAnimDuration = fDuration;
    }

    return true;
}

void LooseBallInfo::InitInstance(cPlayer* pPlayer, int nAnimID,
    eLooseBallAnimType eAnimType)
{
    mfAnimDuration = 0.0f;
    mfPickupTime = 0.0f;
    mnAnimID = nAnimID;
    mAnimType = eAnimType;
    GetAnimTriggerInfo(pPlayer, nAnimID, LooseBallCallback, this);
    pPlayer->GetJointPositionFuture(&mv3PickupPos, nAnimID, pPlayer->m_nBallJointIndex, mfPickupTime, true, true, false, true);
    mfPickupDistance = nlSqrt(
        mv3PickupPos.e[0] * mv3PickupPos.e[0]
            + mv3PickupPos.e[1] * mv3PickupPos.e[1],
        true);
    maPickupAngle = (unsigned short)(s16)(10430.378f
                                          * nlATan2f(mv3PickupPos.y, mv3PickupPos.x));
}

void LooseBallAnims::Init(cPlayer* pPlayer)
{
    if (mpLooseBallInfo != 0)
        return;

    muNumLooseBallAnims = 9;
    mpLooseBallInfo = gCatches;
    int i;

    for (i = 0; (unsigned int)i < muNumLooseBallAnims; i++)
    {
        mpLooseBallInfo[i].InitInstance(
            pPlayer, gLooseBallAnims[i], LOOSEBALL_ANIM_PICKUP);
    }

    for (i = 0; i < 3; i++)
    {
        mLooseBallKickInfo[i].InitInstance(
            pPlayer, gLooseBallKickAnims[i], LOOSEBALL_ANIM_KICK);
    }

    mTrapBallInfo.InitInstance(pPlayer, 0x85, LOOSEBALL_ANIM_TRAP);
    mAttackSTSInfo.InitInstance(pPlayer, 0x7E, LOOSEBALL_ANIM_ATTACK);
    mUnknownD0BC.InitInstance(pPlayer, 0x6F, LOOSEBALL_ANIM_ATTACK);
    mDiveBallInfo.InitInstance(pPlayer, 0x80, LOOSEBALL_ANIM_PICKUP);

    mLooseBallDesperationInfo[0] = &mTrapBallInfo;
    mLooseBallDesperationInfo[1] = &mDiveBallInfo;
    mLooseBallDesperationInfo[2] = GetLooseBallAnim(7);
    mLooseBallDesperationInfo[3] = GetLooseBallAnim(8);
}

void LooseBallAnims::Destroy()
{
    mpLooseBallInfo = 0;
    muNumLooseBallAnims = 0;
}

const LooseBallInfo* LooseBallAnims::FindLooseBallAnim(
    const nlVector3& v3LocalBallPosition, bool bFrontOnly, float fParam2)
{
    float fClosestDistSq = 1000000.0f;
    float fDistFromOrigSq = 0.2f
                          + (v3LocalBallPosition.x * v3LocalBallPosition.x
                              + v3LocalBallPosition.y * v3LocalBallPosition.y
                              + v3LocalBallPosition.z * v3LocalBallPosition.z);
    const LooseBallInfo* pInfo = 0;

    for (unsigned int i = 0; i < muNumLooseBallAnims; i++)
    {
        if ((!bFrontOnly || GetLooseBallAnim(i)->mv3PickupPos.x >= 0.0f)
            && (fParam2 - GetLooseBallAnim(i)->mv3PickupPos.z) < 0.0f)
        {
            nlVector3 v3Delta;
            nlVec3Sub(v3Delta, v3LocalBallPosition, GetLooseBallAnim(i)->mv3PickupPos);
            float fDist2D = nlVec3Length(v3Delta);

            if (fDist2D < fClosestDistSq)
            {
                float fPickupDistSq = GetLooseBallAnim(i)->mfPickupDistance
                                    * GetLooseBallAnim(i)->mfPickupDistance;
                if (fDistFromOrigSq > fPickupDistSq || pInfo == 0)
                {
                    fClosestDistSq = fDist2D;
                    pInfo = &mpLooseBallInfo[i];
                }
            }
        }
    }

    return pInfo;
}

LooseBallInfo* LooseBallAnims::GetLooseBallAnim(unsigned int uIndex)
{
    return &mpLooseBallInfo[uIndex];
}

const LooseBallInfo* LooseBallAnims::GetDesperationInfo(unsigned int type)
{
    return mLooseBallDesperationInfo[type];
}

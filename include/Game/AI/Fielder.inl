#ifndef GAME_AI_FIELDER_INL
#define GAME_AI_FIELDER_INL

#include "Game/AI/Fielder.h"
#include "Game/AnimInventory.h"

/**
 * Offset/Address/Size: 0x9538 | 0x800D1C34 | size: 0x8
 */
extern "C" inline bool fn_800D1C34(const cFielder* fielder)
{
    return fielder->mUnidentified3DC;
}

static inline float GetNormalizedContactTime(
    const cSAnim* anim, float contactFrame)
{
    float numKeys = anim->m_nNumKeys;
    return contactFrame / numKeys;
}

inline void cFielder::GetReceivePassBallContactOffset(nlVector3& v3Offset,
    unsigned short aFacingDirection, const LooseBallContactAnimInfo* pBestBallContactAnimInfo)
{
    nlVector3 v3ContactOffsetLocal;
    float fAnimContactFrame = pBestBallContactAnimInfo->fAnimContactFrame;
    const cSAnim* guessContactAnim = m_pAnimInventory->GetAnim(pBestBallContactAnimInfo->nAnimID);
    GetJointPositionFuture(&v3ContactOffsetLocal, pBestBallContactAnimInfo->nAnimID,
        m_nBallJointIndex, GetNormalizedContactTime(guessContactAnim, fAnimContactFrame),
        true, true, false, true);
    nlVec2Rotate(*(nlVector2*)&v3Offset,
        *(const nlVector2*)&v3ContactOffsetLocal, aFacingDirection);
    v3Offset.z = v3ContactOffsetLocal.z;
}

#endif // GAME_AI_FIELDER_INL

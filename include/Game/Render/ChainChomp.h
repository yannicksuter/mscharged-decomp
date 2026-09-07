#ifndef _CHAINCHOMP_H_
#define _CHAINCHOMP_H_

#include "Game/Render/SkinAnimatedMovableNPC.h"
#include "Game/EventDataTypes.h"
#include "NL/nlSlotPool.h"
#include "NL/nlTimer.h"

class cFielder;
class cSAnim;
class cTeam;
class AvoidablePowerup;
template <class T>
class cInventory;
class SFXEmitter;

enum eChainChompState
{
    CHAIN_STATE_HIDDEN = 0,
    CHAIN_STATE_FALL = 1,
    CHAIN_STATE_RECOVER = 2,
    CHAIN_STATE_CHASE = 3,
    CHAIN_STATE_CHOMP = 4,
    CHAIN_STATE_FROZEN = 5,
    CHAIN_STATE_LEAVE = 6,
};

class ChainChomp : public SkinAnimatedMovableNPC
{
public:
    ChainChomp(cSHierarchy& pHierarchy, int nModelID, PhysicsNPC& rPhysObj,
        cInventory<cSAnim>* pInventorySAnim, void* resource);
    virtual ~ChainChomp();
    virtual SkinAnimatedNPC_Type GetSkinAnimatedNPC_Type() const
    {
        return SkinAnimatedNPC_CHAIN_CHOMP;
    }
    virtual void Update(float fDeltaT);
    virtual void Move(float fDeltaT);
    virtual void DrawShadow(
        const cPoseAccumulator& pa, const nlMatrix4& worldMatrix);

    bool IsHidden() const;
    bool IsFrozen() const;
    void Freeze(float duration, bool immediate);
    void Leave();
    void Hide();
    cFielder* FindTarget(cTeam* pTeam);
    void Fall();
    void Spawn(cFielder* pThrower);
    static void CollisionCallback(PhysicsObject* pPhysObj,
        PhysicsObject* pObjA, const nlVector3& v3Pos);

    /* 0x84 */ cSAnim* mpIdleAnim;
    /* 0x88 */ cSAnim* mpRecoverAnim;
    /* 0x8C */ cSAnim* mpDropAnim;
    /* 0x90 */ cFielder* mpTarget;
    /* 0x94 */ eChainChompState meChainChompState;
    /* 0x98 */ Timer mtStateTimer;
    /* 0xA0 */ float mfChaseSpeed;
    /* 0xA4 */ SFXEmitter* mpInEffectSFX;
    /* 0xA8 */ cFielder* mpThrower;
    /* 0xAC */ int mnThrowerPadID;
    /* 0xB0 */ AvoidablePowerup* mpAvoidable;
}; // total size: 0xB4

inline void FreeCollisionChainPlayerData(CollisionChainPlayerData* pData)
{
    gCollisionChainPlayerDataPool.Free(pData);
}

#endif // _CHAINCHOMP_H_

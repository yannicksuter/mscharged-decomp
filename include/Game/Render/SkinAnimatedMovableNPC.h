#ifndef _SKINANIMATEDMOVABLENPC_H_
#define _SKINANIMATEDMOVABLENPC_H_

#include "Game/Physics/PhysicsNPC.h"
#include "Game/Render/SkinAnimatedNPC.h"

class SkinAnimatedMovableNPC : public SkinAnimatedNPC
{
public:
    SkinAnimatedMovableNPC(cSHierarchy& pHierarchy, int nModelID,
        PhysicsNPC& pPhysicsObj, void* resource);
    virtual ~SkinAnimatedMovableNPC();
    virtual SkinAnimatedNPC_Type GetSkinAnimatedNPC_Type() const
    {
        return SkinAnimatedNPC_MOVABLE;
    }
    virtual void Render();
    virtual void RenderFromReplay(
        const cPoseAccumulator& poseAcc,
        const nlMatrix4* pMatrix);
    virtual void SetPosition(const nlVector3& pos);
    virtual void Update(float dt);
    virtual void Move(float fDeltaT) = 0;
    virtual void AnimTranslate(float fDeltaT, bool bUseZ);
    virtual void AnimMove(float speed, bool applyPhysics);
    virtual void AnimMoveSeek(
        float speed,
        float turnRate,
        float seekRate,
        bool applyPhysics);

    /* 0x6C */ nlVector3 mv3Velocity;
    /* 0x78 */ u16 maDesiredFacingDirection;
    /* 0x7A */ u16 m_unk7A;
    /* 0x7C */ float mfDesiredSpeed;
    /* 0x80 */ PhysicsNPC* mpPhysObj;
}; // total size: 0x84

#endif // _SKINANIMATEDMOVABLENPC_H_

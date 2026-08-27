#ifndef _SKINANIMATEDNPC_H_
#define _SKINANIMATEDNPC_H_

#include "NL/nlMath.h"

class cPN_SAnimController;
class cPoseAccumulator;
class cPoseNode;
class GLSkinMesh;
struct glModel;

enum SkinAnimatedNPC_Type
{
    SkinAnimatedNPC_BASE = 0,
    SkinAnimatedNPC_MOVABLE = 1,
    SkinAnimatedNPC_CAMERA_GUY = 2,
    SkinAnimatedNPC_CHAIN_CHOMP = 3,
    SkinAnimatedNPC_BOWSER = 4,
    NUM_SkinAnimatedNPC_TYPES = 5,
};

class SkinAnimatedNPC
{
public:
    virtual ~SkinAnimatedNPC();
    virtual SkinAnimatedNPC_Type GetSkinAnimatedNPC_Type() const;
    virtual void Render();
    virtual void RenderFromReplay(
        const cPoseAccumulator& poseAccumulator,
        const nlMatrix4* pWorldMatrix);
    virtual void SetPosition(const nlVector3& pos);
    virtual void Update(float dt);
    virtual void DrawShadow(const glModel* pModel, const nlMatrix4& matrix);
    virtual void DrawShadow(
        const cPoseAccumulator& poseAccumulator,
        const nlMatrix4& worldMatrix);

    /* 0x04 */ nlMatrix4 mWorldMatrix;
    /* 0x44 */ u16 maFacingDirection;
    /* 0x46 */ u16 m_unk46;
    /* 0x48 */ nlVector3 mv3Position;
    /* 0x54 */ cPN_SAnimController* mpAnimController;
    /* 0x58 */ cPoseAccumulator* mpPoseAccumulator;
    /* 0x5C */ GLSkinMesh* mpSkinMesh;
    /* 0x60 */ cPoseNode* mpPoseTree;
    /* 0x64 */ u8 mbIsVisible;
    /* 0x65 */ u8 m_unk65[3];
    /* 0x68 */ glModel* mpLastModel;
}; // total size: 0x6C

#endif // _SKINANIMATEDNPC_H_

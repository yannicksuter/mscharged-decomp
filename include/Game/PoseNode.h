#ifndef GAME_POSE_NODE_H
#define GAME_POSE_NODE_H

#include "Game/PoseAccumulator.h"
#include "NL/nlMath.h"

class cPoseNode
{
public:
    cPoseNode();
    cPoseNode(int nNumChildren);
    virtual ~cPoseNode();
    virtual void Evaluate(int nodeIndex, float weight, cPoseAccumulator* accumulator) const = 0;
    virtual void Evaluate(float weight, cPoseAccumulator* accumulator) const = 0;
    virtual void EvaluateScale(
        int nodeIndex, float weight, cPoseAccumulator* accumulator) const;
    virtual cPoseNode* Update(float dt) = 0;
    virtual int GetType() = 0;
    virtual void BlendRootTrans(nlVector3* outBase, float weight, float* scratch) = 0;
    virtual void BlendRootRot(u16* outRot, float weight, float* scratch) = 0;

    void SetChild(int idx, cPoseNode* child);
    int GetNumChildren() const;
    cPoseNode** GetChildPtr(int idx);
    cPoseNode* GetChild(int idx) const;
    cPoseNode* GetChild(int idx);
    void GetRootRot(unsigned short* out);
    void GetRootTrans(nlVector3* out, unsigned short ang, float scale = 1.0f);

    cPoseNode* m_children[3];
    int m_numChildren;
};

#endif // GAME_POSE_NODE_H

#ifndef GAME_POSE_NODE_H
#define GAME_POSE_NODE_H

#include "Game/PoseAccumulator.h"
#include "NL/nlMath.h"

class cPoseNode
{
public:
    cPoseNode(int numChildren);
    virtual ~cPoseNode();
    virtual void Evaluate(int nodeIndex, float weight, cPoseAccumulator* accumulator) const = 0;
    virtual void Evaluate(float weight, cPoseAccumulator* accumulator) const = 0;
    virtual void EvaluateScale(
        int nodeIndex, float weight, cPoseAccumulator* accumulator) const;
    virtual cPoseNode* Update(float dt) = 0;
    virtual int GetType() = 0;
    virtual void BlendRootTrans(nlVector3* outBase, float weight, float* scratch) = 0;
    virtual void BlendRootRot(u16* outRot, float weight, float* scratch) = 0;

    void SetChild(int index, cPoseNode* child);
    cPoseNode* GetChild(int index) const;
    cPoseNode* GetChild(int index);
    void GetRootTrans(nlVector3* out, unsigned short ang);

    cPoseNode* m_Children[3];
    int m_NumChildren;
};

#endif // GAME_POSE_NODE_H

#include "Game/PoseNode.h"

/**
 * Offset/Address/Size: 0x0 | 0x803077B4 | size: 0x18
 */
cPoseNode::cPoseNode()
{
    m_numChildren = 0;
}

/**
 * Offset/Address/Size: 0x18 | 0x803077CC | size: 0x14
 */
cPoseNode::cPoseNode(int nNumChildren)
{
    m_numChildren = 0;
    m_numChildren = nNumChildren;
}

/**
 * Offset/Address/Size: 0x2C | 0x803077E0 | size: 0xB0
 */
cPoseNode::~cPoseNode()
{
    for (int i = 0; i < m_numChildren; ++i)
    {
        cPoseNode* ch = m_children[i];
        if (ch)
        {
            delete ch;
        }
    }
}

/**
 * Offset/Address/Size: 0xDC | 0x80307890 | size: 0x10
 */
cPoseNode* cPoseNode::GetChild(int idx)
{
    return m_children[idx];
}

/**
 * Offset/Address/Size: 0xEC | 0x803078A0 | size: 0x10
 */
cPoseNode* cPoseNode::GetChild(int idx) const
{
    return m_children[idx];
}

/**
 * Offset/Address/Size: 0xFC | 0x803078B0 | size: 0x10
 */
cPoseNode** cPoseNode::GetChildPtr(int idx)
{
    return &m_children[idx];
}

/**
 * Offset/Address/Size: 0x10C | 0x803078C0 | size: 0x8
 */
int cPoseNode::GetNumChildren() const
{
    return m_numChildren;
}

/**
 * Offset/Address/Size: 0x114 | 0x803078C8 | size: 0x10
 */
void cPoseNode::SetChild(int idx, cPoseNode* child)
{
    m_children[idx] = child;
}

/**
 * Offset/Address/Size: 0x124 | 0x803078D8 | size: 0xEC
 */
void cPoseNode::GetRootTrans(
    nlVector3* out, unsigned short ang, float scale)
{
    nlVector3 base;
    float pw = 0.0f;
    base.x = 0.0f;
    base.y = 0.0f;
    base.z = 0.0f;

    BlendRootTrans(&base, 1.0f, &pw);
    nlVec3Scale(base, scale);

    float c, s;
    nlSinCos(&s, &c, ang);

    out->x = base.x * c - (base.y * s);
    out->y = base.y * c + (base.x * s);
    out->z = base.z;
}

/**
 * Offset/Address/Size: 0x210 | 0x803079C4 | size: 0x44
 */
void cPoseNode::GetRootRot(unsigned short* out)
{
    float fAccumulatedWeight = 0.0f;
    *out = 0;
    BlendRootRot(out, 1.0f, &fAccumulatedWeight);
}

/**
 * Offset/Address/Size: 0x254 | 0x80307A08 | size: 0x4
 */
void cPoseNode::EvaluateScale(
    int, float, cPoseAccumulator*) const
{
}

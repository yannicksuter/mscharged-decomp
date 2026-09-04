#include "Game/PoseAccumulator.h"

#include "Game/PoseNode.h"
#include "Game/SHierarchy.h"
#include "NL/nlMemory.h"

#include <math.h>
#include <string.h>

static nlQuaternion qRotIdentity = { 0.0f, 0.0f, 0.0f, 1.0f };
static nlVector3 v3ScaleIdentity = { 1.0f, 1.0f, 1.0f };
static nlVector3 v3TransIdentity = { 0.0f, 0.0f, 0.0f };

extern "C" void fn_802B549C(nlQuaternion& out, unsigned short angle);

/**
 * Offset/Address/Size: 0x0 | 0x8030A9D0 | size: 0x228
 */
cPoseAccumulator::cPoseAccumulator(
    cSHierarchy* pSHierarchy, bool bStorePrevNodeMatrices)
{
    m_BaseSHierarchy = pSHierarchy;
    m_Scale = 1.0f;
    m_bUseObject = false;
    m_Unknown70 = pSHierarchy->m_nNumNodes;

    if (bStorePrevNodeMatrices)
    {
        m_PrevNodeMatrices = new (8, false) nlMatrix4[m_Unknown70 + 1];
    }
    else
    {
        m_PrevNodeMatrices = NULL;
    }

    m_NodeMatrices = new (8, false) nlMatrix4[m_Unknown70 + 1];
    m_pQuaternions = new (8, false) nlQuaternion[m_Unknown70 + 1];
    m_rot = new (8, false) RotAccum[m_Unknown70];
    m_scale = new (8, false) ScaleAccum[m_Unknown70];
    m_trans = new (8, false) TransAccum[m_Unknown70];
    m_cb = new (8, false) cBuildNodeMatrixCallbackInfo[m_Unknown70];

    for (int i = 0; i < m_BaseSHierarchy->m_nNumNodes; ++i)
    {
        if (m_PrevNodeMatrices != NULL)
        {
            m_PrevNodeMatrices[i].SetIdentity();
        }
        m_NodeMatrices[i].SetIdentity();
        m_cb[i].funcCallback = NULL;
        nlQuatIdentity(m_pQuaternions[i]);

        if (m_BaseSHierarchy->PreserveBoneLength(i))
        {
            const nlVector3& t = m_BaseSHierarchy->GetTranslationOffset(i);
            m_trans[i].t = t;
            m_trans[i].fAccumulatedWeight = 1.0f;
            m_trans[i].bIdentity = false;
        }
    }
}

/**
 * Offset/Address/Size: 0x228 | 0x8030ABF8 | size: 0x11C
 */
cPoseAccumulator::cPoseAccumulator(const cPoseAccumulator& other)
{
    new (this) cPoseAccumulator(
        other.m_BaseSHierarchy, other.m_PrevNodeMatrices != NULL);

    m_BaseSHierarchy = other.m_BaseSHierarchy;
    if (m_PrevNodeMatrices != NULL && other.m_PrevNodeMatrices != NULL)
    {
        memcpy(m_PrevNodeMatrices, other.m_PrevNodeMatrices,
            other.m_Unknown70 * sizeof(nlMatrix4));
    }
    memcpy(m_NodeMatrices, other.m_NodeMatrices,
        other.m_Unknown70 * sizeof(nlMatrix4));
    memcpy(m_pQuaternions, other.m_pQuaternions,
        other.m_Unknown70 * sizeof(nlQuaternion));
    memcpy(m_rot, other.m_rot, other.m_Unknown70 * sizeof(RotAccum));
    memcpy(m_scale, other.m_scale, other.m_Unknown70 * sizeof(ScaleAccum));
    memcpy(m_trans, other.m_trans, other.m_Unknown70 * sizeof(TransAccum));
    memcpy(m_cb, other.m_cb,
        other.m_Unknown70 * sizeof(cBuildNodeMatrixCallbackInfo));
    memcpy(&m_MorphWeights, &other.m_MorphWeights,
        sizeof(MorphWeightAccum));
    m_Scale = other.m_Scale;
    m_bUseObject = other.m_bUseObject;
}

/**
 * Offset/Address/Size: 0x344 | 0x8030AD14 | size: 0x100
 */
cPoseAccumulator& cPoseAccumulator::operator=(const cPoseAccumulator& other)
{
    m_BaseSHierarchy = other.m_BaseSHierarchy;
    if (m_PrevNodeMatrices != NULL && other.m_PrevNodeMatrices != NULL)
    {
        memcpy(m_PrevNodeMatrices, other.m_PrevNodeMatrices,
            other.m_Unknown70 * sizeof(nlMatrix4));
    }
    memcpy(m_NodeMatrices, other.m_NodeMatrices,
        other.m_Unknown70 * sizeof(nlMatrix4));
    memcpy(m_pQuaternions, other.m_pQuaternions,
        other.m_Unknown70 * sizeof(nlQuaternion));
    memcpy(m_rot, other.m_rot, other.m_Unknown70 * sizeof(RotAccum));
    memcpy(m_scale, other.m_scale, other.m_Unknown70 * sizeof(ScaleAccum));
    memcpy(m_trans, other.m_trans, other.m_Unknown70 * sizeof(TransAccum));
    memcpy(m_cb, other.m_cb,
        other.m_Unknown70 * sizeof(cBuildNodeMatrixCallbackInfo));
    memcpy(&m_MorphWeights, &other.m_MorphWeights,
        sizeof(MorphWeightAccum));
    m_Scale = other.m_Scale;
    m_bUseObject = other.m_bUseObject;
    return *this;
}

/**
 * Offset/Address/Size: 0x444 | 0x8030AE14 | size: 0x9C
 */
cPoseAccumulator::~cPoseAccumulator()
{
    if (m_PrevNodeMatrices != NULL)
    {
        delete[] m_PrevNodeMatrices;
    }
    delete[] m_NodeMatrices;
    delete[] m_pQuaternions;
    delete[] m_rot;
    delete[] m_scale;
    delete[] m_trans;
    delete[] m_cb;
}

static inline void PoseAccumulatorInitNodeAccumulators(cPoseAccumulator* pose)
{
    for (int i = 0; i < pose->m_BaseSHierarchy->m_nNumNodes; ++i)
    {
        RotAccum& r = pose->m_rot[i];
        r.q.x = 0.0f;
        r.q.y = 0.0f;
        r.q.z = 0.0f;
        r.q.w = 1.0f;
        r.quatAccumulatedWeight = 0.0f;
        r.rotAroundZ = 0;
        r.rotAroundZAccumulatedWeight = 0.0f;
        r.bIdentity = true;

        ScaleAccum& s = pose->m_scale[i];
        s.s.x = 1.0f;
        s.s.y = 1.0f;
        s.s.z = 1.0f;
        s.fAccumulatedWeight = 0.0f;
        s.bIdentity = true;

        if (!pose->m_BaseSHierarchy->PreserveBoneLength(i))
        {
            TransAccum& t = pose->m_trans[i];
            t.t.x = 0.0f;
            t.t.y = 0.0f;
            t.t.z = 0.0f;
            t.fAccumulatedWeight = 0.0f;
            t.bIdentity = true;
        }
        pose->m_bUseObject = false;
    }
}

static inline void PoseAccumulatorClearMorphWeights(cPoseAccumulator* pose)
{
    for (int i = 0; i < 20; ++i)
    {
        pose->m_MorphWeights.mData[i] = 0.0f;
    }
}

/**
 * Offset/Address/Size: 0x4E0 | 0x8030AEB0 | size: 0x188
 */
void cPoseAccumulator::Pose(
    const cPoseNode& pPoseTree, const nlMatrix4& pWorldMatrix)
{
    PoseAccumulatorInitNodeAccumulators(this);
    PoseAccumulatorClearMorphWeights(this);
    pPoseTree.Evaluate(1.0f, this);
    BuildNodeMatrices(pWorldMatrix);
}

extern "C" void fn_8030B038(cPoseAccumulator* pAccumulator,
    const cPoseNode* pPoseTree, const nlMatrix4* pWorldMatrix)
{
    PoseAccumulatorInitNodeAccumulators(pAccumulator);
    PoseAccumulatorClearMorphWeights(pAccumulator);
    pPoseTree->Evaluate(1.0f, pAccumulator);
    pAccumulator->BuildNodeMatrices(*pWorldMatrix);
}

/**
 * Offset/Address/Size: 0x7F0 | 0x8030B1C0 | size: 0x158
 */
void cPoseAccumulator::InitAccumulators()
{
    for (int i = 0; i < m_BaseSHierarchy->m_nNumNodes; ++i)
    {
        RotAccum& r = m_rot[i];
        r.q.x = 0.0f;
        r.q.y = 0.0f;
        r.q.z = 0.0f;
        r.q.w = 1.0f;
        r.quatAccumulatedWeight = 0.0f;
        r.rotAroundZ = 0;
        r.rotAroundZAccumulatedWeight = 0.0f;
        r.bIdentity = true;

        ScaleAccum& s = m_scale[i];
        s.s.x = 1.0f;
        s.s.y = 1.0f;
        s.s.z = 1.0f;
        s.fAccumulatedWeight = 0.0f;
        s.bIdentity = true;

        if (!m_BaseSHierarchy->PreserveBoneLength(i))
        {
            TransAccum& t = m_trans[i];
            t.t.x = 0.0f;
            t.t.y = 0.0f;
            t.t.z = 0.0f;
            t.fAccumulatedWeight = 0.0f;
            t.bIdentity = true;
        }
        m_bUseObject = false;
    }

    for (int k = 0; k < 20; ++k)
    {
        m_MorphWeights.mData[k] = 0.0f;
    }
}

/**
 * Offset/Address/Size: 0x948 | 0x8030B318 | size: 0x6B0
 */
void cPoseAccumulator::BuildNodeMatrices(const nlMatrix4& pWorldMatrix)
{
    if (m_PrevNodeMatrices != NULL)
    {
        nlMatrix4* pTemp = m_PrevNodeMatrices;
        m_PrevNodeMatrices = m_NodeMatrices;
        m_NodeMatrices = pTemp;
    }

    int ParentStack[32];
    bool ScaleIdentityStack[32];
    nlVector3 ScaleStack[32];
    int nStackIndex = -1;

    if (m_scale[0].bIdentity && fabsf(m_Scale - 1.0f) < 0.0001f)
    {
        ScaleIdentityStack[0] = true;
        ScaleStack[0] = v3ScaleIdentity;
    }
    else
    {
        ScaleIdentityStack[0] = false;
        ScaleStack[0].x = m_scale[0].s.x * m_Scale;
        ScaleStack[0].y = m_scale[0].s.y * m_Scale;
        ScaleStack[0].z = m_scale[0].s.z * m_Scale;
    }

    for (int i = 0; i < m_BaseSHierarchy->m_nNumNodes; ++i)
    {
        nlQuaternion* pLocalQuaternion = &m_pQuaternions[i + 1];
        RotAccum& r = m_rot[i];
        if (!r.bIdentity)
        {
            if (r.quatAccumulatedWeight == 0.0f)
            {
                fn_802B549C(*pLocalQuaternion, r.rotAroundZ);
            }
            else
            {
                float fTotalWeight = r.rotAroundZAccumulatedWeight
                    + r.quatAccumulatedWeight;
                if (fabsf(fTotalWeight) > 0.0001f)
                {
                    nlQuaternion quatAroundZ;
                    fn_802B549C(quatAroundZ, r.rotAroundZ);
                    nlQuatNLerp(r.q, r.q, quatAroundZ,
                        r.rotAroundZAccumulatedWeight / fTotalWeight);
                }
                *pLocalQuaternion = r.q;
            }
        }
        else
        {
            nlQuatIdentity(*pLocalQuaternion);
        }

        nlVector3 v3Position;
        if (!m_trans[i].bIdentity)
        {
            v3Position = m_trans[i].t;
        }
        else
        {
            v3Position = v3TransIdentity;
        }

        int nParentIndex = -1;
        int nScaleIndex = nStackIndex + 1;
        if (i > 0)
        {
            nParentIndex = ParentStack[nStackIndex];
            nlMultQuat(m_pQuaternions[i], m_pQuaternions[nParentIndex],
                *pLocalQuaternion);
            nlMultPosVectorMatrix(
                v3Position, v3Position, m_NodeMatrices[nParentIndex]);

            ScaleAccum& s = m_scale[i];
            if (s.bIdentity)
            {
                ScaleIdentityStack[nScaleIndex]
                    = ScaleIdentityStack[nStackIndex];
                if (!ScaleIdentityStack[nStackIndex])
                {
                    ScaleStack[nScaleIndex] = ScaleStack[nStackIndex];
                }
            }
            else
            {
                ScaleIdentityStack[nScaleIndex] = false;
                if (ScaleIdentityStack[nStackIndex])
                {
                    ScaleStack[nScaleIndex] = s.s;
                }
                else
                {
                    ScaleStack[nScaleIndex].x
                        = ScaleStack[nStackIndex].x * s.s.x;
                    ScaleStack[nScaleIndex].y
                        = ScaleStack[nStackIndex].y * s.s.y;
                    ScaleStack[nScaleIndex].z
                        = ScaleStack[nStackIndex].z * s.s.z;
                }
            }
        }
        else
        {
            nlVector3 v3WorldScale;
            v3WorldScale.x = nlSqrt(pWorldMatrix.m11 * pWorldMatrix.m11
                    + pWorldMatrix.m12 * pWorldMatrix.m12
                    + pWorldMatrix.m13 * pWorldMatrix.m13,
                true);
            v3WorldScale.y = nlSqrt(pWorldMatrix.m21 * pWorldMatrix.m21
                    + pWorldMatrix.m22 * pWorldMatrix.m22
                    + pWorldMatrix.m23 * pWorldMatrix.m23,
                true);
            v3WorldScale.z = nlSqrt(pWorldMatrix.m31 * pWorldMatrix.m31
                    + pWorldMatrix.m32 * pWorldMatrix.m32
                    + pWorldMatrix.m33 * pWorldMatrix.m33,
                true);

            nlQuaternion qWorld;
            if (v3WorldScale.x < 0.9999f || v3WorldScale.x > 1.0001f
                || v3WorldScale.y < 0.9999f || v3WorldScale.y > 1.0001f
                || v3WorldScale.z < 0.9999f || v3WorldScale.z > 1.0001f)
            {
                nlMatrix4 mWorldNoScale = pWorldMatrix;
                mWorldNoScale.m11 /= v3WorldScale.x;
                mWorldNoScale.m12 /= v3WorldScale.x;
                mWorldNoScale.m13 /= v3WorldScale.x;
                mWorldNoScale.m21 /= v3WorldScale.y;
                mWorldNoScale.m22 /= v3WorldScale.y;
                mWorldNoScale.m23 /= v3WorldScale.y;
                mWorldNoScale.m31 /= v3WorldScale.z;
                mWorldNoScale.m32 /= v3WorldScale.z;
                mWorldNoScale.m33 /= v3WorldScale.z;
                ScaleIdentityStack[0] = false;
                ScaleStack[0].x *= v3WorldScale.x;
                ScaleStack[0].y *= v3WorldScale.y;
                ScaleStack[0].z *= v3WorldScale.z;
                nlMatrixToQuat(qWorld, mWorldNoScale);
            }
            else
            {
                nlMatrixToQuat(qWorld, pWorldMatrix);
            }
            nlMultQuat(m_pQuaternions[i], qWorld, *pLocalQuaternion);
            nlMultPosVectorMatrix(v3Position, v3Position, pWorldMatrix);
        }

        nlMatrix4& mNode = m_NodeMatrices[i];
        nlQuatToMatrix(mNode, m_pQuaternions[i], false);
        mNode.m41 = v3Position.x;
        mNode.m42 = v3Position.y;
        mNode.m43 = v3Position.z;
        mNode.m44 = 1.0f;

        int nPushPop = m_BaseSHierarchy->GetPushPop(i);
        nStackIndex += nPushPop;
        if (nPushPop > 0)
        {
            ParentStack[nStackIndex] = i;
        }

        if (!ScaleIdentityStack[nScaleIndex])
        {
            mNode.m11 *= ScaleStack[nScaleIndex].x;
            mNode.m12 *= ScaleStack[nScaleIndex].x;
            mNode.m13 *= ScaleStack[nScaleIndex].x;
            mNode.m21 *= ScaleStack[nScaleIndex].y;
            mNode.m22 *= ScaleStack[nScaleIndex].y;
            mNode.m23 *= ScaleStack[nScaleIndex].y;
            mNode.m31 *= ScaleStack[nScaleIndex].z;
            mNode.m32 *= ScaleStack[nScaleIndex].z;
            mNode.m33 *= ScaleStack[nScaleIndex].z;
        }

        cBuildNodeMatrixCallbackInfo* pCallback = &m_cb[i];
        if (pCallback->funcCallback != NULL)
        {
            pCallback->funcCallback(pCallback->nParam1, pCallback->nParam2,
                this, i, nParentIndex);
        }
    }
}

extern "C" void fn_8030B9C8(
    cPoseAccumulator* pAccumulator, const nlMatrix4* pWorldMatrix)
{
    bool ScaleIdentityStack[32];
    nlVector3 ScaleStack[32];
    int nStackIndex = 0;

    if (pAccumulator->m_scale[0].bIdentity
        && fabsf(pAccumulator->m_Scale - 1.0f) < 0.0001f)
    {
        ScaleIdentityStack[0] = true;
    }
    else
    {
        ScaleIdentityStack[0] = false;
        ScaleStack[0].x
            = pAccumulator->m_scale[0].s.x * pAccumulator->m_Scale;
        ScaleStack[0].y
            = pAccumulator->m_scale[0].s.y * pAccumulator->m_Scale;
        ScaleStack[0].z
            = pAccumulator->m_scale[0].s.z * pAccumulator->m_Scale;
    }

    for (int i = 0; i < pAccumulator->m_BaseSHierarchy->m_nNumNodes; ++i)
    {
        nlVector3 v3Position;
        if (!pAccumulator->m_trans[i].bIdentity)
        {
            v3Position = pAccumulator->m_trans[i].t;
        }
        else
        {
            v3Position = v3TransIdentity;
        }

        if (i > 0)
        {
            ScaleAccum& s = pAccumulator->m_scale[i];
            if (s.bIdentity)
            {
                ScaleIdentityStack[nStackIndex]
                    = ScaleIdentityStack[nStackIndex - 1];
                if (!ScaleIdentityStack[nStackIndex - 1])
                {
                    ScaleStack[nStackIndex] = ScaleStack[nStackIndex - 1];
                }
            }
            else
            {
                ScaleIdentityStack[nStackIndex] = false;
                if (ScaleIdentityStack[nStackIndex - 1])
                {
                    ScaleStack[nStackIndex] = s.s;
                }
                else
                {
                    ScaleStack[nStackIndex].x
                        = ScaleStack[nStackIndex - 1].x * s.s.x;
                    ScaleStack[nStackIndex].y
                        = ScaleStack[nStackIndex - 1].y * s.s.y;
                    ScaleStack[nStackIndex].z
                        = ScaleStack[nStackIndex - 1].z * s.s.z;
                }
            }

            int nParentIndex = pAccumulator->m_BaseSHierarchy->GetParent(i);
            nlMultPosVectorMatrix(v3Position, v3Position,
                pAccumulator->m_NodeMatrices[nParentIndex]);
        }
        else
        {
            nlMultPosVectorMatrix(v3Position, v3Position, *pWorldMatrix);
        }

        nlMatrix4& mNode = pAccumulator->m_NodeMatrices[i];
        nlQuatToMatrix(mNode, pAccumulator->m_pQuaternions[i], false);
        mNode.m41 = v3Position.x;
        mNode.m42 = v3Position.y;
        mNode.m43 = v3Position.z;
        mNode.m44 = 1.0f;

        if (!ScaleIdentityStack[nStackIndex])
        {
            mNode.m11 *= ScaleStack[nStackIndex].x;
            mNode.m12 *= ScaleStack[nStackIndex].x;
            mNode.m13 *= ScaleStack[nStackIndex].x;
            mNode.m21 *= ScaleStack[nStackIndex].y;
            mNode.m22 *= ScaleStack[nStackIndex].y;
            mNode.m23 *= ScaleStack[nStackIndex].y;
            mNode.m31 *= ScaleStack[nStackIndex].z;
            mNode.m32 *= ScaleStack[nStackIndex].z;
            mNode.m33 *= ScaleStack[nStackIndex].z;
        }

        nStackIndex += pAccumulator->m_BaseSHierarchy->GetPushPop(i);
    }
}

/**
 * Offset/Address/Size: 0x1348 | 0x8030BD18 | size: 0x150
 */
void cPoseAccumulator::BlendRot(int nNode, const nlQuaternion* pRot,
    float fWeight, bool bMirror)
{
    RotAccum* e = m_rot + nNode;
    bool bNegligibleWeight = fabsf(fWeight) < 0.001f;
    if (bNegligibleWeight)
        return;

    nlQuaternion qtemp;
    if (bMirror)
    {
        cSHierarchy* h = m_BaseSHierarchy;
        if (nNode == h->m_nSpineNodeIndex
            || nNode == h->m_nPelvisNodeIndex)
        {
            qtemp.x = -pRot->w;
            qtemp.y = pRot->z;
            qtemp.z = pRot->y;
            qtemp.w = -pRot->x;
        }
        else if (nNode < h->m_nPelvisNodeIndex)
        {
            qtemp.x = -pRot->x;
            qtemp.y = pRot->y;
            qtemp.z = -pRot->z;
            qtemp.w = pRot->w;
        }
        else
        {
            qtemp.x = -pRot->x;
            qtemp.y = -pRot->y;
            qtemp.z = pRot->z;
            qtemp.w = pRot->w;
        }
        pRot = &qtemp;
    }

    e->quatAccumulatedWeight += fWeight;
    float t = fWeight / e->quatAccumulatedWeight;
    nlQuaternion tmp = e->q;
    nlQuatNLerp(e->q, tmp, *pRot, t);
    e = m_rot + nNode;
    e->bIdentity = false;
}

/**
 * Offset/Address/Size: 0x1498 | 0x8030BE68 | size: 0x98
 */
void cPoseAccumulator::BlendRotAroundZ(
    int nNode, unsigned short rot, float fWeight)
{
    bool bNegligibleWeight = fabsf(fWeight) < 0.001f;
    if (bNegligibleWeight)
        return;

    RotAccum* e = m_rot + nNode;
    e->rotAroundZAccumulatedWeight += fWeight;
    float t = fWeight / e->rotAroundZAccumulatedWeight;
    int delta = (short)(rot - e->rotAroundZ);
    delta = (short)(t * delta);
    e->rotAroundZ = e->rotAroundZ + delta;
    e = m_rot + nNode;
    e->bIdentity = false;
}

/**
 * Offset/Address/Size: 0x1530 | 0x8030BF00 | size: 0x88
 */
void cPoseAccumulator::BlendScale(int nNode, const nlVector3* pScale,
    float fWeight, bool bMirror)
{
    bool bNegligibleWeight = fabsf(fWeight) < 0.001f;
    if (bNegligibleWeight)
        return;

    ScaleAccum* e = m_scale + nNode;
    e->fAccumulatedWeight += fWeight;
    float t = fWeight / e->fAccumulatedWeight;
    float inv = 1.0f - t;
    e->s.x = inv * e->s.x + t * pScale->x;
    e->s.y = inv * e->s.y + t * pScale->y;
    e->s.z = inv * e->s.z + t * pScale->z;
    e->bIdentity = false;
}

/**
 * Offset/Address/Size: 0x15B8 | 0x8030BF88 | size: 0xFC
 */
void cPoseAccumulator::BlendTrans(int nNode, const nlVector3* pTrans,
    float fWeight, bool bMirror)
{
    bool bNegligibleWeight = fabsf(fWeight) < 0.001f;
    if (bNegligibleWeight)
        return;

    if (bMirror)
    {
        cSHierarchy* h = m_BaseSHierarchy;
        nlVector3 vtemp;
        if (nNode <= h->m_nPelvisNodeIndex
            || nNode == h->m_nSpineNodeIndex)
        {
            vtemp.x = pTrans->x;
            vtemp.y = -pTrans->y;
            vtemp.z = pTrans->z;
        }
        else
        {
            vtemp.x = pTrans->x;
            vtemp.y = pTrans->y;
            vtemp.z = -pTrans->z;
        }
        pTrans = &vtemp;
    }

    TransAccum* e = m_trans + nNode;
    e->fAccumulatedWeight += fWeight;
    float t = fWeight / e->fAccumulatedWeight;
    float inv = 1.0f - t;
    e->t.x = inv * e->t.x + t * pTrans->x;
    e->t.y = inv * e->t.y + t * pTrans->y;
    e->t.z = inv * e->t.z + t * pTrans->z;
    e = m_trans + nNode;
    e->bIdentity = false;
}

/**
 * Offset/Address/Size: 0x16B4 | 0x8030C084 | size: 0x94
 */
void cPoseAccumulator::BlendRotIdentity(int nNode, float fWeight)
{
    bool bNegligibleWeight = fabsf(fWeight) < 0.001f;
    if (bNegligibleWeight)
        return;

    RotAccum* a = &m_rot[nNode];
    a->quatAccumulatedWeight += fWeight;
    if (a->bIdentity)
        return;

    const float t = fWeight / a->quatAccumulatedWeight;
    nlQuaternion tmp = a->q;
    nlQuatNLerp(a->q, tmp, qRotIdentity, t);
}

/**
 * Offset/Address/Size: 0x1748 | 0x8030C118 | size: 0x98
 */
void cPoseAccumulator::BlendScaleIdentity(int nNode, float fWeight)
{
    bool bNegligibleWeight = fabsf(fWeight) < 0.001f;
    if (bNegligibleWeight)
        return;

    ScaleAccum* e = &m_scale[nNode];
    e->fAccumulatedWeight += fWeight;
    if (e->bIdentity)
        return;

    float f3 = fWeight / e->fAccumulatedWeight;
    float f2 = 1.0f - f3;
    e->s.x = f2 * e->s.x + f3 * v3ScaleIdentity.x;
    e->s.y = f2 * e->s.y + f3 * v3ScaleIdentity.y;
    e->s.z = f2 * e->s.z + f3 * v3ScaleIdentity.z;
}

/**
 * Offset/Address/Size: 0x17E0 | 0x8030C1B0 | size: 0xA8
 */
void cPoseAccumulator::MultiplyScale(
    int nNode, const nlVector3* pScale, float fWeight)
{
    bool bNegligibleWeight = fabsf(fWeight) < 0.001f;
    if (bNegligibleWeight)
        return;

    nlVector3 v3Scale;
    float fInvWeight = 1.0f - fWeight;
    v3Scale.x = fInvWeight * v3ScaleIdentity.x + fWeight * pScale->x;
    v3Scale.y = fInvWeight * v3ScaleIdentity.y + fWeight * pScale->y;
    v3Scale.z = fInvWeight * v3ScaleIdentity.z + fWeight * pScale->z;

    ScaleAccum* e = &m_scale[nNode];
    e->s.x *= v3Scale.x;
    e->s.y *= v3Scale.y;
    e->s.z *= v3Scale.z;
    e->bIdentity = false;
}

/**
 * Offset/Address/Size: 0x1888 | 0x8030C258 | size: 0x98
 */
void cPoseAccumulator::BlendTransIdentity(int nNode, float fWeight)
{
    bool bNegligibleWeight = fabsf(fWeight) < 0.001f;
    if (bNegligibleWeight)
        return;

    TransAccum* e = &m_trans[nNode];
    e->fAccumulatedWeight += fWeight;
    if (e->bIdentity)
        return;

    const float f3 = fWeight / e->fAccumulatedWeight;
    const float f2 = 1.0f - f3;
    e->t.x = f2 * e->t.x + f3 * v3TransIdentity.x;
    e->t.y = f2 * e->t.y + f3 * v3TransIdentity.y;
    e->t.z = f2 * e->t.z + f3 * v3TransIdentity.z;
}

/**
 * Offset/Address/Size: 0x1920 | 0x8030C2F0 | size: 0x10
 */
nlMatrix4& cPoseAccumulator::GetNodeMatrix(int nNode) const
{
    return m_NodeMatrices[nNode];
}

/**
 * Offset/Address/Size: 0x1930 | 0x8030C300 | size: 0x74
 */
nlMatrix4& cPoseAccumulator::GetNodeMatrixByHashID(
    unsigned int nHashID) const
{
    int index = 0;
    while (index < m_BaseSHierarchy->m_nNumNodes)
    {
        unsigned int nodeID = m_BaseSHierarchy->GetNodeID(index);
        if (nHashID == nodeID)
        {
            break;
        }
        ++index;
    }
    return m_NodeMatrices[index];
}

/**
 * Offset/Address/Size: 0x19A4 | 0x8030C374 | size: 0xC
 */
s32 cPoseAccumulator::GetNumNodes() const
{
    return m_BaseSHierarchy->m_nNumNodes;
}

/**
 * Offset/Address/Size: 0x19B0 | 0x8030C380 | size: 0x28
 */
void cPoseAccumulator::SetBuildNodeMatrixCallback(int nNode,
    BuildNodeMatrixFn funcCallback, unsigned int nParam1,
    unsigned int nParam2)
{
    int offset = nNode * (int)sizeof(cBuildNodeMatrixCallbackInfo);
    *(BuildNodeMatrixFn*)((char*)m_cb + offset) = funcCallback;
    *(unsigned int*)((char*)m_cb + offset + 4) = nParam1;
    *(unsigned int*)((char*)m_cb + offset + 8) = nParam2;
}

#ifndef GAME_SANIM_H
#define GAME_SANIM_H

#include "NL/nlMath.h"
#include "NL/nlChunk.h"
#include "types.h"

class cPoseAccumulator;
class cSAnim;

enum ePlayMode
{
    PM_CYCLIC = 0,
    PM_HOLD = 1,
    PM_PING_PONG = 2,
};

class cSAnimCallback
{
public:
    float m_fTime;
    unsigned int m_nParam1;
    void (*m_funcCallback)(cSAnim*, unsigned int);
    cSAnimCallback* next;
};

struct PackedScale
{
    unsigned short x;
    unsigned short y;
    unsigned short z;
};

struct PackedTrans
{
    float x;
    float y;
    float z;
};

class cIdentifier
{
public:
    unsigned int GetHashID() const
    {
        return m_uHashID;
    }

    void Destroy()
    {
    }

    const char* m_szName;

protected:
    unsigned int m_uHashID;
};

class cSAnim : public cIdentifier
{
public:
    typedef char* MemType;

    static cSAnim* Initialize(nlChunk* pChunk);
    static u8 IsValidChunkID(u32 id)
    {
        return (id & 0x80FFFFFF) == 0x80017000;
    }

    void Destroy();
    void fn_80308610(nlChunk* nodeChunk, int nodeIndex);

    void BlendRot(int accumulatorNode, int animNode, float time, float weight,
        cPoseAccumulator* accumulator, bool mirror) const;
    void BlendScale(int accumulatorNode, int animNode, float time, float weight,
        cPoseAccumulator* accumulator, bool mirror) const;
    void BlendScaleMultiply(int accumulatorNode, int animNode, float time,
        float weight, cPoseAccumulator* accumulator) const;
    void BlendTrans(int accumulatorNode, int animNode, float time, float weight,
        cPoseAccumulator* accumulator, bool mirror) const;
    void GetRootRot(float time, u16* rootRotation) const;
    void GetRootTrans(float time, nlVector3* rootTranslation) const;
    bool fn_8030939C(int channel, float time, float* weight) const;
    void CreateCallback(float fTime, unsigned int nParam1,
        void (*funcCallback)(cSAnim*, unsigned int));
    float GetMorphWeight(int channel, float time) const;

    cSAnimCallback* GetCallbackList() const
    {
        return m_pCallbackList;
    }

    float GetDuration() const
    {
        return (float)m_nNumKeys / 30.0f;
    }

    unsigned int m_nNumKeys;
    unsigned int m_nNumNodes;
    unsigned int m_nNumMorphChannels;
    const unsigned int* m_pNodeProperties;
    const unsigned int* m_Unknown18;
    const unsigned int* m_Unknown1C;
    void** m_pRotKeys;
    PackedScale** m_pScaleKeys;
    PackedTrans** m_pTransKeys;
    unsigned char** m_Unknown2C;
    void** m_Unknown30;
    unsigned int m_nNumRootKeys;
    unsigned short* m_pRootRot;
    nlVector3* m_pRootTrans;
    unsigned long* m_nMorphIds;
    const unsigned int* m_pNumMorphKeys;
    unsigned char* m_pMorphKeys;
    cSAnimCallback* m_pCallbackList;
    float m_fLinearSpeed;
    unsigned long m_nHierarchySignature;
};

#endif // GAME_SANIM_H

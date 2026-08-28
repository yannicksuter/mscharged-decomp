#ifndef GAME_SANIM_H
#define GAME_SANIM_H

#include "NL/nlMath.h"
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
    unsigned int m_nParam;
    void (*m_pCallback)(cSAnim*, unsigned int);
    cSAnimCallback* m_pNext;
};

class nlChunk
{
public:
    nlChunk* GetNextChunk();
    nlChunk* GetLastChunk();
    nlChunk* GetFirstChunk();
    void* GetData();
    void* GetUnalignedData();
    void* GetAlignedData();
    u32 GetChunkAlignment();
    bool IsAlignedChunk();
    u32 GetSize();
    u32 GetID();

    u32 m_ID;
    u32 m_Size;
};

inline nlChunk* nlChunk::GetNextChunk()
{
    u8* address = (u8*)GetUnalignedData() + GetSize();
    u32 offset = (u32)address & 3;
    return (nlChunk*)(address + (offset != 0) * (4 - offset));
}

inline nlChunk* nlChunk::GetLastChunk()
{
    return (nlChunk*)((u8*)this + GetSize() + sizeof(nlChunk));
}

inline nlChunk* nlChunk::GetFirstChunk()
{
    return (nlChunk*)GetData();
}

inline void* nlChunk::GetData()
{
    bool isAligned = IsAlignedChunk();
    if (isAligned)
    {
        return GetAlignedData();
    }
    return GetUnalignedData();
}

inline void* nlChunk::GetUnalignedData()
{
    return this + 1;
}

inline void* nlChunk::GetAlignedData()
{
    bool hasAlignment = GetChunkAlignment();
    if (!hasAlignment)
    {
        return GetUnalignedData();
    }

    u32 alignment = GetChunkAlignment();
    u32 address = (u32)GetUnalignedData();
    u32 remainder = address % alignment;
    return (void*)(address
        + (remainder != 0) * (alignment - remainder));
}

inline u32 nlChunk::GetChunkAlignment()
{
    u32 alignmentBits = m_ID & 0x0F000000;
    return alignmentBits != 0 ? 1u << (alignmentBits >> 24) : 0;
}

inline bool nlChunk::IsAlignedChunk()
{
    return m_ID & 0x0F000000;
}

inline u32 nlChunk::GetSize()
{
    return m_Size;
}

inline u32 nlChunk::GetID()
{
    return m_ID & 0x80FFFFFF;
}

class cSAnim
{
public:
    typedef char* MemType;

    static cSAnim* Initialize(nlChunk* pChunk);
    static u8 IsValidChunkID(u32 id)
    {
        return (id & 0x80FFFFFF) == 0x80017000;
    }

    void Destroy();

    unsigned int GetHashID() const
    {
        return m_uHashID;
    }

    void BlendRot(int accumulatorNode, int animNode, float time, float weight,
        cPoseAccumulator* accumulator, bool mirror) const;
    void BlendScale(int accumulatorNode, int animNode, float time, float weight,
        cPoseAccumulator* accumulator, bool mirror) const;
    void fn_80308E30(int accumulatorNode, int animNode, float time,
        float weight, cPoseAccumulator* accumulator) const;
    void BlendTrans(int accumulatorNode, int animNode, float time, float weight,
        cPoseAccumulator* accumulator, bool mirror) const;
    void GetRootRot(float time, u16* rootRotation) const;
    void GetRootTrans(float time, nlVector3* rootTranslation) const;
    float GetMorphWeight(int channel, float time) const;

    cSAnimCallback* GetCallbackList() const
    {
        return m_pCallbackList;
    }

    float GetDuration() const
    {
        return (float)m_nNumKeys / 30.0f;
    }

    const char* m_szName;
    unsigned int m_uHashID;
    unsigned int m_nNumKeys;
    unsigned int m_nNumNodes;
    unsigned int m_nNumMorphChannels;
    u8 m_Unknown14[0x38];
    cSAnimCallback* m_pCallbackList;
};

#endif // GAME_SANIM_H

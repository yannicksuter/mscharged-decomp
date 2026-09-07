#ifndef NL_CHUNK_H
#define NL_CHUNK_H

#include "types.h"

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
    return (u8*)this + sizeof(nlChunk);
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

#endif // NL_CHUNK_H

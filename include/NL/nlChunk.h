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
    unsigned int GetChunkAlignment();
    bool IsAlignedChunk();
    unsigned int GetSize();
    unsigned int GetID();

    unsigned int m_ID;
    unsigned int m_Size;
};

inline nlChunk* nlChunk::GetNextChunk()
{
    u8* address = (u8*)GetUnalignedData() + GetSize();
    unsigned int offset = (unsigned int)address & 3;
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

    unsigned int alignment = GetChunkAlignment();
    unsigned int address = (unsigned int)GetUnalignedData();
    unsigned int remainder = address % alignment;
    return (void*)(address
        + (remainder != 0) * (alignment - remainder));
}

inline unsigned int nlChunk::GetChunkAlignment()
{
    unsigned int alignmentBits = m_ID & 0x0F000000;
    return alignmentBits != 0 ? 1u << (alignmentBits >> 24) : 0;
}

inline bool nlChunk::IsAlignedChunk()
{
    return m_ID & 0x0F000000;
}

inline unsigned int nlChunk::GetSize()
{
    return m_Size;
}

inline unsigned int nlChunk::GetID()
{
    return m_ID & 0x80FFFFFF;
}

#endif // NL_CHUNK_H

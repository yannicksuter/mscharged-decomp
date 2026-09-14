#ifndef NL_CHUNK_H
#define NL_CHUNK_H

#include "NL/nlMath.h"
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
    unsigned long GetDataSize();
    unsigned long GetID();

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
    unsigned int offset;
    u8* address = (u8*)GetUnalignedData() + GetSize();
    offset = (unsigned int)address & 3;
    return (nlChunk*)(address + (offset != 0) * (4 - offset));
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

    return (void*)nlAlignUp((unsigned int)GetUnalignedData(), GetChunkAlignment());
}

inline unsigned int nlChunk::GetChunkAlignment()
{
    unsigned int alignmentBits = m_ID & 0x0F000000;
    return alignmentBits != 0 ? 1u << (alignmentBits >> 24) : 0;
}

inline bool nlChunk::IsAlignedChunk()
{
    unsigned int alignmentBits = m_ID;
    alignmentBits &= 0x0F000000;
    return alignmentBits != 0;
}

inline unsigned int nlChunk::GetSize()
{
    return m_Size;
}

inline unsigned long nlChunk::GetDataSize()
{
    unsigned long size = (u8*)GetAlignedData() - (u8*)GetUnalignedData();
    size = m_Size - size;
    return size;
}

inline unsigned long nlChunk::GetID()
{
    return m_ID & 0x80FFFFFF;
}

#endif // NL_CHUNK_H

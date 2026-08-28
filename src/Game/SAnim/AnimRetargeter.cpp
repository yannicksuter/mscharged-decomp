#include "Game/SAnim/AnimRetargeter.h"

AnimRetargetList* AnimRetargetList::Initialize(nlChunk* chunkData)
{
    nlChunk* chunk = chunkData->GetFirstChunk();
    AnimRetargetList* data = (AnimRetargetList*)chunk->GetData();

    nlChunk* nextChunk = chunk->GetNextChunk();
    nextChunk = nextChunk->GetFirstChunk();
    data->m_pAnimRetarget = (AnimRetarget*)nextChunk->GetData();

    for (int i = 0; i < data->m_NumAnimRetargets; i++)
    {
        nextChunk = nextChunk->GetNextChunk();
        data->m_pAnimRetarget[i].m_pMap
            = (signed short*)nextChunk->GetData();
    }

    return data;
}

AnimRetarget* AnimRetargetList::GetAnimRetargetWithSignature(
    const cSAnim* anim)
{
    return GetAnimRetargetWithSignature(anim->m_nHierarchySignature);
}

AnimRetarget* AnimRetargetList::GetAnimRetargetWithSignature(
    unsigned long signature)
{
    AnimRetarget* p;
    long offset;
    AnimRetarget* result = 0;
    offset = (long)result;

    for (long i = m_NumAnimRetargets; i > 0; i--)
    {
        p = (AnimRetarget*)((char*)m_pAnimRetarget + offset);
        if (signature == p->m_TargetHierarchySignature)
        {
            result = p;
            break;
        }
        offset += sizeof(AnimRetarget);
    }

    return result;
}

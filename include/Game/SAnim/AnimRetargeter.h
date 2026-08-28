#ifndef GAME_SANIM_ANIM_RETARGETER_H
#define GAME_SANIM_ANIM_RETARGETER_H

#include "Game/SAnim.h"

class AnimRetarget
{
public:
    unsigned long m_TargetHierarchySignature;
    long m_NumBones;
    u32 m_Unknown08;
    signed short* m_pMap;
};

class AnimRetargetList : public cIdentifier
{
public:
    typedef char* MemType;

    static AnimRetargetList* Initialize(nlChunk* chunkData);
    static bool IsValidChunkID(u32 id)
    {
        return (id & 0x80FFFFFF) == 0x80017104;
    }

    AnimRetarget* GetAnimRetargetWithSignature(unsigned long signature);
    AnimRetarget* GetAnimRetargetWithSignature(const cSAnim* anim);

    long m_NumAnimRetargets;
    AnimRetarget* m_pAnimRetarget;
};

#endif // GAME_SANIM_ANIM_RETARGETER_H

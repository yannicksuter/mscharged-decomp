#ifndef GAME_SANIM_ANIM_RETARGETER_H
#define GAME_SANIM_ANIM_RETARGETER_H

#include "types.h"

class AnimRetarget
{
public:
    u32 m_TargetHierarchySignature;
    int m_NumBones;
    u32 m_Unknown08;
    s16* m_pMap;
};

#endif // GAME_SANIM_ANIM_RETARGETER_H

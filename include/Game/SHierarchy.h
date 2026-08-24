#ifndef GAME_S_HIERARCHY_H
#define GAME_S_HIERARCHY_H

#include "NL/nlMath.h"
#include "types.h"

class cSHierarchy
{
public:
    int GetChild(int nodeIndex, int childIndex) const;
    int GetNumChildren(int nodeIndex) const;
    int GetParent(int nodeIndex) const;
    int GetMirroredNode(int nodeIndex) const;
    nlVector3& GetTranslationOffset(int nodeIndex) const;

    const char* m_Name;
    u32 m_HashID;
    int m_nNumNodes;
    u32* m_pNodeID;
    int* m_pParent;
    int* m_pNumChildren;
    int** m_pChildren;
    int* m_pPushPop;
    int* m_pMirrorTable;
    int m_nPelvisNodeIndex;
    int m_nSpineNodeIndex;
    nlVector3* m_pTranslationOffset;
    u8* m_pPreserveBoneLength;
};

#endif // GAME_S_HIERARCHY_H

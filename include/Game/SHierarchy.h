#ifndef GAME_S_HIERARCHY_H
#define GAME_S_HIERARCHY_H

#include "Game/SAnim.h"
#include "NL/nlMath.h"
#include "types.h"

class cSHierarchy : public cIdentifier
{
public:
    typedef char* MemType;

    static cSHierarchy* Initialize(nlChunk* pChunk);
    static bool IsValidChunkID(u32 id)
    {
        return (id & 0x80FFFFFF) == 0x80018000;
    }

    void BuildPushPopFlags(int nNode, int nParentDepth, int& nCurrentDepth);
    int GetNodeIndexByID(unsigned int id) const;
    int GetChild(int i, int j) const;
    u32 GetNodeID(int i) const;
    int GetNumChildren(int i) const;
    int GetParent(int i) const;
    int GetMirroredNode(int i) const;
    int GetPushPop(int i) const;
    nlVector3& GetTranslationOffset(int i) const;
    bool PreserveBoneLength(int i) const;

    int GetNumNodes() const
    {
        return m_nNumNodes;
    }

    int GetPelvisNodeIndex() const
    {
        return m_nPelvisNodeIndex;
    }

    int GetSpineNodeIndex() const
    {
        return m_nSpineNodeIndex;
    }

    int m_nNumNodes;
    u32* m_pNodeID;
    int* m_pParent;
    int* m_pNumChildren;
    int** m_pChildren;
    int* m_pPushPop;
    int* m_pMirrorTable;
    int m_nPelvisNodeIndex;
    int m_nSpineNodeIndex;
    nlVector3* m_pV3TranslationOffset;
    u8* m_pPreserveBoneLength;
};

#endif // GAME_S_HIERARCHY_H

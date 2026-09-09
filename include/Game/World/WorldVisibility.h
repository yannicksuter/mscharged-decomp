#ifndef GAME_WORLD_WORLD_VISIBILITY_H
#define GAME_WORLD_WORLD_VISIBILITY_H

#include "NL/nlMath.h"

class nlChunk;

struct WorldVisibilityNode
{
    /* 0x00 */ nlVector3 mBoundsMin;
    /* 0x0C */ nlVector3 mBoundsMax;
    /* 0x18 */ int mVisible;
    /* 0x1C */ int mNumModelHashes;
    /* 0x20 */ unsigned long* mModelHashes;
    /* 0x24 */ WorldVisibilityNode* mChildren[2];
    /* 0x2C */ unsigned long mModelHashData[1];
};

typedef void (*WorldVisibilityCallback)(WorldVisibilityNode* node);

WorldVisibilityNode* LoadWorldVisibilityNode(nlChunk* chunk);
WorldVisibilityNode* LoadWorldVisibilityTree(nlChunk* chunk);
void UpdateWorldVisibilityNode(WorldVisibilityNode* node,
    const nlVector4* pPlanes, WorldVisibilityCallback callback,
    unsigned long planeMask, int state);
void UpdateWorldVisibility(WorldVisibilityNode* node,
    const nlVector4* pPlanes, WorldVisibilityCallback callback);

#endif // GAME_WORLD_WORLD_VISIBILITY_H

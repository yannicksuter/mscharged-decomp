#include "Game/World/WorldVisibility.h"

#include "Game/Render/Frustum.h"
#include "NL/nlChunk.h"
#include "Game/UnidentifiedStaticStorage.h"

static nlChunk* spWorldVisibilityChunk;

WorldVisibilityNode* LoadWorldVisibilityNode(nlChunk* chunk)
{
    int i;
    WorldVisibilityNode* node;

    spWorldVisibilityChunk = chunk;
    node = (WorldVisibilityNode*)chunk->GetData();

    if (node->mNumModelHashes != 0)
    {
        node->mModelHashes = node->mModelHashData;
    }

    for (i = 0; i < 2; ++i)
    {
        if (node->mChildren[i] != 0)
        {
            node->mChildren[i] = LoadWorldVisibilityNode(
                spWorldVisibilityChunk->GetNextChunk());
        }
    }
    return node;
}

WorldVisibilityNode* LoadWorldVisibilityTree(nlChunk* chunk)
{
    return LoadWorldVisibilityNode((nlChunk*)chunk->GetData());
}

void UpdateWorldVisibilityNode(WorldVisibilityNode* node,
    const nlVector4* pPlanes, WorldVisibilityCallback callback,
    unsigned long planeMask, int state)
{
    FrustumResult result;
    if (state == 0)
    {
        result = FRUSTUM_OUTSIDE;
    }
    else if (state == 1)
    {
        result = FRUSTUM_INSIDE;
    }
    else
    {
        result = ClassifyBoxInFrustum(pPlanes,
            &node->mBoundsMin,
            &node->mBoundsMax, &planeMask);
    }

    if (result)
    {
        node->mVisible = 1;
        if (callback != 0)
        {
            callback(node);
        }
    }
    else
    {
        node->mVisible = 0;
    }

    for (int i = 0; i < 2; ++i)
    {
        if (node->mChildren[i] != 0)
        {
            UpdateWorldVisibilityNode(node->mChildren[i], pPlanes,
                callback, planeMask, result);
        }
    }
}

void UpdateWorldVisibility(WorldVisibilityNode* node,
    const nlVector4* pPlanes, WorldVisibilityCallback callback)
{
    UpdateWorldVisibilityNode(node, pPlanes, callback, 0, 2);
}

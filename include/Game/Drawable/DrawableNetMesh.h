#ifndef _DRAWABLENETMESH_H_
#define _DRAWABLENETMESH_H_

#include "types.h"
#include "NL/nlMath.h"

class NetMesh;
struct LoadFrame;
struct SaveFrame;

class DrawableNetMesh
{
public:
    DrawableNetMesh(bool);
    ~DrawableNetMesh();

    static void Reset();
    void Initialize(int, int);
    void Destroy();
    void Grab(NetMesh&);
    void Blend(float, const DrawableNetMesh&, const DrawableNetMesh&);
    void Replay(LoadFrame&);
    void Replay(SaveFrame&);
    void Render() const;
    void RenderInvisiblePlanes() const;

    nlVector3* mPositions;
    int mNetIndex;
    int mDisplayList;
    int mNumVertices;
    int mNumTriIndices;
    float mJoltCache;
    NetMesh* mNetMesh;
    bool mInitialized;
    bool mVisible;
    char _01E[2];
};


class GLView;

extern GLView* g_pNetMeshView;
extern u32 g_NetMeshInvisiblePlaneView;

#endif // _DRAWABLENETMESH_H_

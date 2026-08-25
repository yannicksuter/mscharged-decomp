#ifndef GAME_RENDER_NET_MESH_H
#define GAME_RENDER_NET_MESH_H

#include "types.h"

class NetMesh
{
public:
    static bool s_bAnimatedNetMeshEnabled;

    NetMesh(bool positiveEnd);
    ~NetMesh();

private:
    u8 mUnidentified[0x84];
}; // size: 0x84

#endif // GAME_RENDER_NET_MESH_H

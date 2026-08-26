#include "Game/Physics/NetMeshEdge.h"

static inline bool HasStream(const glModelPacket* packet, int streamID)
{
    for (u8 i = 0; i < packet->numStreams; ++i)
    {
        if (packet->streams[i].id == streamID)
            return true;
    }
    return false;
}

static inline const glModelStream* GetStream(
    const glModelPacket* packet, int streamID)
{
    for (u8 i = 0; i < packet->numStreams; ++i)
    {
        if (packet->streams[i].id == streamID)
            return &packet->streams[i];
    }
    return packet->streams;
}

const nlVector3* NetMeshModelLoader::NetMeshVertex::GetPosition() const
{
    const glModelStream* layout = GetStream(mpPacket, 1);
    u8 stride = layout->stride;
    s8* base = (s8*)layout->address;
    return (const nlVector3*)(base + mIndex * stride);
}

void NetMeshModelLoader::NetMeshVertex::GetNormal(nlVector3& normal) const
{
    if (!HasStream(mpPacket, 2))
    {
        normal.x = 1.0f;
        normal.y = 0.0f;
        normal.z = 0.0f;
        return;
    }

    const glModelStream* layout = GetStream(mpPacket, 2);

    if (layout->stride == 3)
    {
        s8 normalX = ((const s8*)layout->address
                      + mIndex * layout->stride)[0];
        normal.x = (float)normalX;
        normal.x *= 0.015625f;
        s8 normalY = ((const s8*)layout->address
                      + mIndex * layout->stride)[1];
        normal.y = (float)normalY;
        normal.y *= 0.015625f;
        s8 normalZ = ((const s8*)layout->address
                      + mIndex * layout->stride)[2];
        normal.z = (float)normalZ;
        normal.z *= 0.015625f;
    }
    else if (layout->stride == 12)
    {
        float f = 1.0f;
        normal.x = *(const float*)((const u8*)layout->address
                                   + mIndex * layout->stride)
                 / f;
        normal.y = *(const float*)((const u8*)layout->address
                                   + mIndex * layout->stride + 4)
                 / f;
        normal.z = *(const float*)((const u8*)layout->address
                                   + mIndex * layout->stride + 8)
                 / f;
    }
}

void NetMeshModelLoader::NetMeshVertex::GetTextureCoord(
    nlVector2& txtCoord) const
{
    const glModelStream* layout = GetStream(mpPacket, 4);

    if (layout->stride == 4)
    {
        s16 texCoordX = *(const s16*)((const u8*)layout->address
                                      + mIndex * layout->stride);
        txtCoord.x = (float)texCoordX;
        txtCoord.x *= 0.0009765625f;
        s16 texCoordY = *(const s16*)((const u8*)layout->address
                                      + mIndex * layout->stride + 2);
        txtCoord.y = (float)texCoordY;
        txtCoord.y *= 0.0009765625f;
    }
    else if (layout->stride == 8)
    {
        float f = 1.0f;
        txtCoord.x = *(const float*)((const u8*)layout->address
                                     + mIndex * layout->stride)
                   / f;
        txtCoord.y = *(const float*)((const u8*)layout->address
                                     + mIndex * layout->stride + 4)
                   / f;
    }
}

#include "Game/Drawable/DrawableModel.h"

#include "Game/Debug/ShapeRender.h"
#include "NL/gl/glModel.h"
#include "NL/nlAVLTree.h"

#include <string.h>

glModelStream* fn_8027263C(const glModelPacket* packet, int id);

static nlAVLTreeSlotPool<unsigned long, AABBDimensions,
    DefaultKeyCompare<unsigned long> >
    boundingBoxCache(16, 16);

void GetAABBDimensions(const glModel* model,
    AABBDimensions& dimensions, unsigned long boundingBoxCacheKey)
{
    AABBDimensions* foundValue = NULL;
    int packetOffset;

    boundingBoxCache.Find(boundingBoxCacheKey, &foundValue, NULL);
    if (foundValue != NULL)
    {
        dimensions = *foundValue;
        return;
    }

    unsigned char first = 1;
    unsigned int packetIndex = 0;
    glModelPacket* packet;
    int vertexIndex;
    packetOffset = 0;
    nlVector3 min;
    nlVector3 max;

    while (packetIndex < model->numPackets)
    {
        packet = (glModelPacket*)((u8*)model->packets + packetOffset);
        glModelStream* stream = fn_8027263C(packet, 1);
        u16* pVert = packet->indexBuffer;
        vertexIndex = 0;

        while (vertexIndex < packet->numVertices)
        {
            u16 vert = *pVert;
            nlVector3 point;

            if (stream->stride == 12)
            {
                memcpy(&point,
                    (u8*)stream->address + vert * stream->stride,
                    12);
            }
            else if (stream->stride == 6)
            {
                float scale = 1.0f / 128.0f;

                point.x = (float)*(s8*)((u8*)stream->address
                                        + vert * stream->stride)
                        * scale;
                point.y = (float)*(s8*)((u8*)stream->address
                                        + vert * stream->stride + 1)
                        * scale;
                point.z = (float)*(s8*)((u8*)stream->address
                                        + vert * stream->stride + 2)
                        * scale;
            }

            if (point.x < min.x || first)
                min.x = point.x;
            if (point.y < min.y || first)
                min.y = point.y;
            if (point.z < min.z || first)
                min.z = point.z;
            if (point.x > max.x || first)
                max.x = point.x;
            if (point.y > max.y || first)
                max.y = point.y;
            if (point.z > max.z || first)
                max.z = point.z;

            first = 0;
            pVert++;
            vertexIndex++;
        }

        packetOffset += sizeof(glModelPacket);
        packetIndex++;
    }

    dimensions.mMin = min;
    dimensions.mMax = max;
    float dz = dimensions.mMax.z - dimensions.mMin.z;
    float dy = dimensions.mMax.y - dimensions.mMin.y;
    float dx = dimensions.mMax.x - dimensions.mMin.x;
    dimensions.mDim.x = dx;
    dimensions.mDim.y = dy;
    dimensions.mDim.z = dz;

    if (boundingBoxCacheKey != 0)
    {
        AVLTreeNode* existingNode;
        boundingBoxCache.AddAVLNode((AVLTreeNode**)&boundingBoxCache.m_Root,
            &boundingBoxCacheKey,
            &dimensions,
            &existingNode);
    }
}

void CleanBoundingBoxCache()
{
    boundingBoxCache.Clear();
    boundingBoxCache.m_Allocator.FreeBlocks();
}

void RenderBoundingBox(const glModel* model, const nlMatrix4& matrix)
{
    AABBDimensions dimensions;
    GetAABBDimensions(model, dimensions, 0);

    nlVector4 points[8];

    nlVec4Set(points[0], dimensions.mMin.x, dimensions.mMin.y, dimensions.mMin.z, 1.0f);
    nlVec4Set(points[1], dimensions.mMin.x, dimensions.mMin.y, dimensions.mMax.z, 1.0f);
    nlVec4Set(points[2], dimensions.mMin.x, dimensions.mMax.y, dimensions.mMax.z, 1.0f);
    nlVec4Set(points[3], dimensions.mMin.x, dimensions.mMax.y, dimensions.mMin.z, 1.0f);
    nlVec4Set(points[4], dimensions.mMax.x, dimensions.mMin.y, dimensions.mMin.z, 1.0f);
    nlVec4Set(points[5], dimensions.mMax.x, dimensions.mMin.y, dimensions.mMax.z, 1.0f);
    nlVec4Set(points[6], dimensions.mMax.x, dimensions.mMax.y, dimensions.mMax.z, 1.0f);
    nlVec4Set(points[7], dimensions.mMax.x, dimensions.mMax.y, dimensions.mMin.z, 1.0f);

    for (int i = 0; i < 8; i++)
    {
        nlVector4 temp;
        nlMultVectorMatrix(temp, points[i], matrix);
        points[i] = temp;
    }

    const nlColour blue = { 0, 0, 0xFF, 0 };

    g_ShapeRenderer.DrawLine3D(
        (nlVector3&)points[0], (nlVector3&)points[1], blue, true);
    g_ShapeRenderer.DrawLine3D(
        (nlVector3&)points[1], (nlVector3&)points[2], blue, true);
    g_ShapeRenderer.DrawLine3D(
        (nlVector3&)points[2], (nlVector3&)points[3], blue, true);
    g_ShapeRenderer.DrawLine3D(
        (nlVector3&)points[3], (nlVector3&)points[0], blue, true);
    g_ShapeRenderer.DrawLine3D(
        (nlVector3&)points[4], (nlVector3&)points[5], blue, true);
    g_ShapeRenderer.DrawLine3D(
        (nlVector3&)points[5], (nlVector3&)points[6], blue, true);
    g_ShapeRenderer.DrawLine3D(
        (nlVector3&)points[6], (nlVector3&)points[7], blue, true);
    g_ShapeRenderer.DrawLine3D(
        (nlVector3&)points[7], (nlVector3&)points[4], blue, true);
    g_ShapeRenderer.DrawLine3D(
        (nlVector3&)points[4], (nlVector3&)points[0], blue, true);
    g_ShapeRenderer.DrawLine3D(
        (nlVector3&)points[5], (nlVector3&)points[1], blue, true);
    g_ShapeRenderer.DrawLine3D(
        (nlVector3&)points[6], (nlVector3&)points[2], blue, true);
    g_ShapeRenderer.DrawLine3D(
        (nlVector3&)points[7], (nlVector3&)points[3], blue, true);
}

#ifndef GAME_DRAWABLE_DRAWABLE_MODEL_H
#define GAME_DRAWABLE_DRAWABLE_MODEL_H

#include "Game/Drawable/RenderObject.h"
#include "NL/nlMath.h"

struct glModel;

class AABBDimensions
{
public:
    float GetDimensionX() const { return mDim.x; }
    float GetDimensionY() const { return mDim.y; }
    float GetDimensionZ() const { return mDim.z; }

    /* 0x00 */ nlVector3 mMin;
    /* 0x0C */ nlVector3 mMax;
    /* 0x18 */ nlVector3 mDim;
}; // total size: 0x24

void CleanBoundingBoxCache();
void RenderBoundingBox(const glModel* model, const nlMatrix4& matrix);
void GetAABBDimensions(const glModel* model,
    AABBDimensions& dimensions, unsigned long boundingBoxCacheKey);

// A drawable the game poses itself: it keeps the orientation, translation
// and scale the gameplay code writes and rebuilds the inherited world
// matrix from them once the flag at 0xE0 is cleared.
class DrawableModel : public RenderObject
{
public:
    /* 0x78 */ u32 renderFlags;
    /* 0x7C */ nlQuaternion orientation;
    /* 0x8C */ nlVector3 translation;
    /* 0x98 */ float modelScale;
    /* 0x9C */ float snapshotScale;
    /* 0xA0 */ char _0A0[0x40];
    /* 0xE0 */ bool worldMatrixUpToDate;
}; // size: 0xE4

#endif // GAME_DRAWABLE_DRAWABLE_MODEL_H

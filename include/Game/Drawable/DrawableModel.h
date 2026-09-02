#ifndef GAME_DRAWABLE_DRAWABLE_MODEL_H
#define GAME_DRAWABLE_DRAWABLE_MODEL_H

#include "Game/Drawable/RenderObject.h"
#include "NL/nlMath.h"

struct glModel;

class AABBDimensions
{
public:
    /* 0x00 */ nlVector3 mMin;
    /* 0x0C */ nlVector3 mMax;
    /* 0x18 */ nlVector3 mDim;
}; // total size: 0x24

void CleanBoundingBoxCache();
void RenderBoundingBox(const glModel* model, const nlMatrix4& matrix);
void GetAABBDimensions(const glModel* model,
    AABBDimensions& dimensions, unsigned long boundingBoxCacheKey);

class DrawableModel : public RenderObject
{
};

#endif // GAME_DRAWABLE_DRAWABLE_MODEL_H

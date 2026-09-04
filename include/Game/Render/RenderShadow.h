#ifndef GAME_RENDER_RENDERSHADOW_H
#define GAME_RENDER_RENDERSHADOW_H

#include "types.h"
#include "NL/nlMath.h"

struct glModel;
class RLView;

struct ProjectedShadowParams
{
    /* 0x00 */ nlVector4 vLight;
    /* 0x10 */ nlVector3 vPosition;
    /* 0x1C */ float fRadius;
    /* 0x20 */ glModel* pModel;
    /* 0x24 */ float fHeight;
    /* 0x28 */ float fScalar;
    /* 0x2C */ int nPartitionIndex;
    /* 0x30 */ int nVisibleInterval;
    /* 0x34 */ int nInvisibleInterval;
}; // size: 0x38

float GetCoPlanarZ();
void SetCoPlanarZ(float z);
void SetPlanarShadowOpacity(float opacity);
float GetPlanarShadowOpacity();
bool ShouldShadowBeUpdated(const ProjectedShadowParams& params);
void RenderCharacterIntoTexture(const ProjectedShadowParams& params);
void RenderProjectedShadow(const ProjectedShadowParams& params);

RLView* SetCharacterShadowView(RLView* view);
void ClearCharacterShadowsUpdated();

#endif // GAME_RENDER_RENDERSHADOW_H

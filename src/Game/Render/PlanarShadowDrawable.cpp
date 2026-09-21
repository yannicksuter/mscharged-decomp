#include "Game/Render/PlanarShadowDrawable.h"

#include "Game/BasicStadium.h"
#include "Game/Drawable/DrawableModel.h"
#include "Game/Render/RenderShadow.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glView.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"

/**
 * Address/Size: 0x8027A4BC | size: 0x54
 */
PlanarShadowDrawable::PlanarShadowDrawable(
    WorldObjectLoadContext*, glModel* model, unsigned long type)
{
    m_uObjectFlags = 0;
    Initialize(model, type);
}

/**
 * Address/Size: 0x8027A510 | size: 0x104
 */
void PlanarShadowDrawable::Initialize(glModel* model, unsigned long hash)
{
    m_pModel = model;
    m_uRenderLayer = 0x1000D;
    m_uObjectCreationFlags = 2;
    m_uHashID = hash;
    m_pAnimController = 0;
    m_nAnimNode = 0;
    m_pWorldContext = BasicStadium::GetCurrentStadium();

    nlMatrix4 transform;
    transform.SetIdentity();
    SetWorldMatrix(transform);

    m_uObjectCreationFlags &= ~1;

    AABBDimensions dimensions;
    GetAABBDimensions(m_pModel, dimensions, 0);
    float opacity;
    float radius;
    if (dimensions.GetDimensionX() >= dimensions.GetDimensionY()
        && dimensions.GetDimensionX() > dimensions.GetDimensionZ())
        radius = dimensions.GetDimensionX();
    else if (dimensions.GetDimensionY() >= dimensions.GetDimensionZ())
        radius = dimensions.GetDimensionY();
    else
        radius = dimensions.GetDimensionZ();
    m_fBoundingRadius = radius;

    opacity = 1.0f;
    m_fTranslucency = opacity;
    if (opacity < 0.0f)
        m_fTranslucency = 0.0f;
    if (GetTranslucency() > 1.0f)
        m_fTranslucency = 1.0f;
}

/**
 * Address/Size: 0x8027A614 | size: 0xE0
 */
PlanarShadowDrawable* PlanarShadowDrawable::Clone(unsigned long hash)
{
    PlanarShadowDrawable* copy;
    WorldObjectLoadContext* context
        = (WorldObjectLoadContext*)nlMalloc(
            sizeof(WorldObjectLoadContext), 8, true);
    new (context) WorldObjectLoadContext(m_pWorldContext);

    copy = (PlanarShadowDrawable*)nlMalloc(
        sizeof(PlanarShadowDrawable), 8, false);
    copy = new (copy) PlanarShadowDrawable(context, m_pModel, hash);

    copy->m_pModel
        = glModelDupNoStreams(m_pModel, true, glGetCurrentResourcePool());
    delete context;
    return copy;
}

/**
 * Address/Size: 0x8027A6F4 | size: 0x14
 */
void PlanarShadowDrawable::Draw()
{
    V8(0);
}

/**
 * Address/Size: 0x8027A708 | size: 0x58
 */
void PlanarShadowDrawable::V8(GLView* view)
{
    if ((m_uObjectFlags & 1) != 0)
    {
        WorldDrawable::V8(view);
        if ((m_uObjectFlags & 4) != 0)
            DrawPlanarShadow();
    }
}

/**
 * Address/Size: 0x8027A760 | size: 0x90
 */
void PlanarShadowDrawable::DrawPlanarShadow()
{
    glModel* shadow = glModelDupNoStreams(m_pModel, false, 0);
    ::DrawPlanarShadow(shadow, *GetWorldMatrix(), 1, 0, this,
        GetPlanarShadowOpacity() * GetTranslucency());
}

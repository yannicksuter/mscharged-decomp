#include "NL/nlDLListContainer.inl"
#include "Game/Render/PlanarShadowDrawable.h"

#include "Game/BasicStadium.h"
#include "Game/Drawable/DrawableModel.h"
#include "Game/Field.h"
#include "Game/GameInfo.h"
#include "Game/GL/GLInventory.h"
#include "Game/Render/RenderShadow.h"
#include "Game/Render/Presentation.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glStateBundle.h"
#include "NL/gl/glView.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "NL/nlstring_tmpl.h"

// Distance the charge glow is allowed to reach past the sideline.
static float sSidelineMargin = 0.25f;

/**
 * Address/Size: 0x8027A7F0 | size: 0x78
 */
ChargeShadowDrawable::ChargeShadowDrawable(
    WorldObjectLoadContext* context, glModel* model, unsigned long type)
    : PlanarShadowDrawable(context, model, type)
{
    m_uChargeFlags = 0;
    Initialize(model, type);
}


/**
 * Address/Size: 0x8027A868 | size: 0x1CC
 */
void ChargeShadowDrawable::Initialize(glModel* model, unsigned long hash)
{
    PlanarShadowDrawable::Initialize(model, hash);

    m_pModel = model;
    m_uRenderLayer = 0x1000B;
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
    float radius;
    if (dimensions.GetDimensionX() >= dimensions.GetDimensionY()
        && dimensions.GetDimensionX() > dimensions.GetDimensionZ())
        radius = dimensions.GetDimensionX();
    else if (dimensions.GetDimensionY() >= dimensions.GetDimensionZ())
        radius = dimensions.GetDimensionY();
    else
        radius = dimensions.GetDimensionZ();
    m_fBoundingRadius = radius;

    m_uObjectFlags |= 1;
    m_worldMatrix.SetIdentity();
    m_orientation.z = 0.0f;
    m_orientation.y = 0.0f;
    m_orientation.x = 0.0f;
    m_orientation.w = 1.0f;
    m_translation.x = 0.0f;
    m_translation.y = 0.0f;
    m_translation.z = 0.0f;
    m_fScale = 1.0f;
    m_fCharge = 0.0f;
    m_bWorldMatrixUpToDate = true;

    GLInventory* inventory = glGetCurrentResourcePool()->m_inventory;
    m_pChargeModels[0] = 0;
    for (int level = 1; level < 6; level++)
    {
        char name[24];
        nlSNPrintf(name, sizeof(name), "gameplay/charge%d", level - 1);
        m_pChargeModels[level]
            = inventory->GetModel(nlStringHash(name));

        for (unsigned int packet = 0;
             packet < m_pChargeModels[level]->numPackets; packet++)
        {
            glSetRasterState(
                m_pChargeModels[level]->packets[packet].rasterState,
                (eGLState)5, 3);
        }
    }
}

/**
 * Address/Size: 0x8027AA34 | size: 0x4
 */
void ChargeShadowDrawable::ReleaseResources()
{
}

/**
 * Address/Size: 0x8027AA38 | size: 0x224
 */
void ChargeShadowDrawable::Draw()
{
    if ((m_uObjectFlags & 1) == 0)
        return;

    if (!m_bWorldMatrixUpToDate)
    {
        nlMatrix4 rotation;
        nlQuatToMatrix(rotation, m_orientation, true);
        rotation.m41 = m_translation.x;
        rotation.m42 = m_translation.y;
        rotation.m43 = m_translation.z;
        rotation.m44 = 1.0f;

        nlMatrix4 scale;
        nlMakeScaleMatrix(scale, m_fScale, m_fScale, m_fScale);
        nlMultMatrices(m_worldMatrix, scale, rotation);

        m_bWorldMatrixUpToDate = true;
        SetWorldMatrix(m_worldMatrix);
    }

    float maxCharge = 4.0f;
    float charge = m_fCharge / maxCharge;
    if (charge > 1.0f)
        charge = 1.0f;
    if ((m_uChargeFlags & 4) == 0)
        charge = 0.0f;

    GLView* previous;
    int level = (int)(4.0f * charge);
    if (nlTaskManager::m_pInstance->mCurrentState != 0x10)
    {
        previous = m_pWorldContext->m_pOpaqueView;
        m_pWorldContext->m_pOpaqueView = GetLayerView((eCLV)0xD);
        WorldDrawable::Draw();
        m_pWorldContext->m_pOpaqueView = previous;
    }
    else
        WorldDrawable::Draw();

    glModel* charged = m_pChargeModels[level];
    if (charged != 0)
    {
        glModelSetMatrix(charged, m_worldMatrix);
        GLView* view = (GLView*)GetLayerView((eCLV)0x1A);
        if (view == 0)
            view = m_pWorldContext->m_pOpaqueView;
        view->AttachModel(m_pChargeModels[level], 1);
    }

    if (!GetPresentation()->mChargeShadowsVisible)
        return;

    bool visible = true;
    if (nlSingleton<GameInfoManager>::Instance()->GetStadium() == 0xB)
    {
        nlMatrix4* transform = GetWorldMatrix();
        float edge = nlAbs(transform->m42);
        edge += 0.18f;
        if (edge > sSidelineMargin + cField::GetSidelineY(1))
            visible = false;
        cField::GetSidelineY(1);
    }

    if (visible && (m_uChargeFlags & 2) != 0)
        DrawBallShadowAndGlow(this);
}

/**
 * Address/Size: 0x8027AC5C | size: 0x104
 */
PlanarShadowDrawable* ChargeShadowDrawable::Clone(unsigned long hash)
{
    WorldObjectLoadContext* context
        = new (8, true) WorldObjectLoadContext(m_pWorldContext);

    ChargeShadowDrawable* copy
        = new (8, false) ChargeShadowDrawable(context, m_pModel, hash);

    copy->m_pModel = glModelDupNoStreams(
        m_pModel, true, glGetCurrentResourcePool());
    copy->m_fScale = m_fScale;
    copy->m_uChargeFlags = m_uChargeFlags;
    copy->m_fCharge = m_fCharge;
    delete context;
    return copy;
}

/**
 * Address/Size: 0x8027AD60 | size: 0x40
 */
ChargeShadowDrawable::~ChargeShadowDrawable()
{
}

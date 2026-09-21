#include "Game/Render/PlanarShadowDrawable.h"

#include "Game/BasicStadium.h"
#include "Game/Drawable/DrawableModel.h"
#include "Game/Field.h"
#include "Game/GameInfo.h"
#include "Game/GL/GLInventory.h"
#include "Game/Render/RenderShadow.h"
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

extern "C"
{
    void fn_80343C00(PlanarShadowDrawable* object);
    void fn_80186354(ChargeShadowDrawable* object);
    void* GetPresentation();
}

// Distance the charge glow is allowed to reach past the sideline.
static const float sSidelineMargin = 0.25f;

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
extern "C" void fn_8027A868(
    ChargeShadowDrawable* self, glModel* model, unsigned long hash)
{
    self->PlanarShadowDrawable::Initialize(model, hash);

    self->m_pModel = model;
    self->m_uRenderLayer = 0x1000B;
    self->m_uObjectCreationFlags = 2;
    self->m_uHashID = hash;
    self->m_pAnimController = 0;
    self->m_nAnimNode = 0;
    self->m_pWorldContext = BasicStadium::GetCurrentStadium();

    nlMatrix4 transform;
    transform.SetIdentity();
    self->SetWorldMatrix(transform);

    self->m_uObjectCreationFlags &= ~1;

    AABBDimensions dimensions;
    GetAABBDimensions(self->m_pModel, dimensions, 0);
    float radius;
    if (dimensions.GetDimensionX() >= dimensions.GetDimensionY()
        && dimensions.GetDimensionX() > dimensions.GetDimensionZ())
        radius = dimensions.GetDimensionX();
    else if (dimensions.GetDimensionY() >= dimensions.GetDimensionZ())
        radius = dimensions.GetDimensionY();
    else
        radius = dimensions.GetDimensionZ();
    self->m_fBoundingRadius = radius;

    self->m_uObjectFlags |= 1;
    self->m_worldMatrix.SetIdentity();
    self->m_orientation.z = 0.0f;
    self->m_orientation.y = 0.0f;
    self->m_orientation.x = 0.0f;
    self->m_orientation.w = 1.0f;
    self->m_translation.x = 0.0f;
    self->m_translation.y = 0.0f;
    self->m_translation.z = 0.0f;
    self->m_fScale = 1.0f;
    self->m_fCharge = 0.0f;
    self->m_bWorldMatrixUpToDate = true;

    GLInventory* inventory = glGetCurrentResourcePool()->m_inventory;
    self->m_pChargeModels[0] = 0;
    for (int level = 1; level < 6; level++)
    {
        char name[24];
        nlSNPrintf(name, sizeof(name), "gameplay/charge%d", level - 1);
        self->m_pChargeModels[level]
            = inventory->GetModel(nlStringHash(name));

        for (unsigned int packet = 0;
             packet < self->m_pChargeModels[level]->numPackets; packet++)
        {
            glSetRasterState(
                self->m_pChargeModels[level]->packets[packet].rasterState,
                (eGLState)5, 3);
        }
    }
}

/**
 * Address/Size: 0x8027AA34 | size: 0x4
 */
extern "C" void fn_8027AA34(ChargeShadowDrawable*)
{
}

/**
 * Address/Size: 0x8027AA38 | size: 0x224
 */
extern "C" void fn_8027AA38(ChargeShadowDrawable* self)
{
    if ((self->m_uObjectFlags & 1) == 0)
        return;

    if (!self->m_bWorldMatrixUpToDate)
    {
        nlMatrix4 rotation;
        nlQuatToMatrix(rotation, self->m_orientation, true);

        nlMatrix4 scale;
        nlMakeScaleMatrix(scale, self->m_fScale, self->m_fScale, self->m_fScale);
        nlMultMatrices(self->m_worldMatrix, scale, rotation);

        self->m_bWorldMatrixUpToDate = true;
        self->SetWorldMatrix(self->m_worldMatrix);
    }

    float charge = self->m_fCharge / 4.0f;
    if (charge > 1.0f)
        charge = 1.0f;
    if ((self->m_uChargeFlags & 4) == 0)
        charge = 0.0f;

    int level = (int)(4.0f * charge);
    if (nlTaskManager::m_pInstance->mCurrentState == 0x10)
    {
        fn_80343C00(self);
    }
    else
    {
        GLView* previous = (GLView*)((BasicStadium*)self->m_pWorldContext)->m_pHighRangeTweaks;
        ((BasicStadium*)self->m_pWorldContext)->m_pHighRangeTweaks
            = (HighRangeTweaks*)GetLayerView((eCLV)0xD);
        fn_80343C00(self);
        ((BasicStadium*)self->m_pWorldContext)->m_pHighRangeTweaks
            = (HighRangeTweaks*)previous;
    }

    glModel* charged = self->m_pChargeModels[level];
    if (charged != 0)
    {
        glModelSetMatrix(charged, self->m_worldMatrix);
        GLView* view = (GLView*)GetLayerView((eCLV)0x1A);
        if (view == 0)
            view = (GLView*)((BasicStadium*)self->m_pWorldContext)->m_pHighRangeTweaks;
        view->AttachModel(self->m_pChargeModels[level], 1);
    }

    if (!*((bool*)GetPresentation() + 0x143))
        return;

    bool visible = true;
    if (nlSingleton<GameInfoManager>::Instance()->GetStadium() == 0xB)
    {
        nlMatrix4* transform = self->GetWorldMatrix();
        float edge = nlAbs(transform->m23) + 0.18f;
        if (edge > sSidelineMargin + cField::GetSidelineY(1))
            visible = false;
        cField::GetSidelineY(1);
    }

    if (visible && (self->m_uChargeFlags & 2) != 0)
        fn_80186354(self);
}

/**
 * Address/Size: 0x8027AC5C | size: 0x104
 */
extern "C" ChargeShadowDrawable* fn_8027AC5C(
    ChargeShadowDrawable* self, unsigned long hash)
{
    WorldObjectLoadContext* context
        = new (8, true) WorldObjectLoadContext(self->m_pWorldContext);

    ChargeShadowDrawable* copy
        = new (8, false) ChargeShadowDrawable(context, self->m_pModel, hash);

    copy->m_pModel = glModelDupNoStreams(
        self->m_pModel, true, glGetCurrentResourcePool());
    copy->m_fScale = self->m_fScale;
    copy->m_uChargeFlags = self->m_uChargeFlags;
    copy->m_fCharge = self->m_fCharge;
    delete context;
    return copy;
}

/**
 * Address/Size: 0x8027AD60 | size: 0x40
 */
extern "C" ChargeShadowDrawable* fn_8027AD60(
    ChargeShadowDrawable* self, int destroy)
{
    if (self != 0 && destroy > 0)
        operator delete(self);
    return self;
}

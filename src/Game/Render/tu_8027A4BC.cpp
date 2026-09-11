#include "Game/Render/tu_8027A4BC.h"

#include "Game/BasicStadium.h"
#include "Game/Drawable/DrawableModel.h"
#include "Game/GL/GLInventory.h"
#include "Game/Field.h"
#include "Game/GameInfo.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/Render/RenderShadow.h"
#include "unclassified/tu_80186524.h"
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
    void fn_80343C00(PlanarShadowDrawable_8027A4BC* object);
    void fn_80343C14(PlanarShadowDrawable_8027A4BC* object, GLView* view);
    void fn_80186354(ChargeShadowDrawable_8027A7F0* object);
    void* GetPresentation();
}

// Distance the charge glow is allowed to reach past the sideline.
static const float sSidelineMargin = 0.25f;

/**
 * Address/Size: 0x8027A4BC | size: 0x54
 */
extern "C" PlanarShadowDrawable_8027A4BC* fn_8027A4BC(
    PlanarShadowDrawable_8027A4BC* self, ShadowLoadContext_8027A4BC*,
    glModel* model, unsigned long type)
{
    *(void***)self = lbl_80523138;
    self->m_uFlags = 0;
    self->Initialize(model, type);
    return self;
}

/**
 * Address/Size: 0x8027A510 | size: 0x104
 */
extern "C" void fn_8027A510(
    PlanarShadowDrawable_8027A4BC* self, glModel* model, unsigned long hash)
{
    self->m_pModel = model;
    self->m_uType = 0x1000D;
    self->m_uObjectCreationFlags = 2;
    self->m_uHashID = hash;
    self->m_pUnidentified18 = 0;
    self->m_pUnidentified14 = 0;
    self->m_pWorld = BasicStadium::GetCurrentStadium();

    nlMatrix4 transform;
    transform.SetIdentity();
    self->SetWorldMatrix(&transform);

    self->m_uObjectCreationFlags &= ~1;

    AABBDimensions dimensions;
    GetAABBDimensions(self->m_pModel, dimensions, 0);
    float radius = dimensions.mDim.x;
    if (!(radius >= dimensions.mDim.y && radius > dimensions.mDim.z))
    {
        radius = dimensions.mDim.y >= dimensions.mDim.z ? dimensions.mDim.y
                                                        : dimensions.mDim.z;
    }
    self->m_fRadius = radius;

    self->m_fOpacity = 1.0f;
    if (1.0f < 0.0f)
        self->m_fOpacity = 0.0f;
    if (self->GetOpacity() > 1.0f)
        self->m_fOpacity = 1.0f;
}

/**
 * Address/Size: 0x8027A614 | size: 0xE0
 */
extern "C" PlanarShadowDrawable_8027A4BC* fn_8027A614(
    PlanarShadowDrawable_8027A4BC* self, unsigned long hash)
{
    ShadowLoadContext_8027A4BC* context
        = (ShadowLoadContext_8027A4BC*)nlMalloc(
            sizeof(ShadowLoadContext_8027A4BC), 8, true);
    if (context != 0)
    {
        context->m_pUnidentified00 = 0;
        context->m_pWorld = self->m_pWorld;
        context->m_pUnidentified08 = 0;
        context->m_pUnidentified0C = 0;
    }

    PlanarShadowDrawable_8027A4BC* copy
        = (PlanarShadowDrawable_8027A4BC*)nlMalloc(
            sizeof(PlanarShadowDrawable_8027A4BC), 8, false);
    if (copy != 0)
    {
        *(void***)copy = lbl_80523138;
        copy->m_uFlags = 0;
        copy->Initialize(self->m_pModel, hash);
    }

    copy->m_pModel
        = glModelDupNoStreams(self->m_pModel, true, glGetCurrentResourcePool());
    delete context;
    return copy;
}

/**
 * Address/Size: 0x8027A6F4 | size: 0x14
 */
extern "C" void fn_8027A6F4(PlanarShadowDrawable_8027A4BC* self)
{
    self->V8(0);
}

/**
 * Address/Size: 0x8027A708 | size: 0x58
 */
extern "C" void fn_8027A708(
    PlanarShadowDrawable_8027A4BC* self, GLView* view)
{
    if ((self->m_uFlags & 1) != 0)
    {
        fn_80343C14(self, view);
        if ((self->m_uFlags & 4) != 0)
            self->DrawShadow();
    }
}

/**
 * Address/Size: 0x8027A760 | size: 0x90
 */
extern "C" void fn_8027A760(PlanarShadowDrawable_8027A4BC* self)
{
    glModel* shadow = glModelDupNoStreams(self->m_pModel, false, 0);
    fn_801869AC(shadow, self->GetWorldMatrix(), 1, 0, self,
        GetPlanarShadowOpacity() * self->GetOpacity());
}

/**
 * Address/Size: 0x8027A7F0 | size: 0x78
 */
extern "C" ChargeShadowDrawable_8027A7F0* fn_8027A7F0(
    ChargeShadowDrawable_8027A7F0* self, ShadowLoadContext_8027A4BC* context,
    glModel* model, unsigned long type)
{
    fn_8027A4BC(self, context, model, type);
    self->m_uChargeFlags = 0;
    *(void***)self = lbl_80523188;
    self->Initialize(model, type);
    return self;
}

/**
 * Address/Size: 0x8027A868 | size: 0x1CC
 */
extern "C" void fn_8027A868(
    ChargeShadowDrawable_8027A7F0* self, glModel* model, unsigned long hash)
{
    fn_8027A510(self, model, hash);

    self->m_pModel = model;
    self->m_uType = 0x1000B;
    self->m_uObjectCreationFlags = 2;
    self->m_uHashID = hash;
    self->m_pUnidentified18 = 0;
    self->m_pUnidentified14 = 0;
    self->m_pWorld = BasicStadium::GetCurrentStadium();

    nlMatrix4 transform;
    transform.SetIdentity();
    self->SetWorldMatrix(&transform);

    self->m_uObjectCreationFlags &= ~1;

    AABBDimensions dimensions;
    GetAABBDimensions(self->m_pModel, dimensions, 0);
    float radius = dimensions.mDim.x;
    if (!(radius >= dimensions.mDim.y && radius > dimensions.mDim.z))
    {
        radius = dimensions.mDim.y >= dimensions.mDim.z ? dimensions.mDim.y
                                                        : dimensions.mDim.z;
    }
    self->m_fRadius = radius;

    self->m_uFlags |= 1;
    self->m_worldMatrix.SetIdentity();
    self->m_orientation.x = 0.0f;
    self->m_orientation.y = 0.0f;
    self->m_orientation.z = 0.0f;
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

        glModel* charge = self->m_pChargeModels[level];
        for (unsigned int packet = 0; packet < charge->numPackets; packet++)
        {
            glSetRasterState(
                charge->packets[packet].rasterState, (eGLState)5, 3);
        }
    }
}

/**
 * Address/Size: 0x8027AA34 | size: 0x4
 */
extern "C" void fn_8027AA34(ChargeShadowDrawable_8027A7F0*)
{
}

/**
 * Address/Size: 0x8027AA38 | size: 0x224
 */
extern "C" void fn_8027AA38(ChargeShadowDrawable_8027A7F0* self)
{
    if ((self->m_uFlags & 1) == 0)
        return;

    if (!self->m_bWorldMatrixUpToDate)
    {
        nlMatrix4 rotation;
        nlQuatToMatrix(rotation, self->m_orientation, true);

        nlMatrix4 scale;
        nlMakeScaleMatrix(scale, self->m_fScale, self->m_fScale, self->m_fScale);
        nlMultMatrices(self->m_worldMatrix, scale, rotation);

        self->m_bWorldMatrixUpToDate = true;
        self->SetWorldMatrix(&self->m_worldMatrix);
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
        GLView* previous = (GLView*)self->m_pWorld->m_pHighRangeTweaks;
        self->m_pWorld->m_pHighRangeTweaks
            = (HighRangeTweakValues_801A2004*)GetLayerView((eCLV)0xD);
        fn_80343C00(self);
        self->m_pWorld->m_pHighRangeTweaks
            = (HighRangeTweakValues_801A2004*)previous;
    }

    glModel* charged = self->m_pChargeModels[level];
    if (charged != 0)
    {
        glModelSetMatrix(charged, self->m_worldMatrix);
        GLView* view = (GLView*)GetLayerView((eCLV)0x1A);
        if (view == 0)
            view = (GLView*)self->m_pWorld->m_pHighRangeTweaks;
        view->AttachModel(self->m_pChargeModels[level], 1);
    }

    if (!*((bool*)GetPresentation() + 0x143))
        return;

    bool visible = true;
    if (nlSingleton<GameInfoManager>::Instance()->GetStadium() == 0xB)
    {
        nlMatrix4* transform
            = self->GetWorldMatrix();
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
extern "C" ChargeShadowDrawable_8027A7F0* fn_8027AC5C(
    ChargeShadowDrawable_8027A7F0* self, unsigned long hash)
{
    ShadowLoadContext_8027A4BC* context
        = (ShadowLoadContext_8027A4BC*)nlMalloc(
            sizeof(ShadowLoadContext_8027A4BC), 8, true);
    if (context != 0)
    {
        context->m_pUnidentified00 = 0;
        context->m_pWorld = self->m_pWorld;
        context->m_pUnidentified08 = 0;
        context->m_pUnidentified0C = 0;
    }

    ChargeShadowDrawable_8027A7F0* copy
        = (ChargeShadowDrawable_8027A7F0*)nlMalloc(
            sizeof(ChargeShadowDrawable_8027A7F0), 8, false);
    if (copy != 0)
    {
        glModel* model = self->m_pModel;
        fn_8027A4BC(copy, context, model, hash);
        *(void***)copy = lbl_80523188;
        copy->m_uChargeFlags = 0;
        copy->Initialize(model, hash);
    }

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
extern "C" ChargeShadowDrawable_8027A7F0* fn_8027AD60(
    ChargeShadowDrawable_8027A7F0* self, int destroy)
{
    if (self != 0 && destroy > 0)
        operator delete(self);
    return self;
}

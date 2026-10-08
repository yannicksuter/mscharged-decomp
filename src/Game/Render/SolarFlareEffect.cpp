#include "Game/Render/SolarFlareEffect.h"
#include "Game/BasicStadium.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/World/WorldDrawable.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glView.h"

float gSolarFlareEffectLifetime = 0.5f;
float gSolarFlareEffectLifetimeVariation = 0.1f;
float lbl_806DEE90 = 0.3f;
float lbl_806DEE94 = 0.2f;

bool gForceDrawSolarFlareDrawable;
SolarFlareDrawable* g_pSolarFlareDrawable;

void SolarFlareDrawable::Initialize(WorldObjectLoadContext* context)
{
    WorldDrawable::Initialize(context);
    g_pSolarFlareDrawable = this;
}

void SolarFlareDrawable::ReleaseResources()
{
}

void SolarFlareDrawable::Draw()
{
    if (m_uDrawEnabled != 0 || gForceDrawSolarFlareDrawable)
        WorldDrawable::Draw();
}

SolarFlareEffect::SolarFlareEffect(const nlVector3& targetPosition)
    : TimedObject(gSolarFlareEffectLifetime
          + nlRandomf(-gSolarFlareEffectLifetimeVariation,
              gSolarFlareEffectLifetimeVariation, &nlDefaultSeed))
    , mTargetPosition(targetPosition)
    , mUnidentified020(false)
{
    mUnidentified01C = lbl_806DEE90
        + nlRandomf(-lbl_806DEE94, lbl_806DEE94, &nlDefaultSeed);
}

SolarFlareEffect::~SolarFlareEffect()
{
}

static inline void OrientTowardPosition(
    SolarFlareDrawable* drawable, const nlVector3& position)
{
    nlVector3 direction;
    nlVec3Sub(direction, position, drawable->mWorldMatrix.GetTranslation());
    nlVec3Normalize(direction, direction);

    nlVector3 up;
    nlVec3Set(up, 0.0f, 1.0f, 0.0f);
    nlVector3 right;
    nlVec3CrossProduct(right, direction, up);
    nlVec3CrossProduct(up, right, direction);

    drawable->mWorldMatrix.SetRow_(0, direction);
    drawable->mWorldMatrix.SetRow_(1, up);
    drawable->mWorldMatrix.SetRow_(2, right);
}

void SolarFlareEffect::Update(float)
{
    if (g_pSolarFlareDrawable != 0)
    {
        OrientTowardPosition(g_pSolarFlareDrawable, mTargetPosition);

        nlMatrix4 transform = *g_pSolarFlareDrawable->GetWorldMatrix();
        glModel* model = glModelDupNoStreams(
            g_pSolarFlareDrawable->GetModel(), false, glGetCurrentResourcePool());
        glModelSetMatrix(model, transform);
        BasicStadium::GetCurrentStadium()->m_pAlphaView->AttachModel(model, 0);
    }
}

SolarFlareDrawable::~SolarFlareDrawable()
{
}

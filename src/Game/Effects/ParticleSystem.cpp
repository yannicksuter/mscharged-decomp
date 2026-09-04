#include "Game/Effects/ParticleSystem.h"

#include <math.h>

#include "Game/Effects/EmissionManager.h"
#include "Game/GL/GLInventory.h"
#include "Game/GL/GLMeshWriter.h"
#include "Game/GL/GLVertexAnim.h"
#include "NL/gl/gl.h"
#include "NL/gl/glDraw3.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialCrystalTweaks.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/platvmath.h"

struct TextureFrame
{
    float mUnidentified000;
    float mUnidentified004;
    float mUnidentified008;
}; // size: 0x0C

static GXMaterialFloatTweak_804F4190 sfParticleRedScale(
    "sfParticleRedScale", "Render/Particles/Colour Scaling", 1.0f);
static GXMaterialFloatTweak_804F4190 sfParticleGreenScale(
    "sfParticleGreenScale", "Render/Particles/Colour Scaling", 1.0f);
static GXMaterialFloatTweak_804F4190 sfParticleBlueScale(
    "sfParticleBlueScale", "Render/Particles/Colour Scaling", 1.0f);

static TextureFrame* textureFrames[36];

static bool sUnidentified_806DF470 = true;
float ParticleSystem::m_fAspect = 1.0f;
bool ParticleSystem::m_AllowInFront = true;

static bool sUnidentified_806E1F98;
static bool sUnidentified_806E1F99;
int ParticleSystem::m_NumInstances;
bool (*ParticleSystem::m_Callback)(ParticleSystem*, GLView*,
    nlDLListSlotPool<Particle*>*, const nlVector3&, const nlVector3&,
    const nlMatrix4*);
glModel* (*ParticleSystem::m_LightingCallback)(glModel*);
static int MaxNumParticles;
static int sUnidentified_806E1FAC;
static unsigned short hackyFacingAngle;

extern GLInventory* lbl_806E1FFC;
extern const nlVector3 lbl_804EB340;

extern "C" unsigned long fn_802CDFCC(unsigned long texture);
extern "C" void fn_802CC3C8(glModelPacket* packet, unsigned long hash,
    const void* value, unsigned long count);
extern "C" bool fn_802CC8FC(
    const glModelPacket* packet, unsigned long hash);
extern "C" void fn_8004F594(int channel, const char* format, ...);

ParticleSystem::ParticleSystem(EffectsTemplate* pTemplate,
    nlDLListSlotPool<Particle*>* pFreeParticles, EffectsSpec* spec,
    unsigned long resourceID)
    : mUnidentified000(resourceID)
    , m_Particles()
    , mUnidentified0C0(pFreeParticles)
{
    ++m_NumInstances;
    mUnidentified0BC = 0;
    m_pTemplate = pTemplate;
    m_pSpec = spec;
    m_fElapsedTime = 0.0f;
    mUnidentified014 = 0.0f;
    m_fNumParticlesToCreate = 0.0f;
    m_fDelay = 0.0f;
    m_uLayer = 0;
    mUnidentified020 = 0.0f;
    nlVec3Set(m_vVelocity, 0.0f, 0.0f, 0.0f);
    nlVec3Set(m_vPosition, 0.0f, 0.0f, 0.0f);
    nlVec3Set(m_vForward, 0.0f, 1.0f, 0.0f);
    nlVec3Set(m_vSourcePosition, 0.0f, 0.0f, 0.0f);
    mUnidentified058.SetIdentity();
    m_aFacing = 0;
    m_bAmDying = false;
    m_bVisible = false;
    mUnidentified09C = fn_802CDFCC(m_pTemplate->m_hTexture);
}

ParticleSystem::~ParticleSystem()
{
    if (m_pTemplate->mUnidentified040 != 0)
    {
        fn_8004F594(3, "OnEmitterDeath: %d\n",
            m_pTemplate->mUnidentified040);
    }

    --m_NumInstances;
    while (m_Particles.m_Head != 0)
    {
        Particle* particle;
        m_Particles.RemoveStart(&particle);
        mUnidentified0C0->AddEnd(particle);
    }
    mUnidentified0BC = 0;
}

void ParticleSystem::UpdateCoordSys()
{
    UpdateCoordSys(mUnidentified058);
}

void ParticleSystem::UpdateCoordSys(nlMatrix4& mCoordSys)
{
    float lenSq = nlVec3LengthSquared(m_vForward);
    float rsqrt = nlRecipSqrt(lenSq, true);

    nlVector3 grav;
    nlVec3Scale(grav, m_vForward, rsqrt);

    nlVector3 ref;
    nlVec3Set(ref, 0.0f, 0.0f, 1.0f);
    if ((float)__fabs(nlVec3DotProduct(ref, grav)) > 0.99f)
    {
        nlVec3Set(ref, 0.0f, 1.0f, 0.0f);
    }

    nlVector3 right;
    nlVec3CrossProduct(right, grav, ref);
    nlVec3Scale(right, nlRecipSqrt(nlVec3LengthSquared(right), true));

    nlVector3 up;
    nlVec3CrossProduct(up, right, grav);
    nlVec3Scale(up, nlRecipSqrt(nlVec3LengthSquared(up), true));

    mCoordSys.SetRow_(0, right);
    mCoordSys.SetRow_(1, up);
    mCoordSys.e[8] = -grav.x;
    mCoordSys.e[9] = -grav.y;
    mCoordSys.e[10] = -grav.z;
    mCoordSys.SetTranslation(m_vPosition);
    mCoordSys.e[11] = 0.0f;
    mCoordSys.e[7] = 0.0f;
    mCoordSys.e[3] = 0.0f;
}

typedef void (*EmitParticlePosition)(nlVector3&, nlVector3&,
    ParticleSystem*, EffectsSpec*, const nlMatrix4&);

static void EmitCircularPosition(nlVector3& pos, nlVector3& dir,
    ParticleSystem* pSystem, EffectsSpec* pSpec,
    const nlMatrix4& mLocalToWorld)
{
    float randomAngle = RandomizedValue(0.0f, 6.2831855f);
    float sinVal;
    float cosVal;
    nlSinCos(&sinVal, &cosVal,
        (unsigned short)(int)(10430.378f * randomAngle));

    float radius
        = pSystem->m_pTemplate->mUnidentified058[4]->fn_802E0010(
            pSystem->mUnidentified014);
    nlVector3 localPos;
    localPos.x = cosVal * radius;
    localPos.y = -sinVal * radius;
    localPos.z = 0.0f;

    if (pSpec != 0)
    {
        nlVec3Add(localPos, localPos, pSpec->m_vLocalOffset);
    }

    if (pSystem->m_pTemplate->IsLocalSpace())
        pos = localPos;
    else
        nlMultPosVectorMatrix(pos, localPos, mLocalToWorld);
}

static void fn_802E0CA4(nlVector3& pos, nlVector3& dir,
    ParticleSystem* pSystem, EffectsSpec* pSpec,
    const nlMatrix4& mLocalToWorld)
{
    float randomAngle = RandomizedValue(0.0f, 6.2831855f);
    float sinVal;
    float cosVal;
    nlSinCos(&sinVal, &cosVal,
        (unsigned short)(int)(10430.378f * randomAngle));

    float radius
        = pSystem->m_pTemplate->mUnidentified058[4]->fn_802E0010(
            pSystem->mUnidentified014);
    radius = RandomizedValue(0.0f, radius);

    nlVector3 localPos;
    localPos.x = cosVal * radius;
    localPos.y = -sinVal * radius;
    localPos.z = 0.0f;

    if (pSpec != 0)
        nlVec3Add(localPos, localPos, pSpec->m_vLocalOffset);

    if (pSystem->m_pTemplate->IsLocalSpace())
        pos = localPos;
    else
        nlMultPosVectorMatrix(pos, localPos, mLocalToWorld);
}

static void EmitSphericalPosition(nlVector3& pos, nlVector3& dir,
    ParticleSystem* pSystem, EffectsSpec* pSpec,
    const nlMatrix4& mLocalToWorld)
{
    float randomZ = RandomizedValue(0.0f, 2.0f);
    float randomAngleValue = RandomizedValue(0.0f, 6.2831855f);
    float xyRadius = nlSqrt(1.0f - randomZ * randomZ, true);

    float sinVal;
    float cosVal;
    nlSinCos(&sinVal, &cosVal,
        (unsigned short)(int)(10430.378f * randomAngleValue));

    nlVector3 localDir;
    nlVec3Set(localDir, xyRadius * cosVal, xyRadius * sinVal, randomZ);
    float radius
        = pSystem->m_pTemplate->mUnidentified058[4]->fn_802E0010(
            pSystem->mUnidentified014);
    nlVector3 localPos;
    nlVec3Scale(localPos, localDir, radius);

    if (pSpec != 0)
        nlVec3Add(localPos, localPos, pSpec->m_vLocalOffset);

    if (pSystem->m_pTemplate->IsLocalSpace())
    {
        pos = localPos;
        dir = localDir;
    }
    else
    {
        nlMultPosVectorMatrix(pos, localPos, mLocalToWorld);
        nlMultDirVectorMatrix(dir, localDir, mLocalToWorld);
    }
}

static void EmitHemisphericalPosition(nlVector3& pos, nlVector3& dir,
    ParticleSystem* pSystem, EffectsSpec* pSpec,
    const nlMatrix4& mLocalToWorld)
{
    float randomZ = RandomizedValue(-0.5f, 1.0f);
    float randomAngleValue = RandomizedValue(0.0f, 6.2831855f);
    float xyRadius = nlSqrt(1.0f - randomZ * randomZ, true);

    float sinVal;
    float cosVal;
    nlSinCos(&sinVal, &cosVal,
        (unsigned short)(int)(10430.378f * randomAngleValue));

    nlVector3 localDir;
    nlVec3Set(localDir, xyRadius * cosVal, xyRadius * sinVal, randomZ);
    float radius
        = pSystem->m_pTemplate->mUnidentified058[4]->fn_802E0010(
            pSystem->mUnidentified014);
    nlVector3 localPos;
    nlVec3Scale(localPos, localDir, radius);

    if (pSpec != 0)
        nlVec3Add(localPos, localPos, pSpec->m_vLocalOffset);

    if (pSystem->m_pTemplate->IsLocalSpace())
    {
        pos = localPos;
        dir = localDir;
    }
    else
    {
        nlMultPosVectorMatrix(pos, localPos, mLocalToWorld);
        nlMultDirVectorMatrix(dir, localDir, mLocalToWorld);
    }
}

static inline void RotateXYInPlace(nlVector3& v, float sn, float cs)
{
    float x = v.x * cs - v.y * sn;
    float y = v.x * sn + v.y * cs;
    v.x = x;
    v.y = y;
}

static inline void RotateXZInPlace(nlVector3& v, float sn, float cs)
{
    float x = v.x * cs + v.z * sn;
    float z = -v.x * sn + v.z * cs;
    v.x = x;
    v.z = z;
}

static void EmitSpindularPosition(nlVector3& pos, nlVector3& dir,
    ParticleSystem* pSystem, EffectsSpec* pSpec,
    const nlMatrix4& mLocalToWorld)
{
    float sin;
    float cos;
    float randomAngle = RandomizedValue(0.0f, 6.2831855f);
    nlSinCos(&sin, &cos,
        (unsigned short)(int)(10430.378f * randomAngle));

    float radius
        = pSystem->m_pTemplate->mUnidentified058[4]->fn_802E0010(
            pSystem->mUnidentified014);
    nlVector3 localPos;
    nlVec3Set(localPos, cos * radius, -sin * radius, 0.0f);

    float tilt = pSystem->m_pTemplate->mUnidentified058[6]->fn_802E0010(
        pSystem->mUnidentified014);
    if (tilt <= -90.0f)
        tilt = -89.9f;
    else if (tilt >= 90.0f)
        tilt = 89.9f;

    nlVector3 localDir;
    localDir.z = nlTan((unsigned short)(((int)(-tilt * 65536.0f)) / 360));
    localDir.x = cos;
    localDir.y = -sin;
    nlVec3Scale(localDir,
        nlRecipSqrt(nlVec3LengthSquared(localDir), false));

    float tiltRotation
        = pSystem->m_pTemplate->mUnidentified058[7]->fn_802E0010(
        pSystem->mUnidentified014);
    tiltRotation = -tiltRotation * 3.14159265f / 180.0f;
    if (tiltRotation != 0.0f)
    {
        nlSinCos(&sin, &cos,
            (unsigned short)(int)(10430.378f * tiltRotation));
        RotateXZInPlace(localDir, sin, cos);
        RotateXZInPlace(localPos, sin, cos);
    }

    if (pSpec != 0)
        nlVec3Add(localPos, localPos, pSpec->m_vLocalOffset);

    if (pSystem->m_pTemplate->IsLocalSpace())
    {
        pos = localPos;
        dir = localDir;
    }
    else
    {
        nlMultDirVectorMatrix(pos, localPos, mLocalToWorld);
        nlMultDirVectorMatrix(dir, localDir, mLocalToWorld);
        if (hackyFacingAngle != 0)
        {
            nlSinCos(&sin, &cos, hackyFacingAngle);
            RotateXYInPlace(dir, sin, cos);
            RotateXYInPlace(pos, sin, cos);
        }
        nlVec3Add(pos, pos, mLocalToWorld.GetTranslation());
    }
}

void ParticleSystem::CreateNewParticles(int numParticles)
{
    nlMatrix4 mCoordSys;
    UpdateCoordSys(mCoordSys);

    nlVector3 baseDir;
    if (m_pTemplate->IsLocalSpace())
        nlVec3Set(baseDir, 0.0f, 0.0f, -1.0f);
    else
        baseDir = m_vForward;

    EmitParticlePosition emit = 0;
    switch (m_pTemplate->m_eEmitter)
    {
    case Emitter_Circle:
        emit = EmitCircularPosition;
        break;
    case Emitter_Sphere:
        emit = EmitSphericalPosition;
        break;
    case Emitter_Spindle:
        emit = EmitSpindularPosition;
        hackyFacingAngle = m_aFacing;
        break;
    case Emitter_Hemisphere:
        emit = EmitHemisphericalPosition;
        break;
    case Emitter_Unidentified4:
        emit = fn_802E0CA4;
        break;
    }

    for (int i = 0; i < numParticles; ++i)
    {
        Particle* pPart = 0;
        if (mUnidentified0C0->m_Head != 0)
            mUnidentified0C0->RemoveStart(&pPart);
        if (pPart == 0)
            break;

        if (m_pTemplate->mUnidentified044 != 0)
        {
            fn_8004F594(3, "OnParticleCreation: %d\n",
                m_pTemplate->mUnidentified044);
        }

        m_Particles.AddEnd(pPart);
        ++mUnidentified0BC;
        pPart->mUnidentified000 = m_pTemplate;

        nlVector3 dir = baseDir;
        emit(pPart->mUnidentified010, dir, this, m_pSpec, mCoordSys);
        nlVec3Add(pPart->position, pPart->mUnidentified010,
            m_vSourcePosition);

        pPart->lifeSpan = RandomizedValue(m_pTemplate->m_rParticleLife);
        pPart->mUnidentified05C
            = RandomizedValue(m_pTemplate->m_rRotation);
        pPart->rot = pPart->mUnidentified05C + mUnidentified020;
        pPart->dRot
            = m_pTemplate->mUnidentified058[3]->fn_802E0010(0.0f);
        pPart->mass = RandomizedValue(m_pTemplate->m_rMass);
        pPart->size
            = m_pTemplate->mUnidentified058[1]->fn_802E0010(0.0f);
        pPart->mUnidentified040
            = m_pTemplate->mUnidentified058[2]->fn_802E0010(0.0f);
        pPart->mUnidentified060
            = nlRandomf(100.0f, &uSeed) < m_pTemplate->mUnidentified030;

        float inheritVelocity
            = RandomizedValue(m_pTemplate->m_rInheritVelocity);
        nlVector3 velocity;
        nlVec3Scale(velocity, m_vVelocity, inheritVelocity);
        float vel
            = m_pTemplate->mUnidentified058[5]->fn_802E0010(0.0f);
        nlVec3ScaleAdd(velocity, vel, dir, velocity);
        float speedSquared = nlVec3LengthSquared(velocity);
        pPart->velocity = nlSqrt(speedSquared, true);
        if (pPart->velocity == 0.0f)
            pPart->velDir = dir;
        else
            nlVec3Scale(pPart->velDir, velocity,
                nlRecipSqrt(speedSquared, true));

        pPart->acceleration
            = RandomizedValue(m_pTemplate->m_rAcceleration);
        pPart->frame = 0.0f;
        pPart->FPS = RandomizedValue(m_pTemplate->m_rFPS);
        pPart->timeElapsed = 0.0f;
        pPart->mUnidentified008 = 0.0f;
    }
}

void ParticleSystem::UpdateAllParticles(float dt,
    const nlMatrix4* pCoordSys)
{
    nlDLListIterator<Particle*> iterator = m_Particles.Begin();
    while (iterator.hasNext())
    {
        Particle* p = *iterator;
        p->timeElapsed += dt;
        p->mUnidentified008 = p->timeElapsed / p->lifeSpan;
        if (p->timeElapsed >= p->lifeSpan)
        {
            if (m_pTemplate->mUnidentified048 != 0)
            {
                fn_8004F594(3, "OnParticleDeath: %d\n",
                    m_pTemplate->mUnidentified048);
            }
            m_Particles.Remove(&iterator);
            --mUnidentified0BC;
            mUnidentified0C0->AddEnd(p);
        }
        else
        {
            iterator.Step();
            fn_802E1EC0(p, pCoordSys);
        }
    }
}

void ParticleSystem::UpdateLight(EffectsLight* pLight, Particle* pPart,
    EffectsTemplate* pTemplate, const nlVector3& viewRight,
    const nlVector3& viewUp,
    const nlMatrix4* pCoordSys)
{
    int colourIndex = (int)(24.5f * pPart->mUnidentified008);
    pLight->m_Colour = pTemplate->m_cColour[colourIndex];

    float size;
    if (pTemplate->mUnidentified058[1]->mUnidentified000 != 0)
        size = pTemplate->mUnidentified058[1]->fn_802E0010(
            pPart->mUnidentified008);
    else
        size = pPart->size;

    if (pTemplate->mUnidentified058[2]->mUnidentified000 != 0)
        size *= pTemplate->mUnidentified058[2]->fn_802E0010(
            mUnidentified014);
    else
        size *= pPart->mUnidentified040;
    pLight->m_fRadius = 0.5f * size;

    pLight->m_v3Position = pPart->position;
    if (pCoordSys != 0)
    {
        nlVector3 position;
        nlMultPosVectorMatrix(position, pLight->m_v3Position, *pCoordSys);
        pLight->m_v3Position = position;
    }
}

void ParticleSystem::fn_802E1EC0(Particle* pPart,
    const nlMatrix4* pCoordSys)
{
    float velocityCurve = 0.0f;
    if (pPart->mUnidentified000->mUnidentified058[5]->mUnidentified000 != 0)
    {
        velocityCurve
            = pPart->mUnidentified000->mUnidentified058[5]->fn_802E0010(
                pPart->mUnidentified008);
    }

    float rotationDelta;
    if (pPart->mUnidentified000->mUnidentified058[3]->mUnidentified000 != 0)
    {
        rotationDelta
            = pPart->mUnidentified000->mUnidentified058[3]->fn_802E0010(
                pPart->mUnidentified008);
    }
    else
    {
        rotationDelta = pPart->dRot;
    }
    pPart->rot += mUnidentified010 * rotationDelta;

    float velocity = pPart->velocity + velocityCurve
        + pPart->acceleration * pPart->timeElapsed;
    float distance = mUnidentified010 * velocity;
    nlVec3ScaleAdd(
        pPart->position, distance, pPart->velDir, pPart->position);

    nlVector3 gravity = lbl_804EB340;
    if (pCoordSys != 0)
        nlMultDirVectorMatrix(gravity, gravity, *pCoordSys);

    float gravityDistance
        = pPart->mass * mUnidentified010 * pPart->timeElapsed;
    nlVec3ScaleAdd(
        pPart->position, gravityDistance, gravity, pPart->position);
}

static unsigned long fn_802E2034(const Particle* pPart,
    const EffectsTemplate* pTemplate)
{
    float frame = 24.0f * (pPart->timeElapsed / pPart->lifeSpan);
    int first = (int)floor(frame);
    int second = first + 1;
    nlColour colour;
    if (first >= 24)
    {
        colour = pTemplate->m_cColour[24];
    }
    else
    {
        float fraction = frame - (float)first;
        for (int i = 0; i < 4; ++i)
        {
            colour.c[i] = (unsigned char)(
                fraction * pTemplate->m_cColour[second].c[i]
                + (1.0f - fraction) * pTemplate->m_cColour[first].c[i]);
        }
    }

    float red = colour.c[0] * sfParticleRedScale.value;
    float green = colour.c[1] * sfParticleGreenScale.value;
    float blue = colour.c[2] * sfParticleBlueScale.value;
    if (red < 0.0f)
        red = 0.0f;
    else if (red > 255.0f)
        red = 255.0f;
    if (green < 0.0f)
        green = 0.0f;
    else if (green > 255.0f)
        green = 255.0f;
    if (blue < 0.0f)
        blue = 0.0f;
    else if (blue > 255.0f)
        blue = 255.0f;
    colour.c[0] = (unsigned char)red;
    colour.c[1] = (unsigned char)green;
    colour.c[2] = (unsigned char)blue;
    return *(unsigned long*)&colour;
}

void ParticleSystem::UpdateParticle(ParticleReturn* pReturn,
    Particle* pPart, EffectsTemplate* pTemplate,
    const nlVector3& viewRight, const nlVector3& viewUp,
    const nlMatrix4* pCoordSys)
{
    *(unsigned long*)&pReturn->c = fn_802E2034(pPart, pTemplate);

    float size;
    if (pTemplate->mUnidentified058[1]->mUnidentified000 != 0)
        size = pTemplate->mUnidentified058[1]->fn_802E0010(
            pPart->mUnidentified008);
    else
        size = pPart->size;
    if (pTemplate->mUnidentified058[2]->mUnidentified000 != 0)
        size *= pTemplate->mUnidentified058[2]->fn_802E0010(
            mUnidentified014);
    else
        size *= pPart->mUnidentified040;

    nlVector3 position = pPart->position;
    if (pCoordSys != 0)
    {
        nlVector3 transformed;
        nlMultPosVectorMatrix(transformed, position, *pCoordSys);
        position = transformed;
    }

    if (pTemplate->m_uModelID != 0xFFFFFFFF)
    {
        pReturn->position[0] = position;
        pReturn->position[1].x = size;
        pReturn->position[1].y = pPart->rot;
        return;
    }

    int animFrame
        = (int)(pPart->FPS * pPart->timeElapsed + pPart->frame);
    animFrame %= pTemplate->m_nFrames;
    TextureFrame* frame
        = &textureFrames[pTemplate->m_nFrames - 1][animFrame];
    if (pPart->mUnidentified060)
    {
        pReturn->texcoord[0].x = frame->mUnidentified000;
        pReturn->texcoord[0].y = frame->mUnidentified004;
        pReturn->texcoord[1].x
            = frame->mUnidentified000 + frame->mUnidentified008;
        pReturn->texcoord[1].y = frame->mUnidentified004;
        pReturn->texcoord[2].x = frame->mUnidentified000;
        pReturn->texcoord[2].y
            = frame->mUnidentified004 + frame->mUnidentified008;
        pReturn->texcoord[3].x
            = frame->mUnidentified000 + frame->mUnidentified008;
        pReturn->texcoord[3].y
            = frame->mUnidentified004 + frame->mUnidentified008;
    }
    else
    {
        pReturn->texcoord[0].x
            = frame->mUnidentified000 + frame->mUnidentified008;
        pReturn->texcoord[0].y = frame->mUnidentified004;
        pReturn->texcoord[1].x = frame->mUnidentified000;
        pReturn->texcoord[1].y = frame->mUnidentified004;
        pReturn->texcoord[2].x
            = frame->mUnidentified000 + frame->mUnidentified008;
        pReturn->texcoord[2].y
            = frame->mUnidentified004 + frame->mUnidentified008;
        pReturn->texcoord[3].x = frame->mUnidentified000;
        pReturn->texcoord[3].y
            = frame->mUnidentified004 + frame->mUnidentified008;
    }

    float sn;
    float cs;
    nlSinCos(&sn, &cs,
        (unsigned short)(((int)(65536.0f * pPart->rot)) / 360));
    sn *= 0.5f * size;
    cs *= 0.5f * size;

    nlVector3 a;
    nlVector3 b;
    nlVec3Set(a, cs * viewRight.x + sn * viewUp.x,
        cs * viewRight.y + sn * viewUp.y,
        cs * viewRight.z + sn * viewUp.z);
    nlVec3Set(b, -sn * viewRight.x + cs * viewUp.x,
        -sn * viewRight.y + cs * viewUp.y,
        -sn * viewRight.z + cs * viewUp.z);

    nlVec3Set(pReturn->position[0], position.x + a.x + b.x,
        position.y + a.y + b.y, position.z + a.z + b.z);
    nlVec3Set(pReturn->position[1], position.x - a.x + b.x,
        position.y - a.y + b.y, position.z - a.z + b.z);
    nlVec3Set(pReturn->position[2], position.x - a.x - b.x,
        position.y - a.y - b.y, position.z - a.z - b.z);
    nlVec3Set(pReturn->position[3], position.x + a.x - b.x,
        position.y + a.y - b.y, position.z + a.z - b.z);
}

static void RenderLightOnField(GLView* view, const EffectsLight& light)
{
    if (light.m_fRadius == 0.0f)
        return;
    float heightFrac
        = 1.0f - light.m_v3Position.z / light.m_fRadius;
    if (heightFrac <= 0.0f)
        return;
    if (heightFrac > 1.0f)
        heightFrac = 1.0f;

    glSetDefaultState(true);
    glSetCurrentTexture(glGetTexture("global/light_blob"), GLTT_Diffuse);
    glSetRasterState(GLS_AlphaBlend, 3);
    glSetRasterState(GLS_DepthWrite, 0);
    glSetCurrentRasterState(glHandleizeRasterState());

    float dim = 1.4f * (2.0f * light.m_fRadius)
        * (heightFrac * heightFrac);
    nlMatrix4 mRot;
    mRot.SetIdentity();
    glQuad3 q;
    q.SetupRotatedRectangle(dim, dim, mRot, false, false);
    q.SetColour(light.m_Colour);
    for (int i = 0; i < 4; ++i)
    {
        nlVec3Add(q.m_pos[i], q.m_pos[i], light.m_v3Position);
        q.m_pos[i].z = 0.03125f;
        q.m_colour[i].c[3] /= 3;
    }
    view->AttachModel(q.GetModel(), 0);
}

void ParticleSystem::fn_802E2848()
{
    nlDLListIterator<Particle*> iterator = m_Particles.Begin();
    while (iterator.hasNext())
    {
        Particle* pPart = *iterator;
        m_Particles.Remove(&iterator);
        --mUnidentified0BC;
        mUnidentified0C0->AddEnd(pPart);
    }
}

int ParticleSystem::RenderAllParticles(GLView* view)
{
    static int _tris[6] = { 0, 1, 2, 0, 2, 3 };

    if (!m_bVisible || !sUnidentified_806DF470 || mUnidentified0BC == 0)
        return 0;

    fn_802E9F94(mUnidentified000, mUnidentified0BC);
    if ((unsigned int)sUnidentified_806E1FAC
        > (unsigned int)MaxNumParticles)
        return 0;
    sUnidentified_806E1FAC += mUnidentified0BC;

    nlVector3 viewRight;
    nlVector3 viewUp;
    bool cullBackFaces = true;
    if (m_pTemplate->m_eBillboard == EfBill_Billboard)
    {
        nlMatrix4 viewMatrix;
        view->m_Interface->GetViewMatrix(viewMatrix);
        nlVec3Set(viewRight, viewMatrix.e[0], viewMatrix.e[4], viewMatrix.e[8]);
        nlVec3Set(viewUp, viewMatrix.e[1], viewMatrix.e[5], viewMatrix.e[9]);
        nlVec3Scale(viewRight, m_fAspect);
    }
    else if (m_pTemplate->m_eBillboard == EfBill_Groundboard)
    {
        nlVec3Set(viewRight, 1.0f, 0.0f, 0.0f);
        nlVec3Set(viewUp, 0.0f, 1.0f, 0.0f);
    }
    else
    {
        nlVec3Set(viewRight, 1.0f, 0.0f, 0.0f);
        nlVec3Set(viewUp, 0.0f, 0.0f, 1.0f);
        cullBackFaces = false;
        nlMatrix4 rot;
        nlMakeRotationMatrixZ(rot,
            0.0000958738f * (float)(unsigned short)(m_aFacing + 0x4000));
        nlVector3 rotated;
        nlMultDirVectorMatrix(rotated, viewRight, rot);
        viewRight = rotated;
    }

    glSetDefaultState(true);
    glSetRasterState(GLS_DepthWrite, 0);
    glSetRasterState(GLS_Culling, cullBackFaces ? 1 : 0);
    if (sUnidentified_806E1F99
        || (m_AllowInFront
            && (m_pTemplate->IsInFront()
                || (m_pSpec != 0 && m_pSpec->m_bInFront))))
    {
        glSetRasterState(GLS_DepthTest, 0);
    }
    if (sUnidentified_806E1F98
        || m_pTemplate->m_eBlend == EfBlend_Additive)
        glSetRasterState(GLS_AlphaBlend, 3);
    else if (m_pTemplate->m_eBlend == EfBlend_Normal)
        glSetRasterState(GLS_AlphaBlend, 1);
    glSetRasterState(GLS_AlphaTest, 1);
    glSetCurrentRasterState(glHandleizeRasterState());

    static unsigned long WhiteTexture = glGetTexture("global/white");
    glSetCurrentTexture(
        mUnidentified09C == 0xFFFFFFFF ? WhiteTexture : mUnidentified09C,
        GLTT_Diffuse);

    const nlMatrix4* pCoord
        = m_pTemplate->IsLocalSpace() ? &mUnidentified058 : 0;
    if (m_pSpec != 0 && m_pSpec->m_bLight)
    {
        nlDLListIterator<Particle*> iterator = m_Particles.Begin();
        while (iterator.hasNext())
        {
            Particle* pPart = *iterator;
            iterator.Step();
            EffectsLight light;
            UpdateLight(&light, pPart, m_pTemplate, viewRight, viewUp,
                pCoord);
            EmissionManager::Instance()->AddEffectsLight(light);
            RenderLightOnField(view, light);
        }
    }
    else if (m_pTemplate->m_uModelID != 0xFFFFFFFF)
    {
        glModel* baseModel = lbl_806E1FFC->GetModel(m_pTemplate->m_uModelID);
        nlDLListIterator<Particle*> iterator = m_Particles.Begin();
        while (iterator.hasNext())
        {
            Particle* pPart = *iterator;
            iterator.Step();
            ParticleReturn ret;
            UpdateParticle(&ret, pPart, m_pTemplate, viewRight, viewUp,
                pCoord);
            glModel* pModel = glModelDupNoStreams(baseModel, true, 0);
            nlMatrix4 mRot;
            nlMatrix4 mScale;
            nlMatrix4 m;
            nlMakeRotationMatrixZ(mRot,
                3.1415927f * ret.position[1].y / 180.0f);
            nlMakeScaleMatrix(mScale, ret.position[1].x,
                ret.position[1].x, ret.position[1].x);
            nlMultMatrices(m, mScale, mRot);
            m.SetTranslation(ret.position[0]);
            if (m_pTemplate->IsLit() && m_LightingCallback != 0)
                pModel = m_LightingCallback(pModel);

            unsigned long hMatrix = glAllocMatrix();
            if (hMatrix != 0xFFFFFFFF)
                glSetMatrix(hMatrix, m);

            static unsigned long constantColourHash_806E1FBC
                = nlStringLowerHash("constantcolour");
            glModelPacket* pPacket = pModel->packets;
            while (pPacket < pModel->packets + pModel->numPackets)
            {
                if (fn_802CC8FC(pPacket, constantColourHash_806E1FBC))
                {
                    nlVector4 colour;
                    colour.x = (float)ret.c.c[0] / 255.0f;
                    colour.y = (float)ret.c.c[1] / 255.0f;
                    colour.z = (float)ret.c.c[2] / 255.0f;
                    colour.w = (float)ret.c.c[3] / 255.0f;
                    fn_802CC3C8(pPacket, constantColourHash_806E1FBC,
                        &colour, 4);
                }
                glSetRasterState(pPacket->rasterState, GLS_Culling, 0);
                glSetRasterState(pPacket->rasterState, GLS_AlphaBlend,
                    m_pTemplate->m_eBlend == EfBlend_Additive ? 3 : 1);
                glSetRasterState(pPacket->rasterState, GLS_AlphaTest, 1);
                glSetRasterState(pPacket->rasterState, GLS_AlphaTestRef, 3);
                if (m_pTemplate->IsInFront())
                    glSetRasterState(
                        pPacket->rasterState, GLS_DepthTest, 0);
                pPacket->matrix = hMatrix;
                ++pPacket;
            }
            glModelSetMatrix(pModel, hMatrix);
            view->AttachModel(pModel, m_uLayer + 1);
        }
    }
    else if (m_Callback == 0)
    {
        GLMeshWriter mesh;
        bool bQuads = glHasQuads();
        bool began = mesh.Begin(
            mUnidentified0BC * (bQuads ? 4 : 6),
            bQuads ? GLP_QuadList : GLP_TriList, 0);
        if (began)
        {
            nlDLListIterator<Particle*> iterator = m_Particles.Begin();
            while (iterator.hasNext())
            {
                Particle* pPart = *iterator;
                iterator.Step();
                ParticleReturn ret;
                UpdateParticle(&ret, pPart, m_pTemplate, viewRight,
                    viewUp, pCoord);
                int count = bQuads ? 4 : 6;
                for (int i = 0; i < count; ++i)
                {
                    int index = bQuads ? i : _tris[i];
                    mesh.Texcoord(ret.texcoord[index]);
                    mesh.Colour(ret.c);
                    mesh.Vertex(ret.position[index]);
                }
            }
            if (mesh.End())
                view->AttachModel(mesh.GetModel(), m_uLayer);
            else
                fn_8004F594(3,
                    "couldn't end mesh built by sprites\n");
        }
        else
        {
            fn_8004F594(3,
                "could not begin a mesh for sprites\n");
        }
    }
    else if (!m_Callback(this, view, &m_Particles, viewRight, viewUp,
                 pCoord))
    {
        fn_8004F594(3,
            "too many particles for the fast-path\n");
    }

    return mUnidentified0BC;
}

void ParticleSystem::Die()
{
    m_fDelay = 0.0f;
    m_fElapsedTime = 100000000000000000000.0f;
    m_bAmDying = true;
}

bool ParticleSystem::Update(float dt)
{
    if (m_fDelay > 0.0f)
    {
        m_fDelay -= dt;
        if (m_fDelay < 0.0f)
            m_fDelay = 0.0f;
        return true;
    }

    mUnidentified010 = dt;
    m_fElapsedTime += dt;
    if (m_pSpec != 0 && m_pSpec->m_fLingerEnd >= 0.0f
        && !m_bAmDying && m_fElapsedTime > m_pSpec->m_fLingerEnd)
    {
        m_fElapsedTime = m_pSpec->m_fLingerStart;
    }

    if (m_pTemplate->m_fFountainLife <= 0.0f)
    {
        if (!m_bAmDying && m_Particles.m_Head == 0)
        {
            if (m_pSpec == 0 || m_pSpec->m_fLingerStart < 0.0f)
                m_bAmDying = true;
            m_fNumParticlesToCreate
                += m_pTemplate->mUnidentified058[0]->fn_802E0010(0.0f);
        }
    }
    else
    {
        if (m_fElapsedTime >= m_pTemplate->m_fFountainLife)
        {
            m_bAmDying = true;
        }
        mUnidentified014
            = m_fElapsedTime / m_pTemplate->m_fFountainLife;
        if (mUnidentified014 > 1.0f)
            mUnidentified014 = 1.0f;
        if (m_fElapsedTime < m_pTemplate->m_fFountainLife)
        {
            m_fNumParticlesToCreate += dt
                * m_pTemplate->mUnidentified058[0]->fn_802E0010(
                    mUnidentified014);
        }
    }

    int numParticles = (int)m_fNumParticlesToCreate;
    m_fNumParticlesToCreate -= (float)numParticles;
    if (m_fNumParticlesToCreate < 0.0f)
        m_fNumParticlesToCreate = 0.0f;
    if (numParticles > 0)
        CreateNewParticles(numParticles);

    UpdateAllParticles(dt,
        m_pTemplate->IsLocalSpace() ? &mUnidentified058 : 0);
    if (m_bAmDying && m_Particles.m_Head == 0)
        return false;
    return true;
}

float ParticleSystem::GetRemainingTime() const
{
    return m_pTemplate->m_fFountainLife - m_fElapsedTime;
}

static TextureFrame* BuildFrameLookup(int numFrames, float inc)
{
    TextureFrame* p = (TextureFrame*)nlMalloc(
        numFrames * sizeof(TextureFrame), 8, false);
    TextureFrame* q = p;
    float u = 0.0f;
    float v = 0.0f;
    for (int i = 0; i < numFrames; ++i, ++q)
    {
        q->mUnidentified000 = u;
        q->mUnidentified004 = v;
        q->mUnidentified008 = inc;
        u += inc;
        if (u >= 0.999f)
        {
            u = 0.0f;
            v += inc;
        }
    }
    return p;
}

bool fxParticleStartup(int maxNumParticles)
{
    textureFrames[0]
        = (TextureFrame*)nlMalloc(sizeof(TextureFrame), 8, false);
    textureFrames[0]->mUnidentified000 = 0.0f;
    textureFrames[0]->mUnidentified004 = 0.0f;
    textureFrames[0]->mUnidentified008 = 1.0f;
    textureFrames[3] = BuildFrameLookup(4, 0.5f);
    textureFrames[8] = BuildFrameLookup(9, 1.0f / 3.0f);
    textureFrames[15] = BuildFrameLookup(16, 0.25f);
    textureFrames[24] = BuildFrameLookup(25, 0.2f);
    textureFrames[35] = BuildFrameLookup(36, 1.0f / 6.0f);
    if (MaxNumParticles == 0)
        MaxNumParticles = maxNumParticles;
    return true;
}

bool fxParticleShutdown()
{
    for (int i = 0; i < 36; ++i)
    {
        if (textureFrames[i] != 0)
        {
            delete[] (unsigned char*)textureFrames[i];
            textureFrames[i] = 0;
        }
    }
    return true;
}

void fn_802E3AC0(int maxNumParticles)
{
    MaxNumParticles = maxNumParticles;
}

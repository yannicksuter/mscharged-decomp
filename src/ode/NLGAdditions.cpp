#include <ode/NLGAdditions.h>
#include <ode/matrix.h>

#include "collision_kernel.h"
#include "joint.h"
#include "objects.h"

extern dxJoint* createJoint(dWorldID w, dJointGroupID group, dxJoint::Vtable* vtable);

struct dxJointCharacter : public dxJoint
{
    float direction[4];
    unsigned int constraintRows;
};

enum
{
    dJointTypeCharacter = 10,
};

/**
 * Offset/Address/Size: 0x0 | 0x80358E7C | size: 0x34
 */
void dBodySetUpdateMode(dxBody* body, int arg1, int arg2)
{
    body->flags &= 0xFFFFFF9F;
    if (arg1 == 0)
    {
        body->flags |= 0x20;
    }
    if (arg2 == 0)
    {
        body->flags |= 0x40;
    }
}

/**
 * Offset/Address/Size: 0x34 | 0x80358EB0 | size: 0x8
 */
dxBody* dWorldGetFirstBody(dxWorld* world)
{
    return world->firstbody;
}

/**
 * Offset/Address/Size: 0x3C | 0x80358EB8 | size: 0x8
 */
dxBody* dBodyGetNextBody(dxBody* body)
{
    return (dxBody*)body->next;
}

/**
 * Offset/Address/Size: 0x44 | 0x80358EC0 | size: 0x8
 */
void dWorldSetClearAccumulators(dxWorld* world, int flags)
{
    world->clear_accumulators = flags;
}

/**
 * Offset/Address/Size: 0x4C | 0x80358EC8 | size: 0x4
 */
void dClearCachedData()
{
}

/**
 * Offset/Address/Size: 0x50 | 0x80358ECC | size: 0x1C
 */
static void characterInit(dxJointCharacter* character)
{
    character->constraintRows = 3;
    character->direction[0] = 0.f;
    character->direction[1] = 0.f;
    character->direction[2] = 0.f;
}

/**
 * Offset/Address/Size: 0x6C | 0x80358EE8 | size: 0x14
 */
static void characterGetInfo1(dxJointCharacter* joint, dxJoint::Info1* info)
{
    info->m = joint->constraintRows;
    info->nub = 0;
}

static inline dReal nlgDot(const dReal* a, const dReal* b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

/**
 * Offset/Address/Size: 0x80 | 0x80358EFC | size: 0x154
 */
static void characterGetInfo2(dxJointCharacter* j, dxJoint::Info2* info)
{
    // Row 0: angular identity
    info->J1a[0] = 1;
    info->c[0] = 0;
    info->lo[0] = -3.402823466e+38F;
    info->hi[0] = 3.402823466e+38F;

    // Row 1
    {
        int row = info->rowskip + 1;
        info->J1a[row] = 1;
    }
    info->c[1] = 0;
    info->lo[1] = -3.402823466e+38F;
    info->hi[1] = 3.402823466e+38F;

    // Row 2
    {
        int row = (info->rowskip + 1) * 2;
        info->J1a[row] = 1;
    }
    info->c[2] = 0;
    info->lo[2] = -3.402823466e+38F;
    info->hi[2] = 3.402823466e+38F;

    unsigned int jStart = info->rowskip * 3;
    if (j->constraintRows == 4)
    {
        // Row 3: linear Jacobian = body R * direction
        dxBody* body = ((dxJoint*)j)->node[0].body;
        dReal* R = body->R;
        dVector3 v;
        v[0] = nlgDot(R, j->direction);
        v[1] = nlgDot(R + 4, j->direction);
        v[2] = nlgDot(R + 8, j->direction);

        info->J1l[jStart] = v[0];
        {
            unsigned int idx = jStart + 1;
            info->J1l[idx] = v[1];
        }
        {
            unsigned int idx = jStart + 2;
            info->J1l[idx] = v[2];
        }

        info->c[3] = 0;
        info->lo[3] = -3.402823466e+38F;
        info->hi[3] = 3.402823466e+38F;
    }
}

dxJoint::Vtable __dcharacter_vtable = {
    sizeof(dxJointCharacter),
    (dxJoint::init_fn*)characterInit,
    (dxJoint::getInfo1_fn*)characterGetInfo1,
    (dxJoint::getInfo2_fn*)characterGetInfo2,
    dJointTypeCharacter
};

/**
 * Offset/Address/Size: 0x1D4 | 0x80359050 | size: 0xC
 */
dxJoint* dJointCreateCharacter(dxWorld* world, dxJointGroup* jointGroup)
{
    return createJoint(world, jointGroup, &__dcharacter_vtable);
}

/**
 * Offset/Address/Size: 0x1E0 | 0x8035905C | size: 0x24
 */
void dJointSetCharacterNoMotionDirection(dxJoint* joint, float* v3)
{
    dxJointCharacter* c = (dxJointCharacter*)joint;
    c->direction[0] = v3[0];
    c->direction[1] = v3[1];
    c->direction[2] = v3[2];
    c->constraintRows = 4;
}

/**
 * Offset/Address/Size: 0x204 | 0x80359080 | size: 0x34
 */
void dGeomGetPosition(dxGeom* geom, float* position)
{
    position[0] = geom->pos[0];
    position[1] = geom->pos[1];
    position[2] = geom->pos[2];
    position[3] = geom->pos[3];
}

/**
 * Offset/Address/Size: 0x238 | 0x803590B4 | size: 0x94
 */
void dGeomGetRotation(dxGeom* geom, float* rotation)
{
    for (int i = 0; i < 12; i++)
    {
        rotation[i] = geom->R[i];
    }
}

/**
 * Offset/Address/Size: 0x2CC | 0x80359148 | size: 0xC
 */
int dGeomIsPlaceable(dxGeom* geom)
{
    return (geom->gflags & GEOM_PLACEABLE) != 0;
}

/**
 * Offset/Address/Size: 0x2D8 | 0x80359154 | size: 0x8
 */
int dGeomGetGFlags(dxGeom* geom)
{
    return geom->gflags;
}

/**
 * Offset/Address/Size: 0x2E0 | 0x8035915C | size: 0x8
 */
void dGeomSetGFlags(dxGeom* geom, int flags)
{
    geom->gflags = flags;
}

/**
 * Offset/Address/Size: 0x2E8 | 0x80359164 | size: 0x88
 */
void dMultiplyMatrix3Vector3(float* result, const float* R, const float* v, bool transposeMatrix)
{
    if (!transposeMatrix)
    {
        dMultiply0(result, R, v, 3, 3, 1);
    }
    if (transposeMatrix)
    {
        dMultiply1(result, R, v, 3, 3, 1);
    }
}

/**
 * Offset/Address/Size: 0x370 | 0x803591EC | size: 0x10
 */
void dMultiplyMatrix4Vector4(float* m1, const float* m2, const float* m3)
{
    dMultiply0(m1, m2, m3, 4, 4, 1);
}

/**
 * Offset/Address/Size: 0x380 | 0x803591FC | size: 0xDC
 */
void dInvertRigidTransformation(dReal* TInv, const dReal* R, const dReal* p)
{
    dVector3 temp;

    TInv[0] = R[0];
    TInv[1] = R[4];
    TInv[2] = R[8];
    TInv[4] = R[1];
    TInv[5] = R[5];
    TInv[6] = R[9];
    TInv[8] = R[2];
    TInv[9] = R[6];
    TInv[10] = R[10];

    dMultiply0(temp, TInv, p, 3, 3, 1);

    temp[0] *= REAL(-1.0);
    temp[1] *= REAL(-1.0);
    temp[2] *= REAL(-1.0);

    TInv[3] = temp[0];
    TInv[7] = temp[1];
    TInv[11] = temp[2];
    TInv[12] = REAL(0.0);
    TInv[13] = REAL(0.0);
    TInv[14] = REAL(0.0);
    TInv[15] = REAL(1.0);
}

/**
 * Offset/Address/Size: 0x45C | 0x803592D8 | size: 0x30
 */
void dExtractColumn3(float* __restrict arg0, const float* __restrict arg1, int col)
{
    arg0[0] = arg1[col];
    arg0[1] = arg1[col + 4];
    arg0[2] = arg1[col + 8];
}

/**
 * Offset/Address/Size: 0x48C | 0x80359308 | size: 0x10
 */
void dVector3Set(float* v, float x, float y, float z)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
}

/**
 * Offset/Address/Size: 0x49C | 0x80359318 | size: 0x14
 */
void dVector4Set(float* v, float x, float y, float z, float w)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
    v[3] = w;
}

/**
 * Offset/Address/Size: 0x4B0 | 0x8035932C | size: 0x28
 */
void dVectorScale(float* v1, float arg8)
{
    v1[0] *= arg8;
    v1[1] *= arg8;
    v1[2] *= arg8;
}

/**
 * Offset/Address/Size: 0x4D8 | 0x80359354 | size: 0x34
 */
void dVector3Add(float* __restrict v1, const float* __restrict v2)
{
    v1[0] += v2[0];
    v1[1] += v2[1];
    v1[2] += v2[2];
}

/**
 * Offset/Address/Size: 0x50C | 0x80359388 | size: 0x44
 */
void dGeomComputeAABB(dxGeom* geomID)
{
    geomID->computeAABB();
    geomID->gflags &= 0xFFFFFFFC;
}

/**
 * Offset/Address/Size: 0x550 | 0x803593CC | size: 0x10
 */
void dGeomMarkAABBAsValid(dxGeom* geomID)
{
    geomID->gflags = geomID->gflags & 0xFFFFFFFC;
}

/**
 * Offset/Address/Size: 0x560 | 0x803593DC | size: 0x4
 */
static void collideAABBs(dxGeom*, dxGeom*, void*, dNearCallback*);

void dGeomCollideAABBs(dxGeom* arg0, dxGeom* arg1, void* arg2,
    void (*arg3)(void*, dxGeom*, dxGeom*))
{
    collideAABBs(arg0, arg1, arg2, arg3);
}


/**
 * Offset/Address/Size: 0x564 | 0x803593E0 | size: 0x14C
 */
static void collideAABBs(dxGeom* g1, dxGeom* g2, void* data,
    dNearCallback* callback)
{
    dIASSERT((g1->gflags & GEOM_AABB_BAD) == 0);
    dIASSERT((g2->gflags & GEOM_AABB_BAD) == 0);

    if (g1->body == g2->body && g1->body)
        return;

    if (((g1->category_bits & g2->collide_bits)
            || (g2->category_bits & g1->collide_bits))
        == 0)
    {
        return;
    }

    dReal* bounds1 = g1->aabb;
    dReal* bounds2 = g2->aabb;
    if (bounds1[0] > bounds2[1] || bounds1[1] < bounds2[0]
        || bounds1[2] > bounds2[3] || bounds1[3] < bounds2[2]
        || bounds1[4] > bounds2[5] || bounds1[5] < bounds2[4])
    {
        return;
    }

    if (g1->AABBTest(g2, bounds2) == 0)
        return;
    if (g2->AABBTest(g1, bounds1) == 0)
        return;

    callback(data, g1, g2);
}

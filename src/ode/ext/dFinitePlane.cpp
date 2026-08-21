#include <ode/NLGAdditions.h>
#include <ode/ext/dFinitePlane.h>

static inline double fabs(double value)
{
    return __fabs(value);
}

int dFinitePlaneClassUser = -1;

struct FinitePlane
{
    float a;
    float b;
    float c;
    float d;
    bool flag;
    float param;
};

/**
 * Offset/Address/Size: 0x0 | 0x80351F34 | size: 0x3B8
 */
void dFinitePlaneAABB(dxGeom* geomID, float* aabb)
{
    dVector3 p[4];
    dVector3 x;

    float plane_a;
    float plane_b;
    float plane_c;
    float plane_d;
    unsigned int plane_flag;
    float plane_param;
    float* rotation;
    float* position;
    FinitePlane* planeData;
    int i;

    planeData = (FinitePlane*)dGeomGetClassData(geomID);
    plane_a = planeData->a;
    plane_b = planeData->b;
    plane_c = planeData->c;
    plane_d = planeData->d;
    plane_flag = planeData->flag;
    plane_param = planeData->param;
    rotation = (float*)dGeomGetRotation(geomID);
    position = (float*)dGeomGetPosition(geomID);

    dVector3Set(x, plane_a, plane_c, 0.0f);
    dMultiplyMatrix3Vector3(p[0], rotation, x, 0);
    dVector3Add(p[0], position);

    dVector3Set(x, plane_a, plane_d, 0.0f);
    dMultiplyMatrix3Vector3(p[1], rotation, x, 0);
    dVector3Add(p[1], position);

    dVector3Set(x, plane_b, plane_d, 0.0f);
    dMultiplyMatrix3Vector3(p[2], rotation, x, 0);
    dVector3Add(p[2], position);

    dVector3Set(x, plane_b, plane_c, 0.0f);
    dMultiplyMatrix3Vector3(p[3], rotation, x, 0);
    dVector3Add(p[3], position);

    aabb[0] = p[0][0];
    aabb[1] = p[0][0];
    aabb[2] = p[0][1];
    aabb[3] = p[0][1];
    aabb[4] = p[0][2];
    aabb[5] = p[0][2];

    for (i = 1; i < 4; i++)
    {
        if (p[i][0] < aabb[0])
            aabb[0] = p[i][0];
        if (p[i][0] > aabb[1])
            aabb[1] = p[i][0];
        if (p[i][1] < aabb[2])
            aabb[2] = p[i][1];
        if (p[i][1] > aabb[3])
            aabb[3] = p[i][1];
        if (p[i][2] < aabb[4])
            aabb[4] = p[i][2];
        if (p[i][2] > aabb[5])
            aabb[5] = p[i][2];
    }

    if (plane_flag != 0)
    {
        dVector3Set(x, plane_a, plane_c, -plane_param);
        dMultiplyMatrix3Vector3(p[0], rotation, x, 0);
        dVector3Add(p[0], position);

        dVector3Set(x, plane_a, plane_d, -plane_param);
        dMultiplyMatrix3Vector3(p[1], rotation, x, 0);
        dVector3Add(p[1], position);

        dVector3Set(x, plane_b, plane_d, -plane_param);
        dMultiplyMatrix3Vector3(p[2], rotation, x, 0);
        dVector3Add(p[2], position);

        dVector3Set(x, plane_b, plane_c, -plane_param);
        dMultiplyMatrix3Vector3(p[3], rotation, x, 0);
        dVector3Add(p[3], position);

        for (i = 1; i < 4; i++)
        {
            if (p[i][0] < aabb[0])
                aabb[0] = p[i][0];
            if (p[i][0] > aabb[1])
                aabb[1] = p[i][0];
            if (p[i][1] < aabb[2])
                aabb[2] = p[i][1];
            if (p[i][1] > aabb[3])
                aabb[3] = p[i][1];
            if (p[i][2] < aabb[4])
                aabb[4] = p[i][2];
            if (p[i][2] > aabb[5])
                aabb[5] = p[i][2];
        }
    }
}

/**
 * Offset/Address/Size: 0x3B8 | 0x803522EC | size: 0x214
 */
int dCollideFinitePlaneSphere(dxGeom* planeGeomID, dxGeom* sphereGeomID, int, dContactGeom* contact, int)
{
    float sp28[16];
    dVector3 sp18;
    dVector3 sp8;
    float plane_a;
    float plane_b;
    float plane_c;
    float plane_d;
    float radius;
    float plane_param;
    float var_f25;
    unsigned int plane_flag;
    float* plane_pos;
    float* plane_rot;
    float* sphere_pos;
    FinitePlane* planeData;

    planeData = (FinitePlane*)dGeomGetClassData(planeGeomID);
    plane_a = planeData->a;
    plane_b = planeData->b;
    plane_c = planeData->c;
    plane_d = planeData->d;
    plane_flag = planeData->flag;
    plane_param = planeData->param;
    radius = dGeomSphereGetRadius(sphereGeomID);
    plane_pos = (float*)dGeomGetPosition(planeGeomID);
    sphere_pos = (float*)dGeomGetPosition(sphereGeomID);
    plane_rot = (float*)dGeomGetRotation(planeGeomID);

    dInvertRigidTransformation(&sp28[0], plane_rot, plane_pos);
    dVector4Set(sp18, sphere_pos[0], sphere_pos[1], sphere_pos[2], 1.0f);
    dMultiplyMatrix4Vector4(&sp8[0], &sp28[0], sp18);

    if (plane_flag != 0)
    {
        var_f25 = sp8[2];
    }
    else
    {
        var_f25 = (float)fabs(sp8[2]);
    }

    if ((var_f25 < radius) && (sp8[0] > plane_a) && (sp8[0] < plane_b) && (sp8[1] > plane_c) && (sp8[1] < plane_d))
    {
        dExtractColumn3(contact->normal, plane_rot, 2);
        if ((sp8[2] > 0.0f) || (plane_flag != 0))
        {
            dVectorScale(contact->normal, -1.0f);
        }

        contact->pos[0] = (float)((contact->normal[0] * radius) + sphere_pos[0]);
        contact->pos[1] = (float)((contact->normal[1] * radius) + sphere_pos[1]);
        contact->pos[2] = (float)((contact->normal[2] * radius) + sphere_pos[2]);
        contact->depth = (float)(radius - var_f25);

        if ((plane_param != -1.0f) && (contact->depth > plane_param))
        {
            return 0;
        }

        contact->g1 = planeGeomID;
        contact->g2 = sphereGeomID;
        return 1;
    }

    return 0;
}

/**
 * Offset/Address/Size: 0x5CC | 0x80352500 | size: 0x18
 */
static dColliderFn* dFinitePlaneColliderFn(int num)
{
    if (num == 0)
    {
        return &dCollideFinitePlaneSphere;
    }
    return 0;
}

/**
 * Offset/Address/Size: 0x5E4 | 0x80352518 | size: 0xE8
 */
dGeomID dCreateFinitePlane(dSpaceID space, dReal a, dReal b, dReal c, dReal d, bool flag, float param)
{
    FinitePlane* planeData;
    dGeomClass geomClass;
    dGeomID geomID;

    if ((int)dFinitePlaneClassUser == -1)
    {
        geomClass.bytes = 0x18;
        geomClass.collider = &dFinitePlaneColliderFn;
        geomClass.aabb = &dFinitePlaneAABB;
        geomClass.aabb_test = 0;
        geomClass.dtor = 0;
        dFinitePlaneClassUser = dCreateGeomClass(&geomClass);
    }

    geomID = dCreateGeom(dFinitePlaneClassUser);
    if (space != 0U)
    {
        dSpaceAdd(space, geomID);
    }

    planeData = (FinitePlane*)dGeomGetClassData(geomID);
    planeData->a = a;
    planeData->b = b;
    planeData->c = c;
    planeData->d = d;
    planeData->flag = flag;
    planeData->param = param;

    return geomID;
}

#include <ode/ext/dRoundedCorner.h>
#include <ode/ext/dColumn.h>

extern "C" double sqrt(double);
extern int dColumnClassUser;

int dRoundedCornerClassUser = -1;

struct RoundedCornerData
{
    float param;
    bool flag1;
    bool flag2;
};

static void dRoundedCornerAABB(dGeomID geomID, float* aabb)
{
    float radius;
    float minX;
    float maxX;
    float minY;
    float maxY;
    unsigned int extendPosX;
    unsigned int extendPosY;
    RoundedCornerData* data;
    const float* position;

    data = (RoundedCornerData*)dGeomGetClassData(geomID);
    radius = data->param;
    extendPosX = data->flag1;
    extendPosY = data->flag2;
    position = dGeomGetPosition(geomID);
    minX = position[0];
    minY = position[1];
    maxX = minX;
    maxY = minY;

    if (extendPosX != 0)
    {
        maxX += radius;
    }
    else
    {
        minX -= radius;
    }

    if (extendPosY != 0)
    {
        maxY += radius;
    }
    else
    {
        minY -= radius;
    }

    aabb[0] = minX;
    aabb[1] = maxX;
    aabb[2] = minY;
    aabb[3] = maxY;
    aabb[4] = -3.402823466e+38f;
    aabb[5] = 3.402823466e+38f;
}

static int dCollideRoundedCorner(dGeomID cornerGeomID, dGeomID otherGeomID,
    bool flag1, bool flag2, float cornerRadius, float otherRadius,
    const float* cornerPosition, const float* otherPosition, dContactGeom* contact)
{
    float distanceSquared;
    float deltaX;
    float deltaY;
    float normalY;
    float innerRadius;
    float normalX;
    float distance;
    int result;
    bool xCheck;
    bool yCheck;

    deltaX = otherPosition[0] - cornerPosition[0];
    deltaY = otherPosition[1] - cornerPosition[1];

    xCheck = deltaX > 0.0f;
    yCheck = deltaY > 0.0f;
    if (xCheck != flag1 || yCheck != flag2)
    {
        result = 0;
    }
    else
    {
        distanceSquared = deltaX * deltaX + deltaY * deltaY;
        innerRadius = cornerRadius - otherRadius;
        if (distanceSquared >= innerRadius * innerRadius)
        {
            distance = (float)sqrt(distanceSquared);
            normalX = deltaX / distance;
            result = 1;
            normalY = deltaY / distance;

            contact->pos[0] = cornerRadius * normalX + cornerPosition[0];
            contact->pos[1] = cornerRadius * normalY + cornerPosition[1];
            contact->pos[2] = otherPosition[2];
            contact->normal[0] = normalX;
            contact->normal[1] = normalY;
            contact->normal[2] = 0.0f;
            contact->depth = distance - innerRadius;
            contact->g1 = cornerGeomID;
            contact->g2 = otherGeomID;
        }
        else
        {
            result = 0;
        }
    }
    return result;
}

int dCollideRoundedCornerSphere(dGeomID cornerGeomID, dGeomID sphereGeomID, int,
    dContactGeom* contact, int)
{
    RoundedCornerData* data = (RoundedCornerData*)dGeomGetClassData(cornerGeomID);
    float cornerRadius = data->param;
    bool flag1 = data->flag1;
    bool flag2 = data->flag2;
    float sphereRadius = dGeomSphereGetRadius(sphereGeomID);
    const float* cornerPosition = dGeomGetPosition(cornerGeomID);
    const float* spherePosition = dGeomGetPosition(sphereGeomID);

    return dCollideRoundedCorner(cornerGeomID, sphereGeomID, flag1, flag2, cornerRadius, sphereRadius, cornerPosition, spherePosition, contact);
}

int dCollideRoundedCornerColumn(dGeomID cornerGeomID, dGeomID columnGeomID, int,
    dContactGeom* contact, int)
{
    RoundedCornerData* data = (RoundedCornerData*)dGeomGetClassData(cornerGeomID);
    float cornerRadius = data->param;
    bool flag1 = data->flag1;
    bool flag2 = data->flag2;
    float columnRadius;
    dGeomColumnGetParams(columnGeomID, &columnRadius);
    const float* cornerPosition = dGeomGetPosition(cornerGeomID);
    const float* columnPosition = dGeomGetPosition(columnGeomID);

    return dCollideRoundedCorner(cornerGeomID, columnGeomID, flag1, flag2, cornerRadius, columnRadius, cornerPosition, columnPosition, contact);
}

static dColliderFn* dRoundedCornerColliderFn(int num)
{
    if (num == 0)
    {
        return &dCollideRoundedCornerSphere;
    }

    if (num == dColumnClassUser)
    {
        return &dCollideRoundedCornerColumn;
    }

    return 0;
}

dGeomID dCreateRoundedCorner(dSpaceID space, float param, bool flag1, bool flag2)
{
    if (dRoundedCornerClassUser == -1)
    {
        dGeomClass geomClass;
        geomClass.bytes = sizeof(RoundedCornerData);
        geomClass.collider = &dRoundedCornerColliderFn;
        geomClass.aabb = &dRoundedCornerAABB;
        geomClass.aabb_test = 0;
        geomClass.dtor = 0;
        dRoundedCornerClassUser = dCreateGeomClass(&geomClass);
    }

    dGeomID geomID = dCreateGeom(dRoundedCornerClassUser);
    if (space != 0)
    {
        dSpaceAdd(space, geomID);
    }

    RoundedCornerData* data = (RoundedCornerData*)dGeomGetClassData(geomID);
    data->param = param;
    data->flag1 = flag1;
    data->flag2 = flag2;
    return geomID;
}

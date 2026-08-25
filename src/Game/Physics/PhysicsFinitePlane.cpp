#include "Game/Physics/PhysicsFinitePlane.h"

#include "Game/Physics/CollisionSpace.h"
#include "ode/ext/dFinitePlane.h"

PhysicsFinitePlane::PhysicsFinitePlane(
    CollisionSpace* collision_space, nlVector3& centre, nlVector3& v1, nlVector3& v2,
    bool isOneSided, float errorCorrectionDepth)
    : PhysicsObject(0)
{
    mErrorCorrectionDepth = errorCorrectionDepth;

    xMin = 0.0f;
    xMax = 0.0f;
    yMin = 0.0f;
    yMax = 0.0f;

    xMax = nlSqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z, true);
    yMax = nlSqrt(v2.x * v2.x + v2.y * v2.y + v2.z * v2.z, true);

    xMin = -xMax;
    yMin = -yMax;

    const float l = 1.0f / xMax;
    nlVec3Set(v1, l * v1.x, l * v1.y, l * v1.z);

    const float l2 = 1.0f / yMax;
    const float z2 = l2 * v2.z;
    const float y2 = l2 * v2.y;
    const float x2 = l2 * v2.x;
    nlVec3Set(v2, x2, y2, z2);

    nlMatrix3 R;
    nlVector3 normal;
    normal.x = (v1.y * v2.z) - (v1.z * v2.y);
    normal.y = (-v1.x * v2.z) + (v1.z * v2.x);
    normal.z = (v1.x * v2.y) - (v1.y * v2.x);
    R.e[0] = v1.x;
    R.e[1] = v1.y;
    R.e[2] = v1.z;
    R.e[3] = v2.x;
    R.e[4] = v2.y;
    R.e[5] = v2.z;
    R.e[6] = normal.x;
    R.e[7] = normal.y;
    R.e[8] = normal.z;

    dSpaceID space = 0;
    if (collision_space != 0)
    {
        space = collision_space->m_spaceID;
    }

    m_geomID = dCreateFinitePlane(
        space, xMin, xMax, yMin, yMax, isOneSided, errorCorrectionDepth);
    dGeomSetData(m_geomID, this);
    SetRotation(R);
    SetPosition(centre, WORLD_COORDINATES);
    SetDefaultCollideBits();
}

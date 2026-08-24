/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001,2002 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or       *
 *       (at your option) any later version. The text of the GNU Lesser   *
 *       General Public License is included with this library in the      *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in      *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/

#include <ode/ext/dColumn.h>
#include <ode/odemath.h>

extern "C" double sqrt(double);

static inline double odeFabs(double value)
{
    return __fabs(value);
}

static inline float odeFabsf(float value)
{
    return (float)odeFabs((double)value);
}

#define dFabs(x) ((float)odeFabsf(float(x)))

static unsigned char lengthwiseAxis = 2;
int dColumnClassUser = -1;

static int dCollideColumnRadius(dGeomID column, dGeomID other,
    dContactGeom* contact, float otherRadius, float lengthwisePosition);

void dGeomColumnGetParams(dGeomID geomID, float* radius)
{
    *radius = *(float*)dGeomGetClassData(geomID);
}

void dGeomColumnSetParams(dGeomID geomID, float radius)
{
    *(float*)dGeomGetClassData(geomID) = radius;
}

static void dColumnAABB(dGeomID geomID, float* aabb)
{
    float radius = *(float*)dGeomGetClassData(geomID);
    const float* position = dGeomGetPosition(geomID);

    aabb[0] = position[0] - radius;
    aabb[1] = position[0] + radius;
    aabb[2] = position[1] - radius;
    aabb[3] = position[1] + radius;
    aabb[4] = -3.402823466e+38f;
    aabb[5] = 3.402823466e+38f;
}

int dCollideColumnColumn(dGeomID column, dGeomID other, int,
    dContactGeom* contact, int)
{
    float otherRadius = *(float*)dGeomGetClassData(other);
    return dCollideColumnRadius(column, other, contact, otherRadius, 0.0f);
}

int dCollideColumnSphere(dGeomID column, dGeomID sphere, int,
    dContactGeom* contact, int)
{
    float sphereRadius = dGeomSphereGetRadius(sphere);
    float lengthwisePosition = dGeomGetPosition(sphere)[lengthwiseAxis];
    return dCollideColumnRadius(
        column, sphere, contact, sphereRadius, lengthwisePosition);
}

static int dCollideColumnRadius(dGeomID column, dGeomID other,
    dContactGeom* contact, float otherRadius, float lengthwisePosition)
{
    float columnRadius = *(float*)dGeomGetClassData(column);
    const float* columnPosition = dGeomGetPosition(column);
    const float* otherPosition = dGeomGetPosition(other);
    unsigned char axis = lengthwiseAxis;
    unsigned char axis1 = 0xFF;
    unsigned char axis2 = 0xFF;

    if (axis == 0)
    {
        axis1 = 1;
        axis2 = 2;
    }
    else if (axis == 1)
    {
        axis1 = 0;
        axis2 = 2;
    }
    else if (axis == 2)
    {
        axis1 = 0;
        axis2 = 1;
    }

    float totalRadiusSquared = columnRadius + otherRadius;
    totalRadiusSquared *= totalRadiusSquared;
    float distanceSquared = (columnPosition[axis1] - otherPosition[axis1])
                              * (columnPosition[axis1] - otherPosition[axis1])
                          + (columnPosition[axis2] - otherPosition[axis2])
                                * (columnPosition[axis2] - otherPosition[axis2]);

    if (distanceSquared < totalRadiusSquared)
    {
        float distance = (float)sqrt(distanceSquared);
        if (distance > 0.0001f)
        {
            contact->normal[axis1] = (columnPosition[axis1] - otherPosition[axis1]) / distance;
            contact->normal[axis2] = (columnPosition[axis2] - otherPosition[axis2]) / distance;
            contact->normal[axis] = 0.0f;
        }
        else
        {
            contact->normal[axis1] = 1.0f;
            contact->normal[axis2] = 0.0f;
            contact->normal[axis] = 0.0f;
        }

        float halfOverlap = ((distance + otherRadius) - columnRadius) / 2.0f;
        contact->pos[axis1] = contact->normal[axis1] * halfOverlap + otherPosition[axis1];
        contact->pos[axis2] = contact->normal[axis2] * halfOverlap + otherPosition[axis2];
        contact->pos[axis] = lengthwisePosition;
        contact->depth = otherRadius + columnRadius - distance;
        contact->g1 = column;
        contact->g2 = other;
        return 1;
    }

    return 0;
}

int dCollideColumnPlane(dGeomID column, dGeomID plane, int,
    dContactGeom* contact, int)
{
    dVector4 normal;
    float radius = *(float*)dGeomGetClassData(column);
    dGeomPlaneGetParams(plane, normal);
    float planeDistance = normal[3];
    const float* position = dGeomGetPosition(column);
    unsigned char axis1 = 0xFF;
    unsigned char axis2 = 0xFF;
    int perpendicular = 0;

    if (lengthwiseAxis == 0)
    {
        axis1 = 1;
        axis2 = 2;
        if (normal[0] < 0.001f)
            perpendicular = 1;
    }
    else if (lengthwiseAxis == 1)
    {
        axis1 = 0;
        axis2 = 2;
        if (normal[1] < 0.001f)
            perpendicular = 1;
    }
    else if (lengthwiseAxis == 2)
    {
        axis1 = 0;
        axis2 = 1;
        if (normal[2] < 0.001f)
            perpendicular = 1;
    }

    if (!perpendicular)
        return 0;

    float distance = normal[axis1] * position[axis1]
                   + normal[axis2] * position[axis2] - planeDistance;
    if (distance < radius)
    {
        contact->normal[0] = normal[0];
        contact->normal[1] = normal[1];
        contact->normal[2] = normal[2];
        contact->pos[axis1] = position[axis1] - contact->normal[axis1] * radius;
        contact->pos[axis2] = position[axis2] - contact->normal[axis2] * radius;
        contact->pos[lengthwiseAxis] = 0.0f;
        contact->depth = distance - radius;
        contact->g1 = column;
        contact->g2 = plane;
        return 1;
    }

    return 0;
}

int dCollideColumnBox(dGeomID column, dGeomID box, int,
    dContactGeom* contact, int)
{
    dVector3 halfSide;
    dVector3 localPoint;
    dVector3 relativePoint;
    dVector3 closestPoint;
    dVector3 delta;
    dVector3 side;
    float depth;
    int onBorder = 0;
    float radius = *(float*)dGeomGetClassData(column);

    dGeomBoxGetLengths(box, side);
    const float* columnPosition = dGeomGetPosition(column);
    const float* boxPosition = dGeomGetPosition(box);
    const float* boxRotation = dGeomGetRotation(box);

    contact->g1 = column;
    contact->g2 = box;

    unsigned char axis = lengthwiseAxis;
    unsigned char axis1 = 0xFF;
    unsigned char axis2 = 0xFF;
    if (axis == 0)
    {
        axis1 = 1;
        axis2 = 2;
    }
    else if (axis == 1)
    {
        axis1 = 0;
        axis2 = 2;
    }
    else if (axis == 2)
    {
        axis1 = 0;
        axis2 = 1;
    }

    relativePoint[axis1] = columnPosition[axis1] - boxPosition[axis1];
    relativePoint[axis2] = columnPosition[axis2] - boxPosition[axis2];
    relativePoint[axis] = 0.0f;

    halfSide[0] = side[0] * 0.5f;
    localPoint[0] = dDOT14(relativePoint, boxRotation);
    if (localPoint[0] < -halfSide[0])
    {
        localPoint[0] = -halfSide[0];
        onBorder = 1;
    }
    if (localPoint[0] > halfSide[0])
    {
        localPoint[0] = halfSide[0];
        onBorder = 1;
    }

    halfSide[1] = side[1] * 0.5f;
    localPoint[1] = dDOT14(relativePoint, boxRotation + 1);
    if (localPoint[1] < -halfSide[1])
    {
        localPoint[1] = -halfSide[1];
        onBorder = 1;
    }
    if (localPoint[1] > halfSide[1])
    {
        localPoint[1] = halfSide[1];
        onBorder = 1;
    }

    localPoint[2] = dDOT14(relativePoint, boxRotation + 2);
    halfSide[2] = side[2] * 0.5f;
    if (localPoint[2] < -halfSide[2])
    {
        localPoint[2] = -halfSide[2];
        onBorder = 1;
    }
    if (localPoint[2] > halfSide[2])
    {
        localPoint[2] = halfSide[2];
        onBorder = 1;
    }

    if (!onBorder)
    {
        float minDistance = halfSide[0] - dFabs(localPoint[0]);
        int minAxis = 0;
        for (int i = 1; i < 3; i++)
        {
            float faceDistance = halfSide[i] - dFabs(localPoint[i]);
            if (faceDistance < minDistance)
            {
                minDistance = faceDistance;
                minAxis = i;
            }
        }

        contact->pos[axis1] = columnPosition[axis1];
        contact->pos[axis2] = columnPosition[axis2];
        contact->pos[axis] = boxPosition[axis];

        dVector3 normal;
        normal[0] = 0.0f;
        normal[1] = 0.0f;
        normal[2] = 0.0f;
        normal[minAxis] = localPoint[minAxis] > 0.0f ? 1.0f : -1.0f;
        dMULTIPLY0_331(contact->normal, boxRotation, normal);
        contact->depth = minDistance + radius;
        return 1;
    }

    localPoint[3] = 0.0f;
    dMULTIPLY0_331(closestPoint, boxRotation, localPoint);
    delta[0] = relativePoint[0] - closestPoint[0];
    delta[1] = relativePoint[1] - closestPoint[1];
    delta[2] = relativePoint[2] - closestPoint[2];
    depth = radius - dSqrt(dDOT(delta, delta));
    if (depth < 0.0f)
        return 0;

    contact->pos[0] = closestPoint[0] + boxPosition[0];
    contact->pos[1] = closestPoint[1] + boxPosition[1];
    contact->pos[2] = closestPoint[2] + boxPosition[2];
    contact->normal[0] = delta[0];
    contact->normal[1] = delta[1];
    contact->normal[2] = delta[2];
    dNormalize3(contact->normal);
    contact->depth = depth;
    return 1;
}

static dColliderFn* dColumnColliderFn(int num)
{
    if (num == dColumnClassUser)
        return &dCollideColumnColumn;
    if (num == dPlaneClass)
        return &dCollideColumnPlane;
    if (num == dSphereClass)
        return &dCollideColumnSphere;
    if (num == dBoxClass)
        return &dCollideColumnBox;
    return 0;
}

dGeomID dCreateColumn(dSpaceID space, float radius)
{
    if (dColumnClassUser == -1)
    {
        dGeomClass geomClass;
        geomClass.bytes = sizeof(float);
        geomClass.collider = &dColumnColliderFn;
        geomClass.aabb = &dColumnAABB;
        geomClass.aabb_test = 0;
        geomClass.dtor = 0;
        dColumnClassUser = dCreateGeomClass(&geomClass);
    }

    dGeomID geomID = dCreateGeom(dColumnClassUser);
    if (space != 0)
        dSpaceAdd(space, geomID);

    *(float*)dGeomGetClassData(geomID) = radius;
    return geomID;
}

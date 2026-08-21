#ifndef ODE_NLGADDITIONS_H
#define ODE_NLGADDITIONS_H

#include <ode/collision.h>

void dGeomCollideAABBs(dxGeom*, dxGeom*, void*, dNearCallback*);
void dGeomMarkAABBAsValid(dxGeom*);
void dGeomComputeAABB(dxGeom*);
void dVector3Add(float*, const float*);
void dVectorScale(float*, float);
void dVector4Set(float*, float, float, float, float);
void dVector3Set(float*, float, float, float);
void dExtractColumn3(float* __restrict, const float* __restrict, int);
void dInvertRigidTransformation(float*, const float*, const float*);
void dMultiplyMatrix4Vector4(float*, const float*, const float*);
void dMultiplyMatrix3Vector3(float*, const float*, const float*, bool);
void dGeomSetGFlags(dxGeom*, int);
int dGeomGetGFlags(dxGeom*);
int dGeomIsPlaceable(dxGeom*);
void dGeomGetRotation(dxGeom*, float*);
void dGeomGetPosition(dxGeom*, float*);
void dJointSetCharacterNoMotionDirection(dxJoint*, float*);
dxJoint* dJointCreateCharacter(dxWorld*, dxJointGroup*);
void dClearCachedData();
void dWorldSetClearAccumulators(dxWorld*, int);
dxBody* dBodyGetNextBody(dxBody*);
dxBody* dWorldGetFirstBody(dxWorld*);
void dBodySetUpdateMode(dxBody*, int, int);

#endif

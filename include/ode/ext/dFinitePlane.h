#ifndef ODE_EXT_DFINITEPLANE_H
#define ODE_EXT_DFINITEPLANE_H

#include <ode/collision.h>

dGeomID dCreateFinitePlane(dSpaceID space, dReal a, dReal b, dReal c, dReal d, bool flag, float param);
int dCollideFinitePlaneSphere(dxGeom*, dxGeom*, int, dContactGeom*, int);
void dFinitePlaneAABB(dxGeom*, float*);

#endif

#ifndef ODE_EXT_DCYLINDER_H
#define ODE_EXT_DCYLINDER_H

#include <ode/collision.h>

struct dxCylinder;
extern int dCylinderClassUser;

dxGeom* dCreateCylinder(dSpaceID space, dReal radius, dReal length);
void dGeomCylinderSetParams(dGeomID geom, dReal radius, dReal length);
void dGeomCylinderGetParams(dGeomID geom, dReal* radius, dReal* length);

#endif // ODE_EXT_DCYLINDER_H

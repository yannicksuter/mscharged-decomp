#ifndef ODE_EXT_DCOLUMN_H
#define ODE_EXT_DCOLUMN_H

#include <ode/collision.h>

dGeomID dCreateColumn(dSpaceID, float);
void dGeomColumnSetParams(dGeomID, float);
void dGeomColumnGetParams(dGeomID, float*);
int dCollideColumnColumn(dGeomID, dGeomID, int, dContactGeom*, int);
int dCollideColumnPlane(dGeomID, dGeomID, int, dContactGeom*, int);
int dCollideColumnSphere(dGeomID, dGeomID, int, dContactGeom*, int);
int dCollideColumnBox(dGeomID, dGeomID, int, dContactGeom*, int);

#endif

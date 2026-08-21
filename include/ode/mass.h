/*************************************************************************
 * Open Dynamics Engine, Copyright (C) 2001-2004 Russell L. Smith.
 * Distributed under the BSD-style license in LICENSES/ODE-BSD.txt.
 *************************************************************************/

#ifndef ODE_MASS_H
#define ODE_MASS_H

#include <ode/common.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dMass dMass;
void dMassSetZero(dMass* m);

struct dMass {
    dReal mass;
    dVector4 c;
    dMatrix3 I;

#ifdef __cplusplus
    dMass() { dMassSetZero(this); }
#endif
};

void dMassSetParameters(dMass* m, dReal mass, dReal cgx, dReal cgy,
                        dReal cgz, dReal I11, dReal I22, dReal I33,
                        dReal I12, dReal I13, dReal I23);
void dMassSetSphereTotal(dMass* m, dReal total_mass, dReal radius);
void dMassSetCappedCylinderTotal(dMass* m, dReal total_mass, int direction,
                                 dReal radius, dReal length);
void dMassSetBoxTotal(dMass* m, dReal total_mass, dReal lx, dReal ly,
                      dReal lz);
void dMassAdjust(dMass* m, dReal newmass);

#ifdef __cplusplus
}
#endif

#endif

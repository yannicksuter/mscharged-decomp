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
void dMassSetSphere(dMass* m, dReal density, dReal radius);
void dMassSetSphereTotal(dMass* m, dReal total_mass, dReal radius);
void dMassSetCappedCylinder(dMass* m, dReal density, int direction,
                            dReal radius, dReal length);
void dMassSetCappedCylinderTotal(dMass* m, dReal total_mass, int direction,
                                 dReal radius, dReal length);
void dMassSetCylinder(dMass* m, dReal density, int direction,
                      dReal radius, dReal length);
void dMassSetCylinderTotal(dMass* m, dReal total_mass, int direction,
                           dReal radius, dReal length);
void dMassSetBox(dMass* m, dReal density, dReal lx, dReal ly, dReal lz);
void dMassSetBoxTotal(dMass* m, dReal total_mass, dReal lx, dReal ly,
                      dReal lz);
void dMassAdjust(dMass* m, dReal newmass);
void dMassTranslate(dMass* m, dReal x, dReal y, dReal z);
void dMassRotate(dMass* m, const dMatrix3 R);
void dMassAdd(dMass* a, const dMass* b);

#ifdef __cplusplus
}
#endif

#endif

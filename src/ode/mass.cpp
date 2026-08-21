/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001,2002 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This file is distributed under the BSD-style license in               *
 * LICENSES/ODE-BSD.txt.                                                 *
 *                                                                       *
 *************************************************************************/

#include <ode/mass.h>
#include <ode/matrix.h>
#include <ode/odemath.h>

#define _I(i, j) I[(i) * 4 + (j)]

static int checkMass(dMass* m);

/* MWCC emits this TU's function bodies in reverse source order. */

void dMassAdjust(dMass* m, dReal newmass)
{
    dReal scale;
    int i;
    int j;

    dAASSERT(m);
    scale = newmass / m->mass;
    m->mass = newmass;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            m->_I(i, j) *= scale;
}

void dMassSetBoxTotal(dMass* m, dReal total_mass, dReal lx, dReal ly,
                      dReal lz)
{
    dAASSERT(m);
    dMassSetZero(m);
    m->mass = total_mass;
    m->_I(0, 0) = total_mass / REAL(12.0) * (ly * ly + lz * lz);
    m->_I(1, 1) = total_mass / REAL(12.0) * (lx * lx + lz * lz);
    m->_I(2, 2) = total_mass / REAL(12.0) * (lx * lx + ly * ly);
}

void dMassSetCappedCylinderTotal(dMass* m, dReal total_mass, int direction,
                                 dReal a, dReal b)
{
    dReal M1;
    dReal M2;
    dReal Ia;
    dReal Ib;
    dMassSetZero(m);

    dReal density = REAL(1.0);
    dReal M1raw = REAL(3.1415927) * a * a * b;
    M1 = density * M1raw;
    M2 = REAL(4.1887903) * a * a * a * density;
    m->mass = M1 + M2;

    Ia = M1 * (REAL(0.25) * a * a + REAL(0.083333336) * b * b)
       + M2 * (REAL(0.4) * a * a + REAL(0.375) * a * b
             + REAL(0.25) * b * b);
    Ib = (M1 * REAL(0.5) + M2 * REAL(0.4)) * a * a;

    m->_I(0, 0) = Ia;
    m->_I(1, 1) = Ia;
    m->_I(2, 2) = Ia;
    m->_I(direction - 1, direction - 1) = Ib;

    dReal scale = total_mass / m->mass;
    m->mass = total_mass;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            m->_I(i, j) *= scale;
}

void dMassSetSphereTotal(dMass* m, dReal total_mass, dReal radius)
{
    dReal II;

    dAASSERT(m);
    dMassSetZero(m);
    m->mass = total_mass;
    II = REAL(0.4) * total_mass * radius * radius;
    m->_I(0, 0) = II;
    m->_I(1, 1) = II;
    m->_I(2, 2) = II;
}

void dMassSetParameters(dMass* m, dReal themass, dReal cgx, dReal cgy,
                        dReal cgz, dReal I11, dReal I22, dReal I33,
                        dReal I12, dReal I13, dReal I23)
{
    dAASSERT(m);
    dMassSetZero(m);
    m->mass = themass;
    m->c[0] = cgx;
    m->c[1] = cgy;
    m->c[2] = cgz;
    m->_I(0, 0) = I11;
    m->_I(1, 1) = I22;
    m->_I(2, 2) = I33;
    m->_I(0, 1) = I12;
    m->_I(0, 2) = I13;
    m->_I(1, 2) = I23;
    m->_I(1, 0) = I12;
    m->_I(2, 0) = I13;
    m->_I(2, 1) = I23;
    checkMass(m);
}

void dMassSetZero(dMass* m)
{
    dAASSERT(m);
    m->mass = REAL(0.0);
    dSetZero(m->c, sizeof(m->c) / sizeof(dReal));
    dSetZero(m->I, sizeof(m->I) / sizeof(dReal));
}

static int checkMass(dMass* m)
{
    int i;

    if (m->mass <= REAL(0.0)) {
        dDEBUGMSG("mass must be > 0");
        return 0;
    }
    if (!dIsPositiveDefinite(m->I, 3)) {
        dDEBUGMSG("inertia must be positive definite");
        return 0;
    }

    dMatrix3 I2, chat;
    dSetZero(chat, 12);
    dCROSSMAT(chat, m->c, 4, +, -);
    dMULTIPLY0_333(I2, chat, chat);
    for (i = 0; i < 3; i++)
        I2[i] = m->I[i] + m->mass * I2[i];
    for (i = 4; i < 7; i++)
        I2[i] = m->I[i] + m->mass * I2[i];
    for (i = 8; i < 11; i++)
        I2[i] = m->I[i] + m->mass * I2[i];

    if (!dIsPositiveDefinite(I2, 3)) {
        dDEBUGMSG("center of mass inconsistent with mass parameters");
        return 0;
    }
    return 1;
}

/*
 * MWCC lays out this TU's literal pool using constants from all source
 * routines, including ODE entry points later removed by the linker. This
 * helper preserves that observable order. Its custom code section lies outside
 * the retail DOL layout, so the helper contributes no DOL code.
 */
#pragma section code_type ".mass_literals"

void massLiteralPoolOrder(float& v0, float& v1, float& v2, float& v3,
                          float& v4, float& v5, float& v6, float& v7,
                          float& v8, float& v9)
{
    v0 = 0.0f;
    v1 = 4.1887903f;
    v2 = 0.4f;
    v3 = 3.1415927f;
    v4 = 0.25f;
    v5 = 0.083333336f;
    v6 = 0.375f;
    v7 = 0.5f;
    v8 = 1.0f;
    v9 = 12.0f;
}

#pragma section code_type

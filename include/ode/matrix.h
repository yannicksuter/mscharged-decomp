/*************************************************************************
 * Open Dynamics Engine, Copyright (C) 2001-2004 Russell L. Smith.
 * Distributed under the BSD-style license in LICENSES/ODE-BSD.txt.
 *************************************************************************/

#ifndef ODE_MATRIX_H
#define ODE_MATRIX_H

#include <ode/common.h>

#ifdef __cplusplus
extern "C" {
#endif

void dSetZero(dReal* a, int n);
void dSetValue(dReal* a, int n, dReal value);
void dMultiply0(dReal* A, const dReal* B, const dReal* C, int p, int q, int r);
void dMultiply1(dReal* A, const dReal* B, const dReal* C, int p, int q, int r);
int dFactorCholesky(dReal* A, int n);
void dSolveCholesky(const dReal* L, dReal* b, int n);
int dInvertPDMatrix(const dReal* A, dReal* Ainv, int n);
int dIsPositiveDefinite(const dReal* A, int n);

#ifdef __cplusplus
}
#endif

#endif

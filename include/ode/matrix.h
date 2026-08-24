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
dReal dDot(const dReal* a, const dReal* b, int n);
void dMultiply0(dReal* A, const dReal* B, const dReal* C, int p, int q, int r);
void dMultiply1(dReal* A, const dReal* B, const dReal* C, int p, int q, int r);
void dMultiply2(dReal* A, const dReal* B, const dReal* C, int p, int q, int r);
int dFactorCholesky(dReal* A, int n);
void dSolveCholesky(const dReal* L, dReal* b, int n);
int dInvertPDMatrix(const dReal* A, dReal* Ainv, int n);
int dIsPositiveDefinite(const dReal* A, int n);
void dFactorLDLT(dReal* A, dReal* d, int n, int nskip);
void dSolveL1(const dReal* L, dReal* b, int n, int nskip);
void dSolveL1T(const dReal* L, dReal* b, int n, int nskip);
void dVectorScale(dReal* a, const dReal* d, int n);
void dSolveLDLT(const dReal* L, const dReal* d, dReal* b, int n, int nskip);
void dLDLTAddTL(dReal* L, dReal* d, const dReal* a, int n, int nskip);
void dLDLTRemove(dReal** A, const int* p, dReal* L, dReal* d,
                 int n1, int n2, int r, int nskip);
void dRemoveRowCol(dReal* A, int n, int nskip, int r);

#ifdef __cplusplus
}
#endif

#endif

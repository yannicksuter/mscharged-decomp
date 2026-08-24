/*************************************************************************
 * Open Dynamics Engine, Copyright (C) 2001-2004 Russell L. Smith.
 * Distributed under the BSD-style license in LICENSES/ODE-BSD.txt.
 *************************************************************************/

#ifndef ODE_ODEMATH_H
#define ODE_ODEMATH_H

#include <ode/common.h>

#ifdef __GNUC__
#define PURE_INLINE extern inline
#else
#define PURE_INLINE inline
#endif

#define dDOTpq(a, b, p, q) \
    ((a)[0] * (b)[0] + (a)[p] * (b)[q] + (a)[2 * (p)] * (b)[2 * (q)])

#define dCROSS(a, op, b, c)                         \
    (a)[0] op((b)[1] * (c)[2] - (b)[2] * (c)[1]); \
    (a)[1] op((b)[2] * (c)[0] - (b)[0] * (c)[2]); \
    (a)[2] op((b)[0] * (c)[1] - (b)[1] * (c)[0])

#ifdef __cplusplus
PURE_INLINE dReal dDOT(const dReal* a, const dReal* b)
{
    return dDOTpq(a, b, 1, 1);
}

PURE_INLINE dReal dDOT14(const dReal* a, const dReal* b)
{
    return dDOTpq(a, b, 1, 4);
}

PURE_INLINE dReal dDOT41(const dReal* a, const dReal* b)
{
    return dDOTpq(a, b, 4, 1);
}

PURE_INLINE dReal dDOT44(const dReal* a, const dReal* b)
{
    return dDOTpq(a, b, 4, 4);
}

PURE_INLINE dReal dDISTANCE(const dReal a[3], const dReal b[3])
{
    return dSqrt((a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1])
                 + (a[2] - b[2]) * (a[2] - b[2]));
}
#else
#define dDOT(a, b) dDOTpq(a, b, 1, 1)
#define dDOT14(a, b) dDOTpq(a, b, 1, 4)
#define dDOT41(a, b) dDOTpq(a, b, 4, 1)
#define dDISTANCE(a, b)                                                                  \
    (dSqrt(((a)[0] - (b)[0]) * ((a)[0] - (b)[0]) + ((a)[1] - (b)[1]) * ((a)[1] - (b)[1]) \
           + ((a)[2] - (b)[2]) * ((a)[2] - (b)[2])))
#endif

#define dCROSSMAT(A, a, skip, plus, minus) \
    (A)[1] = minus(a)[2];                   \
    (A)[2] = plus(a)[1];                    \
    (A)[(skip)] = plus(a)[2];               \
    (A)[(skip) + 2] = minus(a)[0];          \
    (A)[2 * (skip)] = minus(a)[1];          \
    (A)[2 * (skip) + 1] = plus(a)[0]

#define dMULTIPLYOP0_331(A, op, B, C) \
    (A)[0] op dDOT((B), (C));           \
    (A)[1] op dDOT((B) + 4, (C));       \
    (A)[2] op dDOT((B) + 8, (C))

#define dMULTIPLYOP1_331(A, op, B, C) \
    (A)[0] op dDOT41((B), (C));       \
    (A)[1] op dDOT41((B) + 1, (C));   \
    (A)[2] op dDOT41((B) + 2, (C))

#define dMULTIPLYOP0_333(A, op, B, C)   \
    (A)[0] op dDOT14((B), (C));         \
    (A)[1] op dDOT14((B), (C) + 1);     \
    (A)[2] op dDOT14((B), (C) + 2);     \
    (A)[4] op dDOT14((B) + 4, (C));     \
    (A)[5] op dDOT14((B) + 4, (C) + 1); \
    (A)[6] op dDOT14((B) + 4, (C) + 2); \
    (A)[8] op dDOT14((B) + 8, (C));     \
    (A)[9] op dDOT14((B) + 8, (C) + 1); \
    (A)[10] op dDOT14((B) + 8, (C) + 2)

#ifdef __cplusplus
template <class TA, class TB, class TC>
PURE_INLINE void dMULTIPLY0_331(TA* A, const TB* B, const TC* C)
{
    dMULTIPLYOP0_331(A, =, B, C);
}

template <class TA, class TB, class TC>
PURE_INLINE void dMULTIPLY1_331(TA* A, const TB* B, const TC* C)
{
    dMULTIPLYOP1_331(A, =, B, C);
}

template <class TA, class TB, class TC>
PURE_INLINE void dMULTIPLY0_333(TA* A, const TB* B, const TC* C)
{
    dMULTIPLYOP0_333(A, =, B, C);
}
#else
#define dMULTIPLY0_331(A, B, C) dMULTIPLYOP0_331(A, =, B, C)
#define dMULTIPLY1_331(A, B, C) dMULTIPLYOP1_331(A, =, B, C)
#define dMULTIPLY0_333(A, B, C) dMULTIPLYOP0_333(A, =, B, C)
#endif

#ifdef __cplusplus
extern "C" {
#endif

void dNormalize3(dVector3 a);
void dNormalize4(dVector4 a);
void dPlaneSpace(const dVector3 n, dVector3 p, dVector3 q);

#ifdef __cplusplus
}
#endif

#endif

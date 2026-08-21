/*************************************************************************
 * Open Dynamics Engine, Copyright (C) 2001-2004 Russell L. Smith.
 * Distributed under the BSD-style license in LICENSES/ODE-BSD.txt.
 *************************************************************************/

#include <ode/rotation.h>
#include <ode/odemath.h>

#define _R(i, j) R[(i) * 4 + (j)]

#define SET_3x3_IDENTITY   \
    _R(0, 0) = REAL(1.0); \
    _R(0, 1) = REAL(0.0); \
    _R(0, 2) = REAL(0.0); \
    _R(0, 3) = REAL(0.0); \
    _R(1, 0) = REAL(0.0); \
    _R(1, 1) = REAL(1.0); \
    _R(1, 2) = REAL(0.0); \
    _R(1, 3) = REAL(0.0); \
    _R(2, 0) = REAL(0.0); \
    _R(2, 1) = REAL(0.0); \
    _R(2, 2) = REAL(1.0); \
    _R(2, 3) = REAL(0.0);

/* MWCC emits this TU's function bodies in reverse source order. */

void dDQfromW(dReal dq[4], const dVector3 w, const dQuaternion q)
{
    dAASSERT(w && q && dq);
    dq[0] = REAL(0.5) * (-w[0] * q[1] - w[1] * q[2] - w[2] * q[3]);
    dq[1] = REAL(0.5) * (w[0] * q[0] + w[1] * q[3] - w[2] * q[2]);
    dq[2] = REAL(0.5) * (-w[0] * q[3] + w[1] * q[0] + w[2] * q[1]);
    dq[3] = REAL(0.5) * (w[0] * q[2] - w[1] * q[1] + w[2] * q[0]);
}

void dQfromR(dQuaternion q, const dMatrix3 R)
{
    dAASSERT(q && R);
    dReal tr, s;
    tr = _R(0, 0) + _R(1, 1) + _R(2, 2);

    if (tr >= 0)
    {
        s = dSqrt(tr + REAL(1.0));
        q[0] = REAL(0.5) * s;
        s = REAL(0.5) * (REAL(1.0) / s);
        q[1] = (_R(2, 1) - _R(1, 2)) * s;
        q[2] = (_R(0, 2) - _R(2, 0)) * s;
        q[3] = (_R(1, 0) - _R(0, 1)) * s;
    }
    else
    {
        if (_R(1, 1) > _R(0, 0))
        {
            if (_R(2, 2) > _R(1, 1))
                goto case_2;
            goto case_1;
        }
        if (_R(2, 2) > _R(0, 0))
            goto case_2;
        goto case_0;

    case_0:
    {
        s = dSqrt((_R(0, 0) - (_R(1, 1) + _R(2, 2))) + REAL(1.0));
        q[1] = REAL(0.5) * s;
        s = REAL(0.5) * (REAL(1.0) / s);
        q[2] = (_R(0, 1) + _R(1, 0)) * s;
        q[3] = (_R(2, 0) + _R(0, 2)) * s;
        q[0] = (_R(2, 1) - _R(1, 2)) * s;
        return;
    }

    case_1:
    {
        s = dSqrt((_R(1, 1) - (_R(2, 2) + _R(0, 0))) + REAL(1.0));
        q[2] = REAL(0.5) * s;
        s = REAL(0.5) * (REAL(1.0) / s);
        q[3] = (_R(1, 2) + _R(2, 1)) * s;
        q[1] = (_R(0, 1) + _R(1, 0)) * s;
        q[0] = (_R(0, 2) - _R(2, 0)) * s;
        return;
    }

    case_2:
    {
        s = dSqrt((_R(2, 2) - (_R(0, 0) + _R(1, 1))) + REAL(1.0));
        q[3] = REAL(0.5) * s;
        s = REAL(0.5) * (REAL(1.0) / s);
        q[1] = (_R(2, 0) + _R(0, 2)) * s;
        q[2] = (_R(1, 2) + _R(2, 1)) * s;
        q[0] = (_R(1, 0) - _R(0, 1)) * s;
        return;
    }
    }
}

void dRfromQ(dMatrix3 R, const dQuaternion q)
{
    dAASSERT(q && R);
    dReal qq1 = 2 * q[1] * q[1];
    dReal qq2 = 2 * q[2] * q[2];
    dReal qq3 = 2 * q[3] * q[3];
    _R(0, 0) = 1 - qq2 - qq3;
    _R(0, 1) = 2 * (q[1] * q[2] - q[0] * q[3]);
    _R(0, 2) = 2 * (q[1] * q[3] + q[0] * q[2]);
    _R(1, 0) = 2 * (q[1] * q[2] + q[0] * q[3]);
    _R(1, 1) = 1 - qq1 - qq3;
    _R(1, 2) = 2 * (q[2] * q[3] - q[0] * q[1]);
    _R(2, 0) = 2 * (q[1] * q[3] - q[0] * q[2]);
    _R(2, 1) = 2 * (q[2] * q[3] + q[0] * q[1]);
    _R(2, 2) = 1 - qq1 - qq2;
}

void dQMultiply0(dQuaternion qa, const dQuaternion qb, const dQuaternion qc)
{
    dAASSERT(qa && qb && qc);
    qa[0] = qb[0] * qc[0] - qb[1] * qc[1] - qb[2] * qc[2] - qb[3] * qc[3];
    qa[1] = qb[0] * qc[1] + qb[1] * qc[0] + qb[2] * qc[3] - qb[3] * qc[2];
    qa[2] = qb[0] * qc[2] + qb[2] * qc[0] + qb[3] * qc[1] - qb[1] * qc[3];
    qa[3] = qb[0] * qc[3] + qb[3] * qc[0] + qb[1] * qc[2] - qb[2] * qc[1];
}

void dRSetIdentity(dMatrix3 R)
{
    dAASSERT(R);
    SET_3x3_IDENTITY;
}

/* Keep the target literal order without adding retail code. */
#pragma section code_type ".rotation_literals"

void rotationLiteralPoolOrder(float& one, float& zero, float& half, float& two)
{
    one = 1.0f;
    zero = 0.0f;
    half = 0.5f;
    two = 2.0f;
}

#pragma section code_type

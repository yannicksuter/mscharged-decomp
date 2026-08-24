/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001,2002 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/

#include <ode/collision.h>
#include <ode/common.h>
#include <ode/odemath.h>

#include "collision_util.h"

int dCollideSpheres(dVector3 p1, dReal r1, dVector3 p2, dReal r2, dContactGeom* c)
{
    dReal d = dDISTANCE(p1, p2);
    if (d > (r1 + r2))
        return 0;
    if (d <= 0)
    {
        c->pos[0] = p1[0];
        c->pos[1] = p1[1];
        c->pos[2] = p1[2];
        c->normal[0] = 1;
        c->normal[1] = 0;
        c->normal[2] = 0;
        c->depth = r1 + r2;
    }
    else
    {
        dReal d1 = dRecip(d);
        c->normal[0] = (p1[0] - p2[0]) * d1;
        c->normal[1] = (p1[1] - p2[1]) * d1;
        c->normal[2] = (p1[2] - p2[2]) * d1;
        dReal k = REAL(0.5) * (r2 - r1 - d);
        c->pos[0] = p1[0] + c->normal[0] * k;
        c->pos[1] = p1[1] + c->normal[1] * k;
        c->pos[2] = p1[2] + c->normal[2] * k;
        c->depth = r1 + r2 - d;
    }
    return 1;
}

void dLineClosestApproach(const dVector3 pa, const dVector3 ua, const dVector3 pb,
    const dVector3 ub, dReal* alpha, dReal* beta)
{
    dVector3 p;
    p[0] = pb[0] - pa[0];
    p[1] = pb[1] - pa[1];
    p[2] = pb[2] - pa[2];
    dReal uaub = dDOT(ua, ub);
    dReal q1 = dDOT(ua, p);
    dReal q2 = -dDOT(ub, p);
    dReal d = 1 - uaub * uaub;
    if (d <= REAL(0.0001))
    {
        *alpha = 0;
        *beta = 0;
    }
    else
    {
        d = dRecip(d);
        *alpha = (q1 + uaub * q2) * d;
        *beta = (uaub * q1 + q2) * d;
    }
}

void dClosestLineSegmentPoints(const dVector3 a1, const dVector3 a2, const dVector3 b1,
    const dVector3 b2, dVector3 cp1, dVector3 cp2)
{
    dVector3 a1a2, b1b2, a1b1, a1b2, a2b1, a2b2, n;
    dReal la, lb, k, da1, da2, da3, da4, db1, db2, db3, db4, det;

#define SET2(a, b) \
    a[0] = b[0];   \
    a[1] = b[1];   \
    a[2] = b[2];
#define SET3(a, b, op, c) \
    a[0] = b[0] op c[0];  \
    a[1] = b[1] op c[1];  \
    a[2] = b[2] op c[2];

    SET3(a1a2, a2, -, a1);
    SET3(b1b2, b2, -, b1);
    SET3(a1b1, b1, -, a1);
    da1 = dDOT(a1a2, a1b1);
    db1 = dDOT(b1b2, a1b1);
    if (da1 <= 0 && db1 >= 0)
    {
        SET2(cp1, a1);
        SET2(cp2, b1);
        return;
    }

    SET3(a1b2, b2, -, a1);
    da2 = dDOT(a1a2, a1b2);
    db2 = dDOT(b1b2, a1b2);
    if (da2 <= 0 && db2 <= 0)
    {
        SET2(cp1, a1);
        SET2(cp2, b2);
        return;
    }

    SET3(a2b1, b1, -, a2);
    da3 = dDOT(a1a2, a2b1);
    db3 = dDOT(b1b2, a2b1);
    if (da3 >= 0 && db3 >= 0)
    {
        SET2(cp1, a2);
        SET2(cp2, b1);
        return;
    }

    SET3(a2b2, b2, -, a2);
    da4 = dDOT(a1a2, a2b2);
    db4 = dDOT(b1b2, a2b2);
    if (da4 >= 0 && db4 <= 0)
    {
        SET2(cp1, a2);
        SET2(cp2, b2);
        return;
    }

    la = dDOT(a1a2, a1a2);
    if (da1 >= 0 && da3 <= 0)
    {
        k = da1 / la;
        SET3(n, a1b1, -, k * a1a2);
        if (dDOT(b1b2, n) >= 0)
        {
            SET3(cp1, a1, +, k * a1a2);
            SET2(cp2, b1);
            return;
        }
    }

    if (da2 >= 0 && da4 <= 0)
    {
        k = da2 / la;
        SET3(n, a1b2, -, k * a1a2);
        if (dDOT(b1b2, n) <= 0)
        {
            SET3(cp1, a1, +, k * a1a2);
            SET2(cp2, b2);
            return;
        }
    }

    lb = dDOT(b1b2, b1b2);
    if (db1 <= 0 && db2 >= 0)
    {
        k = -db1 / lb;
        SET3(n, -a1b1, -, k * b1b2);
        if (dDOT(a1a2, n) >= 0)
        {
            SET2(cp1, a1);
            SET3(cp2, b1, +, k * b1b2);
            return;
        }
    }

    if (db3 <= 0 && db4 >= 0)
    {
        k = -db3 / lb;
        SET3(n, -a2b1, -, k * b1b2);
        if (dDOT(a1a2, n) <= 0)
        {
            SET2(cp1, a2);
            SET3(cp2, b1, +, k * b1b2);
            return;
        }
    }

    k = dDOT(a1a2, b1b2);
    det = la * lb - k * k;
    if (det <= 0)
    {
        SET2(cp1, a1);
        SET2(cp2, b1);
        return;
    }
    det = dRecip(det);
    dReal alpha = (lb * da1 - k * db1) * det;
    dReal beta = (k * da1 - la * db1) * det;
    SET3(cp1, a1, +, alpha * a1a2);
    SET3(cp2, b1, +, beta * b1b2);

#undef SET2
#undef SET3
}

void dClosestLineBoxPoints(const dVector3 p1, const dVector3 p2, const dVector3 c,
    const dMatrix3 R, const dVector3 side, dVector3 lret, dVector3 bret)
{
    int i;
    dVector3 tmp, s, v;
    tmp[0] = p1[0] - c[0];
    tmp[1] = p1[1] - c[1];
    tmp[2] = p1[2] - c[2];
    dMULTIPLY1_331(s, R, tmp);
    tmp[0] = p2[0] - p1[0];
    tmp[1] = p2[1] - p1[1];
    tmp[2] = p2[2] - p1[2];
    dMULTIPLY1_331(v, R, tmp);

    dVector3 sign;
    for (i = 0; i < 3; i++)
    {
        if (v[i] < 0)
        {
            s[i] = -s[i];
            v[i] = -v[i];
            sign[i] = -1;
        }
        else
            sign[i] = 1;
    }

    dVector3 v2;
    v2[0] = v[0] * v[0];
    v2[1] = v[1] * v[1];
    v2[2] = v[2] * v[2];

    dReal h[3];
    h[0] = REAL(0.5) * side[0];
    h[1] = REAL(0.5) * side[1];
    h[2] = REAL(0.5) * side[2];

    int region[3];
    dReal tanchor[3];

    for (i = 0; i < 3; i++)
    {
        if (v[i] > 0)
        {
            if (s[i] < -h[i])
            {
                region[i] = -1;
                tanchor[i] = (-h[i] - s[i]) / v[i];
            }
            else
            {
                region[i] = (s[i] > h[i]);
                tanchor[i] = (h[i] - s[i]) / v[i];
            }
        }
        else
        {
            region[i] = 0;
            tanchor[i] = 2;
        }
    }

    dReal t = 0;
    dReal dd2dt = 0;
    for (i = 0; i < 3; i++)
        dd2dt -= (region[i] ? v2[i] : 0) * tanchor[i];
    if (dd2dt >= 0)
        goto got_answer;

    do
    {
        dReal next_t = 1;
        for (i = 0; i < 3; i++)
        {
            if (tanchor[i] > t && tanchor[i] < 1 && tanchor[i] < next_t)
                next_t = tanchor[i];
        }

        dReal next_dd2dt = 0;
        for (i = 0; i < 3; i++)
            next_dd2dt += (region[i] ? v2[i] : 0) * (next_t - tanchor[i]);

        if (next_dd2dt >= 0)
        {
            dReal m = (next_dd2dt - dd2dt) / (next_t - t);
            t -= dd2dt / m;
            goto got_answer;
        }

        for (i = 0; i < 3; i++)
        {
            if (tanchor[i] == next_t)
            {
                tanchor[i] = (h[i] - s[i]) / v[i];
                region[i]++;
            }
        }
        t = next_t;
        dd2dt = next_dd2dt;
    } while (t < 1);
    t = 1;

got_answer:
    for (i = 0; i < 3; i++)
        lret[i] = p1[i] + t * tmp[i];

    for (i = 0; i < 3; i++)
    {
        tmp[i] = sign[i] * (s[i] + t * v[i]);
        if (tmp[i] < -h[i])
            tmp[i] = -h[i];
        else if (tmp[i] > h[i])
            tmp[i] = h[i];
    }
    dMULTIPLY0_331(s, R, tmp);
    for (i = 0; i < 3; i++)
        bret[i] = s[i] + c[i];
}

/*************************************************************************
 * Open Dynamics Engine, Copyright (C) 2001-2004 Russell L. Smith.
 * Distributed under the BSD-style license in LICENSES/ODE-BSD.txt.
 *************************************************************************/

#include <ode/matrix.h>
#include <ode/odemath.h>
#include <ode/rotation.h>

#include "joint.h"

#define dInfinity REAL(3.402823466e+38)

enum
{
    dJointTypeContact = 4,
};

static void contactInit(dxJointContact*)
{
}

static void contactGetInfo1(dxJointContact* j, dxJoint::Info1* info)
{
    int m = 1;
    int nub = 0;

    if (j->contact.surface.mu < 0)
        j->contact.surface.mu = 0;

    if (j->contact.surface.mode & dContactMu2)
    {
        if (j->contact.surface.mu > 0)
            m++;
        if (j->contact.surface.mu2 < 0)
            j->contact.surface.mu2 = 0;
        if (j->contact.surface.mu2 > 0)
            m++;
        if (j->contact.surface.mu == dInfinity)
            nub++;
        if (j->contact.surface.mu2 == dInfinity)
            nub++;
    }
    else
    {
        if (j->contact.surface.mu > 0)
            m += 2;
        if (j->contact.surface.mu == dInfinity)
            nub += 2;
    }

    j->the_m = m;
    info->m = m;
    info->nub = nub;
}

static void contactGetInfo2(dxJointContact* j, dxJoint::Info2* info)
{
    int i;
    int s = info->rowskip;
    int s2 = 2 * s;

    dVector3 normal;
    if (j->flags & dJOINT_REVERSE)
    {
        normal[0] = -j->contact.geom.normal[0];
        normal[1] = -j->contact.geom.normal[1];
        normal[2] = -j->contact.geom.normal[2];
    }
    else
    {
        normal[0] = j->contact.geom.normal[0];
        normal[1] = j->contact.geom.normal[1];
        normal[2] = j->contact.geom.normal[2];
    }
    normal[3] = 0;

    dVector3 c1;
    dVector3 c2;
    for (i = 0; i < 3; i++)
        c1[i] = j->contact.geom.pos[i] - j->node[0].body->pos[i];

    info->J1l[0] = normal[0];
    info->J1l[1] = normal[1];
    info->J1l[2] = normal[2];
    dCROSS(info->J1a, =, c1, normal);
    if (j->node[1].body)
    {
        for (i = 0; i < 3; i++)
            c2[i] = j->contact.geom.pos[i] - j->node[1].body->pos[i];
        info->J2l[0] = -normal[0];
        info->J2l[1] = -normal[1];
        info->J2l[2] = -normal[2];
        dCROSS(info->J2a, = -, c2, normal);
    }

    dReal erp = info->erp;
    if (j->contact.surface.mode & dContactSoftERP)
        erp = j->contact.surface.soft_erp;
    dReal k = info->fps * erp;
    dReal depth = j->contact.geom.depth - j->world->contactp.min_depth;
    if (depth < 0)
        depth = 0;
    dReal maxvel = j->world->contactp.max_vel;
    if (k * depth > maxvel)
        info->c[0] = maxvel;
    else
        info->c[0] = k * depth;
    if (j->contact.surface.mode & dContactSoftCFM)
        info->cfm[0] = j->contact.surface.soft_cfm;

    if (j->contact.surface.mode & dContactBounce)
    {
        dReal outgoing = dDOT(info->J1l, j->node[0].body->lvel) +
            dDOT(info->J1a, j->node[0].body->avel);
        if (j->node[1].body)
        {
            outgoing += dDOT(info->J2l, j->node[1].body->lvel) +
                dDOT(info->J2a, j->node[1].body->avel);
        }
        if (j->contact.surface.bounce_vel >= 0 &&
            -outgoing > j->contact.surface.bounce_vel)
        {
            dReal newc = -j->contact.surface.bounce * outgoing;
            if (newc > info->c[0])
                info->c[0] = newc;
        }
    }

    info->lo[0] = 0;
    info->hi[0] = dInfinity;

    dVector3 t1;
    dVector3 t2;
    if (j->the_m >= 2)
    {
        if (j->contact.surface.mode & dContactFDir1)
        {
            t1[0] = j->contact.fdir1[0];
            t1[1] = j->contact.fdir1[1];
            t1[2] = j->contact.fdir1[2];
            dCROSS(t2, =, normal, t1);
        }
        else
        {
            dPlaneSpace(normal, t1, t2);
        }

        info->J1l[s + 0] = t1[0];
        info->J1l[s + 1] = t1[1];
        info->J1l[s + 2] = t1[2];
        dCROSS(info->J1a + s, =, c1, t1);
        if (j->node[1].body)
        {
            info->J2l[s + 0] = -t1[0];
            info->J2l[s + 1] = -t1[1];
            info->J2l[s + 2] = -t1[2];
            dCROSS(info->J2a + s, = -, c2, t1);
        }
        if (j->contact.surface.mode & dContactMotion1)
            info->c[1] = j->contact.surface.motion1;
        info->lo[1] = -j->contact.surface.mu;
        info->hi[1] = j->contact.surface.mu;
        if (j->contact.surface.mode & dContactApprox1_1)
            info->findex[1] = 0;
        if (j->contact.surface.mode & dContactSlip1)
            info->cfm[1] = j->contact.surface.slip1;
    }

    if (j->the_m >= 3)
    {
        info->J1l[s2 + 0] = t2[0];
        info->J1l[s2 + 1] = t2[1];
        info->J1l[s2 + 2] = t2[2];
        dCROSS(info->J1a + s2, =, c1, t2);
        if (j->node[1].body)
        {
            info->J2l[s2 + 0] = -t2[0];
            info->J2l[s2 + 1] = -t2[1];
            info->J2l[s2 + 2] = -t2[2];
            dCROSS(info->J2a + s2, = -, c2, t2);
        }
        if (j->contact.surface.mode & dContactMotion2)
            info->c[2] = j->contact.surface.motion2;
        if (j->contact.surface.mode & dContactMu2)
        {
            info->lo[2] = -j->contact.surface.mu2;
            info->hi[2] = j->contact.surface.mu2;
        }
        else
        {
            info->lo[2] = -j->contact.surface.mu;
            info->hi[2] = j->contact.surface.mu;
        }
        if (j->contact.surface.mode & dContactApprox1_2)
            info->findex[2] = 0;
        if (j->contact.surface.mode & dContactSlip2)
            info->cfm[2] = j->contact.surface.slip2;
    }
}

dxJoint::Vtable __dcontact_vtable = {
    sizeof(dxJointContact),
    (dxJoint::init_fn*)contactInit,
    (dxJoint::getInfo1_fn*)contactGetInfo1,
    (dxJoint::getInfo2_fn*)contactGetInfo2,
    dJointTypeContact,
};

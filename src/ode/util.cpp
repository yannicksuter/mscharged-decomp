/*************************************************************************
 * Open Dynamics Engine, Copyright (C) 2001-2004 Russell L. Smith.
 * Distributed under the BSD-style license in LICENSES/ODE-BSD.txt.
 *************************************************************************/

#include "NL/nlMath.h"
#include <ode/collision.h>
#include <ode/compatibility.h>
#include <ode/odemath.h>
#include <ode/rotation.h>

#include "joint.h"
#include "objects.h"
#include "util.h"

#define ALLOCA   dALLOCA16
#define dFabs(x) ((float)__fabs(float(x)))

static inline unsigned short radiansToAngle(dReal angle)
{
    return (unsigned short)(int)(10430.378f * angle);
}

/**
 * Offset/Address/Size: 0x0 | 0x80358240 | size: 0xDC
 */
void dInternalHandleAutoDisabling(dxWorld* world, dReal stepsize)
{
    dxBody* bb;
    for (bb = world->firstbody; bb; bb = (dxBody*)bb->next)
    {
        // nothing to do unless this body is currently enabled and has
        // the auto-disable flag set
        if ((bb->flags & (dxBodyAutoDisable | dxBodyDisabled)) != dxBodyAutoDisable)
            continue;

        // see if the body is idle
        int idle = 1; // initial assumption
        dReal lspeed2 = dDOT(bb->lvel, bb->lvel);
        if (lspeed2 > bb->adis.linear_threshold)
        {
            idle = 0; // moving fast - not idle
        }
        else
        {
            dReal aspeed = dDOT(bb->avel, bb->avel);
            if (aspeed > bb->adis.angular_threshold)
            {
                idle = 0; // turning fast - not idle
            }
        }

        // if it's idle, accumulate steps and time.
        // these counters won't overflow because this code doesn't run for disabled
        // bodies.
        if (idle)
        {
            bb->adis_stepsleft--;
            bb->adis_timeleft -= stepsize;
        }
        else
        {
            bb->adis_stepsleft = bb->adis.idle_steps;
            bb->adis_timeleft = bb->adis.idle_time;
        }

        // disable the body if it's idle for a long enough time
        if (bb->adis_stepsleft < 0 && bb->adis_timeleft < 0)
        {
            bb->flags |= dxBodyDisabled;
        }
    }
}

// given a body b, apply its linear and angular rotation over the time
// interval h, thereby adjusting its position and orientation.

/**
 * Offset/Address/Size: 0xDC | 0x8035831C | size: 0x3B4
 */
void dxStepBody(dxBody* b, dReal h)
{
    int j;
    unsigned char changed = 0;

    // handle linear velocity
    if (!(b->flags & dxBodyLinearUpdateDisabled))
    {
        changed = 1;
        b->pos[0] += h * b->lvel[0];
        b->pos[1] += h * b->lvel[1];
        b->pos[2] += h * b->lvel[2];
    }

    if (!(b->flags & dxBodyAngularUpdateDisabled))
    {
        if (b->flags & dxBodyFlagFiniteRotation)
        {
            dVector3 irv;  // infitesimal rotation vector
            dQuaternion q; // quaternion for finite rotation

            if (b->flags & dxBodyFlagFiniteRotationAxis)
            {
                // split the angular velocity vector into a component along the finite
                // rotation axis, and a component orthogonal to it.
                dVector3 frv; // finite rotation vector
                dReal k = dDOT(b->finite_rot_axis, b->avel);
                frv[0] = b->finite_rot_axis[0] * k;
                frv[1] = b->finite_rot_axis[1] * k;
                frv[2] = b->finite_rot_axis[2] * k;
                irv[0] = b->avel[0] - frv[0];
                irv[1] = b->avel[1] - frv[1];
                irv[2] = b->avel[2] - frv[2];

                // make a rotation quaternion q that corresponds to frv * h.
                // compare this with the full-finite-rotation case below.
                h *= REAL(0.5);
                dReal theta = k * h;
                q[0] = nlSin((unsigned short)(radiansToAngle(theta) + 0x4000));
                dReal s;
                if (dFabs(theta) < 1.0e-4f)
                {
                    s = REAL(1.0) - theta * theta * REAL(0.166666666666666666667);
                }
                else
                {
                    s = nlSin(radiansToAngle(theta)) / theta;
                }
                s = h * s;
                q[1] = frv[0] * s;
                q[2] = frv[1] * s;
                q[3] = frv[2] * s;
            }
            else
            {
                // make a rotation quaternion q that corresponds to w * h
                dReal wlen = dSqrt(b->avel[0] * b->avel[0] + b->avel[1] * b->avel[1] + b->avel[2] * b->avel[2]);
                h *= REAL(0.5);
                dReal theta = wlen * h;
                q[0] = nlSin((unsigned short)(radiansToAngle(theta) + 0x4000));
                dReal s;
                if (dFabs(theta) < 1.0e-4f)
                {
                    s = REAL(1.0) - theta * theta * REAL(0.166666666666666666667);
                }
                else
                {
                    s = nlSin(radiansToAngle(theta)) / theta;
                }
                s = h * s;
                q[1] = b->avel[0] * s;
                q[2] = b->avel[1] * s;
                q[3] = b->avel[2] * s;
            }

            // do the finite rotation
            dQuaternion q2;
            dQMultiply0(q2, q, b->q);
            for (j = 0; j < 4; j++)
                b->q[j] = q2[j];

            // do the infitesimal rotation if required
            if (b->flags & dxBodyFlagFiniteRotationAxis)
            {
                dReal dq[4];
                dWtoDQ(irv, b->q, dq);
                for (j = 0; j < 4; j++)
                    b->q[j] += h * dq[j];
            }
        }
        else
        {
            // the normal way - do an infitesimal rotation
            dReal dq[4];
            dWtoDQ(b->avel, b->q, dq);
            for (j = 0; j < 4; j++)
                b->q[j] += h * dq[j];
        }

        // normalize the quaternion and convert it to a rotation matrix
        dNormalize4(b->q);
        dQtoR(b->q, b->R);
        changed = 1;
    }

    // notify all attached geoms that this body has moved
    if (changed)
    {
        for (dxGeom* geom = b->geom; geom; geom = dGeomGetBodyNext(geom))
            dGeomMoved(geom);
    }
}

// this groups all joints and bodies in a world into islands. all objects
// in an island are reachable by going through connected bodies and joints.
// each island can be simulated separately.
// note that joints that are not attached to anything will not be included
// in any island, an so they do not affect the simulation.
//
// this function starts new island from unvisited bodies. however, it will
// never start a new islands from a disabled body. thus islands of disabled
// bodies will not be included in the simulation. disabled bodies are
// re-enabled if they are found to be part of an active island.

/**
 * Offset/Address/Size: 0x490 | 0x803586D0 | size: 0x438
 */
void dxProcessIslands(dxWorld* world, dReal stepsize, dstepper_fn_t stepper)
{
    dxBody *b, *bb, **body;
    dxJoint *j, **joint;

    // nothing to do if no bodies
    if (world->nb <= 0)
        return;

    // handle auto-disabling of bodies
    dInternalHandleAutoDisabling(world, stepsize);

    // make arrays for body and joint lists (for a single island) to go into
    body = (dxBody**)ALLOCA(world->nb * sizeof(dxBody*));
    joint = (dxJoint**)ALLOCA(world->nj * sizeof(dxJoint*));
    int bcount = 0; // number of bodies in `body'
    int jcount = 0; // number of joints in `joint'

    // set all body/joint tags to 0
    for (b = world->firstbody; b; b = (dxBody*)b->next)
        b->tag = 0;
    for (j = world->firstjoint; j; j = (dxJoint*)j->next)
        j->tag = 0;

    // allocate a stack of unvisited bodies in the island. the maximum size of
    // the stack can be the lesser of the number of bodies or joints, because
    // new bodies are only ever added to the stack by going through untagged
    // joints. all the bodies in the stack must be tagged!
    int stackalloc = (world->nj < world->nb) ? world->nj : world->nb;
    dxBody** stack = (dxBody**)ALLOCA(stackalloc * sizeof(dxBody*));

    for (bb = world->firstbody; bb; bb = (dxBody*)bb->next)
    {
        // get bb = the next enabled, untagged body, and tag it
        if (bb->tag || (bb->flags & dxBodyDisabled))
            continue;
        bb->tag = 1;

        // tag all bodies and joints starting from bb.
        int stacksize = 0;
        b = bb;
        body[0] = bb;
        bcount = 1;
        jcount = 0;
        goto quickstart;
        while (stacksize > 0)
        {
            b = stack[--stacksize]; // pop body off stack
            body[bcount++] = b;     // put body on body list
        quickstart:

            // traverse and tag all body's joints, add untagged connected bodies
            // to stack
            for (dxJointNode* n = b->firstjoint; n; n = n->next)
            {
                if (!n->joint->tag)
                {
                    n->joint->tag = 1;
                    joint[jcount++] = n->joint;
                    if (n->body && !n->body->tag)
                    {
                        n->body->tag = 1;
                        stack[stacksize++] = n->body;
                    }
                }
            }
            dIASSERT(stacksize <= world->nb);
            dIASSERT(stacksize <= world->nj);
        }

        // now do something with body and joint lists
        stepper(world, body, bcount, joint, jcount, stepsize);

        // what we've just done may have altered the body/joint tag values.
        // we must make sure that these tags are nonzero.
        // also make sure all bodies are in the enabled state.
        int i;
        for (i = 0; i < bcount; i++)
        {
            body[i]->tag = 1;
            body[i]->flags &= ~dxBodyDisabled;
        }
        for (i = 0; i < jcount; i++)
            joint[i]->tag = 1;
    }

    // if debugging, check that all objects (except for disabled bodies,
    // unconnected joints, and joints that are connected to disabled bodies)
    // were tagged.
#ifndef dNODEBUG
    for (b = world->firstbody; b; b = (dxBody*)b->next)
    {
        if (b->flags & dxBodyDisabled)
        {
            if (b->tag)
                dDebug(0, "disabled body tagged");
        }
        else
        {
            if (!b->tag)
                dDebug(0, "enabled body not tagged");
        }
    }
    for (j = world->firstjoint; j; j = (dxJoint*)j->next)
    {
        if ((j->node[0].body && (j->node[0].body->flags & dxBodyDisabled) == 0) || (j->node[1].body && (j->node[1].body->flags & dxBodyDisabled) == 0))
        {
            if (!j->tag)
                dDebug(0, "attached enabled joint not tagged");
        }
        else
        {
            if (j->tag)
                dDebug(0, "unattached or disabled joint tagged");
        }
    }
#endif
}

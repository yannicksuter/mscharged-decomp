/*************************************************************************
 * Open Dynamics Engine, Copyright (C) 2001-2004 Russell L. Smith.
 * Distributed under the BSD-style license in LICENSES/ODE-BSD.txt.
 *************************************************************************/

#ifndef ODE_UTIL_INTERNAL_H
#define ODE_UTIL_INTERNAL_H

#include "objects.h"

void dInternalHandleAutoDisabling(dxWorld* world, dReal stepsize);
void dxStepBody(dxBody* body, dReal stepsize);

typedef void (*dstepper_fn_t)(dxWorld* world, dxBody* const* body, int nb,
    dxJoint* const* joint, int nj, dReal stepsize);

void dxProcessIslands(dxWorld* world, dReal stepsize, dstepper_fn_t stepper);

#endif // ODE_UTIL_INTERNAL_H

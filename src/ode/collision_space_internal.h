/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001-2003 Russell L. Smith.       *
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

/*

stuff common to all spaces

*/

#ifndef _ODE_COLLISION_SPACE_INTERNAL_H_
#define _ODE_COLLISION_SPACE_INTERNAL_H_

#define ALLOCA(x) dALLOCA16(x)

#define CHECK_NOT_LOCKED(space) \
  dUASSERT ((space)==0 || (space)->lock_count==0, \
	    "invalid operation for locked space");


// collide two geoms together. for the hash table space, this is
// called if the two AABBs inhabit the same hash table cells.
// this only calls the callback function if the AABBs actually
// intersect. if a geom has an AABB test function, that is called to
// provide a further refinement of the intersection.
//
// NOTE: this assumes that the geom AABBs are valid on entry
// and that both geoms are enabled.

static void collideAABBs(dxGeom* g1, dxGeom* g2, void* data,
    dNearCallback* callback);

#endif

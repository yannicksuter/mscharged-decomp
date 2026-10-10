#ifndef ODE_EXT_DROUNDED_CORNER_H
#define ODE_EXT_DROUNDED_CORNER_H

#include <ode/collision.h>

dGeomID dCreateRoundedCorner(dSpaceID space, float radius,
    bool extendPositiveX, bool extendPositiveY);

#endif

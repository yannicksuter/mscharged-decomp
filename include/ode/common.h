/*************************************************************************
 * Open Dynamics Engine, Copyright (C) 2001-2004 Russell L. Smith.
 * Distributed under the BSD-style license in LICENSES/ODE-BSD.txt.
 *************************************************************************/

#ifndef ODE_COMMON_H
#define ODE_COMMON_H

#include <ode/config.h>

#ifdef __cplusplus
float nlSqrt(float value, bool accurate);

extern "C"
{
#endif

    struct dxWorld;
    struct dxSpace;
    struct dxBody;
    struct dxGeom;
    struct dxJoint;
    struct dxJointNode;
    struct dxJointGroup;

    typedef struct dxWorld* dWorldID;
    typedef struct dxSpace* dSpaceID;
    typedef struct dxBody* dBodyID;
    typedef struct dxGeom* dGeomID;
    typedef struct dxJoint* dJointID;
    typedef struct dxJointGroup* dJointGroupID;

#if defined(dSINGLE)
    typedef float dReal;
#define REAL(x) (x##f)
#else
#error ODE must be built with dSINGLE for R4QE01
#endif

    typedef dReal dVector3[4];
    typedef dReal dVector4[4];
    typedef dReal dQuaternion[4];
    typedef dReal dMatrix3[12];

    typedef struct dJointFeedback
    {
        dVector3 f1;
        dVector3 t1;
        dVector3 f2;
        dVector3 t2;
    } dJointFeedback;

#ifdef __cplusplus
#define dSqrt(x) ((float)nlSqrt(float(x), true))
#endif

#define dRecip(x) ((float)(1.0f / (x)))
#define dPAD(a)   (((a) > 1) ? ((((a) - 1) | 3) + 1) : (a))

#define EFFICIENT_ALIGNMENT 16
#define dEFFICIENT_SIZE(x)  ((((x) - 1) | (EFFICIENT_ALIGNMENT - 1)) + 1)

#define dALLOCA16(n) \
    ((char*)dEFFICIENT_SIZE(((size_t)(__alloca((n) + (EFFICIENT_ALIGNMENT - 1))))))

#ifndef dNODEBUG
    void dMessage(int num, const char* msg, ...);
#define dDEBUGMSG(msg) dMessage(2, msg)
#define dUASSERT(condition, msg) \
    do                           \
    {                            \
        if (!(condition))        \
            dMessage(2, msg);    \
    } while (0)
#else
#define dDEBUGMSG(msg)           ((void)0)
#define dUASSERT(condition, msg) ((void)0)
#endif

#define dIASSERT(condition) dUASSERT(condition, "Internal assertion failed")
#define dAASSERT(condition) dUASSERT(condition, "Bad argument(s)")

#ifdef __cplusplus
}
#endif

#endif

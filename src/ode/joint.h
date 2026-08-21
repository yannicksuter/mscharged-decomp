/*************************************************************************
 * Open Dynamics Engine, Copyright (C) 2001-2004 Russell L. Smith.
 * Distributed under the BSD-style license in LICENSES/ODE-BSD.txt.
 *************************************************************************/

#ifndef ODE_JOINT_INTERNAL_H
#define ODE_JOINT_INTERNAL_H

#include "objects.h"
#include "obstack.h"
#include <ode/contact.h>

enum
{
    dJOINT_INGROUP = 1,
    dJOINT_REVERSE = 2,
    dJOINT_TWOBODIES = 4,
};

struct dxJointNode
{
    dxJoint* joint;
    dxBody* body;
    dxJointNode* next;
};

struct dxJoint : public dObject
{
    struct Info1
    {
        int m;
        int nub;
    };

    struct Info2
    {
        dReal fps;
        dReal erp;
        dReal* J1l;
        dReal* J1a;
        dReal* J2l;
        dReal* J2a;
        int rowskip;
        dReal* c;
        dReal* cfm;
        dReal* lo;
        dReal* hi;
        int* findex;
    };

    typedef void init_fn(dxJoint* joint);
    typedef void getInfo1_fn(dxJoint* joint, Info1* info);
    typedef void getInfo2_fn(dxJoint* joint, Info2* info);

    struct Vtable
    {
        int size;
        init_fn* init;
        getInfo1_fn* getInfo1;
        getInfo2_fn* getInfo2;
        int typenum;
    };

    Vtable* vtable;
    int flags;
    dxJointNode node[2];
    dJointFeedback* feedback;
    dReal lambda[6];
};

struct dxJointGroup : public dBase
{
    int num;
    dObStack stack;
};

struct dxJointContact : public dxJoint
{
    int the_m;
    dContact contact;
};

extern dxJoint::Vtable __dcontact_vtable;

#endif

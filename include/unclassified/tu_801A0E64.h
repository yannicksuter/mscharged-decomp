#ifndef UNCLASSIFIED_TU_801A0E64_H
#define UNCLASSIFIED_TU_801A0E64_H

#include "Game/Physics/PhysicsSphere.h"
#include "NL/nlMath.h"
#include "types.h"

class cFielder;
class DrawableObject;
class UnidentifiedHammerObjectMember030;
typedef DrawableObject RenderObject;

struct HammerObject;

class PhysicsSphere_801700D8 : public PhysicsSphere
{
public:
    virtual ~PhysicsSphere_801700D8();

    /* 0x38 */ HammerObject* _038;
};

struct HammerObject
{
    /* 0x00 */ nlQuaternion _000;
    /* 0x10 */ float _010;
    /* 0x14 */ float _014;
    /* 0x18 */ float _018;
    /* 0x1C */ float _01C;
    /* 0x20 */ u32 _020;
    /* 0x24 */ bool _024;
    /* 0x25 */ bool _025;
    /* 0x26 */ u8 _026[2];
    /* 0x28 */ PhysicsSphere_801700D8* _028;
    /* 0x2C */ RenderObject* _02C;
    /* 0x30 */ UnidentifiedHammerObjectMember030* _030;
    /* 0x34 */ cFielder* _034;
    /* 0x38 */ nlVector3 _038;
    /* 0x44 */ float _044;
    /* 0x48 */ float _048;
};

extern "C"
{
    HammerObject* fn_801A0E64(HammerObject*, int, float);
    HammerObject* fn_801A10D0(HammerObject*, int);
    const nlVector3* fn_801A1168(const HammerObject*);
    void fn_801A1170(HammerObject*, const nlVector3*);
    void fn_801A117C(HammerObject*, const nlVector3*);
    const nlQuaternion* fn_801A1298(HammerObject*);
    void fn_801A1650(HammerObject*, float);
    void fn_801A16A4(HammerObject*, float);
    void fn_801A1B54(HammerObject*, cFielder*);
    void fn_801A1CFC(HammerObject*, int);
    void fn_801A1ED0(HammerObject*, bool);
}

#endif // UNCLASSIFIED_TU_801A0E64_H

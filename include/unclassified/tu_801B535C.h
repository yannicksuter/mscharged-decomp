#ifndef UNCLASSIFIED_TU_801B535C_H
#define UNCLASSIFIED_TU_801B535C_H

#include "NL/nlMath.h"
#include "Game/Drawable/RenderObject.h"

class cFielder;
class EmissionController;
class PhysicsYoshiEgg;

class UnidentifiedObject_801B535C
{
public:
    UnidentifiedObject_801B535C(RenderObject*);
    ~UnidentifiedObject_801B535C();

    void fn_801B54AC(bool, float);
    void fn_801B5544(float);
    static void fn_801B57D8(EmissionController&);
    void fn_801B5858(cFielder*);
    void fn_801B59DC(bool);
    float fn_801B5B30() const;
    void fn_801B5B38(float);
    void fn_801B5D14();
    void fn_801B5DD0();
    void fn_801B5DD8(const nlVector3&, float);

    /* 0x00 */ nlQuaternion mUnidentified00;
    /* 0x10 */ nlVector3 mUnidentified10;
    /* 0x1C */ float mUnidentified1C;
    /* 0x20 */ float mUnidentified20;
    /* 0x24 */ float mUnidentified24;
    /* 0x28 */ bool mUnidentified28;
    /* 0x2C */ PhysicsYoshiEgg* mUnidentified2C;
    /* 0x30 */ RenderObject* mUnidentified30;
    /* 0x34 */ cFielder* mUnidentified34;
    /* 0x38 */ float mUnidentified38;
    /* 0x3C */ nlVector3 mUnidentified3C;
    /* 0x48 */ float mUnidentified48;
}; // size: 0x4C

#endif

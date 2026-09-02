#ifndef TU_80176EF4_H
#define TU_80176EF4_H

#include "Game/Physics/PhysicsBox.h"

struct ThwompObject;

class PhysicsBox_80176EF4;
extern PhysicsBox_80176EF4* lbl_806E12E0;

class PhysicsBox_80176EF4 : public PhysicsBox
{
public:
    PhysicsBox_80176EF4(
        ThwompObject* object, float lx, float ly, float lz);
    virtual ~PhysicsBox_80176EF4();

    virtual int GetObjectType() const { return 0x24; }
    virtual bool SetContactInfo(dContact*, PhysicsObject*, bool);
    virtual void PostUpdate();
    virtual void PreCollide();
    virtual ContactType Contact(PhysicsObject*, dContact*, int);

    static void* operator new(unsigned long)
    {
        PhysicsBox_80176EF4* object;
        if (lbl_806E12E0 == 0)
        {
            object = 0;
        }
        else
        {
            object = lbl_806E12E0;
            lbl_806E12E0 = *(PhysicsBox_80176EF4**)lbl_806E12E0;
        }
        return object;
    }

    static void operator delete(void* object)
    {
        *(PhysicsBox_80176EF4**)object = lbl_806E12E0;
        lbl_806E12E0 = (PhysicsBox_80176EF4*)object;
    }

    /* 0x38 */ float mUnidentified038;
    /* 0x3C */ ThwompObject* mUnidentified03C;
}; // size: 0x40

#endif // TU_80176EF4_H

#ifndef GAME_PHYSICS_PHYSICS_COMPOSITE_OBJECT_H
#define GAME_PHYSICS_PHYSICS_COMPOSITE_OBJECT_H

#include "Game/Physics/PhysicsObject.h"
#include "Game/Physics/PhysicsTransform.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlMath.h"

class PhysicsCompositeObject : public PhysicsObject
{
public:
    PhysicsCompositeObject(PhysicsWorld*);
    virtual ~PhysicsCompositeObject();

    virtual void Unknown0();
    virtual int GetObjectType() const;

    int AddObject(PhysicsObject*);
    void AdjustTransform(int, nlMatrix4&, bool);
    PhysicsTransform* GetComponent(unsigned int i)
    {
        nlDLListIterator<PhysicsTransform*> current = m_Components.Begin();
        for (unsigned int count = 0; count < i; ++count)
        {
            if (nlDLRingIsEnd(current.m_Head, current.m_Curr))
            {
                return 0;
            }
            current.next();
        }
        return *current;
    }

    /* 0x38 */ nlDLListContainer<PhysicsTransform*> m_Components;
    /* 0x40 */ int numComponents;
}; // size: 0x44

#endif

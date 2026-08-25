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
    virtual int GetObjectType() const { return 9; }

    void AdjustTransform(int, nlMatrix4&, bool);
    int AddObject(PhysicsObject*);
    PhysicsTransform* GetComponent(unsigned int i)
    {
        DLListEntry<PhysicsTransform*>* current;
        DLListEntry<PhysicsTransform*>* head;
        nlDLListIterator<PhysicsTransform*> iterator = m_Components.Begin();

        head = iterator.m_Head;
        current = iterator.m_Curr;

        for (unsigned int count = 0; count < i; ++count)
        {
            if (nlDLRingIsEnd(head, current))
            {
                return 0;
            }
            if (nlDLRingIsEnd(head, current) || current == 0)
            {
                current = 0;
            }
            else
            {
                current = current->m_next;
            }
        }

        return current->entry;
    }

    /* 0x38 */ nlDLListContainer<PhysicsTransform*> m_Components;
    /* 0x40 */ int numComponents;
}; // size: 0x44

#endif

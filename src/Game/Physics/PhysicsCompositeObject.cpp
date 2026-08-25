#include "Game/Physics/PhysicsCompositeObject.h"

#include "NL/nlMemory.h"

inline void* operator new(unsigned long, void* memory)
{
    return memory;
}

PhysicsCompositeObject::PhysicsCompositeObject(PhysicsWorld* physicsWorld)
    : PhysicsObject(physicsWorld)
{
    numComponents = 0;
    dBodySetData(m_bodyID, this);
}

void PhysicsCompositeObject::Unknown0()
{
    PhysicsObject::Unknown0();

    nlDLListIterator<PhysicsTransform*> current = m_Components.Begin();
    DLListEntry<PhysicsTransform*>* head = current.m_Head;
    DLListEntry<PhysicsTransform*>* entry = current.m_Curr;
    while (!nlDLRingIsEnd(head, entry))
    {
        entry->entry->Unknown0();
        if (nlDLRingIsEnd(head, entry) || entry == 0)
        {
            entry = 0;
        }
        else
        {
            entry = entry->m_next;
        }
    }
}

PhysicsCompositeObject::~PhysicsCompositeObject()
{
    nlDLListIterator<PhysicsTransform*> current = m_Components.Begin();
    while (current.hasNext())
    {
        PhysicsTransform* physObj = *current;
        physObj->m_bodyID = 0;
        delete physObj;
        current.next();
    }
}

int PhysicsCompositeObject::AddObject(PhysicsObject* object)
{
    object->MakeStatic();
    PhysicsTransform* transform
        = new (nlMalloc(sizeof(PhysicsTransform), 8, false)) PhysicsTransform();
    transform->Attach(object, this);

    DLListEntry<PhysicsTransform*>* entry
        = (DLListEntry<PhysicsTransform*>*)nlMalloc(
            sizeof(DLListEntry<PhysicsTransform*>), 8, false);
    if (entry != 0)
    {
        entry->m_next = 0;
        entry->m_prev = 0;
        entry->entry = transform;
    }
    nlDLRingAddEnd<DLListEntry<PhysicsTransform*> >(
        &m_Components.m_Head, entry);

    ++numComponents;
    return numComponents - 1;
}

void PhysicsCompositeObject::AdjustTransform(
    int i, nlMatrix4& m, bool param3)
{
    GetComponent(i)->SetSubObjectTransform(m,
        (PhysicsObject::CoordinateType)param3);
}

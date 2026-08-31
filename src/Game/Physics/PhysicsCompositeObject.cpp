#include "Game/Physics/PhysicsCompositeObject.h"

#include "Game/Physics/PhysicsWorld.h"
#include "NL/nlMemory.h"

PhysicsCompositeObject::PhysicsCompositeObject(PhysicsWorld* physicsWorld)
    : PhysicsObject(physicsWorld)
{
    m_Components.m_Head = 0;
    numComponents = 0;
    dBodySetData(m_bodyID, this);
}

void PhysicsCompositeObject::Unknown0()
{
    PhysicsObject::Unknown0();

    nlDLListIterator<PhysicsTransform*> iterator;
    iterator = m_Components.Begin();
    DLListEntry<PhysicsTransform*>* head = iterator.m_Head;
    DLListEntry<PhysicsTransform*>* current = iterator.m_Curr;
    while (!nlDLRingIsEnd(head, current))
    {
        current->entry->Unknown0();
        if (nlDLRingIsEnd(head, current) || current == 0)
        {
            current = 0;
        }
        else
        {
            current = current->m_next;
        }
    }
}

PhysicsCompositeObject::~PhysicsCompositeObject()
{
    nlDLListIterator<PhysicsTransform*> iterator = m_Components.Begin();
    DLListEntry<PhysicsTransform*>* head = iterator.m_Head;
    DLListEntry<PhysicsTransform*>* current = iterator.m_Curr;

    while (current != 0)
    {
        PhysicsTransform* transform = current->entry;
        transform->m_bodyID = 0;
        delete transform;

        if (nlDLRingIsEnd(head, current) || current == 0)
        {
            current = 0;
        }
        else
        {
            current = current->m_next;
        }
    }
}

int PhysicsCompositeObject::AddObject(PhysicsObject* object)
{
    object->MakeStatic();
    PhysicsTransform* transform = new (
        nlMalloc(sizeof(PhysicsTransform), 8, false)) PhysicsTransform();

    transform->Attach(object, this);

    DLListEntry<PhysicsTransform*>* entry =
        (DLListEntry<PhysicsTransform*>*)nlMalloc(
            sizeof(DLListEntry<PhysicsTransform*>), 8, false);

    if (entry != 0)
    {
        entry->m_next = 0;
        entry->m_prev = 0;
        entry->entry = transform;
    }

    nlDLRingAddEnd(&m_Components.m_Head, entry);
    numComponents++;
    return numComponents - 1;
}

void PhysicsCompositeObject::AdjustTransform(
    int i, nlMatrix4& m, bool type)
{
    GetComponent(i)->SetSubObjectTransform(
        m, (PhysicsObject::CoordinateType)type);
}

#ifndef GAME_EVENT_REGISTRY_H
#define GAME_EVENT_REGISTRY_H

#include "Game/Event.h"
#include "NL/nlAVLTree.h"

struct EventRegistryValue
{
    UnidentifiedEventBase* event;
    void* type;
};

typedef AVLTreeBase<unsigned int, EventRegistryValue,
    NewAdapter<AVLTreeEntry<unsigned int, EventRegistryValue> >,
    DefaultKeyCompare<unsigned int> >
    EventRegistry;

extern EventRegistry* g_pEventRegistry;

template <typename T>
inline UnidentifiedTypedEvent<T>* UnidentifiedFindEvent(const char* name, int length)
{
    unsigned int hash = HashEventName(name, length);
    EventRegistryValue* value = 0;
    g_pEventRegistry->Find(hash, &value, 0);
    return value != 0 ? (UnidentifiedTypedEvent<T>*)value->event : 0;
}

#endif // GAME_EVENT_REGISTRY_H

#ifndef GAME_EVENT_REGISTRY_H
#define GAME_EVENT_REGISTRY_H

#include "Game/Event.h"
#include "NL/nlAVLTree.h"

struct EventRegistryValue
{
    EventBase* event;
    void* type;
};

typedef AVLTreeBase<unsigned int, EventRegistryValue,
    NewAdapter<AVLTreeEntry<unsigned int, EventRegistryValue> >,
    DefaultKeyCompare<unsigned int> >
    EventRegistry;

extern EventRegistry* g_pEventRegistry;

template <typename T>
inline typename UnidentifiedEventInterface<T>::Type* UnidentifiedFindEvent(const char* name, int length)
{
    unsigned int hash = HashEventName(name, length);
    EventRegistryValue* value = 0;
    g_pEventRegistry->Find(hash, &value, 0);
    return value != 0 ? (typename UnidentifiedEventInterface<T>::Type*)value->event : 0;
}

#endif // GAME_EVENT_REGISTRY_H

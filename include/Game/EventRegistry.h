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

#endif // GAME_EVENT_REGISTRY_H

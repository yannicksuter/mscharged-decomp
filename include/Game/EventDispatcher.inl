#ifndef GAME_EVENT_DISPATCHER_INL
#define GAME_EVENT_DISPATCHER_INL

#include "Game/Task/DispatchEventsTask.h"

inline EventDispatcher::EventDispatcher()
{
}

// See UnidentifiedDispatcherHeaderScope in DispatchEventsTask.h.
struct UnidentifiedDispatcherInlineScope
{
    ~UnidentifiedDispatcherInlineScope()
    {
    }
};

inline EventDispatcher::~EventDispatcher()
{
    BasicSlotPool<DLListEntry<EventCallback> >* pool = &callbacks.m_Allocator;
    pool->FreeBlocks();
}

template <typename T>
void EventDispatcherBase<T>::Dispatch(bool flag)
{
    state.fields.dispatching = 1;
    int count;
    do
    {
        count = state.fields.callbackCount;
        state.fields.callbackCount = 0;
        while (count != 0 && !state.fields.stopDispatch)
        {
            (*callbacks.Begin())(true);
            callbacks.DeleteEntry(nlDLRingRemoveStart(&callbacks.m_Head));
            count--;
        }
    } while (!flag && state.fields.callbackCount != 0);

    while (count != 0)
    {
        callbacks.DeleteEntry(nlDLRingRemoveStart(&callbacks.m_Head));
        state.fields.callbackCount--;
    }

    state.fields.dispatching = 0;
    state.fields.stopDispatch = 0;
}

template <typename T>
void EventDispatcherBase<T>::Clear()
{
    if (!state.fields.dispatching)
    {
        nlDLListIterator<T> iterator = callbacks.Begin();
        while (iterator.hasNext())
        {
            (*iterator)(false);
            iterator.next();
        }

        callbacks.Clear();
        state.fields.callbackCount = 0;
    }
}

template <typename T>
void EventDispatcherBase<T>::Add(const T& callback)
{
    callbacks.AddEnd(callback);
    state.fields.callbackCount++;
}

#endif // GAME_EVENT_DISPATCHER_INL

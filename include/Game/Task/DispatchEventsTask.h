#ifndef GAME_TASK_DISPATCH_EVENTS_TASK_H
#define GAME_TASK_DISPATCH_EVENTS_TASK_H

#include "types.h"

#include "NL/nlSlotPool.h"
#include "NL/nlTask.h"

#include "NL/nlFunction.h"

typedef Function<bool> EventCallback;

template <typename T>
struct EventCallbackEntry
{
    EventCallbackEntry* next;
    EventCallbackEntry* previous;
    T callback;
};

template <typename T>
class DeferredSlotPool : public SlotPoolBase
{
public:
    DeferredSlotPool(int initial, int delta)
        : SlotPoolBase()
    {
        m_Delta = delta;
        m_Initial = initial;
        if (m_Initial == 0)
        {
            BaseAddNewBlock(this, sizeof(T));
        }
    }
};

template <typename T>
class EventCallbackList
{
public:
    EventCallbackList()
        : pool(16, 16)
        , head(0)
    {
    }

    DeferredSlotPool<EventCallbackEntry<T> > pool;
    EventCallbackEntry<T>* head;
};

class EventDispatcherBase
{
public:
    EventDispatcherBase()
        : callbacks()
        , state(0)
    {
    }

    virtual void Dispatch(bool) = 0;
    virtual void Clear() = 0;
    virtual ~EventDispatcherBase();
    virtual void Add(const EventCallback&) = 0;

    EventCallbackList<EventCallback> callbacks;
    u32 state;
};

class EventDispatcher : public EventDispatcherBase
{
public:
    EventDispatcher() { }
    EventDispatcher(const char*);

    virtual void Dispatch(bool);
    virtual void Clear();
    virtual ~EventDispatcher();
    virtual void Add(const EventCallback&);
};

class DispatchEventsTask : public nlTask
{
public:
    virtual void Run(float);
    virtual const char* GetName()
    {
        return "Dispatch Events";
    }

    EventDispatcher dispatcher;
};

extern DispatchEventsTask* gDispatchEventsTask;

#endif // GAME_TASK_DISPATCH_EVENTS_TASK_H

#ifndef GAME_TASK_DISPATCH_EVENTS_TASK_H
#define GAME_TASK_DISPATCH_EVENTS_TASK_H

#include "types.h"

#include "NL/nlDLListContainer.h"
#include "NL/nlFunction.h"
#include "NL/nlSlotPool.h"
#include "NL/nlTask.h"

typedef Function<bool> EventCallback;
typedef DLListContainerBase<EventCallback,
    BasicSlotPool<DLListEntry<EventCallback> > > EventCallbackList;

union EventDispatcherState
{
    struct
    {
        u32 dispatching : 1;
        u32 stopDispatch : 1;
        u32 callbackCount : 16;
        u32 unused : 14;
    } fields;
    u32 value;

    EventDispatcherState()
        : value(0)
    {
    }
};

class EventDispatcherBase
{
public:
    EventDispatcherBase()
        : callbacks(16, 16)
        , state()
    {
    }

    virtual void Dispatch(bool);
    virtual void Clear();
    virtual ~EventDispatcherBase() { }
    virtual void Add(const EventCallback&);

    EventCallbackList callbacks;
    EventDispatcherState state;
};

class EventDispatcher : public EventDispatcherBase
{
public:
    EventDispatcher() { }
    EventDispatcher(const char*);

    virtual ~EventDispatcher();
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

#include "NL/nlSlotPool.h"
#include "NL/nlTask.h"
#include "NL/nlMemory.h"

#include "types.h"

// These descriptive dispatcher names are not present in the stripped retail
// executable. Their interface and layout follow from the vtables and callers.
struct EventCallback
{
    u32 kind;
    void* target;
};

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

extern "C" void fn_802B467C(void*);

inline void* operator new(unsigned long, void* memory)
{
    return memory;
}

DispatchEventsTask* gDispatchEventsTask;

extern "C" void fn_80115F10()
{
    gDispatchEventsTask =
        new (nlMalloc(sizeof(DispatchEventsTask), 8, false)) DispatchEventsTask;
}

extern "C" void fn_80115FB4()
{
    gDispatchEventsTask->dispatcher.Clear();

    DeferredSlotPool<EventCallbackEntry<EventCallback> >* pool =
        &gDispatchEventsTask->dispatcher.callbacks.pool;
    fn_802B467C(pool);
    SlotPoolBase::BaseFreeBlocks(pool, sizeof(EventCallbackEntry<EventCallback>));
}

void DispatchEventsTask::Run(float)
{
    dispatcher.Dispatch(true);
}

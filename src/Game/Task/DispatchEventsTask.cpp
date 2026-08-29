#include "Game/Task/DispatchEventsTask.h"

#include "NL/nlMemory.h"

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

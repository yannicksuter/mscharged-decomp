#include "Game/Task/DispatchEventsTask.h"

#include "NL/nlMemory.h"

extern "C" void fn_802B467C(void*);

DispatchEventsTask* gDispatchEventsTask;

extern "C" void fn_80115F10()
{
    gDispatchEventsTask =
        new (nlMalloc(sizeof(DispatchEventsTask), 8, false)) DispatchEventsTask;
}

extern "C" void fn_80115FB4()
{
    gDispatchEventsTask->dispatcher.Clear();

    BasicSlotPool<DLListEntry<EventCallback> >* pool =
        &gDispatchEventsTask->dispatcher.callbacks.m_Allocator;
    fn_802B467C(pool);
    SlotPoolBase::BaseFreeBlocks(pool, sizeof(DLListEntry<EventCallback>));
}

void DispatchEventsTask::Run(float)
{
    dispatcher.Dispatch(true);
}

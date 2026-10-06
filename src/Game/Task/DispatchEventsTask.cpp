#include "NL/nlDLListContainer.inl"
#include "Game/Task/DispatchEventsTask.h"
#include "Game/EventDispatcher.inl"

#include "NL/nlMemory.h"

DispatchEventsTask* gDispatchEventsTask;

void InitializeDispatchEventsTask()
{
    gDispatchEventsTask =
        new (nlMalloc(sizeof(DispatchEventsTask), 8, false)) DispatchEventsTask;
}

void fn_80115FB4()
{
    gDispatchEventsTask->dispatcher.Clear();

    gDispatchEventsTask->dispatcher.FreeBlocks();
}

void DispatchEventsTask::Run(float)
{
    dispatcher.Dispatch(true);
}

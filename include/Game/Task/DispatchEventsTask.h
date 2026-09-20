#ifndef GAME_TASK_DISPATCH_EVENTS_TASK_H
#define GAME_TASK_DISPATCH_EVENTS_TASK_H

#include "types.h"

#include "NL/nlDLListContainer.h"
#include "NL/nlFunction.h"
#include "NL/nlSlotPool.h"
#include "NL/nlTask.h"

typedef Function<bool> EventCallback;

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

// A class template: retail compiles Dispatch, Clear and Add after the vtables
// of the unit that instantiates them (in Game/Task/FixedUpdateTask.cpp the
// pointer-to-member constant of Clear, 0x80507D20, follows all four), which
// only the end-of-unit instantiation of template members does. The parameter
// list is unidentified; the callback type is the only one the members use.
template <typename T>
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
    virtual ~EventDispatcherBase()
    {
    }
    virtual void Add(const T&);

    DLListContainerBase<T, SlotPool<DLListEntry<T> > > callbacks;
    EventDispatcherState state;
};

class EventDispatcher : public EventDispatcherBase<EventCallback>
{
public:
    EventDispatcher();
    EventDispatcher(const char*);

    virtual ~EventDispatcher();
};

// Two objects with empty destructors, one declared here and one in
// EventDispatcher.inl, are constructed by FixedUpdateTask::Reset().
//
// Game/Task/FixedUpdateTask.cpp is built with per-file code sections, and a
// section is created by the first function generated from its file. Retail
// keeps that unit's dispatcher code in the order EventDispatcher.inl
// (~EventDispatcher, Dispatch, Clear, Add), this header
// (~EventDispatcherBase), nlDLListContainer.h (DeleteEntry). The
// FixedUpdateTask constructor generates those destructors innermost first,
// container, then base, then derived, and retail confirms that order with the
// unit's pointer-to-member constants: 0x80507BD8 belongs to the base
// destructor and 0x80507BE4 to the derived one. Each dispatcher file had
// therefore already produced a function before the constructor was compiled,
// and the link kept no byte of either.
//
// The shape is fixed by the compiler, not by us. Of the constructs that make
// fully inlined code emit a function, only an object with a user-declared
// empty destructor leaves nothing behind in its caller. That is the profile of
// a debug facility whose body was compiled out of the release build, as with
// UnidentifiedStaticStorage.
//
// Still unidentified, hence the names: what the two types were, their real
// spelling, and which function declared them. Any non-empty function compiled
// before the constructor reproduces the layout; Reset() is where the unit
// first operates on its dispatcher.
struct UnidentifiedDispatcherHeaderScope
{
    ~UnidentifiedDispatcherHeaderScope()
    {
    }
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


void fn_80115F10();
void fn_80115FB4();

#endif // GAME_TASK_DISPATCH_EVENTS_TASK_H

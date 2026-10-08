#ifndef GAME_EVENT_H
#define GAME_EVENT_H

#include "NL/nlArrayAllocator.h"
#include "NL/nlBind.h"
#include "NL/nlDLListContainer.h"

unsigned int HashEventName(const char* name, int length);
void RegisterEvent(void* event, void* eventType);
void UnregisterEvent(void* event);
void RegisterEventConnection(void* event, void* connection, unsigned int owner, int group);
void* FindEventConnection(void* event, void* owner);
void UnregisterEventConnection(void* event, void* connection);

void PushEventConnectionState();
void PopEventConnectionState();
void DisconnectEventOwner(void* owner);

class EventBase
{
public:
    EventBase(const char* name, int length)
        : mHash(HashEventName(name, length))
    {
    }

    virtual inline ~EventBase();
    virtual void Disconnect(void* owner) = 0;

    friend void RegisterEvent(void*, void*);
    friend void UnregisterEvent(void*);

protected:
    unsigned int mHash;
    void* mCurrentConnection;
};

#include "Game/Task/DispatchEventsTask.h"

struct EventConnection
{
    EventConnection()
        : mOwner(0)
        , mEvent(0)
    {
        mFlags |= 0xC0000000;
        mFlags &= ~0x20000000;
    }

    ~EventConnection();

    // The owner's first word holds the live connection pointer.
    void* mOwner;
    void* mEvent;
    union
    {
        unsigned int mFlags;
        struct
        {
            unsigned int : 2;
            unsigned int mPendingRemoval : 1;
            unsigned int : 29;
        };
        struct
        {
            unsigned short mFlagsHigh;
            unsigned short mGroupCount : 16;
        };
    };
};

// Events without a payload use a zero-argument callback signature.
typedef void NoEventData();

template <typename T>
struct EventCallbackTraits
{
    typedef Function<T*> Type;
    typedef T* Parameter;
};

template <typename P1>
struct EventCallbackTraits<void(P1)>
{
    typedef Function<void(P1)> Type;
    typedef P1 Parameter;
};

template <typename P1, typename P2>
struct EventCallbackTraits<void(P1, P2)>
{
    typedef Function<void(P1, P2)> Type;
};

template <typename ReturnType>
struct EventCallbackTraits<ReturnType()>
{
    typedef Function<ReturnType()> Type;
};

template <>
struct EventCallbackTraits<NoEventData>
{
    typedef Function<FnVoidVoid> Type;
    typedef NoEventData* Parameter;
};

template <typename T>
struct EventListener : public EventConnection
{
    typedef typename EventCallbackTraits<T>::Type Callback;

    EventListener(int = 0)
        : EventConnection()
        , callback()
    {
    }

    Callback callback;
};

template <typename T>
class EventType
{
protected:
    static void* sType;
};

template <typename T>
void* EventType<T>::sType;

template <typename T>
class TypedEvent;

template <typename T>
class TypedEvent : public EventBase, public EventType<T>
{
public:
    typedef typename EventCallbackTraits<T>::Type Callback;

    TypedEvent(const char* name, int length)
        : EventBase(name, length)
    {
        this->mCurrentConnection = 0;
        sType = *(void**)this;
    }

    virtual void Disconnect(void* owner) = 0;
    virtual void Add(const Callback&, unsigned int, int) = 0;

};

template <typename ReturnType>
class TypedEvent0 : public EventBase, public EventType<ReturnType()>
{
public:
    typedef typename EventCallbackTraits<ReturnType()>::Type Callback;

    TypedEvent0(const char* name, int length)
        : EventBase(name, length)
    {
        this->mCurrentConnection = 0;
        sType = *(void**)this;
    }

    virtual inline ~TypedEvent0();
    virtual void Disconnect(void* owner) = 0;
    virtual void Add(const Callback&, unsigned int, int) = 0;

};

template <typename T>
struct EventInterface
{
    typedef TypedEvent<T> Type;
};

template <typename ReturnType>
struct EventInterface<ReturnType()>
{
    typedef TypedEvent0<ReturnType> Type;
};

template <typename T>
typename EventInterface<T>::Type* FindEvent(const char* name, int length);

template <typename ReturnType>
class Event0 : public TypedEvent0<ReturnType>
{
    typedef EventListener<ReturnType()> Listener;
    typedef DLListEntry<Listener> ListenerEntry;

public:
    typedef typename TypedEvent0<ReturnType>::Callback Callback;

    Event0(const char* name, int length)
        : TypedEvent0<ReturnType>(name, length)
        , mListeners(16, 16)
    {
        RegisterEvent(this, TypedEvent0<ReturnType>::sType);
    }

    virtual ~Event0();

    void RemoveAll()
    {
        while (mListeners.m_Head != 0)
        {
            Remove(&*mListeners.Begin());
        }
    }

    virtual void Add(const Callback& callback, unsigned int value, int flags)
    {
        AddListener(callback, value, flags);
    }

    virtual void Disconnect(void* owner);

    void Deliver()
    {
        nlDLListIterator<Listener> iterator;
        iterator = mListeners.Begin();
        while (iterator.hasNext())
        {
            Listener* listener = &*iterator;
            ListenerEntry* currentEntry = iterator.CurrentEntry();
            this->mCurrentConnection = listener;

            if ((listener->mFlags >> 31) != 0)
            {
                listener->callback();
                RestartAt(iterator, currentEntry);
            }

            iterator.next();
            if (((listener->mFlags >> 29) & 1) != 0)
            {
                nlDLListIterator<Listener> position;
                position = mListeners.Begin(
                    (ListenerEntry*)((char*)listener - 8));
                ListenerEntry* entry = position.CurrentEntry();
                nlDLRingRemove(&mListeners.m_Head, entry);
                mListeners.DeleteEntry(entry);
            }
        }
        this->mCurrentConnection = 0;
    }

protected:
    // Add hands the listener its callback by reference: retail's copies
    // clear the caller's Function instead of cloning it.
    void AddListener(
        const Callback& callback, unsigned int value, int flags)
    {
        Listener* listener = mListeners.AllocateAtEnd(0);

        listener->callback.TransferFrom(callback);
        RegisterEventConnection(this, listener, value, flags);
    }

    void Remove(Listener* listener);
    ListenerEntry* GetEntry(Listener* listener);
    void DeleteListener(Listener* listener);
    void RestartAt(nlDLListIterator<Listener>& iterator, ListenerEntry* current);

public:
    // The listener list runs a single Clear()/FreeBlocks() teardown, so it is
    // the plain container rather than nlDLListSlotPool, whose destructor tears
    // down twice (see Game/Render/ImpostorCharacter.cpp). Its adapter is the
    // SlotPool level, like EventDispatcher's callback list.
    DLListContainerBase<Listener, SlotPool<ListenerEntry> > mListeners;
};

template <typename ReturnType>
Event0<ReturnType>::~Event0()
{
    RemoveAll();
    UnregisterEvent(this);
}

// The callback may have changed the list while it ran, so the walk is
// re-anchored on the current list head before it continues past the entry
// that was just delivered.
template <typename ReturnType>
void Event0<ReturnType>::RestartAt(
    nlDLListIterator<Listener>& iterator, ListenerEntry* current)
{
    iterator.Copy(mListeners.Begin());
    iterator.m_Curr = current;
}

template <typename ReturnType>
void Event0<ReturnType>::Remove(Listener* listener)
{
    UnregisterEventConnection(this, listener);
    if (this->mCurrentConnection == listener)
    {
        listener->mPendingRemoval = 1;
        return;
    }

    DeleteListener(listener);
}

template <typename ReturnType>
DLListEntry<EventListener<ReturnType()> >*
Event0<ReturnType>::GetEntry(Listener* listener)
{
    return mListeners.Begin((ListenerEntry*)((char*)listener - 8)).CurrentEntry();
}

template <typename ReturnType>
void Event0<ReturnType>::DeleteListener(Listener* listener)
{
    ListenerEntry* entry = GetEntry(listener);
    nlDLRingRemove(&mListeners.m_Head, entry);
    mListeners.DeleteEntry(entry);
}

template <typename ReturnType>
void Event0<ReturnType>::Disconnect(void* owner)
{
    Listener* listener = (Listener*)FindEventConnection(this, owner);
    Remove(listener);
}

template <typename ReturnType>
class QueuedEventBase0 : public Event0<ReturnType>
{
public:
    QueuedEventBase0(
        EventDispatcher* dispatcher, const char* name, int length)
        : Event0<ReturnType>(name, length)
        , mDispatcher(dispatcher)
    {
    }

    virtual ~QueuedEventBase0() { }

    typedef typename Event0<ReturnType>::Callback Callback;

    virtual void Add(const typename Event0<ReturnType>::Callback& callback,
        unsigned int value, int flags)
    {
        this->AddListener(callback, value, flags);
    }

    void Dispatch(Callback disposer, unsigned char deliver)
    {
        if (deliver)
        {
            this->Deliver();
        }

        if (disposer)
        {
            disposer();
        }
    }

protected:
    EventDispatcher* mDispatcher;
};

template <typename T>
class Event : public TypedEvent<T>
{
    typedef EventListener<T> Listener;
    typedef DLListEntry<Listener> ListenerEntry;

public:
    typedef typename TypedEvent<T>::Callback Callback;

    Event(const char* name, int length)
        : TypedEvent<T>(name, length)
        , mListeners(16, 16)
    {
        RegisterEvent(this, TypedEvent<T>::sType);
    }

    virtual ~Event();

    void RemoveAll()
    {
        while (mListeners.m_Head != 0)
        {
            Remove(&*mListeners.Begin());
        }
    }

    virtual void Add(const Callback& callback, unsigned int value, int flags)
    {
        AddListener(callback, value, flags);
    }

    virtual void Disconnect(void* owner);

    void Deliver(T* data)
    {
        nlDLListIterator<Listener> iterator;
        iterator = mListeners.Begin();
        while (iterator.hasNext())
        {
            Listener* listener = &*iterator;
            ListenerEntry* currentEntry = iterator.CurrentEntry();
            this->mCurrentConnection = listener;

            if ((listener->mFlags >> 31) != 0)
            {
                listener->callback(data);
                RestartAt(iterator, currentEntry);
            }

            iterator.next();
            if (((listener->mFlags >> 29) & 1) != 0)
            {
                nlDLListIterator<Listener> position;
                position = mListeners.Begin(
                    (ListenerEntry*)((char*)listener - 8));
                ListenerEntry* entry = position.CurrentEntry();
                nlDLRingRemove(&mListeners.m_Head, entry);
                mListeners.DeleteEntry(entry);
            }
        }
        this->mCurrentConnection = 0;
    }

    void Deliver()
    {
        nlDLListIterator<Listener> iterator;
        iterator = mListeners.Begin();
        while (iterator.hasNext())
        {
            Listener* listener = &*iterator;
            ListenerEntry* currentEntry = iterator.CurrentEntry();
            this->mCurrentConnection = listener;

            if ((listener->mFlags >> 31) != 0)
            {
                listener->callback();
                RestartAt(iterator, currentEntry);
            }

            iterator.next();
            if (((listener->mFlags >> 29) & 1) != 0)
            {
                nlDLListIterator<Listener> position;
                position = mListeners.Begin(
                    (ListenerEntry*)((char*)listener - 8));
                ListenerEntry* entry = position.CurrentEntry();
                nlDLRingRemove(&mListeners.m_Head, entry);
                mListeners.DeleteEntry(entry);
            }
        }
        this->mCurrentConnection = 0;
    }

protected:
    // Add hands the listener its callback by reference: retail's copies
    // clear the caller's Function instead of cloning it.
    void AddListener(
        const Callback& callback, unsigned int value, int flags)
    {
        Listener* listener = mListeners.AllocateAtEnd(0);

        listener->callback.TransferFrom(callback);
        RegisterEventConnection(this, listener, value, flags);
    }

    void Remove(Listener* listener);
    ListenerEntry* GetEntry(Listener* listener);
    void DeleteListener(Listener* listener);
    void RestartAt(nlDLListIterator<Listener>& iterator, ListenerEntry* current);

public:
    // The listener list runs a single Clear()/FreeBlocks() teardown, so it is
    // the plain container rather than nlDLListSlotPool, whose destructor tears
    // down twice (see Game/Render/ImpostorCharacter.cpp). Its adapter is the
    // SlotPool level, like EventDispatcher's callback list.
    DLListContainerBase<Listener, SlotPool<ListenerEntry> > mListeners;
};

template <typename T>
Event<T>::~Event()
{
    RemoveAll();
    UnregisterEvent(this);
}

// The callback may have changed the list while it ran, so the walk is
// re-anchored on the current list head before it continues past the entry
// that was just delivered.
template <typename T>
void Event<T>::RestartAt(
    nlDLListIterator<Listener>& iterator, ListenerEntry* current)
{
    iterator.Copy(mListeners.Begin());
    iterator.m_Curr = current;
}

template <typename T>
void Event<T>::Remove(Listener* listener)
{
    UnregisterEventConnection(this, listener);
    if (this->mCurrentConnection == listener)
    {
        listener->mPendingRemoval = 1;
        return;
    }

    DeleteListener(listener);
}

template <typename T>
DLListEntry<EventListener<T> >*
Event<T>::GetEntry(Listener* listener)
{
    return mListeners.Begin((ListenerEntry*)((char*)listener - 8)).CurrentEntry();
}

template <typename T>
void Event<T>::DeleteListener(Listener* listener)
{
    ListenerEntry* entry = GetEntry(listener);
    nlDLRingRemove(&mListeners.m_Head, entry);
    mListeners.DeleteEntry(entry);
}

template <typename T>
void Event<T>::Disconnect(void* owner)
{
    Listener* listener = (Listener*)FindEventConnection(this, owner);
    Remove(listener);
}

// Concrete immediate events retain their own runtime type while sharing the
// listener storage and delivery implementation.
template <typename T>
class ImmediateEvent : public Event<T>
{
public:
    ImmediateEvent(const char* name, int length)
        : Event<T>(name, length)
    {
    }

    virtual ~ImmediateEvent() { }
};

template <typename P1, typename P2, typename P3>
struct EventListener3 : public EventConnection
{
    EventListener3(int = 0)
        : EventConnection()
        , callback()
    {
    }

    Function<void(P1, P2, P3)> callback;
};

template <typename P1, typename P2, typename P3>
class TypedEvent3 : public EventBase
{
public:
    typedef Function<void(P1, P2, P3)> Callback;

    TypedEvent3(const char* name, int length)
        : EventBase(name, length)
    {
        this->mCurrentConnection = 0;
        sType = *(void**)this;
    }

    virtual ~TypedEvent3() { }
    virtual void Disconnect(void* owner) = 0;
    virtual void Add(Callback, unsigned int, int) = 0;

protected:
    static void* sType;

};

template <typename P1, typename P2, typename P3>
void* TypedEvent3<P1, P2, P3>::sType;

// Retail queued-event destructors inline one more non-trivial destructor
// level than Event's own copies. This base stores the dispatcher
// before the derived vtable is installed; its own vtable is not retained.
template <typename T>
class QueuedEventBase : public Event<T>
{
public:
    QueuedEventBase(
        EventDispatcher* dispatcher, const char* name, int length)
        : Event<T>(name, length)
        , mDispatcher(dispatcher)
    {
    }

    virtual ~QueuedEventBase() { }

    typedef typename Event<T>::Callback Callback;

    virtual void Add(const typename Event<T>::Callback& callback,
        unsigned int value, int flags)
    {
        this->AddListener(callback, value, flags);
    }

    void Dispatch(T* data, Function<T*> disposer, unsigned char deliver)
    {
        if (deliver)
        {
            this->Deliver(data);
        }

        if (disposer)
        {
            disposer(data);
        }
    }

    void Dispatch(Callback disposer, unsigned char deliver)
    {
        if (deliver)
        {
            this->Deliver();
        }

        if (disposer)
        {
            disposer();
        }
    }

protected:
    EventDispatcher* mDispatcher;
};

template <typename T>
class QueuedEvent : public QueuedEventBase<T>
{
public:
    typedef typename Event<T>::Callback Callback;

    QueuedEvent(EventDispatcher* dispatcher, const char* name, int length)
        : QueuedEventBase<T>(dispatcher, name, length)
    {
    }

    virtual ~QueuedEvent() { }

    void Queue(T* data, const Function<T*>& disposer);
    void Queue(const Callback& disposer);
    void Queue() { Queue(Callback()); }
};

template <typename T>
void QueuedEvent<T>::Queue(T* data, const Function<T*>& disposer)
{
    typedef void (QueuedEventBase<T>::*DispatchFunction)(
        T*, Function<T*>, unsigned char);
    Function<bool> callback(
        Bind<void>(MemFun((DispatchFunction)&QueuedEventBase<T>::Dispatch),
            this, data, disposer, placeholder0));
    this->mDispatcher->Add(callback);
}

template <typename T>
void QueuedEvent<T>::Queue(const Callback& disposer)
{
    typedef void (QueuedEventBase<T>::*DispatchFunction)(
        Callback, unsigned char);
    Function<bool> callback(
        Bind<void>(MemFun((DispatchFunction)&QueuedEventBase<T>::Dispatch),
            this, disposer, placeholder0));
    this->mDispatcher->Add(callback);
}

template <typename P1, typename P2>
class Event2 : public TypedEvent<void(P1, P2)>
{
    typedef EventListener<void(P1, P2)> Listener;
    typedef DLListEntry<Listener> ListenerEntry;

public:
    typedef typename TypedEvent<void(P1, P2)>::Callback Callback;

    Event2(const char* name, int length)
        : TypedEvent<void(P1, P2)>(name, length)
        , mListeners(16, 16)
    {
        RegisterEvent(this, TypedEvent<void(P1, P2)>::sType);
    }

    virtual ~Event2();

    void RemoveAll()
    {
        while (mListeners.m_Head != 0)
        {
            Remove(&*mListeners.Begin());
        }
    }

    virtual void Add(const Callback& callback, unsigned int value, int flags)
    {
        AddListener(callback, value, flags);
    }

    virtual void Disconnect(void* owner);

    void Deliver(P1 p1, P2 p2)
    {
        nlDLListIterator<Listener> iterator;
        iterator = mListeners.Begin();
        while (iterator.hasNext())
        {
            Listener* listener = &*iterator;
            ListenerEntry* currentEntry = iterator.CurrentEntry();
            this->mCurrentConnection = listener;

            if ((listener->mFlags >> 31) != 0)
            {
                listener->callback(p1, p2);
                RestartAt(iterator, currentEntry);
            }

            iterator.next();
            if (((listener->mFlags >> 29) & 1) != 0)
            {
                nlDLListIterator<Listener> position;
                position = mListeners.Begin(
                    (ListenerEntry*)((char*)listener - 8));
                ListenerEntry* entry = position.CurrentEntry();
                nlDLRingRemove(&mListeners.m_Head, entry);
                entry->~ListenerEntry();
                mListeners.m_Allocator.Free(entry);
            }
        }
        this->mCurrentConnection = 0;
    }


protected:
    // Add hands the listener its callback by reference: retail's copies
    // clear the caller's Function instead of cloning it.
    void AddListener(
        const Callback& callback, unsigned int value, int flags)
    {
        Listener* listener = mListeners.AllocateAtEnd(0);

        listener->callback.TransferFrom(callback);
        RegisterEventConnection(this, listener, value, flags);
    }

    void Remove(Listener* listener);
    ListenerEntry* GetEntry(Listener* listener);
    void DeleteListener(Listener* listener);
    void RestartAt(nlDLListIterator<Listener>& iterator, ListenerEntry* current);

public:
    // The listener list runs a single Clear()/FreeBlocks() teardown, so it is
    // the plain container rather than nlDLListSlotPool, whose destructor tears
    // down twice (see Game/Render/ImpostorCharacter.cpp). Its adapter is the
    // SlotPool level, like EventDispatcher's callback list.
    DLListContainerBase<Listener, SlotPool<ListenerEntry> > mListeners;
};

template <typename P1, typename P2>
Event2<P1, P2>::~Event2()
{
    RemoveAll();
    UnregisterEvent(this);
}

// The callback may have changed the list while it ran, so the walk is
// re-anchored on the current list head before it continues past the entry
// that was just delivered.
template <typename P1, typename P2>
void Event2<P1, P2>::RestartAt(
    nlDLListIterator<Listener>& iterator, ListenerEntry* current)
{
    iterator.Copy(mListeners.Begin());
    iterator.m_Curr = current;
}

template <typename P1, typename P2>
void Event2<P1, P2>::Remove(Listener* listener)
{
    UnregisterEventConnection(this, listener);
    if (this->mCurrentConnection == listener)
    {
        listener->mPendingRemoval = 1;
        return;
    }

    DeleteListener(listener);
}

template <typename P1, typename P2>
DLListEntry<EventListener<void(P1, P2)> >*
Event2<P1, P2>::GetEntry(Listener* listener)
{
    return mListeners.Begin((ListenerEntry*)((char*)listener - 8)).CurrentEntry();
}

template <typename P1, typename P2>
void Event2<P1, P2>::DeleteListener(Listener* listener)
{
    ListenerEntry* entry = GetEntry(listener);
    nlDLRingRemove(&mListeners.m_Head, entry);
    mListeners.DeleteEntry(entry);
}

template <typename P1, typename P2>
void Event2<P1, P2>::Disconnect(void* owner)
{
    Listener* listener = (Listener*)FindEventConnection(this, owner);
    Remove(listener);
}


template <typename P1, typename P2>
class ImmediateEvent<void(P1, P2)> : public Event2<P1, P2>
{
public:
    ImmediateEvent(const char* name, int length)
        : Event2<P1, P2>(name, length)
    {
    }
    virtual ~ImmediateEvent() { }
};

template <typename ReturnType>
class ImmediateEvent<ReturnType()> : public Event0<ReturnType>
{
public:
    ImmediateEvent(const char* name, int length)
        : Event0<ReturnType>(name, length)
    {
    }
    virtual ~ImmediateEvent() { }
};

template <typename ReturnType>
class QueuedEvent<ReturnType()> : public QueuedEventBase0<ReturnType>
{
public:
    typedef typename Event0<ReturnType>::Callback Callback;
    QueuedEvent(EventDispatcher* dispatcher, const char* name, int length)
        : QueuedEventBase0<ReturnType>(dispatcher, name, length)
    {
    }
    virtual ~QueuedEvent() { }
    void Queue(const Callback& disposer);
    void Queue() { Queue(Callback()); }
};

template <typename ReturnType>
void QueuedEvent<ReturnType()>::Queue(const Callback& disposer)
{
    typedef void (QueuedEventBase0<ReturnType>::*DispatchFunction)(
        Callback, unsigned char);
    Function<bool> callback(
        Bind<void>(MemFun((DispatchFunction)&QueuedEventBase0<ReturnType>::Dispatch),
            this, disposer, placeholder0));
    this->mDispatcher->Add(callback);
}

#endif // GAME_EVENT_H

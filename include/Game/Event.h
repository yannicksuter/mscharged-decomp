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
            unsigned short mUnidentified08;
            unsigned short mGroupCount : 16;
        };
    };
};

// Events without a payload use a zero-argument callback signature.
typedef void UnidentifiedEventNoData();

template <typename T>
struct UnidentifiedEventCallback
{
    typedef Function<T*> Type;
    typedef T* Parameter;
};

template <typename P1>
struct UnidentifiedEventCallback<void(P1)>
{
    typedef Function<void(P1)> Type;
    typedef P1 Parameter;
};

template <typename P1, typename P2>
struct UnidentifiedEventCallback<void(P1, P2)>
{
    typedef Function<void(P1, P2)> Type;
};

template <typename ReturnType>
struct UnidentifiedEventCallback<ReturnType()>
{
    typedef Function<ReturnType()> Type;
};

template <>
struct UnidentifiedEventCallback<UnidentifiedEventNoData>
{
    typedef Function<FnVoidVoid> Type;
    typedef UnidentifiedEventNoData* Parameter;
};

template <typename T>
struct UnidentifiedListener : public EventConnection
{
    typedef typename UnidentifiedEventCallback<T>::Type Callback;

    UnidentifiedListener(int = 0)
        : EventConnection()
        , callback()
    {
    }

    Callback callback;
};

template <typename T>
class UnidentifiedEventType
{
protected:
    static void* sType;
};

template <typename T>
void* UnidentifiedEventType<T>::sType;

template <typename T>
class UnidentifiedTypedEvent;

template <typename T>
class UnidentifiedTypedEvent : public EventBase, public UnidentifiedEventType<T>
{
public:
    typedef typename UnidentifiedEventCallback<T>::Type Callback;

    UnidentifiedTypedEvent(const char* name, int length)
        : EventBase(name, length)
    {
        this->mCurrentConnection = 0;
        sType = *(void**)this;
    }

    virtual void Disconnect(void* owner) = 0;
    virtual void Add(const Callback&, unsigned int, int) = 0;

};

template <typename ReturnType>
class UnidentifiedTypedEvent0 : public EventBase, public UnidentifiedEventType<ReturnType()>
{
public:
    typedef typename UnidentifiedEventCallback<ReturnType()>::Type Callback;

    UnidentifiedTypedEvent0(const char* name, int length)
        : EventBase(name, length)
    {
        this->mCurrentConnection = 0;
        sType = *(void**)this;
    }

    virtual inline ~UnidentifiedTypedEvent0();
    virtual void Disconnect(void* owner) = 0;
    virtual void Add(const Callback&, unsigned int, int) = 0;

};

template <typename T>
struct UnidentifiedEventInterface
{
    typedef UnidentifiedTypedEvent<T> Type;
};

template <typename ReturnType>
struct UnidentifiedEventInterface<ReturnType()>
{
    typedef UnidentifiedTypedEvent0<ReturnType> Type;
};

template <typename T>
typename UnidentifiedEventInterface<T>::Type* UnidentifiedFindEvent(const char* name, int length);

template <typename ReturnType>
class UnidentifiedEvent0 : public UnidentifiedTypedEvent0<ReturnType>
{
    typedef UnidentifiedListener<ReturnType()> Listener;
    typedef DLListEntry<Listener> ListenerEntry;

public:
    typedef typename UnidentifiedTypedEvent0<ReturnType>::Callback Callback;

    UnidentifiedEvent0(const char* name, int length)
        : UnidentifiedTypedEvent0<ReturnType>(name, length)
        , mListeners(16, 16)
    {
        RegisterEvent(this, UnidentifiedTypedEvent0<ReturnType>::sType);
    }

    virtual ~UnidentifiedEvent0();

    void RemoveAll()
    {
        while (mListeners.m_Head != 0)
        {
            Remove(&*mListeners.Begin());
        }
    }

    virtual void Add(const Callback& callback, unsigned int value, int flags)
    {
        UnidentifiedAddListener(callback, value, flags);
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
    void UnidentifiedAddListener(
        const Callback& callback, unsigned int value, int flags)
    {
        Listener* listener = mListeners.AllocateAtEnd(0);

        listener->callback.UnidentifiedTransfer(callback);
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
UnidentifiedEvent0<ReturnType>::~UnidentifiedEvent0()
{
    RemoveAll();
    UnregisterEvent(this);
}

// The callback may have changed the list while it ran, so the walk is
// re-anchored on the current list head before it continues past the entry
// that was just delivered.
template <typename ReturnType>
void UnidentifiedEvent0<ReturnType>::RestartAt(
    nlDLListIterator<Listener>& iterator, ListenerEntry* current)
{
    iterator.Copy(mListeners.Begin());
    iterator.m_Curr = current;
}

template <typename ReturnType>
void UnidentifiedEvent0<ReturnType>::Remove(Listener* listener)
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
DLListEntry<UnidentifiedListener<ReturnType()> >*
UnidentifiedEvent0<ReturnType>::GetEntry(Listener* listener)
{
    return mListeners.Begin((ListenerEntry*)((char*)listener - 8)).CurrentEntry();
}

template <typename ReturnType>
void UnidentifiedEvent0<ReturnType>::DeleteListener(Listener* listener)
{
    ListenerEntry* entry = GetEntry(listener);
    nlDLRingRemove(&mListeners.m_Head, entry);
    mListeners.DeleteEntry(entry);
}

template <typename ReturnType>
void UnidentifiedEvent0<ReturnType>::Disconnect(void* owner)
{
    Listener* listener = (Listener*)FindEventConnection(this, owner);
    Remove(listener);
}

template <typename ReturnType>
class UnidentifiedQueuedEventBase0 : public UnidentifiedEvent0<ReturnType>
{
public:
    UnidentifiedQueuedEventBase0(
        EventDispatcher* dispatcher, const char* name, int length)
        : UnidentifiedEvent0<ReturnType>(name, length)
        , mDispatcher(dispatcher)
    {
    }

    virtual ~UnidentifiedQueuedEventBase0() { }

    typedef typename UnidentifiedEvent0<ReturnType>::Callback Callback;

    virtual void Add(const typename UnidentifiedEvent0<ReturnType>::Callback& callback,
        unsigned int value, int flags)
    {
        this->UnidentifiedAddListener(callback, value, flags);
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
class UnidentifiedEvent : public UnidentifiedTypedEvent<T>
{
    typedef UnidentifiedListener<T> Listener;
    typedef DLListEntry<Listener> ListenerEntry;

public:
    typedef typename UnidentifiedTypedEvent<T>::Callback Callback;

    UnidentifiedEvent(const char* name, int length)
        : UnidentifiedTypedEvent<T>(name, length)
        , mListeners(16, 16)
    {
        RegisterEvent(this, UnidentifiedTypedEvent<T>::sType);
    }

    virtual ~UnidentifiedEvent();

    void RemoveAll()
    {
        while (mListeners.m_Head != 0)
        {
            Remove(&*mListeners.Begin());
        }
    }

    virtual void Add(const Callback& callback, unsigned int value, int flags)
    {
        UnidentifiedAddListener(callback, value, flags);
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
    void UnidentifiedAddListener(
        const Callback& callback, unsigned int value, int flags)
    {
        Listener* listener = mListeners.AllocateAtEnd(0);

        listener->callback.UnidentifiedTransfer(callback);
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
UnidentifiedEvent<T>::~UnidentifiedEvent()
{
    RemoveAll();
    UnregisterEvent(this);
}

// The callback may have changed the list while it ran, so the walk is
// re-anchored on the current list head before it continues past the entry
// that was just delivered.
template <typename T>
void UnidentifiedEvent<T>::RestartAt(
    nlDLListIterator<Listener>& iterator, ListenerEntry* current)
{
    iterator.Copy(mListeners.Begin());
    iterator.m_Curr = current;
}

template <typename T>
void UnidentifiedEvent<T>::Remove(Listener* listener)
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
DLListEntry<UnidentifiedListener<T> >*
UnidentifiedEvent<T>::GetEntry(Listener* listener)
{
    return mListeners.Begin((ListenerEntry*)((char*)listener - 8)).CurrentEntry();
}

template <typename T>
void UnidentifiedEvent<T>::DeleteListener(Listener* listener)
{
    ListenerEntry* entry = GetEntry(listener);
    nlDLRingRemove(&mListeners.m_Head, entry);
    mListeners.DeleteEntry(entry);
}

template <typename T>
void UnidentifiedEvent<T>::Disconnect(void* owner)
{
    Listener* listener = (Listener*)FindEventConnection(this, owner);
    Remove(listener);
}

// Concrete immediate events retain their own runtime type while sharing the
// listener storage and delivery implementation.
template <typename T>
class ImmediateEvent : public UnidentifiedEvent<T>
{
public:
    ImmediateEvent(const char* name, int length)
        : UnidentifiedEvent<T>(name, length)
    {
    }

    virtual ~ImmediateEvent() { }
};

template <typename P1, typename P2, typename P3>
struct UnidentifiedListener3 : public EventConnection
{
    UnidentifiedListener3(int = 0)
        : EventConnection()
        , callback()
    {
    }

    Function<void(P1, P2, P3)> callback;
};

template <typename P1, typename P2, typename P3>
class UnidentifiedTypedEvent3 : public EventBase
{
public:
    typedef Function<void(P1, P2, P3)> Callback;

    UnidentifiedTypedEvent3(const char* name, int length)
        : EventBase(name, length)
    {
        this->mCurrentConnection = 0;
        sType = *(void**)this;
    }

    virtual ~UnidentifiedTypedEvent3() { }
    virtual void Disconnect(void* owner) = 0;
    virtual void Add(Callback, unsigned int, int) = 0;

protected:
    static void* sType;

};

template <typename P1, typename P2, typename P3>
void* UnidentifiedTypedEvent3<P1, P2, P3>::sType;

// Retail queued-event destructors inline one more non-trivial destructor
// level than UnidentifiedEvent's own copies. This base stores the dispatcher
// before the derived vtable is installed; its own vtable is not retained.
template <typename T>
class UnidentifiedQueuedEventBase : public UnidentifiedEvent<T>
{
public:
    UnidentifiedQueuedEventBase(
        EventDispatcher* dispatcher, const char* name, int length)
        : UnidentifiedEvent<T>(name, length)
        , mDispatcher(dispatcher)
    {
    }

    virtual ~UnidentifiedQueuedEventBase() { }

    typedef typename UnidentifiedEvent<T>::Callback Callback;

    virtual void Add(const typename UnidentifiedEvent<T>::Callback& callback,
        unsigned int value, int flags)
    {
        this->UnidentifiedAddListener(callback, value, flags);
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
class UnidentifiedQueuedEvent : public UnidentifiedQueuedEventBase<T>
{
public:
    typedef typename UnidentifiedEvent<T>::Callback Callback;

    UnidentifiedQueuedEvent(EventDispatcher* dispatcher, const char* name, int length)
        : UnidentifiedQueuedEventBase<T>(dispatcher, name, length)
    {
    }

    virtual ~UnidentifiedQueuedEvent() { }

    void Queue(T* data, const Function<T*>& disposer);
    void Queue(const Callback& disposer);
    void Queue() { Queue(Callback()); }
};

template <typename T>
void UnidentifiedQueuedEvent<T>::Queue(T* data, const Function<T*>& disposer)
{
    typedef void (UnidentifiedQueuedEventBase<T>::*DispatchFunction)(
        T*, Function<T*>, unsigned char);
    Function<bool> callback(
        Bind<void>(MemFun((DispatchFunction)&UnidentifiedQueuedEventBase<T>::Dispatch),
            this, data, disposer, placeholder0));
    this->mDispatcher->Add(callback);
}

template <typename T>
void UnidentifiedQueuedEvent<T>::Queue(const Callback& disposer)
{
    typedef void (UnidentifiedQueuedEventBase<T>::*DispatchFunction)(
        Callback, unsigned char);
    Function<bool> callback(
        Bind<void>(MemFun((DispatchFunction)&UnidentifiedQueuedEventBase<T>::Dispatch),
            this, disposer, placeholder0));
    this->mDispatcher->Add(callback);
}

template <typename P1, typename P2>
class UnidentifiedEvent2 : public UnidentifiedTypedEvent<void(P1, P2)>
{
    typedef UnidentifiedListener<void(P1, P2)> Listener;
    typedef DLListEntry<Listener> ListenerEntry;

public:
    typedef typename UnidentifiedTypedEvent<void(P1, P2)>::Callback Callback;

    UnidentifiedEvent2(const char* name, int length)
        : UnidentifiedTypedEvent<void(P1, P2)>(name, length)
        , mListeners(16, 16)
    {
        RegisterEvent(this, UnidentifiedTypedEvent<void(P1, P2)>::sType);
    }

    virtual ~UnidentifiedEvent2();

    void RemoveAll()
    {
        while (mListeners.m_Head != 0)
        {
            Remove(&*mListeners.Begin());
        }
    }

    virtual void Add(const Callback& callback, unsigned int value, int flags)
    {
        UnidentifiedAddListener(callback, value, flags);
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
    void UnidentifiedAddListener(
        const Callback& callback, unsigned int value, int flags)
    {
        Listener* listener = mListeners.AllocateAtEnd(0);

        listener->callback.UnidentifiedTransfer(callback);
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
UnidentifiedEvent2<P1, P2>::~UnidentifiedEvent2()
{
    RemoveAll();
    UnregisterEvent(this);
}

// The callback may have changed the list while it ran, so the walk is
// re-anchored on the current list head before it continues past the entry
// that was just delivered.
template <typename P1, typename P2>
void UnidentifiedEvent2<P1, P2>::RestartAt(
    nlDLListIterator<Listener>& iterator, ListenerEntry* current)
{
    iterator.Copy(mListeners.Begin());
    iterator.m_Curr = current;
}

template <typename P1, typename P2>
void UnidentifiedEvent2<P1, P2>::Remove(Listener* listener)
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
DLListEntry<UnidentifiedListener<void(P1, P2)> >*
UnidentifiedEvent2<P1, P2>::GetEntry(Listener* listener)
{
    return mListeners.Begin((ListenerEntry*)((char*)listener - 8)).CurrentEntry();
}

template <typename P1, typename P2>
void UnidentifiedEvent2<P1, P2>::DeleteListener(Listener* listener)
{
    ListenerEntry* entry = GetEntry(listener);
    nlDLRingRemove(&mListeners.m_Head, entry);
    mListeners.DeleteEntry(entry);
}

template <typename P1, typename P2>
void UnidentifiedEvent2<P1, P2>::Disconnect(void* owner)
{
    Listener* listener = (Listener*)FindEventConnection(this, owner);
    Remove(listener);
}


template <typename P1, typename P2>
class ImmediateEvent<void(P1, P2)> : public UnidentifiedEvent2<P1, P2>
{
public:
    ImmediateEvent(const char* name, int length)
        : UnidentifiedEvent2<P1, P2>(name, length)
    {
    }
    virtual ~ImmediateEvent() { }
};

template <typename ReturnType>
class ImmediateEvent<ReturnType()> : public UnidentifiedEvent0<ReturnType>
{
public:
    ImmediateEvent(const char* name, int length)
        : UnidentifiedEvent0<ReturnType>(name, length)
    {
    }
    virtual ~ImmediateEvent() { }
};

template <typename ReturnType>
class UnidentifiedQueuedEvent<ReturnType()> : public UnidentifiedQueuedEventBase0<ReturnType>
{
public:
    typedef typename UnidentifiedEvent0<ReturnType>::Callback Callback;
    UnidentifiedQueuedEvent(EventDispatcher* dispatcher, const char* name, int length)
        : UnidentifiedQueuedEventBase0<ReturnType>(dispatcher, name, length)
    {
    }
    virtual ~UnidentifiedQueuedEvent() { }
    void Queue(const Callback& disposer);
    void Queue() { Queue(Callback()); }
};

template <typename ReturnType>
void UnidentifiedQueuedEvent<ReturnType()>::Queue(const Callback& disposer)
{
    typedef void (UnidentifiedQueuedEventBase0<ReturnType>::*DispatchFunction)(
        Callback, unsigned char);
    Function<bool> callback(
        Bind<void>(MemFun((DispatchFunction)&UnidentifiedQueuedEventBase0<ReturnType>::Dispatch),
            this, disposer, placeholder0));
    this->mDispatcher->Add(callback);
}

#endif // GAME_EVENT_H

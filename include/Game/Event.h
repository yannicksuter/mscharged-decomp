#ifndef GAME_EVENT_H
#define GAME_EVENT_H

#include "Game/Task/DispatchEventsTask.h"
#include "NL/nlBind.h"
#include "NL/nlDLListContainer.h"

extern "C" unsigned int fn_802B289C(const char*, int);
extern "C" void fn_802B2940(void*, void*);
extern "C" void fn_802B29C4(void*);
extern "C" void fn_802B2A04(void*, void*, unsigned int, int, void*);
extern "C" void* fn_802B28E0(void*);
extern "C" void fn_802B2CC8(void*, void*);

class UnidentifiedEventBase
{
public:
    UnidentifiedEventBase(const char* name, int length)
        : mHash(fn_802B289C(name, length))
    {
    }

    virtual ~UnidentifiedEventBase() { }
    virtual void Disconnect() = 0;

protected:
    unsigned int mHash;
    void* mCurrentConnection;
};

struct UnidentifiedConnection
{
    UnidentifiedConnection()
        : mEvent(0)
        , mTarget(0)
    {
        mFlags |= 0xC0000000;
        mFlags &= ~0x20000000;
    }

    ~UnidentifiedConnection();

    void* mEvent;
    void* mTarget;
    unsigned int mFlags;
};

template <typename T>
struct UnidentifiedListener : public UnidentifiedConnection
{
    UnidentifiedListener(int = 0)
        : UnidentifiedConnection()
        , callback()
    {
    }

    Function<T*> callback;
};

template <typename T>
class UnidentifiedTypedEvent : public UnidentifiedEventBase
{
public:
    UnidentifiedTypedEvent(const char* name, int length)
        : UnidentifiedEventBase(name, length)
    {
        this->mCurrentConnection = 0;
        sType = *(void**)this;
    }

    virtual ~UnidentifiedTypedEvent() { }
    virtual void Disconnect() = 0;
    virtual void Add(Function<T*>&, unsigned int, int) = 0;

protected:
    static void* sType;
};

template <typename T>
void* UnidentifiedTypedEvent<T>::sType;

template <typename T>
class UnidentifiedEvent : public UnidentifiedTypedEvent<T>
{
    typedef UnidentifiedListener<T> Listener;
    typedef DLListEntry<Listener> ListenerEntry;

public:
    UnidentifiedEvent(const char* name, int length)
        : UnidentifiedTypedEvent<T>(name, length)
        , mListeners(16, 16)
    {
        fn_802B2940(this, UnidentifiedTypedEvent<T>::sType);
    }

    virtual ~UnidentifiedEvent()
    {
        while (mListeners.m_Head != 0)
        {
            Listener* listener = &mListeners.Begin().CurrentEntry()->entry;
            Remove(listener);
        }
        fn_802B29C4(this);
    }

    virtual void Disconnect();

    virtual void Add(Function<T*>& callback, unsigned int value, int flags)
    {
        Listener* listener = mListeners.AllocateAtEnd(0);

        void* target = callback.UnidentifiedTarget();
        listener->callback.UnidentifiedTransfer(callback);
        fn_802B2A04(this, listener, value, flags, target);
    }

    void Dispatch(T* data, Function<T*> disposer, bool deliver)
    {
        if (deliver)
        {
            nlDLListIterator<Listener> iterator = mListeners.Begin();
            while (iterator.hasNext())
            {
                UnidentifiedConnection* connection = &iterator.CurrentEntry()->entry;
                Listener* listener = (Listener*)connection;
                this->mCurrentConnection = connection;

                if ((connection->mFlags >> 31) != 0)
                {
                    listener->callback(data);
                    iterator = mListeners.Begin();
                    iterator.m_Curr = (ListenerEntry*)((char*)connection - 8);
                }

                iterator.next();
                if (((connection->mFlags >> 29) & 1) != 0)
                {
                    ListenerEntry* entry = mListeners.Begin(
                                                         (ListenerEntry*)((char*)connection - 8))
                                               .CurrentEntry();
                    nlDLRingRemove(&mListeners.m_Head, entry);
                    entry->~ListenerEntry();
                    mListeners.m_Allocator.Free(entry);
                }
            }
            this->mCurrentConnection = 0;
        }

        if (disposer)
        {
            disposer(data);
        }
    }

protected:
    void Remove(Listener* listener);

    // The listener list runs a single Clear()/FreeBlocks() teardown, so it is
    // the plain container over a slot-pool adapter rather than nlDLListSlotPool,
    // whose destructor tears down twice (see Game/Render/ImpostorCharacter.cpp).
    DLListContainerBase<Listener, BasicSlotPool<ListenerEntry> > mListeners;
};

template <typename T>
void UnidentifiedEvent<T>::Remove(Listener* listener)
{
    fn_802B2CC8(this, listener);
    if (this->mCurrentConnection == listener)
    {
        listener->mFlags |= 0x20000000;
        return;
    }

    ListenerEntry* entry =
        mListeners.Begin((ListenerEntry*)((char*)listener - 8)).CurrentEntry();
    nlDLRingRemove(&mListeners.m_Head, entry);
    entry->~ListenerEntry();
    mListeners.m_Allocator.Free(entry);
}

template <typename T>
void UnidentifiedEvent<T>::Disconnect()
{
    Listener* listener = (Listener*)fn_802B28E0(this);
    Remove(listener);
}

template <typename T>
class UnidentifiedQueuedEvent : public UnidentifiedEvent<T>
{
public:
    UnidentifiedQueuedEvent(EventDispatcher*, const char*, int);

    virtual ~UnidentifiedQueuedEvent();

    virtual void Add(Function<T*>& callback, unsigned int value, int flags)
    {
        UnidentifiedEvent<T>::Add(callback, value, flags);
    }

    void Queue(T* data, const Function<T*>& disposer)
    {
        Function<bool> callback(
            Bind<void>(MemFun(&UnidentifiedEvent<T>::Dispatch), this, data, disposer, placeholder0));
        mDispatcher->Add(callback);
    }

private:
    EventDispatcher* mDispatcher;
};

template <typename T>
UnidentifiedQueuedEvent<T>::UnidentifiedQueuedEvent(
    EventDispatcher* dispatcher, const char* name, int length)
    : UnidentifiedEvent<T>(name, length)
    , mDispatcher(dispatcher)
{
}

template <typename T>
UnidentifiedQueuedEvent<T>::~UnidentifiedQueuedEvent()
{
}

#endif // GAME_EVENT_H

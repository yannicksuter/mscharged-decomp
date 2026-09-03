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

    void UnidentifiedDeliver(T* data)
    {
        nlDLListIterator<Listener> iterator = mListeners.Begin();
        while (iterator.hasNext())
        {
            Listener* listener = &*iterator;
            ListenerEntry* currentEntry = iterator.CurrentEntry();
            this->mCurrentConnection = listener;

            if ((listener->mFlags >> 31) != 0)
            {
                listener->callback(data);
                iterator = mListeners.Begin();
                iterator.m_Curr = currentEntry;
            }

            iterator.next();
            if (((listener->mFlags >> 29) & 1) != 0)
            {
                UnidentifiedDeleteListener(listener);
            }
        }
        this->mCurrentConnection = 0;
    }

    void Dispatch(T* data, Function<T*> disposer, bool deliver)
    {
        if (deliver)
        {
            UnidentifiedDeliver(data);
        }

        if (disposer)
        {
            disposer(data);
        }
    }

protected:
    void Remove(Listener* listener);
    ListenerEntry* UnidentifiedGetEntry(Listener* listener);
    void UnidentifiedDeleteListener(Listener* listener);

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

    UnidentifiedDeleteListener(listener);
}

template <typename T>
DLListEntry<UnidentifiedListener<T> >*
UnidentifiedEvent<T>::UnidentifiedGetEntry(Listener* listener)
{
    return mListeners.Begin((ListenerEntry*)((char*)listener - 8)).CurrentEntry();
}

template <typename T>
void UnidentifiedEvent<T>::UnidentifiedDeleteListener(Listener* listener)
{
    ListenerEntry* entry = UnidentifiedGetEntry(listener);
    nlDLRingRemove(&mListeners.m_Head, entry);
    mListeners.DeleteEntry(entry);
}

template <typename T>
void UnidentifiedEvent<T>::Disconnect()
{
    Listener* listener = (Listener*)fn_802B28E0(this);
    Remove(listener);
}

template <typename T, int Count>
class UnidentifiedStaticSlotPool
{
public:
    UnidentifiedStaticSlotPool(int, int)
        : mFreeList((T*)mStorage)
        , mEntries((T*)mStorage)
    {
        for (int i = 0; i < Count - 1; ++i)
        {
            *(T**)&mEntries[i] = &mEntries[i + 1];
        }
        *(T**)&mEntries[Count - 1] = 0;
    }

    void Allocate(T*& out)
    {
        out = mFreeList;
        if (mFreeList != 0)
        {
            mFreeList = *(T**)mFreeList;
            new (out) T;
        }
    }

    void DeleteEntry(T* entry)
    {
        *(T**)entry = mFreeList;
        mFreeList = entry;
    }

    /* 0x00 */ T* mFreeList;
    /* 0x04 */ T* mEntries;
    /* 0x08 */ u8 mStorage[sizeof(T) * Count];
};

template <typename T, int Count>
class UnidentifiedStaticEvent : public UnidentifiedTypedEvent<T>
{
    typedef UnidentifiedListener<T> Listener;
    typedef DLListEntry<Listener> ListenerEntry;
    typedef UnidentifiedStaticSlotPool<ListenerEntry, Count> ListenerPool;

public:
    UnidentifiedStaticEvent(const char* name, int length)
        : UnidentifiedTypedEvent<T>(name, length)
        , mListeners(Count, Count)
    {
        fn_802B2940(this, UnidentifiedTypedEvent<T>::sType);
    }

    virtual ~UnidentifiedStaticEvent()
    {
        while (mListeners.m_Head != 0)
        {
            Listener* listener = &mListeners.Begin().CurrentEntry()->entry;
            Remove(listener);
        }
        fn_802B29C4(this);
    }

    virtual void Disconnect()
    {
        Listener* listener = (Listener*)fn_802B28E0(this);
        Remove(listener);
    }

    virtual void Add(Function<T*>& callback, unsigned int value, int flags)
    {
        Listener* listener = mListeners.AllocateAtEnd(0);

        void* target = callback.UnidentifiedTarget();
        listener->callback.UnidentifiedTransfer(callback);
        fn_802B2A04(this, listener, value, flags, target);
    }

    void UnidentifiedDeliver(T* data)
    {
        nlDLListIterator<Listener> iterator = mListeners.Begin();
        while (iterator.hasNext())
        {
            Listener* listener = &*iterator;
            ListenerEntry* currentEntry = iterator.CurrentEntry();
            this->mCurrentConnection = listener;

            if ((listener->mFlags >> 31) != 0)
            {
                listener->callback(data);
                iterator = mListeners.Begin();
                iterator.m_Curr = currentEntry;
            }

            iterator.next();
            if (((listener->mFlags >> 29) & 1) != 0)
            {
                UnidentifiedDeleteListener(listener);
            }
        }
        this->mCurrentConnection = 0;
    }

protected:
    void Remove(Listener* listener)
    {
        fn_802B2CC8(this, listener);
        if (this->mCurrentConnection == listener)
        {
            listener->mFlags |= 0x20000000;
            return;
        }
        UnidentifiedDeleteListener(listener);
    }

    ListenerEntry* UnidentifiedGetEntry(Listener* listener)
    {
        return mListeners.Begin(
            (ListenerEntry*)((char*)listener - 8)).CurrentEntry();
    }

    void UnidentifiedDeleteListener(Listener* listener)
    {
        ListenerEntry* entry = UnidentifiedGetEntry(listener);
        nlDLRingRemove(&mListeners.m_Head, entry);
        mListeners.DeleteEntry(entry);
    }

    DLListContainerBase<Listener, ListenerPool> mListeners;
};

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

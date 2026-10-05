#ifndef GAME_UNIDENTIFIED_STATIC_EVENT_H
#define GAME_UNIDENTIFIED_STATIC_EVENT_H

#include "Game/Event.h"

// An event whose listeners come from a fixed-size pool. Its virtual members
// are defined out of class, in a file apart from the typed-event base, so a
// unit emits them with its own event's destructor, after its vtables.
template <typename T, int Count>
class UnidentifiedStaticEvent : public UnidentifiedTypedEvent<T>
{
    typedef UnidentifiedListener<T> Listener;
    typedef DLListEntry<Listener> ListenerEntry;
    typedef nlStaticArrayAllocator<ListenerEntry, Count> ListenerPool;

public:
    typedef typename UnidentifiedTypedEvent<T>::Callback Callback;

    UnidentifiedStaticEvent(const char* name, int length)
        : UnidentifiedTypedEvent<T>(name, length)
        , mListeners()
    {
        RegisterEvent(this, UnidentifiedTypedEvent<T>::sType);
    }

    virtual ~UnidentifiedStaticEvent();

    void RemoveAll()
    {
        while (mListeners.m_Head != 0)
        {
            Remove(&*mListeners.Begin());
        }
    }

    virtual void Disconnect(void* owner);
    virtual void Add(const Callback& callback, unsigned int value, int flags);

    void Deliver(typename UnidentifiedEventCallback<T>::Parameter data)
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
                entry->~ListenerEntry();
                mListeners.m_Allocator.Free(entry);
            }
        }
        this->mCurrentConnection = 0;
    }

protected:
    void RestartAt(nlDLListIterator<Listener>& iterator, ListenerEntry* current)
    {
        iterator = mListeners.Begin();
        iterator.m_Curr = current;
    }

    void Remove(Listener* listener)
    {
        UnregisterEventConnection(this, listener);
        if (this->mCurrentConnection == listener)
        {
            listener->mPendingRemoval = 1;
            return;
        }
        DeleteListener(listener);
    }

    ListenerEntry* GetEntry(Listener* listener)
    {
        return mListeners.Begin(
            (ListenerEntry*)((char*)listener - 8)).CurrentEntry();
    }

    void DeleteListener(Listener* listener)
    {
        ListenerEntry* entry = GetEntry(listener);
        nlDLRingRemove(&mListeners.m_Head, entry);
        mListeners.DeleteEntry(entry);
    }

    DLListContainerBase<Listener, ListenerPool> mListeners;
};

template <typename T, int Count>
UnidentifiedStaticEvent<T, Count>::~UnidentifiedStaticEvent()
{
    RemoveAll();
    UnregisterEvent(this);
}

template <typename T, int Count>
void UnidentifiedStaticEvent<T, Count>::Disconnect(void* owner)
{
    Listener* listener = (Listener*)FindEventConnection(this, owner);
    Remove(listener);
}

template <typename T, int Count>
void UnidentifiedStaticEvent<T, Count>::Add(
    const Callback& callback, unsigned int value, int flags)
{
    ListenerEntry* entry;
    mListeners.m_Allocator.Allocate(entry);
    new (entry) ListenerEntry;
    nlDLRingAddEnd(&mListeners.m_Head, entry);
    Listener* listener = &entry->entry;

    listener->callback.UnidentifiedTransfer(callback);
    RegisterEventConnection(this, listener, value, flags);
}

#endif // GAME_UNIDENTIFIED_STATIC_EVENT_H

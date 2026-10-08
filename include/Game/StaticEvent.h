#ifndef GAME_STATIC_EVENT_H
#define GAME_STATIC_EVENT_H

#include "Game/Event.h"

// An event whose listeners come from a fixed-size pool.
template <typename T, int Count>
class StaticEvent : public EventInterface<T>::Type
{
    typedef typename EventInterface<T>::Type Base;
    typedef EventListener<T> Listener;
    typedef DLListEntry<Listener> ListenerEntry;
    typedef nlStaticArrayAllocator<ListenerEntry, Count> ListenerPool;

public:
    typedef typename Base::Callback Callback;

    StaticEvent(const char* name, int length)
        : Base(name, length)
        , mListeners()
    {
        RegisterEvent(this, Base::sType);
    }

    virtual ~StaticEvent();

    void RemoveAll()
    {
        while (mListeners.m_Head != 0)
        {
            Remove(&*mListeners.Begin());
        }
    }

    virtual void Add(const Callback& callback, unsigned int value, int flags);
    virtual void Disconnect(void* owner);

    void Deliver(typename EventCallbackTraits<T>::Parameter data)
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
StaticEvent<T, Count>::~StaticEvent()
{
    RemoveAll();
    UnregisterEvent(this);
}

template <typename T, int Count>
void StaticEvent<T, Count>::Disconnect(void* owner)
{
    Listener* listener = (Listener*)FindEventConnection(this, owner);
    Remove(listener);
}

template <typename T, int Count>
void StaticEvent<T, Count>::Add(
    const Callback& callback, unsigned int value, int flags)
{
    ListenerEntry* entry;
    mListeners.m_Allocator.Allocate(entry);
    new (entry) ListenerEntry;
    nlDLRingAddEnd(&mListeners.m_Head, entry);
    Listener* listener = &entry->entry;

    listener->callback.TransferFrom(callback);
    RegisterEventConnection(this, listener, value, flags);
}

#endif // GAME_STATIC_EVENT_H

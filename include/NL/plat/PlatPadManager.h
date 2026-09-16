#ifndef NL_PLAT_PLATPADMANAGER_H
#define NL_PLAT_PLATPADMANAGER_H

#include "NL/plat/PlatPadStatus.h"
#include "NL/platpad.h"
#include "Game/Event.h"

// The device-changed event is the only three-argument static event. Its
// template members are defined out of class, so they are instantiated with
// the event's own destructor after the unit's vtables.
template <typename P1, typename P2, typename P3, int Count>
class UnidentifiedStaticEvent3
    : public UnidentifiedTypedEvent3<P1, P2, P3>
{
    typedef UnidentifiedListener3<P1, P2, P3> Listener;
    typedef DLListEntry<Listener> ListenerEntry;
    typedef nlStaticArrayAllocator<ListenerEntry, Count> ListenerPool;
    typedef Function<void(P1, P2, P3)> Callback;

public:
    UnidentifiedStaticEvent3(const char* name, int length)
        : UnidentifiedTypedEvent3<P1, P2, P3>(name, length)
        , mListeners()
    {
        RegisterEvent(
            this, UnidentifiedTypedEvent3<P1, P2, P3>::sType);
    }

    virtual ~UnidentifiedStaticEvent3();

    void UnidentifiedRemoveAll()
    {
        while (mListeners.m_Head != 0)
        {
            Remove(&*mListeners.Begin());
        }
    }

    virtual void Add(Callback callback, unsigned int value, int flags);
    virtual void Disconnect(void* owner);

    void Deliver(P1 p1, P2 p2, P3 p3)
    {
        nlDLListIterator<Listener> iterator = mListeners.Begin();
        while (iterator.hasNext())
        {
            Listener* listener = &*iterator;
            ListenerEntry* currentEntry = iterator.CurrentEntry();
            this->mCurrentConnection = listener;

            if ((listener->mFlags >> 31) != 0)
            {
                listener->callback(p1, p2, p3);
                iterator = mListeners.Begin();
                iterator.m_Curr = currentEntry;
            }

            iterator.next();
            if (((listener->mFlags >> 29) & 1) != 0)
            {
                ListenerEntry* entry = UnidentifiedGetEntry(listener);
                nlDLRingRemove(&mListeners.m_Head, entry);
                entry->~ListenerEntry();
                mListeners.m_Allocator.Free(entry);
            }
        }
        this->mCurrentConnection = 0;
    }

protected:
    void Remove(Listener* listener)
    {
        UnregisterEventConnection(this, listener);
        if (this->mCurrentConnection == listener)
        {
            listener->mFlags |= 0x20000000;
            return;
        }
        UnidentifiedDeleteListener(listener);
    }

    ListenerEntry* UnidentifiedGetEntry(Listener* listener)
    {
        return mListeners.Begin((ListenerEntry*)((char*)listener - 8)).CurrentEntry();
    }

    void UnidentifiedDeleteListener(Listener* listener)
    {
        ListenerEntry* entry = UnidentifiedGetEntry(listener);
        nlDLRingRemove(&mListeners.m_Head, entry);
        mListeners.DeleteEntry(entry);
    }

    DLListContainerBase<Listener, ListenerPool> mListeners;
};

template <typename P1, typename P2, typename P3, int Count>
UnidentifiedStaticEvent3<P1, P2, P3, Count>::~UnidentifiedStaticEvent3()
{
    UnidentifiedRemoveAll();
    UnregisterEvent(this);
}

template <typename P1, typename P2, typename P3, int Count>
void UnidentifiedStaticEvent3<P1, P2, P3, Count>::Add(
    Callback callback, unsigned int value, int flags)
{
    Listener* listener = mListeners.AllocateAtEnd(0);

    void* target = listener->callback.UnidentifiedTransfer(callback);
    RegisterEventConnection(this, listener, value, flags, target);
}

template <typename P1, typename P2, typename P3, int Count>
void UnidentifiedStaticEvent3<P1, P2, P3, Count>::Disconnect(void* owner)
{
    Listener* listener = (Listener*)FindEventConnection(this, owner);
    Remove(listener);
}

class PadDeviceChangedEvent : public UnidentifiedStaticEvent3<int, int, int, 5>
{
public:
    PadDeviceChangedEvent()
        : UnidentifiedStaticEvent3<int, int, int, 5>("DeviceChanged", -1)
    {
    }

    virtual ~PadDeviceChangedEvent() { }
};

struct PlatPadManager
{
    PlatPadManager()
        : disableFreestyle(false)
        , disableClassic(false)
        , deviceChanged()
    {
    }

    void Initialize();
    void UpdateChannel(int channel);
    void UpdateDPD(int channel, unsigned int deviceType);
    WiiRemotePadStatus* GetRemoteStatus(int channel);
    WiiFreestylePadStatus* GetFreestyleStatus(int channel);
    WiiClassicPadStatus* GetClassicStatus(int channel);
    void SetDPDEnabled(int channel, bool enabled);
    bool IsDPDEnabled(int channel) const;

    PlatPadStatus status[WPAD_MAX_CONTROLLERS];
    bool connected[WPAD_MAX_CONTROLLERS];
    int type[WPAD_MAX_CONTROLLERS];
    bool disableFreestyle;
    bool disableClassic;
    bool dpdEnabled[WPAD_MAX_CONTROLLERS];
    bool dpdActive[WPAD_MAX_CONTROLLERS];
    bool dataFormatSet[WPAD_MAX_CONTROLLERS];
    unsigned char padding[2];
    PadDeviceChangedEvent deviceChanged;
};

#endif // NL_PLAT_PLATPADMANAGER_H

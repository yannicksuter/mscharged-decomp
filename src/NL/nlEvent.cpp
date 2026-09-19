#include "Game/EventRegistry.h"
#include "Game/Event.h"

#include "NL/nlAVLTree.h"
#include "NL/nlMemory.h"
#include "NL/nlSmallBlockAllocator.h"
#include "NL/nlString.h"

typedef UnidentifiedConnection* ConnectionKey;
typedef unsigned int ConnectionValue;
typedef AVLTreeEntry<ConnectionKey, ConnectionValue> ConnectionTreeEntry;
typedef nlSlotPoolFixed<sizeof(ConnectionTreeEntry)> ConnectionTreePool;
typedef AVLTreeBase<ConnectionKey, ConnectionValue, ConnectionTreePool,
    DefaultKeyCompare<ConnectionKey> >
    ConnectionTree;

typedef ConnectionTree* ConnectionTreePtr;
typedef AVLTreeEntry<unsigned int, ConnectionTreePtr> ConnectionGroupEntry;
typedef nlSlotPoolFixed<sizeof(ConnectionGroupEntry)> ConnectionGroupPool;
typedef AVLTreeBase<unsigned int, ConnectionTreePtr, ConnectionGroupPool,
    DefaultKeyCompare<unsigned int> >
    ConnectionGroupTree;

static const ConnectionValue sDefaultConnectionValue = 0;
static ConnectionGroupTree sConnectionGroups;
EventRegistry* g_pEventRegistry = 0;

static EventRegistry* GetEventRegistry()
{
    if (g_pEventRegistry == 0)
    {
        g_pEventRegistry = new (8, false)
            nlAVLTree<unsigned int, EventRegistryValue,
                DefaultKeyCompare<unsigned int> >;
    }
    return g_pEventRegistry;
}

class ConnectionPoolStateCallback
{
public:
    typedef void (ConnectionTreePool::*PoolCallback)();

    ConnectionPoolStateCallback(PoolCallback callback)
        : mCallback(callback)
    {
    }

    void Apply(const unsigned int&, ConnectionTreePtr* tree);

private:
    unsigned int mUnidentified00;
    PoolCallback mCallback;
};

static void ApplyConnectionPoolState(
    ConnectionPoolStateCallback::PoolCallback callback)
{
    ConnectionPoolStateCallback stateCallback(callback);
    sConnectionGroups.Walk(
        &stateCallback, &ConnectionPoolStateCallback::Apply);
}

void PushEventConnectionState()
{
    sConnectionGroups.m_Allocator.PushState();
    ApplyConnectionPoolState(&ConnectionTreePool::PushState);
}

void PopEventConnectionState()
{
    sConnectionGroups.m_Allocator.PopState();
    ApplyConnectionPoolState(&ConnectionTreePool::PopState);
}

unsigned int HashEventName(const char* name, int length)
{
    unsigned int value = (unsigned int)length;
    unsigned int middleHigh = (value << 8) & 0x00FF0000;
    unsigned int high = value << 24;
    unsigned int swapped = (value >> 24) | ((value >> 8) & 0x0000FF00);
    swapped |= middleHigh;
    swapped |= high;
    unsigned int hash = nlStringLowerHash(name);
    return ~(swapped ^ hash);
}

void* FindEventConnection(void*, void* owner)
{
    return *(void**)owner;
}

UnidentifiedConnection::~UnidentifiedConnection()
{
    if (mEvent != 0)
    {
        *(void**)mEvent = 0;
    }
}

void RegisterEvent(void* eventPtr, void* type)
{
    UnidentifiedEventBase* event = (UnidentifiedEventBase*)eventPtr;
    EventRegistryValue value;
    value.event = event;
    value.type = type;
    unsigned int key = event->mHash;

    GetEventRegistry()->Add(key, value);
}

void UnregisterEvent(void* eventPtr)
{
    UnidentifiedEventBase* event = (UnidentifiedEventBase*)eventPtr;
    unsigned int key = event->mHash;
    g_pEventRegistry->Remove(key);
}

void RegisterEventConnection(void* event, void* connectionPtr,
    unsigned int owner, int group, void*)
{
    UnidentifiedConnection* connection
        = (UnidentifiedConnection*)connectionPtr;
    connection->mEvent = (void*)owner;
    connection->mGroupCount = 0;
    connection->mTarget = event;
    if (owner != 0)
    {
        *(UnidentifiedConnection**)owner = connection;
    }

    if ((unsigned int)group == (unsigned int)-1)
    {
        return;
    }

    ConnectionTree** foundTree;
    ConnectionTree* tree = 0;
    unsigned int key = (unsigned int)group;
    if (!sConnectionGroups.FindGet(key, &foundTree))
    {
        tree = new (8, false) ConnectionTree;
        if (tree == 0)
        {
            return;
        }
        sConnectionGroups.Add(key, tree);
    }
    else
    {
        tree = *foundTree;
    }

    if (tree->Add(connection, sDefaultConnectionValue) == 0)
    {
        connection->mGroupCount++;
    }
}

void UnregisterEventConnection(void*, void* connectionPtr)
{
    UnidentifiedConnection* connection
        = (UnidentifiedConnection*)connectionPtr;
    if (connection->mEvent != 0)
    {
        UnidentifiedConnection* tracked
            = *(UnidentifiedConnection**)connection->mEvent;
        if (tracked != 0)
        {
            if (tracked->mGroupCount != 0)
            {
                typedef nlAVLTreeIterator<unsigned int, ConnectionTreePtr,
                    DefaultKeyCompare<unsigned int> >
                    GroupIterator;
                GroupIterator iterator;
                iterator.Initialize(sConnectionGroups.m_Root);
                while (true)
                {
                    ConnectionKey key = tracked;
                    ConnectionTree* tree = iterator.CurrentValue();
                    if (tree->Remove(key))
                    {
                        key->mGroupCount--;
                    }
                    if (tracked->mGroupCount == 0)
                    {
                        break;
                    }
                    iterator.Next();
                }
            }
        }

        if (connection->mEvent != 0)
        {
            *(UnidentifiedConnection**)connection->mEvent = 0;
        }
    }
}

void DisconnectEventOwner(void* owner)
{
    UnidentifiedConnection* connection = *(UnidentifiedConnection**)owner;
    if (connection != 0)
    {
        ((UnidentifiedEventBase*)connection->mTarget)->Disconnect(owner);
    }
}

void ConnectionPoolStateCallback::Apply(
    const unsigned int&, ConnectionTreePtr* tree)
{
    ((*tree)->m_Allocator.*mCallback)();
}

static EventRegistry* sEventRegistryInitializer = GetEventRegistry();

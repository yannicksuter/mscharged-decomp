#define NL_AVL_TREE_DEFER_DELETE_ENTRY
#include "Game/EventRegistry.h"
#undef NL_AVL_TREE_DEFER_DELETE_ENTRY
#include "Game/Event.h"
#include "NL/nlSlotPoolFixed.inl"

#include "NL/nlAVLTree.h"
#include "NL/nlMemory.h"
#include "NL/nlSmallBlockAllocator.h"
#include "NL/nlString.h"

typedef EventConnection* ConnectionKey;
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

static inline void RestoreConnectionPoolState()
{
    sConnectionGroups.m_Allocator.PopState();
    ConnectionPoolStateCallback stateCallback(&ConnectionTreePool::PopState);
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
    RestoreConnectionPoolState();
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

EventConnection::~EventConnection()
{
    if (mOwner != 0)
    {
        *(void**)mOwner = 0;
    }
}

void RegisterEvent(void* eventPtr, void* eventType)
{
    EventBase* event = (EventBase*)eventPtr;
    EventRegistryValue value;
    value.event = event;
    value.type = eventType;
    unsigned int key = event->mHash;

    GetEventRegistry()->Add(key, value);
}

void UnregisterEvent(void* eventPtr)
{
    EventBase* event = (EventBase*)eventPtr;
    unsigned int key = event->mHash;
    g_pEventRegistry->Remove(key);
}

static inline void AddConnectionToGroup(
    unsigned int key, EventConnection* connection)
{
    ConnectionTree* tree = 0;
    ConnectionTree** foundTree;
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

    if (tree->Add(connection, 0) == 0)
    {
        connection->mGroupCount++;
    }
}

static inline void RegisterConnectionGroup(
    int group, EventConnection* connection)
{
    if ((unsigned int)group == (unsigned int)-1)
    {
        return;
    }
    AddConnectionToGroup((unsigned int)group, connection);
}

void RegisterEventConnection(void* event, void* connectionPtr,
    unsigned int owner, int group)
{
    EventConnection* connection
        = (EventConnection*)connectionPtr;
    connection->mOwner = (void*)owner;
    connection->mGroupCount = 0;
    connection->mEvent = event;
    if (owner != 0)
    {
        *(EventConnection**)owner = connection;
    }

    RegisterConnectionGroup(group, connection);
}

void UnregisterEventConnection(void*, void* connectionPtr)
{
    EventConnection* connection
        = (EventConnection*)connectionPtr;
    if (connection->mOwner != 0)
    {
        EventConnection* tracked
            = *(EventConnection**)connection->mOwner;
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

        if (connection->mOwner != 0)
        {
            *(EventConnection**)connection->mOwner = 0;
        }
    }
}

void DisconnectEventOwner(void* owner)
{
    EventConnection* connection = *(EventConnection**)owner;
    if (connection != 0)
    {
        ((EventBase*)connection->mEvent)->Disconnect(owner);
    }
}

void ConnectionPoolStateCallback::Apply(
    const unsigned int&, ConnectionTreePtr* tree)
{
    ((*tree)->m_Allocator.*mCallback)();
}

class EventRegistryInitializer
{
public:
    EventRegistryInitializer()
    {
        GetEventRegistry();
    }
};

static EventRegistryInitializer sEventRegistryInitializer;

template <typename KeyType, typename ValueType, typename AllocatorType, typename CompareType>
inline void AVLTreeBase<KeyType, ValueType, AllocatorType, CompareType>::DeleteEntry(
    AVLTreeUntemplated* tree, AVLTreeNode* entry)
{
    Entry* e = (Entry*)entry;
    ((AVLTreeBase*)tree)->m_Allocator.Delete(e);
}

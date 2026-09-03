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

struct EventRegistryValue
{
    UnidentifiedEventBase* event;
    void* type;
};

typedef AVLTreeBase<unsigned int, EventRegistryValue,
    NewAdapter<AVLTreeEntry<unsigned int, EventRegistryValue> >,
    DefaultKeyCompare<unsigned int> >
    EventRegistry;

static const ConnectionValue sDefaultConnectionValue = 0;
static ConnectionGroupTree sConnectionGroups;
extern "C" EventRegistry* lbl_806E1D90 = 0;

static EventRegistry* GetEventRegistry()
{
    if (lbl_806E1D90 == 0)
    {
        lbl_806E1D90 = new (8, false) EventRegistry;
    }
    return lbl_806E1D90;
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

extern "C" void fn_802B2518()
{
    sConnectionGroups.m_Allocator.PushState();

    ConnectionPoolStateCallback callback(&ConnectionTreePool::PushState);
    sConnectionGroups.Walk(
        &callback, &ConnectionPoolStateCallback::Apply);
}

extern "C" void fn_802B26D4()
{
    sConnectionGroups.m_Allocator.PopState();

    ConnectionPoolStateCallback callback(&ConnectionTreePool::PopState);
    sConnectionGroups.Walk(
        &callback, &ConnectionPoolStateCallback::Apply);
}

extern "C" unsigned int fn_802B289C(const char* name, int length)
{
    unsigned int value = (unsigned int)length;
    unsigned int middleHigh = (value << 8) & 0x00FF0000;
    unsigned int high = value << 24;
    unsigned int swapped = (value >> 8) & 0x0000FF00;
    swapped |= value >> 24;
    swapped |= middleHigh;
    swapped |= high;
    unsigned int hash = nlStringLowerHash(name);
    return ~(swapped ^ hash);
}

extern "C" void* fn_802B28E0(void*, void* owner)
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

extern "C" void fn_802B2940(void* eventPtr, void* type)
{
    UnidentifiedEventBase* event = (UnidentifiedEventBase*)eventPtr;
    EventRegistryValue value;
    value.event = event;
    value.type = type;
    unsigned int key = event->mHash;

    GetEventRegistry()->Add(key, value);
}

extern "C" void fn_802B29C4(void* eventPtr)
{
    UnidentifiedEventBase* event = (UnidentifiedEventBase*)eventPtr;
    unsigned int key = event->mHash;
    lbl_806E1D90->Remove(key);
}

extern "C" void fn_802B2A04(void* event, void* connectionPtr,
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

    if (group == -1)
    {
        return;
    }

    ConnectionTree** foundTree = 0;
    ConnectionTree* tree;
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

extern "C" void fn_802B2CC8(void*, void* connectionPtr)
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
                while (iterator.IsValid())
                {
                    ConnectionTree* tree = iterator.Current()->value;
                    ConnectionTree::Entry* removed
                        = (ConnectionTree::Entry*)tree->RemoveAVLNode(
                            (AVLTreeNode**)&tree->m_Root, &tracked);
                    if (removed != 0)
                    {
                        tree->m_Allocator.Free(removed);
                        tracked->mGroupCount--;
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

extern "C" void fn_802B2E8C(void* owner)
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

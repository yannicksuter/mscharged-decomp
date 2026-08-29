#include "NL/gl/gl.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glStruct.h"
#include "NL/gl/glView.h"
#include "NL/nlAVLTree.h"
#include "NL/nlMath.h"

extern "C" void* fn_802CC0A8(unsigned long size, int memoryType);

extern "C" void fn_802CCD1C(
    void*, GLView*, GLViewPacketCallback);
extern "C" void fn_802CCD3C(void*, const glModelPacket*, unsigned long);

class PacketCallbackManager
{
public:
    PacketCallbackManager(GLView* view, GLViewPacketCallback callback)
    {
        fn_802CCD1C(this, view, callback);
    }

    void DoCallback(const glModelPacket* packet, unsigned long count)
    {
        fn_802CCD3C(this, packet, count);
    }

    GLView* m_View;
    GLViewPacketCallback m_Callback;
    unsigned long m_LastRaster;
    unsigned long m_LastMatrix;
    void* m_LastState;
};

class GLPacketSorter
{
public:
    static void* operator new(unsigned long size)
    {
        return fn_802CC0A8(size, 0);
    }

    virtual const glModelPacket* Begin() = 0;
    virtual const glModelPacket* Next() = 0;
    virtual void Attach(GLView*, const glModelPacket*) = 0;
};

typedef unsigned long long GLPacketSortKey;
typedef AVLTreeEntry<GLPacketSortKey, const glModelPacket*> GLPacketTreeEntry;

class GLPacketTree : public AVLTreeUntemplated
{
public:
    GLPacketTree()
        : m_Root(0)
        , m_Compare(0)
    {
    }

    virtual int CompareNodes(AVLTreeNode*, AVLTreeNode*);
    virtual int CompareKey(void*, AVLTreeNode*);
    virtual AVLTreeNode* AllocateEntry(void*, void*);

    u8 m_Allocator;
    GLPacketTreeEntry* m_Root;
    DefaultKeyCompare<GLPacketSortKey>* m_Compare;
};

int GLPacketTree::CompareNodes(AVLTreeNode* node1, AVLTreeNode* node2)
{
    GLPacketTreeEntry* entry1 = (GLPacketTreeEntry*)node1;
    GLPacketTreeEntry* entry2 = (GLPacketTreeEntry*)node2;
    DefaultKeyCompare<GLPacketSortKey> compare;
    return compare(entry1->key, entry2->key);
}

int GLPacketTree::CompareKey(void* key, AVLTreeNode* node)
{
    GLPacketTreeEntry* entry = (GLPacketTreeEntry*)node;
    DefaultKeyCompare<GLPacketSortKey> compare;
    return compare(*(GLPacketSortKey*)key, entry->key);
}

AVLTreeNode* GLPacketTree::AllocateEntry(void* key, void* value)
{
    GLPacketTreeEntry* entry = (GLPacketTreeEntry*)fn_802CC0A8(sizeof(GLPacketTreeEntry), 0);
    entry->node.left = 0;
    entry->node.right = 0;
    entry->node.heavy = 0;
    entry->key = *(GLPacketSortKey*)key;
    if (value != 0)
        entry->value = *(const glModelPacket**)value;
    return (AVLTreeNode*)entry;
}

class GLPacketTreeSorter : public GLPacketSorter
{
public:
    virtual const glModelPacket* Begin();
    virtual const glModelPacket* Next();
    virtual void Attach(GLView*, const glModelPacket*);
    virtual unsigned long MakeSortKey(GLView*, const glModelPacket*) = 0;

    GLPacketTree m_Tree;
    nlAVLTreeIterator<GLPacketSortKey, const glModelPacket*,
        DefaultKeyCompare<GLPacketSortKey> >
        m_Iterator;
};

class GLTexturePacketSorter : public GLPacketTreeSorter
{
public:
    virtual unsigned long MakeSortKey(GLView*, const glModelPacket*);
};

class GLDepthPacketSorter : public GLPacketTreeSorter
{
public:
    virtual unsigned long MakeSortKey(GLView*, const glModelPacket*);
};

class GLMatrixDepthPacketSorter : public GLPacketTreeSorter
{
public:
    GLMatrixDepthPacketSorter()
        : m_Sequence(0)
    {
    }

    virtual unsigned long MakeSortKey(GLView*, const glModelPacket*);

    unsigned long m_Sequence;
};

class GLPacketListSorter : public GLPacketSorter
{
public:
    GLPacketListSorter()
        : m_Head(0)
        , m_Tail(0)
        , m_Current(0)
    {
    }

    virtual const glModelPacket* Begin();
    virtual const glModelPacket* Next();

protected:
    u8 m_Allocator;
    ListEntry<const glModelPacket*>* m_Head;
    ListEntry<const glModelPacket*>* m_Tail;
    ListEntry<const glModelPacket*>* m_Current;
};

class GLReversePacketSorter : public GLPacketListSorter
{
public:
    virtual void Attach(GLView*, const glModelPacket*);
};

class GLUnsortedPacketSorter : public GLPacketListSorter
{
public:
    virtual void Attach(GLView*, const glModelPacket*);
};

const glModelPacket* GLPacketListSorter::Next()
{
    if (m_Current == 0)
        return 0;
    const glModelPacket* packet = m_Current->entry;
    m_Current = m_Current->next;
    return packet;
}

const glModelPacket* GLPacketListSorter::Begin()
{
    m_Current = m_Head;
    return Next();
}

void GLReversePacketSorter::Attach(
    GLView*, const glModelPacket* packet)
{
    ListEntry<const glModelPacket*>* entry = (ListEntry<const glModelPacket*>*)fn_802CC0A8(
        sizeof(ListEntry<const glModelPacket*>), 0);
    entry->next = 0;
    entry->entry = packet;
    nlListAddStart(&m_Head, entry, &m_Tail);
}

void GLUnsortedPacketSorter::Attach(
    GLView*, const glModelPacket* packet)
{
    ListEntry<const glModelPacket*>* entry = (ListEntry<const glModelPacket*>*)fn_802CC0A8(
        sizeof(ListEntry<const glModelPacket*>), 0);
    entry->entry = packet;
    entry->next = 0;
    nlListAddEnd(&m_Head, &m_Tail, entry);
}

class GLPacketSorterTree
    : public nlAVLTreeSlotPool<unsigned long, GLPacketSorter*,
          DefaultKeyCompare<unsigned long> >
{
public:
    static void* operator new(unsigned long size)
    {
        return nlMalloc(size, 8, false);
    }

    GLPacketSorterTree(int initial, int delta)
        : nlAVLTreeSlotPool<unsigned long, GLPacketSorter*,
              DefaultKeyCompare<unsigned long> >(initial, delta)
    {
    }
};

class GLPacketSorterIterator
{
public:
    typedef AVLTreeEntry<unsigned long, GLPacketSorter*> Entry;

    GLPacketSorterIterator()
        : m_NumStackEntries(0)
    {
    }

    void Initialize(Entry* entry)
    {
        m_NumStackEntries = 0;
        if (entry != 0)
            PushLeft(entry);
    }

    void PushLeft(Entry*);

    void Next()
    {
        --m_NumStackEntries;
        Entry* entry = (Entry*)m_Stack[m_NumStackEntries];
        Entry* right = (Entry*)entry->node.right;
        if (right != 0)
            PushLeft(right);
    }

    bool IsValid() const
    {
        return m_NumStackEntries != 0;
    }

    Entry* Current() const
    {
        return (Entry*)m_Stack[m_NumStackEntries - 1];
    }

    AVLTreeNode* m_Stack[32];
    unsigned int m_NumStackEntries;
};

inline void GLPacketSorterIterator::PushLeft(Entry* entry)
{
    while (entry->node.left != 0)
    {
        m_Stack[m_NumStackEntries] = (AVLTreeNode*)entry;
        ++m_NumStackEntries;
        entry = *(Entry**)&entry->node.left;
    }
    m_Stack[m_NumStackEntries] = (AVLTreeNode*)entry;
    ++m_NumStackEntries;
}

static const char* s_UninitializedViewName = "<uninitialized>";

extern "C" GLPacketSorter* fn_802CEF1C()
{
    return new GLTexturePacketSorter;
}

extern "C" GLPacketSorter* fn_802CEF74()
{
    return new GLReversePacketSorter;
}

extern "C" GLPacketSorter* fn_802CEFC0()
{
    return new GLUnsortedPacketSorter;
}

extern "C" GLPacketSorter* fn_802CF00C()
{
    return new GLMatrixDepthPacketSorter;
}

extern "C" GLPacketSorter* fn_802CF068()
{
    return new GLDepthPacketSorter;
}

GLView::GLView(GLViewInterface* interface, const GLRenderPair& renderPair,
    GLViewSortMode sortMode)
    : m_RenderPair(renderPair)
{
    m_Unknown38 = 0;
    m_Unknown3C = 0;
    m_Name = s_UninitializedViewName;
    m_Unknown48 = 0;
    m_TriangleCount = 0;
    m_Interface = interface;
    m_Parent = 0;

    GLPacketSorterFactory createSorter;

    switch (sortMode)
    {
    case GLViewSort_TransformedDepth:
        createSorter = fn_802CF068;
        break;
    case GLViewSort_TransformedMatrixDepth:
        createSorter = fn_802CF00C;
        break;
    case GLViewSort_None:
        createSorter = fn_802CEFC0;
        break;
    case GLViewSort_Reverse:
        createSorter = fn_802CEF74;
        break;
    default:
        createSorter = fn_802CEF1C;
        break;
    }

    m_CreateSorter = createSorter;
    m_Sorters = new GLPacketSorterTree(16, 16);
    m_ViewportX = 0;
    m_ViewportY = 0;
    m_ViewportWidth = glGetScreenWidth();
    m_ViewportHeight = glGetScreenHeight();
    m_Enabled = true;
    m_ClearColour = false;
    m_ClearDepth = false;
    m_Unknown33 = false;
    m_Target = 0;
    m_Visible = true;
}

inline GLView::GLView()
    : m_RenderPair(0, 0)
{
    m_Unknown38 = 0;
    m_Unknown3C = 0;
    m_Name = s_UninitializedViewName;
    m_Unknown48 = 0;
    m_TriangleCount = 0;
    m_Interface = &lbl_806E1F38;
    m_Parent = 0;
    m_CreateSorter = fn_802CEF1C;
    m_Sorters = new GLPacketSorterTree(16, 16);
    m_ViewportX = 0;
    m_ViewportY = 0;
    m_ViewportWidth = glGetScreenWidth();
    m_ViewportHeight = glGetScreenHeight();
    m_Enabled = true;
    m_ClearColour = false;
    m_ClearDepth = false;
    m_Unknown33 = false;
    m_Target = 0;
    m_Visible = true;
}

inline GLPacketSorter* GLView::GetSorter(unsigned long sortKey)
{
    GLPacketSorter* sorter;
    GLPacketSorter** foundSorter;
    AVLTreeNode* existingNode;
    if (!m_Sorters->FindGet(sortKey, &foundSorter))
    {
        sorter = m_CreateSorter();
        m_Sorters->AddAVLNode((AVLTreeNode**)&m_Sorters->m_Root,
            &sortKey,
            &sorter,
            &existingNode);
        return sorter;
    }
    return *foundSorter;
}

GLView::~GLView()
{
    while (m_Children.m_Head != 0)
    {
        GLView* child;
        m_Children.RemoveStart(&child);
        delete child;
    }
    delete m_Sorters;
}

void GLView::AttachPacket(const glModelPacket* packet, unsigned long sortKey)
{
    GetSorter(sortKey)->Attach(this, packet);
}

void GLView::AttachModel(const glModel* model, unsigned long sortKey)
{
    GLPacketSorter& sorter = *GetSorter(sortKey);
    unsigned long packetOffset;
    unsigned long index;
    for (index = 0, packetOffset = 0; index < model->numPackets;
         packetOffset += sizeof(glModelPacket), ++index)
    {
        sorter.Attach(this,
            (const glModelPacket*)((const u8*)model->packets + packetOffset));
    }
}

void GLView::Iterate(GLViewPacketCallback callback)
{
    m_TriangleCount = 0;
    if (!m_Visible)
        return;

    BeginRender();

    PacketCallbackManager callbackManager(this, callback);
    GLPacketSorterIterator iterator;
    iterator.Initialize(m_Sorters->m_Root);

    if (iterator.IsValid())
        callback(this, 1, 0);

    while (iterator.IsValid())
    {
        GLPacketSorter* sorter = iterator.Current()->value;
        const glModelPacket* packet = sorter->Begin();
        while (packet != 0)
        {
            m_TriangleCount += glGetNumTriangles(
                (eGLPrimitive)(u8)packet->primType, packet->numUniqueVertices);
            BeginPacket(packet);
            callbackManager.DoCallback(packet, 1);
            EndPacket(packet);
            packet = sorter->Next();
        }
        iterator.Next();
    }

    EndRender();
}

void GLView::RemoveChild(GLView* child)
{
    ListEntry<GLView*>* current = m_Children.m_Head;
    if (current == 0)
        return;

    if (current->entry == child)
    {
        ListEntry<GLView*>* next;
        if (current == m_Children.m_Tail)
        {
            m_Children.m_Tail = 0;
            next = 0;
        }
        else
        {
            next = current->next;
        }
        delete m_Children.m_Head;
        m_Children.m_Head = next;
        return;
    }

    ListEntry<GLView*>* previous = current;
    current = current->next;
    while (current != 0)
    {
        if (current->entry == child)
        {
            previous->next = current->next;
            if (current == m_Children.m_Tail)
                m_Children.m_Tail = previous;
            delete current;
            return;
        }
        previous = current;
        current = current->next;
    }
}

GLRenderPair GLView::GetRenderPair() const
{
    return m_RenderPair.hash != 0 && m_RenderPair.target != 0
             ? m_RenderPair
             : fn_802CD82C();
}

void GLView::BeginRender()
{
}

void GLView::EndRender()
{
}

void GLView::BeginPacket(const glModelPacket*)
{
}

void GLView::EndPacket(const glModelPacket*)
{
}

inline void GLViewIterator::Push(const GLViewIteratorEntry& entry)
{
    GLViewIteratorEntry* stackEntry = &m_Stack[++m_Depth];
    stackEntry->next = entry.next;
    stackEntry->view = entry.view;

    if (entry.view->m_Children.m_Head != 0)
    {
        ListEntry<GLView*>* child = entry.view->m_Children.m_Head;
        GLViewIteratorEntry childEntry;
        childEntry.next = child->next;
        childEntry.view = child->entry;
        Push(childEntry);
    }
}

GLViewIterator::GLViewIterator(GLView* root)
{
    m_Depth = -1;
    GLViewIteratorEntry rootEntry;
    rootEntry.view = root;
    Push(rootEntry);
}
void GLViewIterator::Next()
{
    if (m_Depth < 0)
        return;

    if (m_Stack[m_Depth].next != 0)
    {
        m_Stack[m_Depth] = *(GLViewIteratorEntry*)m_Stack[m_Depth].next;

        GLView* view = m_Stack[m_Depth].view;
        if (view->m_Children.m_Head != 0)
        {
            ListEntry<GLView*>* child = view->m_Children.m_Head;
            GLViewIteratorEntry childEntry;
            childEntry.next = child->next;
            childEntry.view = child->entry;
            Push(childEntry);
        }
    }
    else
    {
        --m_Depth;
    }
}
GLView* GLViewIterator::Current() const
{
    if (m_Depth >= 0)
        return m_Stack[m_Depth].view;
    return 0;
}

bool GLViewIterator::IsDone() const
{
    return m_Depth < 0;
}

extern "C" nlMatrix4 lbl_804EB2B8;

extern "C" GLViewInterface lbl_806E1F38;
extern "C" GLView lbl_8057F250;

void glViewCompact()
{
    GLViewIterator iterator(&lbl_8057F250);
    while (!iterator.IsDone())
    {
        GLView* view = iterator.Current();
        view->m_Sorters->Clear();
        view->m_Sorters->m_Allocator.FreeBlocks();
        iterator.Next();
    }
}

GLViewInterface lbl_806E1F38;

extern "C" GLView lbl_8057F250;
GLView lbl_8057F250;

void GLViewInterface::GetViewMatrix(nlMatrix4& matrix)
{
    matrix.SetIdentity();
}

void GLViewInterface::GetProjectionMatrix(nlMatrix4& matrix)
{
    matrix.SetIdentity();
}

void GLViewInterface::GetInverseViewMatrix(nlMatrix4& matrix)
{
    matrix.SetIdentity();
}

void GLViewInterface::GetViewProjectionMatrix(nlMatrix4& matrix)
{
    matrix.SetIdentity();
}

const nlMatrix4* GLViewInterface::GetViewMatrix() const
{
    return &lbl_804EB2B8;
}

const nlMatrix4* GLViewInterface::GetProjectionMatrix() const
{
    return &lbl_804EB2B8;
}

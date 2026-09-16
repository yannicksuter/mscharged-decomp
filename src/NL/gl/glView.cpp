#include "NL/gl/gl.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glPacketCallback.h"
#include "NL/gl/glPlat.h"
#include "NL/gl/glRenderList.h"
#include "NL/gl/glStruct.h"
#include "NL/gl/glView.h"
#include "NL/nlAVLTree.h"
#include "NL/nlMath.h"

#include <math.h>

class UnidentifiedPacketSorterTree_8052E504
    : public nlAVLTreeSlotPool<long, UnidentifiedPacketSorter*,
          DefaultKeyCompare<long> >
{
public:
    static void* operator new(unsigned long size)
    {
        return nlMalloc(size, 8, false);
    }

    UnidentifiedPacketSorterTree_8052E504(int initial, int delta)
        : nlAVLTreeSlotPool<long, UnidentifiedPacketSorter*,
              DefaultKeyCompare<long> >(initial, delta)
    {
    }
};

static bool IsSimpleProjection(const nlMatrix4* projection)
{
    bool diagonal = projection->m12 == 0.0f && projection->m13 == 0.0f
        && projection->m21 == 0.0f && projection->m23 == 0.0f
        && projection->m31 == 0.0f && projection->m32 == 0.0f;
    return diagonal
        ? projection->m41 == 0.0f && projection->m42 == 0.0f
            && projection->m43 == 0.0f && projection->m44 == 1.0f
        : false;
}

void glViewProjectPoint(GLView* view, const nlVector3& v3world, nlVector3& v3NDC)
{
    const nlMatrix4* pProj = view->m_Interface->GetProjectionMatrix();
    bool simple = IsSimpleProjection(pProj);
    if (!simple)
    {
        glplatViewProjectPoint(view, v3world, v3NDC);
    }
    else
    {
        nlMatrix4 transposed;
        nlMatrix4 projm;
        nlMatrix4 viewm;
        nlVector3 v_out;
        view->m_Interface->GetViewMatrix(viewm);
        view->m_Interface->GetProjectionMatrix(projm);
        nlTransposeMatrix(transposed, projm);
        projm = transposed;
        nlMultPosVectorMatrix(v_out, v3world, viewm);
        nlMultPosVectorMatrix(v3NDC, v_out, projm);
    }
}

void glViewUnprojectOrthographicPoint(GLView* view, const nlVector3* normalized, nlVector3* viewPosition)
{
    const nlMatrix4* pProj = view->m_Interface->GetProjectionMatrix();
    const float xOffset = pProj->m14;
    const float yOffset = pProj->m24;
    const float zOffset = pProj->m34;
    const float xScale = pProj->m11;
    const float yScale = pProj->m22;
    const float zScale = pProj->m33;
    const float x = (normalized->x - xOffset) / xScale;
    const float y = (normalized->y - yOffset) / yScale;
    const float z = (normalized->z - zOffset) / zScale;
    nlVec3Set(*viewPosition, x, y, z);
}

float glViewGetOrthographicWidth(GLView* view)
{
    const nlMatrix4* pProj = view->m_Interface->GetProjectionMatrix();
    return fabsf(2.0f / pProj->m11);
}

float glViewGetOrthographicHeight(GLView* view)
{
    const nlMatrix4* pProj = view->m_Interface->GetProjectionMatrix();
    return fabsf(2.0f / pProj->m22);
}

void glViewProjectPointToViewport(GLView* view, const nlVector3* world, nlVector3* screen)
{
    unsigned long vpWidth = view->m_ViewportWidth;
    unsigned long vpHeight = view->m_ViewportHeight;
    nlVector3 v3NDC;
    glViewProjectPoint(view, *world, v3NDC);
    screen->x = v3NDC.x * (float)vpWidth * 0.5f;
    screen->y = v3NDC.y * (float)vpHeight * 0.5f;
    screen->x += (float)vpWidth * 0.5f;
    screen->y += (float)vpHeight * 0.5f;
}

void glViewProjectPointBetweenViews(GLView* source, GLView* destination, const nlVector3* world, nlVector3* projected)
{
    glViewProjectPoint(source, *world, *projected);
    projected->y = -projected->y;
    glViewUnprojectOrthographicPoint(destination, projected, projected);
}

void gl_ViewReset()
{
    GLViewIterator iterator(&gRootView);
    while (!iterator.IsDone())
    {
        GLView* view = iterator.Current();
        view->m_Sorters->Clear();
        iterator.Next();
    }
}

void gl_ViewStartup()
{
}

const glModelPacket* UnidentifiedPacketSorter_802D033C::fn_0C()
{
    if (m_Current == 0)
        return 0;
    const glModelPacket* packet = m_Current->entry;
    m_Current = m_Current->next;
    return packet;
}

const glModelPacket* UnidentifiedPacketSorter_802D033C::fn_08()
{
    m_Current = m_Head;
    return fn_0C();
}

void UnidentifiedPacketSorter_8052E540::fn_10(
    GLView*, const glModelPacket* packet)
{
    ListEntry<const glModelPacket*>* entry = (ListEntry<const glModelPacket*>*)glFrameAlloc(
        sizeof(ListEntry<const glModelPacket*>), GLM_Header);
    entry->next = 0;
    entry->entry = packet;
    nlListAddStart(&m_Head, entry, &m_Tail);
}

void UnidentifiedPacketSorter_8052E554::fn_10(
    GLView*, const glModelPacket* packet)
{
    ListEntry<const glModelPacket*>* entry = (ListEntry<const glModelPacket*>*)glFrameAlloc(
        sizeof(ListEntry<const glModelPacket*>), GLM_Header);
    entry->entry = packet;
    entry->next = 0;
    nlListAddEnd(&m_Head, &m_Tail, entry);
}

class UnidentifiedPacketSorterIterator
{
public:
    typedef AVLTreeEntry<long, UnidentifiedPacketSorter*> Entry;

    UnidentifiedPacketSorterIterator()
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

inline void UnidentifiedPacketSorterIterator::PushLeft(Entry* entry)
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

UnidentifiedPacketSorter* fn_802CEF1C()
{
    return new UnidentifiedPacketSorter_8052E2D8;
}

UnidentifiedPacketSorter* fn_802CEF74()
{
    return new UnidentifiedPacketSorter_8052E540;
}

UnidentifiedPacketSorter* fn_802CEFC0()
{
    return new UnidentifiedPacketSorter_8052E554;
}

UnidentifiedPacketSorter* fn_802CF00C()
{
    return new UnidentifiedPacketSorter_8052E2A8;
}

UnidentifiedPacketSorter* fn_802CF068()
{
    return new UnidentifiedPacketSorter_8052E2C0;
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

    UnidentifiedPacketSorterFactory createSorter;

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
    m_Sorters = new UnidentifiedPacketSorterTree_8052E504(16, 16);
    m_ViewportX = 0;
    m_ViewportY = 0;
    m_ViewportWidth = glGetScreenWidth();
    m_ViewportHeight = glGetScreenHeight();
    m_Enabled = true;
    m_ClearDepth = false;
    m_Unknown32 = false;
    m_ClearColour = false;
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
    m_Interface = &gDefaultViewInterface;
    m_Parent = 0;
    m_CreateSorter = fn_802CEF1C;
    m_Sorters = new UnidentifiedPacketSorterTree_8052E504(16, 16);
    m_ViewportX = 0;
    m_ViewportY = 0;
    m_ViewportWidth = glGetScreenWidth();
    m_ViewportHeight = glGetScreenHeight();
    m_Enabled = true;
    m_ClearDepth = false;
    m_Unknown32 = false;
    m_ClearColour = false;
    m_Target = 0;
    m_Visible = true;
}

inline UnidentifiedPacketSorter* GLView::GetSorter(long sortKey)
{
    UnidentifiedPacketSorter* sorter;
    UnidentifiedPacketSorter** foundSorter;
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
    GetSorter(sortKey)->fn_10(this, packet);
}

void GLView::AttachModel(const glModel* model, unsigned long sortKey)
{
    UnidentifiedPacketSorter& sorter = *GetSorter(sortKey);
    unsigned long packetOffset;
    unsigned long index;
    for (index = 0, packetOffset = 0; index < model->numPackets;
         packetOffset += sizeof(glModelPacket), ++index)
    {
        sorter.fn_10(this,
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
    UnidentifiedPacketSorterIterator iterator;
    iterator.Initialize(m_Sorters->m_Root);

    if (iterator.IsValid())
        callback(this, 1, 0);

    while (iterator.IsValid())
    {
        UnidentifiedPacketSorter* sorter = iterator.Current()->value;
        const glModelPacket* packet = sorter->fn_08();
        while (packet != 0)
        {
            m_TriangleCount += glGetNumTriangles(
                (eGLPrimitive)(u8)packet->primType, packet->numUniqueVertices);
            BeginPacket(packet);
            callbackManager.DoCallback(packet, 1);
            EndPacket(packet);
            packet = sorter->fn_0C();
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
    if (m_RenderPair)
    {
        return m_RenderPair;
    }
    return glGetBackBufferTarget();
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
    *stackEntry = entry;

    if (entry.view->HasChildren())
    {
        GLViewIteratorEntry childEntry(entry.view->m_Children.m_Head->next,
            entry.view->m_Children.m_Head->Entry());
        Push(childEntry);
    }
}

GLViewIterator::GLViewIterator(GLView* root)
{
    m_Depth = -1;
    Push(GLViewIteratorEntry::Root(root));
}

void GLViewIterator::Next()
{
    if (m_Depth < 0)
        return;

    if (m_Stack[m_Depth].next.IsValid())
    {
        m_Stack[m_Depth] =
            *(GLViewIteratorEntry*)m_Stack[m_Depth].next.CurrentEntry();

        GLView* view = m_Stack[m_Depth].view;
        if (view->HasChildren())
        {
            GLViewIteratorEntry childEntry(view->m_Children.m_Head->next,
                view->m_Children.m_Head->Entry());
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

extern const nlMatrix4 gGLViewIdentityMatrix;


void glViewCompact()
{
    GLViewIterator iterator(&gRootView);
    while (!iterator.IsDone())
    {
        GLView* view = iterator.Current();
        view->m_Sorters->Clear();
        view->m_Sorters->m_Allocator.FreeBlocks();
        iterator.Next();
    }
}

GLViewInterface gDefaultViewInterface;

GLView gRootView;

void GLViewInterface::GetViewMatrix(nlMatrix4& matrix) const
{
    matrix.SetIdentity();
}

void GLViewInterface::GetProjectionMatrix(nlMatrix4& matrix) const
{
    matrix.SetIdentity();
}

void GLViewInterface::GetInverseViewMatrix(nlMatrix4& matrix) const
{
    matrix.SetIdentity();
}

void GLViewInterface::GetViewProjectionMatrix(nlMatrix4& matrix) const
{
    matrix.SetIdentity();
}

const nlMatrix4* GLViewInterface::GetViewMatrix() const
{
    return &gGLViewIdentityMatrix;
}

const nlMatrix4* GLViewInterface::GetProjectionMatrix() const
{
    return &gGLViewIdentityMatrix;
}

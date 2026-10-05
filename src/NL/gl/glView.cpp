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

const nlMatrix4 gGLViewIdentityMatrix = {
    { { { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f } } }
};

class DefaultGLViewInterface : public GLViewInterface
{
public:
    virtual void GetViewMatrix(nlMatrix4& matrix) const
    {
        matrix.SetIdentity();
    }

    virtual void GetProjectionMatrix(nlMatrix4& matrix) const
    {
        matrix.SetIdentity();
    }

    virtual void GetInverseViewMatrix(nlMatrix4& matrix) const
    {
        matrix.SetIdentity();
    }

    virtual void GetViewProjectionMatrix(nlMatrix4& matrix) const
    {
        matrix.SetIdentity();
    }

    virtual const nlMatrix4* GetViewMatrix() const
    {
        return &gGLViewIdentityMatrix;
    }

    virtual const nlMatrix4* GetProjectionMatrix() const
    {
        return &gGLViewIdentityMatrix;
    }
};

static DefaultGLViewInterface gDefaultViewInterface;

class GLPacketSorterTree
    : public nlAVLTreeSlotPool<long, GLPacketSorter*,
          DefaultKeyCompare<long> >
{
public:
    static void* operator new(unsigned long size)
    {
        return nlMalloc(size, 8, false);
    }

    GLPacketSorterTree(int initial, int delta)
        : nlAVLTreeSlotPool<long, GLPacketSorter*,
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
            && diagonal
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
    const float xScale = pProj->m11;
    const float yScale = pProj->m22;
    const float zScale = pProj->m33;
    const float xOffset = pProj->m14;
    const float yOffset = pProj->m24;
    const float zOffset = pProj->m34;
    viewPosition->x = (normalized->x - xOffset) / xScale;
    viewPosition->y = (normalized->y - yOffset) / yScale;
    viewPosition->z = (normalized->z - zOffset) / zScale;
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
    nlVector3 v3NDC;
    GLViewViewport viewport = view->m_Viewport;
    glViewProjectPoint(view, *world, v3NDC);
    unsigned long vpWidth = viewport.width;
    unsigned long vpHeight = viewport.height;
    screen->x = (v3NDC.x * (float)vpWidth) / 2.0f;
    screen->y = (v3NDC.y * (float)vpHeight) / 2.0f;
    screen->x += (float)vpWidth / 2.0f;
    screen->y += (float)vpHeight / 2.0f;
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

class GLPacketSorterIterator
{
public:
    typedef AVLTreeEntry<long, GLPacketSorter*> Entry;

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

GLPacketSorter* CreateTexturePacketSorter()
{
    return new GLTexturePacketSorter;
}

GLPacketSorter* CreateReversePacketSorter()
{
    return new GLReversePacketSorter;
}

GLPacketSorter* CreateUnsortedPacketSorter()
{
    return new GLUnsortedPacketSorter;
}

GLPacketSorter* CreateTransformedMatrixDepthPacketSorter()
{
    return new GLTransformedMatrixDepthPacketSorter;
}

GLPacketSorter* CreateTransformedDepthPacketSorter()
{
    return new GLTransformedDepthPacketSorter;
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
        createSorter = CreateTransformedDepthPacketSorter;
        break;
    case GLViewSort_TransformedMatrixDepth:
        createSorter = CreateTransformedMatrixDepthPacketSorter;
        break;
    case GLViewSort_None:
        createSorter = CreateUnsortedPacketSorter;
        break;
    case GLViewSort_Reverse:
        createSorter = CreateReversePacketSorter;
        break;
    default:
        createSorter = CreateTexturePacketSorter;
        break;
    }

    m_CreateSorter = createSorter;
    m_Sorters = new GLPacketSorterTree(16, 16);
    m_Viewport.x = 0;
    m_Viewport.y = 0;
    m_Viewport.width = glGetScreenWidth();
    m_Viewport.height = glGetScreenHeight();
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
    m_CreateSorter = CreateTexturePacketSorter;
    m_Sorters = new GLPacketSorterTree(16, 16);
    m_Viewport.x = 0;
    m_Viewport.y = 0;
    m_Viewport.width = glGetScreenWidth();
    m_Viewport.height = glGetScreenHeight();
    m_Enabled = true;
    m_ClearDepth = false;
    m_Unknown32 = false;
    m_ClearColour = false;
    m_Target = 0;
    m_Visible = true;
}

inline GLPacketSorter* GLView::GetSorter(long layer)
{
    GLPacketSorter* sorter;
    GLPacketSorter** foundSorter;
    AVLTreeNode* existingNode;
    if (!m_Sorters->FindGet(layer, &foundSorter))
    {
        sorter = m_CreateSorter();
        m_Sorters->AddAVLNode((AVLTreeNode**)&m_Sorters->m_Root,
            &layer,
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

void GLView::AttachPacket(const glModelPacket* packet, unsigned long layer)
{
    GetSorter(layer)->AttachPacket(this, packet);
}

void GLView::AttachModel(const glModel* model, unsigned long layer)
{
    GLPacketSorter& sorter = *GetSorter(layer);
    unsigned long packetOffset;
    unsigned long index;
    for (index = 0, packetOffset = 0; index < model->numPackets;
         packetOffset += sizeof(glModelPacket), ++index)
    {
        sorter.AttachPacket(this,
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
        const glModelPacket* packet = sorter->First();
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
    m_Children.RemoveEntry(child);
}

GLRenderPair GLView::GetRenderPair() const
{
    if (m_RenderPair)
    {
        return m_RenderPair;
    }
    return glGetBackBufferTarget();
}

GLViewIterator::GLViewIterator(GLView* root)
{
    m_Depth = -1;
    Push(Root(root));
}

void GLViewIterator::Push(GLViewIteratorEntry entry)
{
    GLViewIteratorEntry* stackEntry = &m_Stack[++m_Depth];
    *stackEntry = entry;

    if (entry.entry->HasChildren())
    {
        nlListIterator<GLView*> children = entry.entry->m_Children.Begin();
        Push(*children.CurrentEntry());
    }
}

void GLViewIterator::Next()
{
    if (m_Depth < 0)
        return;

    if (m_Stack[m_Depth].next != 0)
    {
        m_Stack[m_Depth] = *m_Stack[m_Depth].next;

        GLView* view = m_Stack[m_Depth].entry;
        if (view->HasChildren())
        {
            nlListIterator<GLView*> children = view->m_Children.Begin();
            Push(*children.CurrentEntry());
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
        return m_Stack[m_Depth].entry;
    return 0;
}

bool GLViewIterator::IsDone() const
{
    return m_Depth < 0;
}

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

GLView gRootView;

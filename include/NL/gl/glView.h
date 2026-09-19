#ifndef NL_GL_GLVIEW_H
#define NL_GL_GLVIEW_H

#include "NL/gl/glModel.h"
#include "NL/gl/glTarget.h"
#include "NL/nlList.h"
#include "NL/nlMath.h"

class UnidentifiedPacketSorter;
class UnidentifiedPacketSorterTree_8052E504;
class GLView;

typedef void (*GLViewPacketCallback)(
    GLView*, unsigned long, const glModelPacket*);

enum GLViewSortMode
{
    GLViewSort_Texture = 0,
    GLViewSort_TransformedDepth = 1,
    GLViewSort_TransformedMatrixDepth = 2,
    GLViewSort_None = 3,
    GLViewSort_Reverse = 4,
    GLViewSort_Count = 5
};

class GLViewInterface
{
public:
    virtual void GetViewMatrix(nlMatrix4&) const;
    virtual void GetProjectionMatrix(nlMatrix4&) const;
    virtual void GetInverseViewMatrix(nlMatrix4&) const;
    virtual void GetViewProjectionMatrix(nlMatrix4&) const;
    virtual const nlMatrix4* GetViewMatrix() const;
    virtual const nlMatrix4* GetProjectionMatrix() const;
    virtual const nlVector4* GetShadowMatrix() const
    {
        return 0;
    }
};

typedef UnidentifiedPacketSorter* (*UnidentifiedPacketSorterFactory)();

// GLView::m_Target selects the platform copy issued after the view is drawn.
// glPlat only acts on 8, 9 and 10; any other value skips the copy entirely.
// The stripped DOL preserves the values but not their names, so they stay
// value-named.
enum GLViewTargetMode
{
    GLViewTarget_None = 0,
    GLViewTarget_Mode8 = 8,
    GLViewTarget_Mode9 = 9,
    GLViewTarget_Mode10 = 10
};

class GLView
{
public:
    inline GLView();
    GLView(GLViewInterface*, const GLRenderPair&, GLViewSortMode);
    virtual ~GLView();

    void AttachPacket(const glModelPacket*, unsigned long);
    void AttachModel(const glModel*, unsigned long);
    void Iterate(GLViewPacketCallback);
    void RemoveChild(GLView*);
    GLRenderPair GetRenderPair() const;
    inline UnidentifiedPacketSorter* GetSorter(long);

    bool HasChildren() const
    {
        return m_Children.m_Head != 0;
    }

    void AddChild(GLView* child)
    {
        m_Children.AddEnd(child);
        child->m_Parent = this;
    }

    void SetParent(GLView* parent)
    {
        parent->AddChild(this);
    }

    void SetRenderPair(GLRenderPair renderPair)
    {
        m_RenderPair = renderPair;
    }

    void SetViewport(unsigned long x, unsigned long y, unsigned long width,
        unsigned long height)
    {
        m_ViewportX = x;
        m_ViewportY = y;
        m_ViewportWidth = width;
        m_ViewportHeight = height;
    }

    virtual void BeginRender();
    virtual void EndRender();
    virtual void BeginPacket(const glModelPacket*);
    virtual void EndPacket(const glModelPacket*);

    nlListContainer<GLView*> m_Children;
    UnidentifiedPacketSorterFactory m_CreateSorter;
    UnidentifiedPacketSorterTree_8052E504* m_Sorters;
    unsigned long m_ViewportX;
    unsigned long m_ViewportY;
    unsigned long m_ViewportWidth;
    unsigned long m_ViewportHeight;
    GLRenderPair m_RenderPair;
    bool m_Enabled;
    bool m_ClearDepth;
    bool m_Unknown32;
    bool m_ClearColour;
    unsigned long m_Target;
    void* m_Unknown38;
    void* m_Unknown3C;
    bool m_Visible;
    u8 m_Pad41[3];
    const char* m_Name;
    unsigned long m_Unknown48;
    unsigned long m_TriangleCount;
    GLViewInterface* m_Interface;
    GLView* m_Parent;
};

struct GLViewIteratorEntry
{
    GLViewIteratorEntry()
        : next((ListEntry<GLView*>*)0)
    {
    }

    GLViewIteratorEntry(
        ListEntry<GLView*>* nextEntry, GLView* currentView)
        : next(nextEntry)
        , view(currentView)
    {
    }

    static GLViewIteratorEntry Root(GLView* view)
    {
        return GLViewIteratorEntry(0, view);
    }

    nlListIterator<GLView*> next;
    GLView* view;
};

class GLViewIterator
{
public:
    GLViewIterator(GLView*);
    inline void Push(const GLViewIteratorEntry&);
    void Next();
    GLView* Current() const;
    bool IsDone() const;

    GLViewIteratorEntry m_Stack[8];
    int m_Depth;
};

extern GLViewInterface gDefaultViewInterface;
extern GLView gRootView;

void glViewCompact();
void glViewProjectPoint(GLView* view, const nlVector3& v3world, nlVector3& v3NDC);
void glViewUnprojectOrthographicPoint(GLView* view, const nlVector3* normalized, nlVector3* viewPosition);
float glViewGetOrthographicWidth(GLView* view);
float glViewGetOrthographicHeight(GLView* view);
void glViewProjectPointToViewport(GLView* view, const nlVector3* world, nlVector3* screen);
void glViewProjectPointBetweenViews(GLView* source, GLView* destination, const nlVector3* world, nlVector3* projected);
void gl_ViewReset();
void gl_ViewStartup();

#endif // NL_GL_GLVIEW_H

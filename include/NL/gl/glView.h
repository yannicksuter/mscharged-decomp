#ifndef NL_GL_GLVIEW_H
#define NL_GL_GLVIEW_H

#include "NL/gl/glModel.h"
#include "NL/gl/glTarget.h"
#include "NL/nlList.h"
#include "NL/nlMath.h"

class GLPacketSorter;
class GLPacketSorterTree;
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
    virtual void GetViewMatrix(nlMatrix4&);
    virtual void GetProjectionMatrix(nlMatrix4&);
    virtual void GetInverseViewMatrix(nlMatrix4&);
    virtual void GetViewProjectionMatrix(nlMatrix4&);
    virtual const nlMatrix4* GetViewMatrix() const;
    virtual const nlMatrix4* GetProjectionMatrix() const;
    virtual const nlMatrix4* GetShadowMatrix()
    {
        return 0;
    }
};

typedef GLPacketSorter* (*GLPacketSorterFactory)();

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
    inline GLPacketSorter* GetSorter(unsigned long);

    void SetRenderPair(GLRenderPair renderPair)
    {
        m_RenderPair = renderPair;
    }

    virtual void BeginRender();
    virtual void EndRender();
    virtual void BeginPacket(const glModelPacket*);
    virtual void EndPacket(const glModelPacket*);

    nlListContainer<GLView*> m_Children;
    GLPacketSorterFactory m_CreateSorter;
    GLPacketSorterTree* m_Sorters;
    unsigned long m_ViewportX;
    unsigned long m_ViewportY;
    unsigned long m_ViewportWidth;
    unsigned long m_ViewportHeight;
    GLRenderPair m_RenderPair;
    bool m_Enabled;
    bool m_ClearColour;
    bool m_ClearDepth;
    bool m_Unknown33;
    unsigned long m_Target;
    void* m_Unknown38;
    void* m_Unknown3C;
    bool m_Visible;
    u8 m_Pad41[3];
    const char* m_Name;
    unsigned long m_Unknown48;
    unsigned long m_TriangleCount;
    GLViewInterface* m_Interface;
    void* m_Parent;
};

struct GLViewIteratorEntry
{
    GLViewIteratorEntry()
        : next(0)
    {
    }

    ListEntry<GLView*>* next;
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

extern "C" GLViewInterface lbl_806E1F38;
extern "C" GLView lbl_8057F250;

void glViewCompact();

#endif // NL_GL_GLVIEW_H

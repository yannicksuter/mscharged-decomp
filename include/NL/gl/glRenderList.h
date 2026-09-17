#ifndef NL_GL_GLRENDERLIST_H
#define NL_GL_GLRENDERLIST_H

#include "NL/gl/glMemory.h"
#include "NL/gl/glPacketCallback.h"
#include "NL/gl/glView.h"
#include "NL/nlAVLTree.h"

class UnidentifiedPacketSorter
{
public:
    static void* operator new(unsigned long size)
    {
        return glFrameAlloc(size, GLM_Header);
    }

    virtual const glModelPacket* fn_08() = 0;
    virtual const glModelPacket* fn_0C() = 0;
    virtual void fn_10(GLView*, const glModelPacket*) = 0;
};

typedef unsigned long long UnidentifiedPacketSortKey;
typedef AVLTreeEntry<UnidentifiedPacketSortKey, const glModelPacket*> UnidentifiedPacketTreeEntry;

template <typename T>
class UnidentifiedFrameAllocator
{
public:
    T* Allocate()
    {
        return (T*)glFrameAlloc(sizeof(T), GLM_Header);
    }

    void Allocate(T*& out)
    {
        out = Allocate();
    }

    void Delete(T* entry)
    {
        entry->~T();
    }
};

class UnidentifiedPacketTree_8052E568
    : public AVLTreeBase<UnidentifiedPacketSortKey, const glModelPacket*,
          UnidentifiedFrameAllocator<UnidentifiedPacketTreeEntry>,
          DefaultKeyCompare<UnidentifiedPacketSortKey> >
{
};

class UnidentifiedPacketSorter_802CCBBC : public UnidentifiedPacketSorter
{
public:
    virtual const glModelPacket* fn_08();
    virtual const glModelPacket* fn_0C();
    virtual void fn_10(GLView*, const glModelPacket*);
    virtual unsigned long fn_14(GLView*, const glModelPacket*) = 0;

    UnidentifiedPacketTree_8052E568 m_Tree;
    nlAVLTreeIterator<UnidentifiedPacketSortKey, const glModelPacket*,
        DefaultKeyCompare<UnidentifiedPacketSortKey> >
        m_Iterator;
};

class UnidentifiedPacketSorter_8052E2D8 : public UnidentifiedPacketSorter_802CCBBC
{
public:
    virtual unsigned long fn_14(GLView*, const glModelPacket*);
};

class UnidentifiedPacketSorter_8052E2C0 : public UnidentifiedPacketSorter_802CCBBC
{
public:
    virtual unsigned long fn_14(GLView*, const glModelPacket*);
};

class UnidentifiedPacketSorter_8052E2A8 : public UnidentifiedPacketSorter_802CCBBC
{
public:
    UnidentifiedPacketSorter_8052E2A8()
        : m_Sequence(0)
    {
    }

    virtual unsigned long fn_14(GLView*, const glModelPacket*);

    unsigned long m_Sequence;
};

class UnidentifiedPacketSorter_802D033C : public UnidentifiedPacketSorter
{
public:
    UnidentifiedPacketSorter_802D033C()
        : m_Head(0)
        , m_Tail(0)
        , m_Current(0)
    {
    }

    virtual const glModelPacket* fn_08();
    virtual const glModelPacket* fn_0C();

protected:
    u8 m_Allocator;
    ListEntry<const glModelPacket*>* m_Head;
    ListEntry<const glModelPacket*>* m_Tail;
    ListEntry<const glModelPacket*>* m_Current;
};

class UnidentifiedPacketSorter_8052E540 : public UnidentifiedPacketSorter_802D033C
{
public:
    virtual void fn_10(GLView*, const glModelPacket*);
};

class UnidentifiedPacketSorter_8052E554 : public UnidentifiedPacketSorter_802D033C
{
public:
    virtual void fn_10(GLView*, const glModelPacket*);
};

#endif // NL_GL_GLRENDERLIST_H

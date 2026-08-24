#ifndef NW4HBM_UT_LINK_LIST_H
#define NW4HBM_UT_LINK_LIST_H

#include <revolution/types.h>

#include "revolution/hbm/HBMAssert.hpp"
#include "revolution/hbm/nw4hbm/ut/inlines.h"

namespace nw4hbm {
namespace ut {

namespace detail {
class LinkListImpl;
}

/******************************************************************************
 *
 * Linked list node
 *
 ******************************************************************************/
class LinkListNode : private NonCopyable {
    friend class detail::LinkListImpl;

public:
    LinkListNode() : mNext(NULL), mPrev(NULL) {}

    LinkListNode* GetNext() const { return mNext; }
    LinkListNode* GetPrev() const { return mPrev; }

private:
    /* 0x00 */ LinkListNode* mNext;
    /* 0x04 */ LinkListNode* mPrev;
}; // size = 0x08

namespace detail {

/******************************************************************************
 *
 * Linked list implementation
 *
 ******************************************************************************/
class LinkListImpl : private NonCopyable {
public:
    class ConstIterator;

    class Iterator {
        friend class LinkListImpl;
        friend class ConstIterator;

    public:
        Iterator() : mPointer(NULL) {}
        explicit Iterator(LinkListNode* pNode) : mPointer(pNode) {}

        Iterator& operator++() {
            mPointer = mPointer->GetNext();
            return *this;
        }

        Iterator& operator--() {
            mPointer = mPointer->GetPrev();
            return *this;
        }

        LinkListNode* operator->() const { return mPointer; }

        friend bool operator==(LinkListImpl::Iterator lhs, LinkListImpl::Iterator rhs) {
            return lhs.mPointer == rhs.mPointer;
        }

    private:
        /* 0x00 */ LinkListNode* mPointer;
    }; // size = 0x04

    class ConstIterator {
        friend class LinkListImpl;

    public:
        explicit ConstIterator(Iterator it) : mNode(it.mPointer) {}

        ConstIterator& operator++() {
            mNode = mNode->GetNext();
            return *this;
        }

        ConstIterator& operator--() {
            mNode = mNode->GetPrev();
            return *this;
        }

        const LinkListNode* operator->() const { return mNode; }

        friend bool operator==(LinkListImpl::ConstIterator lhs, LinkListImpl::ConstIterator rhs) {
            return lhs.mNode == rhs.mNode;
        }

    private:
        /* 0x00 */ LinkListNode* mNode;
    }; // size = 0x04

protected:
    static Iterator GetIteratorFromPointer(LinkListNode* pNode) { return Iterator(pNode); }

    LinkListImpl() { Initialize_(); }
    ~LinkListImpl();

    Iterator GetBeginIter() { return Iterator(mNode.GetNext()); }
    Iterator GetEndIter() { return Iterator(&mNode); }

    Iterator Insert(Iterator it, LinkListNode* pNode);

    Iterator Erase(Iterator it);
    Iterator Erase(LinkListNode* pNode);
    Iterator Erase(Iterator begin, Iterator end);

public:
    u32 GetSize() const { return mSize; }
    bool IsEmpty() const { return mSize == 0; }

    void PopFront() { Erase(GetBeginIter()); }
    void PopBack() { Erase(--GetEndIter()); }

    void Clear();
    void SetPrev(LinkListNode* p, LinkListNode* pPrev);
    void SetNext(LinkListNode* p, LinkListNode* pNext);

private:
    void Initialize_() {
        mSize = 0;
        mNode.mNext = &mNode;
        mNode.mPrev = &mNode;
    }

private:
    /* 0x00 */ u32 mSize;
    /* 0x04 */ LinkListNode mNode;
}; // size = 0x0C

} // namespace detail
} // namespace ut
} // namespace nw4hbm

#endif

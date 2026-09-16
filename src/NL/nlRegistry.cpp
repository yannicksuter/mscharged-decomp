#include "NL/nlRegistry.h"

RegistryNode::~RegistryNode()
{
    if (mValue.mType == 2)
    {
        ((DynamicRegistryContainer*)mOwner)
            ->mAllocator->FreeItem(GetData());
    }
}

RegistryNode* DynamicRegistryContainer::AddNamed(
    const char* name)
{
    RegistryNode* node
        = new (mAllocator->AllocNode()) RegistryNode(name, this);
    RegistryNode* tail = mNamed;
    if (tail == 0)
    {
        mNamed = node;
        node->mNext = node;
        node->mPrev = node;
    }
    else
    {
        tail->mNext->mPrev = node;
        node->mNext = tail->mNext;
        node->mPrev = tail;
        tail->mNext = node;
    }
    mNamed = node;
    return node;
}

void DynamicRegistryContainer::UnidentifiedVirtual34() {}

void DynamicRegistryContainer::UnidentifiedVirtual38() {}

RegistryContainer* DynamicRegistryContainer::AddChild(
    const char* name)
{
    RegistryNode* node = AddNamed(name);
    node->mValue.mType = 3;
    RegistryContainer* child = new (mAllocator->AllocContainer())
        DynamicRegistryContainer(mAllocator);
    node->mValue.mData = child;
    return child;
}

RegistryNode* DynamicRegistryContainer::AddUnnamed()
{
    RegistryNode* node
        = new (mAllocator->AllocNode()) RegistryNode("", this);
    RegistryNode* tail = mUnnamed;
    if (tail == 0)
    {
        mUnnamed = node;
        node->mNext = node;
        node->mPrev = node;
    }
    else
    {
        tail->mNext->mPrev = node;
        node->mNext = tail->mNext;
        node->mPrev = tail;
        tail->mNext = node;
    }
    mUnnamed = node;
    return node;
}

RegistryContainer*
DynamicRegistryContainer::AddUnnamedChild()
{
    RegistryNode* node = AddUnnamed();
    node->mValue.mType = 3;
    RegistryContainer* child = new (mAllocator->AllocContainer())
        DynamicRegistryContainer(mAllocator);
    node->mValue.mData = child;
    return child;
}

bool DynamicRegistryContainer::Has(const u32& hash) const
{
    return Get(hash).mType != 4;
}

RegistryValue DynamicRegistryContainer::Get(
    const u32& hash) const
{
    if (mNamed == 0)
    {
        return RegistryValue();
    }
    RegistryNode* node = First(1);
    do
    {
        if (hash == node->mHash)
        {
            return node->mValue;
        }
        node = node->mNext;
    } while (!IsFirst(node, 1));
    return RegistryValue();
}

RegistryNode* DynamicRegistryContainer::Find(
    const u32& hash)
{
    if (mNamed == 0)
    {
        return &gEmptyRegistryNode;
    }
    RegistryNode* node = First(1);
    do
    {
        if (hash == node->mHash)
        {
            return node;
        }
        node = node->mNext;
    } while (!IsFirst(node, 1));
    return &gEmptyRegistryNode;
}

int DynamicRegistryContainer::CountUnnamed() const
{
    if (mUnnamed == 0)
    {
        return 0;
    }
    int count = 0;
    RegistryNode* node = mUnnamed;
    do
    {
        node = node->mNext;
        count++;
    } while (node != mUnnamed);
    return count;
}

int DynamicRegistryContainer::CountNamed() const
{
    if (mNamed == 0)
    {
        return 0;
    }
    int count = 0;
    RegistryNode* node = mNamed;
    do
    {
        node = node->mNext;
        count++;
    } while (node != mNamed);
    return count;
}

void RegistryListIterator::Next()
{
    mCurrent = AtLast() ? 0 : mCurrent->mNext;
}

bool RegistryListIterator::MovePrevious()
{
    if (AtFirst())
    {
        return false;
    }
    mCurrent->mPrev->mPrev->mNext = mCurrent;
    mCurrent->mPrev->mNext = mCurrent->mNext;
    mCurrent->mNext->mPrev = mCurrent->mPrev;
    RegistryNode* previous = mCurrent->mPrev->mPrev;
    mCurrent->mPrev->mPrev = mCurrent;
    mCurrent->mNext = mCurrent->mPrev;
    mCurrent->mPrev = previous;
    return true;
}

bool RegistryListIterator::MoveNext()
{
    if (AtLast())
    {
        return false;
    }
    mCurrent->mNext->mNext->mPrev = mCurrent;
    mCurrent->mNext->mPrev = mCurrent->mPrev;
    mCurrent->mPrev->mNext = mCurrent->mNext;
    RegistryNode* next = mCurrent->mNext->mNext;
    mCurrent->mNext->mNext = mCurrent;
    mCurrent->mPrev = mCurrent->mNext;
    mCurrent->mNext = next;
    return true;
}

bool PackedRegistryContainer::Has(const u32& hash) const
{
    u16 count = mNamedCount;
    const PackedRegistryEntry* entries
        = RegistryNamedEntries(mWords, count, mUnnamedCount);
    u32 key = hash;
    return FindPackedRegistryEntry(key, entries, count) != 0;
}

RegistryValue PackedRegistryContainer::Get(
    const u32& hash) const
{
    const u32* types = mWords;
    const PackedRegistryEntry* entries
        = RegistryNamedEntries(types, mNamedCount, mUnnamedCount);
    u32 key = hash;
    const PackedRegistryEntry* entry
        = FindPackedRegistryEntry(key, entries, mNamedCount);
    if (entry != 0)
    {
        int index = entry - entries;
        return RegistryValue(
            entry->mData, RegistryTypeAt(types, index));
    }
    RegistryValue none;
    return none;
}

int PackedRegistryContainer::CountUnnamed() const
{
    return mUnnamedCount;
}

int PackedRegistryContainer::CountNamed() const
{
    return mNamedCount;
}

RegistryNode* PackedRegistryContainer::AddNamed(
    const char* name)
{
    return &gEmptyRegistryNode;
}

RegistryContainer* PackedRegistryContainer::AddChild(
    const char* name)
{
    return 0;
}

RegistryNode* PackedRegistryContainer::AddUnnamed()
{
    return AddNamed("");
}

RegistryContainer*
PackedRegistryContainer::AddUnnamedChild()
{
    return AddChild("");
}

bool ScopedRegistryContainer::Has(const u32& hash) const
{
    if (mParent != 0 && mParent->Has(hash))
    {
        return true;
    }
    return Get(hash).mType != 4;
}

RegistryValue ScopedRegistryContainer::Get(
    const u32& hash) const
{
    if (mParent != 0 && mParent->Has(hash))
    {
        return mParent->Get(hash);
    }
    return DynamicRegistryContainer::Get(hash);
}

RegistryNode* ScopedRegistryContainer::Find(
    const u32& hash)
{
    if (mParent != 0)
    {
        mParent->Has(hash);
    }
    return DynamicRegistryContainer::Find(hash);
}

int ScopedRegistryContainer::CountUnnamed() const
{
    int count = mParent != 0 ? mParent->CountUnnamed() : 0;
    return count + DynamicRegistryContainer::CountUnnamed();
}

int ScopedRegistryContainer::CountNamed() const
{
    int count = mParent != 0 ? mParent->CountNamed() : 0;
    return count + DynamicRegistryContainer::CountNamed();
}

RegistryContainer* ScopedRegistryContainer::AddChild(
    const char* name)
{
    RegistryNode* node = AddNamed(name);
    node->mValue.mType = 3;
    RegistryContainer* child = new (mAllocator->AllocContainer())
        ScopedRegistryContainer(mAllocator);
    node->mValue.mData = child;
    return child;
}

RegistryNode gEmptyRegistryNode("");

#include "NL/nlRegistry.inl"

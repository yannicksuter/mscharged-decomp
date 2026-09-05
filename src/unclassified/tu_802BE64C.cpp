#include "unclassified/tu_802BE64C.h"

UnidentifiedRegistryNode_802BE64C::~UnidentifiedRegistryNode_802BE64C()
{
    if (mValue.mType == 2)
    {
        ((UnidentifiedRegistryDynamic_802BEA10*)mOwner)
            ->mAllocator->FreeValue(GetData());
    }
}

UnidentifiedRegistryNode_802BE64C* UnidentifiedRegistryDynamic_802BEA10::AddNamed(
    const char* name)
{
    UnidentifiedRegistryNode_802BE64C* node
        = new (mAllocator->AllocNode()) UnidentifiedRegistryNode_802BE64C(name, this);
    UnidentifiedRegistryNode_802BE64C* tail = mNamed;
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

void UnidentifiedRegistryDynamic_802BEA10::UnidentifiedVirtual34() {}

void UnidentifiedRegistryDynamic_802BEA10::UnidentifiedVirtual38() {}

UnidentifiedRegistryContainer* UnidentifiedRegistryDynamic_802BEA10::AddChild(
    const char* name)
{
    UnidentifiedRegistryNode_802BE64C* node = AddNamed(name);
    node->mValue.mType = 3;
    UnidentifiedRegistryContainer* child = new (mAllocator->AllocContainer())
        UnidentifiedRegistryDynamic_802BEA10(mAllocator);
    node->mValue.mData = child;
    return child;
}

UnidentifiedRegistryNode_802BE64C* UnidentifiedRegistryDynamic_802BEA10::AddUnnamed()
{
    UnidentifiedRegistryNode_802BE64C* node
        = new (mAllocator->AllocNode()) UnidentifiedRegistryNode_802BE64C("", this);
    UnidentifiedRegistryNode_802BE64C* tail = mUnnamed;
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

UnidentifiedRegistryContainer*
UnidentifiedRegistryDynamic_802BEA10::AddUnnamedChild()
{
    UnidentifiedRegistryNode_802BE64C* node = AddUnnamed();
    node->mValue.mType = 3;
    UnidentifiedRegistryContainer* child = new (mAllocator->AllocContainer())
        UnidentifiedRegistryDynamic_802BEA10(mAllocator);
    node->mValue.mData = child;
    return child;
}

bool UnidentifiedRegistryDynamic_802BEA10::Has(const u32& hash) const
{
    return Get(hash).mType != 4;
}

UnidentifiedRegistryValue UnidentifiedRegistryDynamic_802BEA10::Get(
    const u32& hash) const
{
    if (mNamed == 0)
    {
        return UnidentifiedRegistryValue();
    }
    UnidentifiedRegistryNode_802BE64C* node = First(1);
    do
    {
        if (hash == node->mHash)
        {
            return node->mValue;
        }
        node = node->mNext;
    } while (!IsFirst(node, 1));
    return UnidentifiedRegistryValue();
}

UnidentifiedRegistryNode_802BE64C* UnidentifiedRegistryDynamic_802BEA10::Find(
    const u32& hash)
{
    if (mNamed == 0)
    {
        return &lbl_8057C498;
    }
    UnidentifiedRegistryNode_802BE64C* node = First(1);
    do
    {
        if (hash == node->mHash)
        {
            return node;
        }
        node = node->mNext;
    } while (!IsFirst(node, 1));
    return &lbl_8057C498;
}

int UnidentifiedRegistryDynamic_802BEA10::CountUnnamed() const
{
    if (mUnnamed == 0)
    {
        return 0;
    }
    int count = 0;
    UnidentifiedRegistryNode_802BE64C* node = mUnnamed;
    do
    {
        node = node->mNext;
        count++;
    } while (node != mUnnamed);
    return count;
}

int UnidentifiedRegistryDynamic_802BEA10::CountNamed() const
{
    if (mNamed == 0)
    {
        return 0;
    }
    int count = 0;
    UnidentifiedRegistryNode_802BE64C* node = mNamed;
    do
    {
        node = node->mNext;
        count++;
    } while (node != mNamed);
    return count;
}

void UnidentifiedRegistryListIterator_802BF828::Next()
{
    mCurrent = AtLast() ? 0 : mCurrent->mNext;
}

bool UnidentifiedRegistryListIterator_802BF828::UnidentifiedVirtual28()
{
    if (AtFirst())
    {
        return false;
    }
    mCurrent->mPrev->mPrev->mNext = mCurrent;
    mCurrent->mPrev->mNext = mCurrent->mNext;
    mCurrent->mNext->mPrev = mCurrent->mPrev;
    UnidentifiedRegistryNode_802BE64C* previous = mCurrent->mPrev->mPrev;
    mCurrent->mPrev->mPrev = mCurrent;
    mCurrent->mNext = mCurrent->mPrev;
    mCurrent->mPrev = previous;
    return true;
}

bool UnidentifiedRegistryListIterator_802BF828::UnidentifiedVirtual2C()
{
    if (AtLast())
    {
        return false;
    }
    mCurrent->mNext->mNext->mPrev = mCurrent;
    mCurrent->mNext->mPrev = mCurrent->mPrev;
    mCurrent->mPrev->mNext = mCurrent->mNext;
    UnidentifiedRegistryNode_802BE64C* next = mCurrent->mNext->mNext;
    mCurrent->mNext->mNext = mCurrent;
    mCurrent->mPrev = mCurrent->mNext;
    mCurrent->mNext = next;
    return true;
}

bool UnidentifiedRegistryPacked_802BED28::Has(const u32& hash) const
{
    u16 count = mNamedCount;
    const UnidentifiedRegistryPackedEntry* entries
        = UnidentifiedRegistryNamedEntries(mWords, count, mUnnamedCount);
    u32 key = hash;
    return UnidentifiedFindEntry(key, entries, count) != 0;
}

UnidentifiedRegistryValue UnidentifiedRegistryPacked_802BED28::Get(
    const u32& hash) const
{
    const u32* types = mWords;
    const UnidentifiedRegistryPackedEntry* entries
        = UnidentifiedRegistryNamedEntries(types, mNamedCount, mUnnamedCount);
    u32 key = hash;
    const UnidentifiedRegistryPackedEntry* entry
        = UnidentifiedFindEntry(key, entries, mNamedCount);
    if (entry != 0)
    {
        int index = entry - entries;
        return UnidentifiedRegistryValue(
            entry->mData, UnidentifiedRegistryTypeAt(types, index));
    }
    UnidentifiedRegistryValue none;
    return none;
}

int UnidentifiedRegistryPacked_802BED28::CountUnnamed() const
{
    return mUnnamedCount;
}

int UnidentifiedRegistryPacked_802BED28::CountNamed() const
{
    return mNamedCount;
}

UnidentifiedRegistryNode_802BE64C* UnidentifiedRegistryPacked_802BED28::AddNamed(
    const char* name)
{
    return &lbl_8057C498;
}

UnidentifiedRegistryContainer* UnidentifiedRegistryPacked_802BED28::AddChild(
    const char* name)
{
    return 0;
}

UnidentifiedRegistryNode_802BE64C* UnidentifiedRegistryPacked_802BED28::AddUnnamed()
{
    return AddNamed("");
}

UnidentifiedRegistryContainer*
UnidentifiedRegistryPacked_802BED28::AddUnnamedChild()
{
    return AddChild("");
}

bool UnidentifiedRegistryScoped_802BEF0C::Has(const u32& hash) const
{
    if (mParent != 0 && mParent->Has(hash))
    {
        return true;
    }
    return Get(hash).mType != 4;
}

UnidentifiedRegistryValue UnidentifiedRegistryScoped_802BEF0C::Get(
    const u32& hash) const
{
    if (mParent != 0 && mParent->Has(hash))
    {
        return mParent->Get(hash);
    }
    return UnidentifiedRegistryDynamic_802BEA10::Get(hash);
}

UnidentifiedRegistryNode_802BE64C* UnidentifiedRegistryScoped_802BEF0C::Find(
    const u32& hash)
{
    if (mParent != 0)
    {
        mParent->Has(hash);
    }
    return UnidentifiedRegistryDynamic_802BEA10::Find(hash);
}

int UnidentifiedRegistryScoped_802BEF0C::CountUnnamed() const
{
    int count = mParent != 0 ? mParent->CountUnnamed() : 0;
    return count + UnidentifiedRegistryDynamic_802BEA10::CountUnnamed();
}

int UnidentifiedRegistryScoped_802BEF0C::CountNamed() const
{
    int count = mParent != 0 ? mParent->CountNamed() : 0;
    return count + UnidentifiedRegistryDynamic_802BEA10::CountNamed();
}

UnidentifiedRegistryContainer* UnidentifiedRegistryScoped_802BEF0C::AddChild(
    const char* name)
{
    UnidentifiedRegistryNode_802BE64C* node = AddNamed(name);
    node->mValue.mType = 3;
    UnidentifiedRegistryContainer* child = new (mAllocator->AllocContainer())
        UnidentifiedRegistryScoped_802BEF0C(mAllocator);
    node->mValue.mData = child;
    return child;
}

UnidentifiedRegistryNode_802BE64C lbl_8057C498("");

#include "unclassified/tu_802BE64C_impl.h"

#ifndef NL_REGISTRY_INL
#define NL_REGISTRY_INL

#include "NL/nlRegistry.h"

inline void* RegistryNode::GetData()
{
    return mValue.mData;
}

inline void* RegistryNode::UnidentifiedVirtual0C()
{
    return mValue.mData;
}

inline bool RegistryListIterator::IsDone()
{
    return mCurrent == 0;
}

inline void RegistryListIterator::ResetCursor()
{
    mCurrent = mTail;
}

inline bool RegistryListIterator::IsFirst()
{
    return AtFirst();
}

inline RegistryValue RegistryListIterator::GetValue()
{
    return mCurrent->mValue;
}

inline RegistryNode* RegistryListIterator::GetNode()
{
    return mCurrent;
}

inline u32 RegistryListIterator::GetHash()
{
    return mCurrent->mHash;
}

inline RegistryValue DynamicRegistryContainer::UnnamedList()
{
    return RegistryValue(this, 0);
}

inline RegistryValue DynamicRegistryContainer::NamedList()
{
    return RegistryValue(this, 1);
}

inline void DynamicRegistryContainer::UnidentifiedVirtual28() {}

inline RegistryNode* DynamicRegistryContainer::UnidentifiedVirtual44(
    const u32& hash)
{
    return Find(hash);
}

inline void DynamicRegistryContainer::GetIterator(
    RegistryIteratorBase* iterator, int which)
{
    new (iterator) RegistryListIterator(Tail(which), First(which));
}

inline void PackedRegistryIteratorBase::Next()
{
    mIndex++;
}

inline bool PackedRegistryIteratorBase::IsDone()
{
    return mIndex >= mCount;
}

inline void PackedRegistryIteratorBase::ResetCursor()
{
    mIndex = 0;
}

inline bool PackedRegistryIteratorBase::IsFirst()
{
    return mIndex == 0;
}

inline RegistryNode* PackedRegistryIteratorBase::GetNode()
{
    return &gEmptyRegistryNode;
}

inline bool PackedRegistryIteratorBase::MovePrevious()
{
    return false;
}

inline bool PackedRegistryIteratorBase::MoveNext()
{
    return false;
}

inline RegistryValue PackedNamedRegistryIterator::GetValue()
{
    const PackedRegistryEntry* entries
        = (const PackedRegistryEntry*)mEntries;
    return RegistryValue(
        entries[mIndex].mData, RegistryTypeAt(mTypes, mIndex));
}

inline u32 PackedNamedRegistryIterator::GetHash()
{
    const PackedRegistryEntry* entries
        = (const PackedRegistryEntry*)mEntries;
    return entries[mIndex].mHash;
}

inline void** PackedNamedRegistryIterator::GetValueSlot()
{
    PackedRegistryEntry* entries = (PackedRegistryEntry*)mEntries;
    return &entries[mIndex].mData;
}

inline RegistryValue PackedUnnamedRegistryIterator::GetValue()
{
    void* const* entries = (void* const*)mEntries;
    return RegistryValue(entries[mIndex], RegistryTypeAt(mTypes, mIndex));
}

inline u32 PackedUnnamedRegistryIterator::GetHash()
{
    return 0;
}

inline void** PackedUnnamedRegistryIterator::GetValueSlot()
{
    void** entries = (void**)mEntries;
    return &entries[mIndex];
}

inline RegistryNode* PackedRegistryContainer::Find(const u32& hash)
{
    return &gEmptyRegistryNode;
}

inline RegistryValue PackedRegistryContainer::UnnamedList()
{
    return RegistryValue(this, 0);
}

inline RegistryValue PackedRegistryContainer::NamedList()
{
    return RegistryValue(this, 1);
}

inline void PackedRegistryContainer::UnidentifiedVirtual28() {}

inline void PackedRegistryContainer::UnidentifiedVirtual38() {}

inline void PackedRegistryContainer::UnidentifiedVirtual34() {}

inline void PackedRegistryContainer::GetIterator(
    RegistryIteratorBase* iterator, int which)
{
    if (which != 0)
    {
        PackedNamedRegistryIterator::Construct(this, iterator);
    }
    else
    {
        PackedUnnamedRegistryIterator::Construct(this, iterator);
    }
}

inline RegistryValue ScopedRegistryContainer::UnnamedList()
{
    return RegistryValue(this, 0);
}

inline RegistryValue ScopedRegistryContainer::NamedList()
{
    return RegistryValue(this, 1);
}

inline void ScopedRegistryContainer::UnidentifiedVirtual28() {}

inline void ScopedRegistryContainer::UnidentifiedVirtual38() {}

inline void ScopedRegistryContainer::UnidentifiedVirtual34() {}

#endif // NL_REGISTRY_INL

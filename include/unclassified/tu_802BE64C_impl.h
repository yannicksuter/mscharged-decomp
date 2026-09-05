#ifndef UNCLASSIFIED_TU_802BE64C_IMPL_H
#define UNCLASSIFIED_TU_802BE64C_IMPL_H

#include "unclassified/tu_802BE64C.h"

inline void* UnidentifiedRegistryNode_802BE64C::GetData()
{
    return mValue.mData;
}

inline void* UnidentifiedRegistryNode_802BE64C::UnidentifiedVirtual0C()
{
    return mValue.mData;
}

inline bool UnidentifiedRegistryListIterator_802BF828::IsDone()
{
    return mCurrent == 0;
}

inline void UnidentifiedRegistryListIterator_802BF828::UnidentifiedVirtual14()
{
    mCurrent = mTail;
}

inline bool UnidentifiedRegistryListIterator_802BF828::IsFirst()
{
    return AtFirst();
}

inline UnidentifiedRegistryValue UnidentifiedRegistryListIterator_802BF828::GetValue()
{
    return mCurrent->mValue;
}

inline UnidentifiedRegistryNode_802BE64C* UnidentifiedRegistryListIterator_802BF828::GetNode()
{
    return mCurrent;
}

inline u32 UnidentifiedRegistryListIterator_802BF828::GetHash()
{
    return mCurrent->mHash;
}

inline UnidentifiedRegistryValue UnidentifiedRegistryDynamic_802BEA10::UnnamedList()
{
    return UnidentifiedRegistryValue(this, 0);
}

inline UnidentifiedRegistryValue UnidentifiedRegistryDynamic_802BEA10::NamedList()
{
    return UnidentifiedRegistryValue(this, 1);
}

inline void UnidentifiedRegistryDynamic_802BEA10::UnidentifiedVirtual28() {}

inline UnidentifiedRegistryNode_802BE64C* UnidentifiedRegistryDynamic_802BEA10::UnidentifiedVirtual44(
    const u32& hash)
{
    return Find(hash);
}

inline void UnidentifiedRegistryDynamic_802BEA10::GetIterator(
    UnidentifiedRegistryIterator* iterator, int which)
{
    new (iterator) UnidentifiedRegistryListIterator_802BF828(Tail(which), First(which));
}

inline void UnidentifiedRegistryPackedIteratorBase_802BF614::Next()
{
    mIndex++;
}

inline bool UnidentifiedRegistryPackedIteratorBase_802BF614::IsDone()
{
    return mIndex >= mCount;
}

inline void UnidentifiedRegistryPackedIteratorBase_802BF614::UnidentifiedVirtual14()
{
    mIndex = 0;
}

inline bool UnidentifiedRegistryPackedIteratorBase_802BF614::IsFirst()
{
    return mIndex == 0;
}

inline UnidentifiedRegistryNode_802BE64C* UnidentifiedRegistryPackedIteratorBase_802BF614::GetNode()
{
    return &lbl_8057C498;
}

inline bool UnidentifiedRegistryPackedIteratorBase_802BF614::UnidentifiedVirtual28()
{
    return false;
}

inline bool UnidentifiedRegistryPackedIteratorBase_802BF614::UnidentifiedVirtual2C()
{
    return false;
}

inline UnidentifiedRegistryValue UnidentifiedRegistryPackedNamedIterator_802BF868::GetValue()
{
    const UnidentifiedRegistryPackedEntry* entries
        = (const UnidentifiedRegistryPackedEntry*)mEntries;
    return UnidentifiedRegistryValue(
        entries[mIndex].mData, UnidentifiedRegistryTypeAt(mTypes, mIndex));
}

inline u32 UnidentifiedRegistryPackedNamedIterator_802BF868::GetHash()
{
    const UnidentifiedRegistryPackedEntry* entries
        = (const UnidentifiedRegistryPackedEntry*)mEntries;
    return entries[mIndex].mHash;
}

inline void** UnidentifiedRegistryPackedNamedIterator_802BF868::UnidentifiedVirtual30()
{
    UnidentifiedRegistryPackedEntry* entries = (UnidentifiedRegistryPackedEntry*)mEntries;
    return &entries[mIndex].mData;
}

inline UnidentifiedRegistryValue UnidentifiedRegistryPackedIterator_802BF8A8::GetValue()
{
    void* const* entries = (void* const*)mEntries;
    return UnidentifiedRegistryValue(entries[mIndex], UnidentifiedRegistryTypeAt(mTypes, mIndex));
}

inline u32 UnidentifiedRegistryPackedIterator_802BF8A8::GetHash()
{
    return 0;
}

inline void** UnidentifiedRegistryPackedIterator_802BF8A8::UnidentifiedVirtual30()
{
    void** entries = (void**)mEntries;
    return &entries[mIndex];
}

inline UnidentifiedRegistryNode_802BE64C* UnidentifiedRegistryPacked_802BED28::Find(const u32& hash)
{
    return &lbl_8057C498;
}

inline UnidentifiedRegistryValue UnidentifiedRegistryPacked_802BED28::UnnamedList()
{
    return UnidentifiedRegistryValue(this, 0);
}

inline UnidentifiedRegistryValue UnidentifiedRegistryPacked_802BED28::NamedList()
{
    return UnidentifiedRegistryValue(this, 1);
}

inline void UnidentifiedRegistryPacked_802BED28::UnidentifiedVirtual28() {}

inline void UnidentifiedRegistryPacked_802BED28::UnidentifiedVirtual38() {}

inline void UnidentifiedRegistryPacked_802BED28::UnidentifiedVirtual34() {}

inline void UnidentifiedRegistryPacked_802BED28::GetIterator(
    UnidentifiedRegistryIterator* iterator, int which)
{
    if (which != 0)
    {
        UnidentifiedRegistryPackedNamedIterator_802BF868::Construct(this, iterator);
    }
    else
    {
        UnidentifiedRegistryPackedIterator_802BF8A8::Construct(this, iterator);
    }
}

inline UnidentifiedRegistryValue UnidentifiedRegistryScoped_802BEF0C::UnnamedList()
{
    return UnidentifiedRegistryValue(this, 0);
}

inline UnidentifiedRegistryValue UnidentifiedRegistryScoped_802BEF0C::NamedList()
{
    return UnidentifiedRegistryValue(this, 1);
}

inline void UnidentifiedRegistryScoped_802BEF0C::UnidentifiedVirtual28() {}

inline void UnidentifiedRegistryScoped_802BEF0C::UnidentifiedVirtual38() {}

inline void UnidentifiedRegistryScoped_802BEF0C::UnidentifiedVirtual34() {}

#endif // UNCLASSIFIED_TU_802BE64C_IMPL_H

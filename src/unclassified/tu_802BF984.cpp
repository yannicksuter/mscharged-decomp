#include "unclassified/tu_802BE64C.h"

UnidentifiedRegistryOwner_802BF984::~UnidentifiedRegistryOwner_802BF984()
{
}

UnidentifiedRegistryContainer* UnidentifiedRegistryOwner_802BF984::CreateRoot()
{
    mRoot = new (AllocContainer()) UnidentifiedRegistryScoped_802BEF0C(this);
    return mRoot;
}

bool UnidentifiedRegistryOwner_802BF984::UnidentifiedVirtual18()
{
    return mUnidentified14;
}

bool UnidentifiedRegistryOwner_802BF984::UnidentifiedVirtual1C()
{
    return true;
}

bool UnidentifiedRegistryOwner_802BF984::Load(void* data, unsigned int size, bool flag)
{
    UnidentifiedRegistryPackedImage_802BFA3C* image
        = (UnidentifiedRegistryPackedImage_802BFA3C*)data;
    mUnidentified14 = flag;
    mBase = (u8*)data + image->mBaseOffset;
    mRoot = &image->mRoot;
    new (&image->mRoot) UnidentifiedRegistryPacked_802BED28;
    UnidentifiedRelocate(mRoot);
    return true;
}

// Value of the entry an iterator stands on.
static inline UnidentifiedRegistryValue UnidentifiedIteratorValue(
    UnidentifiedRegistryIterator* iterator)
{
    return iterator->GetValue();
}

// Writable slot of the entry a packed iterator stands on.
static inline void** UnidentifiedIteratorSlot(UnidentifiedRegistryIterator* iterator)
{
    return ((UnidentifiedRegistryPackedIteratorBase_802BF614*)iterator)->UnidentifiedVirtual30();
}

int UnidentifiedRegistryOwner_802BF984::UnidentifiedRelocate(
    UnidentifiedRegistryContainer* container)
{
    UnidentifiedRegistryValue named = container->NamedList();
    UnidentifiedRegistryIterator_802BFE44 iterator;
    ((UnidentifiedRegistryContainer*)named.mData)->GetIterator(&iterator, named.mType);
    UnidentifiedRegistryIterator_802BFE44* packed = &iterator;
    for (; !iterator.IsDone(); iterator.Next())
    {
        UnidentifiedRegistryValue value = UnidentifiedIteratorValue(packed);
        if (value.mType == 2)
        {
            UnidentifiedRegistryValue pointer = UnidentifiedIteratorValue(packed);
            *UnidentifiedIteratorSlot(packed)
                = pointer.mData ? (u8*)mBase + (u32)pointer.mData : 0;
        }
        UnidentifiedRegistryValue childValue = iterator.GetValue();
        if (childValue.mType == 3)
        {
            UnidentifiedRegistryValue offset = iterator.GetValue();
            UnidentifiedRegistryContainer* child = (UnidentifiedRegistryContainer*)((u8*)container
                + (u32)offset.mData);
            *UnidentifiedIteratorSlot(packed) = child;
            if (*(u32*)child != 0xDA07AB1E)
            {
                return 0;
            }
            *(void**)child = *(void**)container;
            if (!UnidentifiedRelocate(child))
            {
                return 0;
            }
        }
    }

    UnidentifiedRegistryValue unnamed = container->UnnamedList();
    UnidentifiedRegistryIterator_802BFE44 unnamedIterator;
    ((UnidentifiedRegistryContainer*)unnamed.mData)->GetIterator(&unnamedIterator, unnamed.mType);
    UnidentifiedRegistryIterator_802BFE44* unnamedPacked = &unnamedIterator;
    for (; !unnamedIterator.IsDone(); unnamedIterator.Next())
    {
        UnidentifiedRegistryValue value = UnidentifiedIteratorValue(unnamedPacked);
        if (value.mType == 2)
        {
            UnidentifiedRegistryValue pointer = UnidentifiedIteratorValue(unnamedPacked);
            u8* relocated = (u8*)mBase + (u32)pointer.mData;
            *UnidentifiedIteratorSlot(unnamedPacked) = relocated;
        }
        UnidentifiedRegistryValue childValue = unnamedIterator.GetValue();
        if (childValue.mType == 3)
        {
            UnidentifiedRegistryValue offset = unnamedIterator.GetValue();
            UnidentifiedRegistryContainer* child = (UnidentifiedRegistryContainer*)((u8*)container
                + (u32)offset.mData);
            *UnidentifiedIteratorSlot(unnamedPacked) = child;
            if (*(u32*)child != 0xDA07AB1E)
            {
                return 0;
            }
            *(void**)child = *(void**)container;
            if (!UnidentifiedRelocate(child))
            {
                return 0;
            }
        }
    }
    return 1;
}

bool UnidentifiedRegistryOwner_802BF984::UnidentifiedVirtual20()
{
    return false;
}

UnidentifiedRegistryContainer* UnidentifiedRegistryOwner_802BF984::GetRoot()
{
    return mRoot;
}

UnidentifiedRegistryContainer*
UnidentifiedRegistryOwner_802BF984::UnidentifiedVirtual10()
{
    return mRoot;
}

#include "unclassified/tu_802BF984_impl.h"

#include "NL/nlRegistry.h"

RegistryOwner::~RegistryOwner()
{
}

RegistryContainer* RegistryOwner::CreateRoot()
{
    mRoot = new (AllocContainer()) ScopedRegistryContainer(this);
    return mRoot;
}

bool RegistryOwner::GetLoadFlag()
{
    return mLoadFlag;
}

bool RegistryOwner::AlwaysTrue()
{
    return true;
}

bool RegistryOwner::Load(void* data, unsigned int size, bool loadFlag)
{
    PackedRegistryImage* image
        = (PackedRegistryImage*)data;
    mLoadFlag = loadFlag;
    mRelocationBase = (u8*)data + image->mRelocationBaseOffset;
    mRoot = &image->mRoot;
    new (&image->mRoot) PackedRegistryContainer;
    Relocate(mRoot);
    return true;
}

// Value of the entry an iterator stands on.
static inline RegistryValue GetRegistryIteratorValue(
    RegistryIteratorBase* iterator)
{
    return iterator->GetValue();
}

// Writable slot of the entry a packed iterator stands on.
static inline void** GetRegistryIteratorValueSlot(RegistryIteratorBase* iterator)
{
    return ((PackedRegistryIteratorBase*)iterator)->GetValueSlot();
}

int RegistryOwner::Relocate(
    RegistryContainer* container)
{
    RegistryValue named = container->NamedList();
    RegistryIterator iterator;
    ((RegistryContainer*)named.mData)->GetIterator(&iterator, named.mType);
    RegistryIterator* packed = &iterator;
    for (; !iterator.IsDone(); iterator.Next())
    {
        RegistryValue value = GetRegistryIteratorValue(packed);
        if (value.mType == 2)
        {
            RegistryValue pointer = GetRegistryIteratorValue(packed);
            *GetRegistryIteratorValueSlot(packed)
                = pointer.mData ? (u8*)mRelocationBase + (u32)pointer.mData : 0;
        }
        RegistryValue childValue = iterator.GetValue();
        if (childValue.mType == 3)
        {
            RegistryValue offset = iterator.GetValue();
            RegistryContainer* child = (RegistryContainer*)((u8*)container
                + (u32)offset.mData);
            *GetRegistryIteratorValueSlot(packed) = child;
            if (*(u32*)child != 0xDA07AB1E)
            {
                return 0;
            }
            *(void**)child = *(void**)container;
            if (!Relocate(child))
            {
                return 0;
            }
        }
    }

    RegistryValue unnamed = container->UnnamedList();
    RegistryIterator unnamedIterator;
    ((RegistryContainer*)unnamed.mData)->GetIterator(&unnamedIterator, unnamed.mType);
    RegistryIterator* unnamedPacked = &unnamedIterator;
    for (; !unnamedIterator.IsDone(); unnamedIterator.Next())
    {
        RegistryValue value = GetRegistryIteratorValue(unnamedPacked);
        if (value.mType == 2)
        {
            RegistryValue pointer = GetRegistryIteratorValue(unnamedPacked);
            u8* relocated = (u8*)mRelocationBase + (u32)pointer.mData;
            *GetRegistryIteratorValueSlot(unnamedPacked) = relocated;
        }
        RegistryValue childValue = unnamedIterator.GetValue();
        if (childValue.mType == 3)
        {
            RegistryValue offset = unnamedIterator.GetValue();
            RegistryContainer* child = (RegistryContainer*)((u8*)container
                + (u32)offset.mData);
            *GetRegistryIteratorValueSlot(unnamedPacked) = child;
            if (*(u32*)child != 0xDA07AB1E)
            {
                return 0;
            }
            *(void**)child = *(void**)container;
            if (!Relocate(child))
            {
                return 0;
            }
        }
    }
    return 1;
}

bool RegistryOwner::AlwaysFalse()
{
    return false;
}

RegistryContainer* RegistryOwner::GetRoot()
{
    return mRoot;
}

RegistryContainer*
RegistryOwner::GetRootAlias()
{
    return mRoot;
}

#include "NL/nlRegistryOwner.inl"

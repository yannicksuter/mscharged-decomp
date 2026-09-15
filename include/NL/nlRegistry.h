#ifndef NL_REGISTRY_H
#define NL_REGISTRY_H

#include "NL/nlRegistryLookup.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"
#include "types.h"

class RegistryContainer;
class RegistryIteratorBase;
class RegistryNode;
class RegistryOwner;
class PackedRegistryContainer;

// Typed value stored in a node and returned by lookups. Type 4 means "none".
struct RegistryValue
{
    RegistryValue()
        : mData(0)
        , mType(4)
    {
    }
    RegistryValue(void* data, int type)
        : mData(data)
        , mType(type)
    {
    }

    /* 0x00 */ void* mData;
    /* 0x04 */ int mType;
};

// Number of packed type words for a list: two bits per entry, padded to whole
// sixteen-entry words.
inline u32 RegistryTypeWords(u32 count)
{
    u32 remainder = count % 16;
    u16 padded = count + (remainder != 0) * (16 - remainder);
    return (u32)padded / 16;
}

// Two-bit type of the entry at index in a packed type-word array.
inline int RegistryTypeAt(const u32* types, int index)
{
    return (types[index / 16] >> ((index % 16) * 2)) & 3;
}

// Type words of the unnamed list: they follow the named list's words.
inline const u32* RegistryUnnamedTypes(const u32* words, u32 namedCount)
{
    return words + RegistryTypeWords(namedCount);
}

// Hash-sorted named entries: they follow both lists' type words.
inline const PackedRegistryEntry* RegistryNamedEntries(
    const u32* words, u32 namedCount, u32 unnamedCount)
{
    return (const PackedRegistryEntry*)(words
        + RegistryTypeWords(namedCount) + RegistryTypeWords(unnamedCount));
}

// Circular sibling link; it precedes the node's virtual table.
struct RegistryLink
{
    /* 0x00 */ RegistryValue mValue;
    /* 0x08 */ RegistryNode* mNext;
    /* 0x0C */ RegistryNode* mPrev;
};

// Abstract container interface shared by the dynamic and packed forms.
class RegistryContainer
{
public:
    virtual bool Has(const u32& hash) const = 0;
    virtual RegistryValue Get(const u32& hash) const = 0;
    virtual int CountUnnamed() const = 0;
    virtual int CountNamed() const = 0;
    virtual RegistryValue UnnamedList() = 0;
    virtual RegistryValue NamedList() = 0;
    virtual RegistryNode* AddNamed(const char* name) = 0;
    virtual RegistryContainer* AddChild(const char* name) = 0;
    virtual void UnidentifiedVirtual28() = 0;
    virtual RegistryNode* AddUnnamed() = 0;
    virtual RegistryContainer* AddUnnamedChild() = 0;
    virtual void UnidentifiedVirtual34() = 0;
    virtual void UnidentifiedVirtual38() = 0;
    virtual void GetIterator(RegistryIteratorBase* iterator, int which) = 0;
    virtual RegistryNode* Find(const u32& hash) = 0;
};

// Iterator interface over one list of a container. Concrete iterators are
// constructed in place over the storage a caller provides.
class RegistryIteratorBase
{
public:
    virtual ~RegistryIteratorBase() {}
    virtual void Next() = 0;
    virtual bool IsDone() = 0;
    virtual void ResetCursor() = 0;
    virtual bool IsFirst() = 0;
    virtual RegistryValue GetValue() = 0;
    virtual u32 GetHash() = 0;
    virtual RegistryNode* GetNode() = 0;
    virtual bool MovePrevious() = 0;
    virtual bool MoveNext() = 0;

    union
    {
        /* 0x04 */ RegistryNode* mTail;
        /* 0x04 */ const u32* mTypes;
    };
    union
    {
        /* 0x08 */ RegistryNode* mCurrent;
        /* 0x08 */ u32 mIndex;
    };
    /* 0x0C */ u32 mCount;
    /* 0x10 */ const void* mEntries;
}; // size: 0x14

// Iterator storage a caller constructs before asking a container for its
// iterator. Every slot dispatches to the concrete iterator placed over it; the
// bodies live in nlRegistryOwner.inl.
class RegistryIterator : public RegistryIteratorBase
{
public:
    virtual ~RegistryIterator() {}
    virtual void Next();
    virtual bool IsDone();
    virtual void ResetCursor();
    virtual bool IsFirst();
    virtual RegistryValue GetValue();
    virtual u32 GetHash();
    virtual RegistryNode* GetNode();
    virtual bool MovePrevious();
    virtual bool MoveNext();
}; // size: 0x14

// Named value node of a dynamic container.
class RegistryNode : public RegistryLink
{
public:
    RegistryNode(const char* name, RegistryContainer* owner = 0);
    ~RegistryNode();
    virtual void* GetData();
    virtual void* UnidentifiedVirtual0C();

    /* 0x14 */ char mName[32];
    /* 0x34 */ u32 mHash;
    /* 0x38 */ u32 mReserved;
    /* 0x3C */ RegistryContainer* mOwner;
}; // size: 0x40

// Shared empty node returned by lookups that find nothing.
extern RegistryNode gEmptyRegistryNode;

inline RegistryNode::RegistryNode(
    const char* name, RegistryContainer* owner)
    : mReserved(0)
    , mOwner(owner)
{
    nlStrNCpy(mName, name, sizeof(mName));
    mHash = nlStringLowerHash(name);
}


// Iterator over one circular node list of a dynamic container.
class RegistryListIterator : public RegistryIteratorBase
{
public:
    RegistryListIterator(
        RegistryNode* tail, RegistryNode* first)
    {
        mTail = tail;
        mCurrent = first;
    }
    virtual void Next();
    virtual bool IsDone();
    virtual void ResetCursor();
    virtual bool IsFirst();
    virtual RegistryValue GetValue();
    virtual RegistryNode* GetNode();
    virtual u32 GetHash();
    virtual bool MovePrevious();
    virtual bool MoveNext();

    bool AtFirst()
    {
        RegistryNode* current = mCurrent;
        if (mTail == 0)
        {
            return false;
        }
        return mTail->mNext == current;
    }
    bool AtLast()
    {
        RegistryNode* current = mCurrent;
        if (mTail == 0)
        {
            return true;
        }
        return mTail == current;
    }
};

// Growable container: two circular lists of nodes allocated by the owner.
class DynamicRegistryContainer : public RegistryContainer
{
public:
    DynamicRegistryContainer(RegistryOwner* allocator)
        : mAllocator(allocator)
        , mNamed(0)
        , mUnnamed(0)
    {
    }
    virtual bool Has(const u32& hash) const;
    virtual RegistryValue Get(const u32& hash) const;
    virtual int CountUnnamed() const;
    virtual int CountNamed() const;
    virtual RegistryValue UnnamedList();
    virtual RegistryValue NamedList();
    virtual RegistryNode* AddNamed(const char* name);
    virtual RegistryContainer* AddChild(const char* name);
    virtual void UnidentifiedVirtual28();
    virtual RegistryNode* AddUnnamed();
    virtual RegistryContainer* AddUnnamedChild();
    virtual void UnidentifiedVirtual34();
    virtual void UnidentifiedVirtual38();
    virtual RegistryNode* UnidentifiedVirtual44(const u32& hash);
    virtual void GetIterator(RegistryIteratorBase* iterator, int which);
    virtual RegistryNode* Find(const u32& hash);

    RegistryNode* Tail(int which) { return which ? mNamed : mUnnamed; }
    RegistryNode* Tail(int which) const { return which ? mNamed : mUnnamed; }
    RegistryNode* First(int which)
    {
        RegistryNode* tail = Tail(which);
        if (tail == 0)
        {
            return 0;
        }
        return tail->mNext;
    }
    RegistryNode* First(int which) const
    {
        RegistryNode* tail = Tail(which);
        if (tail == 0)
        {
            return 0;
        }
        return tail->mNext;
    }
    bool IsFirst(RegistryNode* node, int which)
    {
        RegistryNode* tail = Tail(which);
        if (tail == 0)
        {
            return false;
        }
        return tail->mNext == node;
    }
    bool IsFirst(RegistryNode* node, int which) const
    {
        RegistryNode* tail = Tail(which);
        if (tail == 0)
        {
            return false;
        }
        return tail->mNext == node;
    }

    /* 0x04 */ RegistryOwner* mAllocator;
    /* 0x08 */ RegistryNode* mNamed;
    /* 0x0C */ RegistryNode* mUnnamed;
}; // size: 0x10

// Index-based iterator over one entry table of a packed container.
class PackedRegistryIteratorBase : public RegistryIteratorBase
{
public:
    PackedRegistryIteratorBase(const u32* types, u32 count)
    {
        mTypes = types;
        mIndex = 0;
        mCount = count;
    }
    virtual ~PackedRegistryIteratorBase() {}
    virtual void Next();
    virtual bool IsDone();
    virtual void ResetCursor();
    virtual bool IsFirst();
    virtual RegistryNode* GetNode();
    virtual bool MovePrevious();
    virtual bool MoveNext();
    virtual void** GetValueSlot() = 0;
};

// Iterator over the hash-keyed named entries of a packed container.
class PackedNamedRegistryIterator
    : public PackedRegistryIteratorBase
{
public:
    PackedNamedRegistryIterator(
        const u32* types, u32 count, const PackedRegistryEntry* entries)
        : PackedRegistryIteratorBase(types, count)
    {
        mEntries = entries;
    }
    static void Construct(const PackedRegistryContainer* packed, RegistryIteratorBase* iterator);
    virtual RegistryValue GetValue();
    virtual u32 GetHash();
    virtual void** GetValueSlot();
};

// Iterator over the unnamed pointer entries of a packed container.
class PackedUnnamedRegistryIterator
    : public PackedRegistryIteratorBase
{
public:
    PackedUnnamedRegistryIterator(const u32* types, u32 count, void* const* entries)
        : PackedRegistryIteratorBase(types, count)
    {
        mEntries = entries;
    }
    static void Construct(const PackedRegistryContainer* packed, RegistryIteratorBase* iterator);
    virtual RegistryValue GetValue();
    virtual u32 GetHash();
    virtual void** GetValueSlot();
};

// Read-only container image: named and unnamed counts, the two packed
// type-word arrays, the hash-sorted named entries, then the unnamed pointers.
class PackedRegistryContainer : public RegistryContainer
{
public:
    virtual bool Has(const u32& hash) const;
    virtual RegistryValue Get(const u32& hash) const;
    virtual int CountUnnamed() const;
    virtual int CountNamed() const;
    virtual RegistryNode* AddNamed(const char* name);
    virtual RegistryContainer* AddChild(const char* name);
    virtual RegistryNode* AddUnnamed();
    virtual RegistryContainer* AddUnnamedChild();
    virtual RegistryNode* Find(const u32& hash);
    virtual RegistryValue UnnamedList();
    virtual RegistryValue NamedList();
    virtual void UnidentifiedVirtual28();
    virtual void UnidentifiedVirtual38();
    virtual void UnidentifiedVirtual34();
    virtual void GetIterator(RegistryIteratorBase* iterator, int which);

    const u32* UnnamedTypes() const
    {
        return RegistryUnnamedTypes(mWords, mNamedCount);
    }
    const PackedRegistryEntry* NamedEntries() const
    {
        return RegistryNamedEntries(mWords, mNamedCount, mUnnamedCount);
    }
    void* const* UnnamedEntries() const { return (void* const*)(NamedEntries() + mNamedCount); }

    /* 0x04 */ u16 mNamedCount;
    /* 0x06 */ u16 mUnnamedCount;
    /* 0x08 */ u32 mWords[1];
};

inline void PackedNamedRegistryIterator::Construct(
    const PackedRegistryContainer* packed, RegistryIteratorBase* iterator)
{
    new (iterator) PackedNamedRegistryIterator(
        packed->mWords, packed->mNamedCount, packed->NamedEntries());
}

inline void PackedUnnamedRegistryIterator::Construct(
    const PackedRegistryContainer* packed, RegistryIteratorBase* iterator)
{
    new (iterator) PackedUnnamedRegistryIterator(
        packed->UnnamedTypes(), packed->mUnnamedCount, packed->UnnamedEntries());
}

// Dynamic container that falls back to a parent container for lookups.
class ScopedRegistryContainer : public DynamicRegistryContainer
{
public:
    ScopedRegistryContainer(RegistryOwner* allocator)
        : DynamicRegistryContainer(allocator)
        , mParent(0)
    {
    }
    virtual bool Has(const u32& hash) const;
    virtual RegistryValue Get(const u32& hash) const;
    virtual int CountUnnamed() const;
    virtual int CountNamed() const;
    virtual RegistryValue UnnamedList();
    virtual RegistryValue NamedList();
    virtual RegistryContainer* AddChild(const char* name);
    virtual void UnidentifiedVirtual28();
    virtual void UnidentifiedVirtual38();
    virtual void UnidentifiedVirtual34();
    virtual RegistryNode* Find(const u32& hash);

    /* 0x10 */ RegistryContainer* mParent;
}; // size: 0x14

// Header of a packed registry image: the relocation base offset and the root
// container that follows it.
struct PackedRegistryImage
{
    /* 0x00 */ u32 mReserved00;
    /* 0x04 */ u32 mReserved04;
    /* 0x08 */ u32 mRelocationBaseOffset;
    /* 0x0C */ PackedRegistryContainer mRoot;
};

// Registry owner: supplies node and container storage for the dynamic form,
// loads packed images and holds the root container. Consumers derive from it
// and provide the storage virtuals.
class RegistryOwner
{
public:
    virtual RegistryContainer* CreateRoot();
    virtual bool Load(void* data, unsigned int size, bool loadFlag);
    virtual RegistryContainer* GetRootAlias();
    virtual RegistryContainer* GetRoot();
    virtual bool GetLoadFlag();
    virtual bool AlwaysTrue();
    virtual bool AlwaysFalse();
    virtual RegistryContainer* AllocContainer() = 0;
    virtual RegistryNode* AllocNode() = 0;
    virtual void* AllocItem(unsigned int size) = 0;
    virtual void FreeContainer(void* container) = 0;
    virtual void FreeNode(void* node) = 0;
    virtual void FreeItem(void* data) = 0;
    virtual ~RegistryOwner();

    int Relocate(RegistryContainer* container);

    /* 0x04 */ void* mRelocationBase;
    /* 0x08 */ u32 mReserved08;
    /* 0x0C */ u32 mReserved0C;
    /* 0x10 */ u32 mReserved10;
    /* 0x14 */ u32 mLoadFlag : 1;
    /* 0x14 */ u32 mReservedFlag : 1;
    /* 0x18 */ RegistryContainer* mRoot;
}; // size: 0x1C

#endif // NL_REGISTRY_H

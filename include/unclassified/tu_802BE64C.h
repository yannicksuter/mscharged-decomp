#ifndef UNCLASSIFIED_TU_802BE64C_H
#define UNCLASSIFIED_TU_802BE64C_H

#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "types.h"

class UnidentifiedRegistryContainer;
class UnidentifiedRegistryIterator;
class UnidentifiedRegistryNode_802BE64C;
class UnidentifiedRegistryOwner_802BF984;
class UnidentifiedRegistryPacked_802BED28;

// Typed value stored in a node and returned by lookups. Type 4 means "none".
struct UnidentifiedRegistryValue
{
    UnidentifiedRegistryValue()
        : mData(0)
        , mType(4)
    {
    }
    UnidentifiedRegistryValue(void* data, int type)
        : mData(data)
        , mType(type)
    {
    }

    /* 0x00 */ void* mData;
    /* 0x04 */ int mType;
};

// Sorted named entry of a packed container.
struct UnidentifiedRegistryPackedEntry
{
    /* 0x00 */ u32 mHash;
    /* 0x04 */ void* mData;
};

// Binary search of a hash-sorted entry table for the entry whose hash equals
// the key's. Defined in tu_802BF8E8.cpp.
const UnidentifiedRegistryPackedEntry* UnidentifiedFindEntry(
    const u32& key, const UnidentifiedRegistryPackedEntry* entries, int count);

// Number of packed type words for a list: two bits per entry, padded to whole
// sixteen-entry words.
inline u32 UnidentifiedRegistryTypeWords(u32 count)
{
    u32 remainder = count % 16;
    u16 padded = count + (remainder != 0) * (16 - remainder);
    return (u32)padded / 16;
}

// Two-bit type of the entry at index in a packed type-word array.
inline int UnidentifiedRegistryTypeAt(const u32* types, int index)
{
    return (types[index / 16] >> ((index % 16) * 2)) & 3;
}

// Type words of the unnamed list: they follow the named list's words.
inline const u32* UnidentifiedRegistryUnnamedTypes(const u32* words, u32 namedCount)
{
    return words + UnidentifiedRegistryTypeWords(namedCount);
}

// Hash-sorted named entries: they follow both lists' type words.
inline const UnidentifiedRegistryPackedEntry* UnidentifiedRegistryNamedEntries(
    const u32* words, u32 namedCount, u32 unnamedCount)
{
    return (const UnidentifiedRegistryPackedEntry*)(words
        + UnidentifiedRegistryTypeWords(namedCount) + UnidentifiedRegistryTypeWords(unnamedCount));
}

// Circular sibling link; it precedes the node's virtual table.
struct UnidentifiedRegistryLink_802BE64C
{
    /* 0x00 */ UnidentifiedRegistryValue mValue;
    /* 0x08 */ UnidentifiedRegistryNode_802BE64C* mNext;
    /* 0x0C */ UnidentifiedRegistryNode_802BE64C* mPrev;
};

// Abstract container interface shared by the dynamic and packed forms.
class UnidentifiedRegistryContainer
{
public:
    virtual bool Has(const u32& hash) const = 0;
    virtual UnidentifiedRegistryValue Get(const u32& hash) const = 0;
    virtual int CountUnnamed() const = 0;
    virtual int CountNamed() const = 0;
    virtual UnidentifiedRegistryValue UnnamedList() = 0;
    virtual UnidentifiedRegistryValue NamedList() = 0;
    virtual UnidentifiedRegistryNode_802BE64C* AddNamed(const char* name) = 0;
    virtual UnidentifiedRegistryContainer* AddChild(const char* name) = 0;
    virtual void UnidentifiedVirtual28() = 0;
    virtual UnidentifiedRegistryNode_802BE64C* AddUnnamed() = 0;
    virtual UnidentifiedRegistryContainer* AddUnnamedChild() = 0;
    virtual void UnidentifiedVirtual34() = 0;
    virtual void UnidentifiedVirtual38() = 0;
    virtual void GetIterator(UnidentifiedRegistryIterator* iterator, int which) = 0;
    virtual UnidentifiedRegistryNode_802BE64C* Find(const u32& hash) = 0;
};

// Iterator interface over one list of a container. Concrete iterators are
// constructed in place over the storage a caller provides.
class UnidentifiedRegistryIterator
{
public:
    virtual ~UnidentifiedRegistryIterator() {}
    virtual void Next() = 0;
    virtual bool IsDone() = 0;
    virtual void UnidentifiedVirtual14() = 0;
    virtual bool IsFirst() = 0;
    virtual UnidentifiedRegistryValue GetValue() = 0;
    virtual u32 GetHash() = 0;
    virtual UnidentifiedRegistryNode_802BE64C* GetNode() = 0;
    virtual bool UnidentifiedVirtual28() = 0;
    virtual bool UnidentifiedVirtual2C() = 0;

    union
    {
        /* 0x04 */ UnidentifiedRegistryNode_802BE64C* mTail;
        /* 0x04 */ const u32* mTypes;
    };
    union
    {
        /* 0x08 */ UnidentifiedRegistryNode_802BE64C* mCurrent;
        /* 0x08 */ u32 mIndex;
    };
    /* 0x0C */ u32 mCount;
    /* 0x10 */ const void* mEntries;
}; // size: 0x14

// Iterator storage a caller constructs before asking a container for its
// iterator. Every slot dispatches to the concrete iterator placed over it; the
// bodies live in tu_802BF984_impl.h.
class UnidentifiedRegistryIterator_802BFE44 : public UnidentifiedRegistryIterator
{
public:
    virtual ~UnidentifiedRegistryIterator_802BFE44() {}
    virtual void Next();
    virtual bool IsDone();
    virtual void UnidentifiedVirtual14();
    virtual bool IsFirst();
    virtual UnidentifiedRegistryValue GetValue();
    virtual u32 GetHash();
    virtual UnidentifiedRegistryNode_802BE64C* GetNode();
    virtual bool UnidentifiedVirtual28();
    virtual bool UnidentifiedVirtual2C();
}; // size: 0x14

// Named value node of a dynamic container.
class UnidentifiedRegistryNode_802BE64C : public UnidentifiedRegistryLink_802BE64C
{
public:
    UnidentifiedRegistryNode_802BE64C(const char* name, UnidentifiedRegistryContainer* owner = 0);
    ~UnidentifiedRegistryNode_802BE64C();
    virtual void* GetData();
    virtual void* UnidentifiedVirtual0C();

    /* 0x14 */ char mName[32];
    /* 0x34 */ u32 mHash;
    /* 0x38 */ u32 mUnidentified38;
    /* 0x3C */ UnidentifiedRegistryContainer* mOwner;
}; // size: 0x40

// Shared empty node returned by lookups that find nothing.
extern UnidentifiedRegistryNode_802BE64C lbl_8057C498;

inline UnidentifiedRegistryNode_802BE64C::UnidentifiedRegistryNode_802BE64C(
    const char* name, UnidentifiedRegistryContainer* owner)
    : mUnidentified38(0)
    , mOwner(owner)
{
    nlStrNCpy(mName, name, sizeof(mName));
    mHash = nlStringLowerHash(name);
}


// Iterator over one circular node list of a dynamic container.
class UnidentifiedRegistryListIterator_802BF828 : public UnidentifiedRegistryIterator
{
public:
    UnidentifiedRegistryListIterator_802BF828(
        UnidentifiedRegistryNode_802BE64C* tail, UnidentifiedRegistryNode_802BE64C* first)
    {
        mTail = tail;
        mCurrent = first;
    }
    virtual void Next();
    virtual bool IsDone();
    virtual void UnidentifiedVirtual14();
    virtual bool IsFirst();
    virtual UnidentifiedRegistryValue GetValue();
    virtual UnidentifiedRegistryNode_802BE64C* GetNode();
    virtual u32 GetHash();
    virtual bool UnidentifiedVirtual28();
    virtual bool UnidentifiedVirtual2C();

    bool AtFirst()
    {
        UnidentifiedRegistryNode_802BE64C* current = mCurrent;
        if (mTail == 0)
        {
            return false;
        }
        return mTail->mNext == current;
    }
    bool AtLast()
    {
        UnidentifiedRegistryNode_802BE64C* current = mCurrent;
        if (mTail == 0)
        {
            return true;
        }
        return mTail == current;
    }
};

// Growable container: two circular lists of nodes allocated by the owner.
class UnidentifiedRegistryDynamic_802BEA10 : public UnidentifiedRegistryContainer
{
public:
    UnidentifiedRegistryDynamic_802BEA10(UnidentifiedRegistryOwner_802BF984* allocator)
        : mAllocator(allocator)
        , mNamed(0)
        , mUnnamed(0)
    {
    }
    virtual bool Has(const u32& hash) const;
    virtual UnidentifiedRegistryValue Get(const u32& hash) const;
    virtual int CountUnnamed() const;
    virtual int CountNamed() const;
    virtual UnidentifiedRegistryValue UnnamedList();
    virtual UnidentifiedRegistryValue NamedList();
    virtual UnidentifiedRegistryNode_802BE64C* AddNamed(const char* name);
    virtual UnidentifiedRegistryContainer* AddChild(const char* name);
    virtual void UnidentifiedVirtual28();
    virtual UnidentifiedRegistryNode_802BE64C* AddUnnamed();
    virtual UnidentifiedRegistryContainer* AddUnnamedChild();
    virtual void UnidentifiedVirtual34();
    virtual void UnidentifiedVirtual38();
    virtual UnidentifiedRegistryNode_802BE64C* UnidentifiedVirtual44(const u32& hash);
    virtual void GetIterator(UnidentifiedRegistryIterator* iterator, int which);
    virtual UnidentifiedRegistryNode_802BE64C* Find(const u32& hash);

    UnidentifiedRegistryNode_802BE64C* Tail(int which) { return which ? mNamed : mUnnamed; }
    UnidentifiedRegistryNode_802BE64C* Tail(int which) const { return which ? mNamed : mUnnamed; }
    UnidentifiedRegistryNode_802BE64C* First(int which)
    {
        UnidentifiedRegistryNode_802BE64C* tail = Tail(which);
        if (tail == 0)
        {
            return 0;
        }
        return tail->mNext;
    }
    UnidentifiedRegistryNode_802BE64C* First(int which) const
    {
        UnidentifiedRegistryNode_802BE64C* tail = Tail(which);
        if (tail == 0)
        {
            return 0;
        }
        return tail->mNext;
    }
    bool IsFirst(UnidentifiedRegistryNode_802BE64C* node, int which)
    {
        UnidentifiedRegistryNode_802BE64C* tail = Tail(which);
        if (tail == 0)
        {
            return false;
        }
        return tail->mNext == node;
    }
    bool IsFirst(UnidentifiedRegistryNode_802BE64C* node, int which) const
    {
        UnidentifiedRegistryNode_802BE64C* tail = Tail(which);
        if (tail == 0)
        {
            return false;
        }
        return tail->mNext == node;
    }

    /* 0x04 */ UnidentifiedRegistryOwner_802BF984* mAllocator;
    /* 0x08 */ UnidentifiedRegistryNode_802BE64C* mNamed;
    /* 0x0C */ UnidentifiedRegistryNode_802BE64C* mUnnamed;
}; // size: 0x10

// Index-based iterator over one entry table of a packed container.
class UnidentifiedRegistryPackedIteratorBase_802BF614 : public UnidentifiedRegistryIterator
{
public:
    UnidentifiedRegistryPackedIteratorBase_802BF614(const u32* types, u32 count)
    {
        mTypes = types;
        mIndex = 0;
        mCount = count;
    }
    virtual ~UnidentifiedRegistryPackedIteratorBase_802BF614() {}
    virtual void Next();
    virtual bool IsDone();
    virtual void UnidentifiedVirtual14();
    virtual bool IsFirst();
    virtual UnidentifiedRegistryNode_802BE64C* GetNode();
    virtual bool UnidentifiedVirtual28();
    virtual bool UnidentifiedVirtual2C();
    virtual void** UnidentifiedVirtual30() = 0;
};

// Iterator over the hash-keyed named entries of a packed container.
class UnidentifiedRegistryPackedNamedIterator_802BF868
    : public UnidentifiedRegistryPackedIteratorBase_802BF614
{
public:
    UnidentifiedRegistryPackedNamedIterator_802BF868(
        const u32* types, u32 count, const UnidentifiedRegistryPackedEntry* entries)
        : UnidentifiedRegistryPackedIteratorBase_802BF614(types, count)
    {
        mEntries = entries;
    }
    static void Construct(const UnidentifiedRegistryPacked_802BED28* packed, UnidentifiedRegistryIterator* iterator);
    virtual UnidentifiedRegistryValue GetValue();
    virtual u32 GetHash();
    virtual void** UnidentifiedVirtual30();
};

// Iterator over the unnamed pointer entries of a packed container.
class UnidentifiedRegistryPackedIterator_802BF8A8
    : public UnidentifiedRegistryPackedIteratorBase_802BF614
{
public:
    UnidentifiedRegistryPackedIterator_802BF8A8(const u32* types, u32 count, void* const* entries)
        : UnidentifiedRegistryPackedIteratorBase_802BF614(types, count)
    {
        mEntries = entries;
    }
    static void Construct(const UnidentifiedRegistryPacked_802BED28* packed, UnidentifiedRegistryIterator* iterator);
    virtual UnidentifiedRegistryValue GetValue();
    virtual u32 GetHash();
    virtual void** UnidentifiedVirtual30();
};

// Read-only container image: named and unnamed counts, the two packed
// type-word arrays, the hash-sorted named entries, then the unnamed pointers.
class UnidentifiedRegistryPacked_802BED28 : public UnidentifiedRegistryContainer
{
public:
    virtual bool Has(const u32& hash) const;
    virtual UnidentifiedRegistryValue Get(const u32& hash) const;
    virtual int CountUnnamed() const;
    virtual int CountNamed() const;
    virtual UnidentifiedRegistryNode_802BE64C* AddNamed(const char* name);
    virtual UnidentifiedRegistryContainer* AddChild(const char* name);
    virtual UnidentifiedRegistryNode_802BE64C* AddUnnamed();
    virtual UnidentifiedRegistryContainer* AddUnnamedChild();
    virtual UnidentifiedRegistryNode_802BE64C* Find(const u32& hash);
    virtual UnidentifiedRegistryValue UnnamedList();
    virtual UnidentifiedRegistryValue NamedList();
    virtual void UnidentifiedVirtual28();
    virtual void UnidentifiedVirtual38();
    virtual void UnidentifiedVirtual34();
    virtual void GetIterator(UnidentifiedRegistryIterator* iterator, int which);

    const u32* UnnamedTypes() const
    {
        return UnidentifiedRegistryUnnamedTypes(mWords, mNamedCount);
    }
    const UnidentifiedRegistryPackedEntry* NamedEntries() const
    {
        return UnidentifiedRegistryNamedEntries(mWords, mNamedCount, mUnnamedCount);
    }
    void* const* UnnamedEntries() const { return (void* const*)(NamedEntries() + mNamedCount); }

    /* 0x04 */ u16 mNamedCount;
    /* 0x06 */ u16 mUnnamedCount;
    /* 0x08 */ u32 mWords[1];
};

inline void UnidentifiedRegistryPackedNamedIterator_802BF868::Construct(
    const UnidentifiedRegistryPacked_802BED28* packed, UnidentifiedRegistryIterator* iterator)
{
    new (iterator) UnidentifiedRegistryPackedNamedIterator_802BF868(
        packed->mWords, packed->mNamedCount, packed->NamedEntries());
}

inline void UnidentifiedRegistryPackedIterator_802BF8A8::Construct(
    const UnidentifiedRegistryPacked_802BED28* packed, UnidentifiedRegistryIterator* iterator)
{
    new (iterator) UnidentifiedRegistryPackedIterator_802BF8A8(
        packed->UnnamedTypes(), packed->mUnnamedCount, packed->UnnamedEntries());
}

// Dynamic container that falls back to a parent container for lookups.
class UnidentifiedRegistryScoped_802BEF0C : public UnidentifiedRegistryDynamic_802BEA10
{
public:
    UnidentifiedRegistryScoped_802BEF0C(UnidentifiedRegistryOwner_802BF984* allocator)
        : UnidentifiedRegistryDynamic_802BEA10(allocator)
        , mParent(0)
    {
    }
    virtual bool Has(const u32& hash) const;
    virtual UnidentifiedRegistryValue Get(const u32& hash) const;
    virtual int CountUnnamed() const;
    virtual int CountNamed() const;
    virtual UnidentifiedRegistryValue UnnamedList();
    virtual UnidentifiedRegistryValue NamedList();
    virtual UnidentifiedRegistryContainer* AddChild(const char* name);
    virtual void UnidentifiedVirtual28();
    virtual void UnidentifiedVirtual38();
    virtual void UnidentifiedVirtual34();
    virtual UnidentifiedRegistryNode_802BE64C* Find(const u32& hash);

    /* 0x10 */ UnidentifiedRegistryContainer* mParent;
}; // size: 0x14

// Header of a packed registry image: the relocation base offset and the root
// container that follows it.
struct UnidentifiedRegistryPackedImage_802BFA3C
{
    /* 0x00 */ u32 mUnidentified00;
    /* 0x04 */ u32 mUnidentified04;
    /* 0x08 */ u32 mBaseOffset;
    /* 0x0C */ UnidentifiedRegistryPacked_802BED28 mRoot;
};

// Registry owner: supplies node and container storage for the dynamic form,
// loads packed images and holds the root container. Consumers derive from it
// and provide the storage virtuals.
class UnidentifiedRegistryOwner_802BF984
{
public:
    virtual UnidentifiedRegistryContainer* CreateRoot();
    virtual bool Load(void* data, unsigned int size, bool flag);
    virtual UnidentifiedRegistryContainer* UnidentifiedVirtual10();
    virtual UnidentifiedRegistryContainer* GetRoot();
    virtual bool UnidentifiedVirtual18();
    virtual bool UnidentifiedVirtual1C();
    virtual bool UnidentifiedVirtual20();
    virtual UnidentifiedRegistryContainer* AllocContainer() = 0;
    virtual UnidentifiedRegistryNode_802BE64C* AllocNode() = 0;
    virtual void UnidentifiedVirtual2C() = 0;
    virtual void UnidentifiedVirtual30() = 0;
    virtual void UnidentifiedVirtual34() = 0;
    virtual void FreeValue(void* data) = 0;
    virtual ~UnidentifiedRegistryOwner_802BF984();

    int UnidentifiedRelocate(UnidentifiedRegistryContainer* container);

    /* 0x04 */ void* mBase;
    /* 0x08 */ u32 mUnidentified08;
    /* 0x0C */ u32 mUnidentified0C;
    /* 0x10 */ u32 mUnidentified10;
    /* 0x14 */ u32 mUnidentified14 : 1;
    /* 0x14 */ u32 mUnidentified14_1 : 1;
    /* 0x18 */ UnidentifiedRegistryContainer* mRoot;
}; // size: 0x1C

#endif // UNCLASSIFIED_TU_802BE64C_H

#ifndef GAME_UNIDENTIFIED_STATIC_STORAGE_H
#define GAME_UNIDENTIFIED_STATIC_STORAGE_H

// One shared four-byte state object, dynamically zeroed behind a guard by the
// static initializer of every translation unit that includes this header:
// 363 objects in R4QE01 carry that block, `Game/AIPad.cpp` is first in link
// order and therefore owns the object (`.sbss 0x806E0B80`) and its guard, and
// no retained code ever reads it. That is the profile of a debug facility
// whose readers were compiled out of the release build while its storage was
// not.
//
// The shape is fixed by the compiler, not by us. A static data member of a
// class template is the only construct that gives every including unit a weak
// copy with a guarded initializer in `__sinit`, and MWCC instantiates such a
// member only when it analyses a function body that forms its address. The
// accessor below is that body: it is what makes plain inclusion reproduce the
// retail initializer, byte for byte, in the position retail keeps it (after the
// unit's own file-scope objects). Its callers did not survive the release
// build, which is exactly why the object is never read.
//
// Still unidentified, hence the names: the template's real spelling, its tag,
// the accessor's real name and the header's real path. The stripped DOL and
// the GameCube predecessor (which has no guarded statics at all) carry no
// evidence for any of them.

struct UnidentifiedStaticState
{
    UnidentifiedStaticState()
        : value(0)
    {
    }

    void* value;
};

template <typename T>
struct UnidentifiedStaticStorage
{
    static UnidentifiedStaticState state;
};

struct UnidentifiedStaticTag;

template <typename T>
UnidentifiedStaticState UnidentifiedStaticStorage<T>::state;

inline UnidentifiedStaticState& UnidentifiedStaticStorageState()
{
    return UnidentifiedStaticStorage<UnidentifiedStaticTag>::state;
}

#endif // GAME_UNIDENTIFIED_STATIC_STORAGE_H

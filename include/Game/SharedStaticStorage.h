#ifndef GAME_SHARED_STATIC_STORAGE_H
#define GAME_SHARED_STATIC_STORAGE_H

// One shared four-byte state object, dynamically zeroed behind a guard by the
// static initializer of each including translation unit. The retail code has
// 363 such initializer blocks. AIPad is first in link order and supplies the
// state at 0x806E0B80 and its guard; no retained code reads the state.
//
// This reconstruction uses a template static member for the weak copies and
// guarded initialization. With the configured compiler, the accessor body
// requests that member during parsing, placing the initializer after each
// unit's own file-scope objects. The accessor has no retained caller.
//
// The original owner, purpose, names and header boundary are unknown. This
// preserves the observed shared storage and initialization order; a debug
// facility with removed readers is only one possible explanation.

struct SharedStaticState
{
    SharedStaticState()
        : value(0)
    {
    }

    void* value;
};

template <typename T>
struct SharedStaticStorage
{
    static SharedStaticState state;
};

struct SharedStaticTag;

template <typename T>
SharedStaticState SharedStaticStorage<T>::state;

inline SharedStaticState& GetSharedStaticState()
{
    return SharedStaticStorage<SharedStaticTag>::state;
}

// No code from this scope survives the link. It preserves the registry's
// observed order of the initializer and inline value methods.
struct SharedStaticStorageScope
{
    ~SharedStaticStorageScope() { }
};

#endif // GAME_SHARED_STATIC_STORAGE_H

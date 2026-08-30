#ifndef GAME_UNIDENTIFIED_STATIC_STORAGE_H
#define GAME_UNIDENTIFIED_STATIC_STORAGE_H

// Every game translation unit that includes this header emits an identical
// guarded initializer for the shared template static below. The stripped DOL
// does not reveal the original template specialization or spelling; the names
// here are explicitly unidentified placeholders.

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

#endif // GAME_UNIDENTIFIED_STATIC_STORAGE_H

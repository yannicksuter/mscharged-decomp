#ifndef GAME_FUNCTION_HASH_H
#define GAME_FUNCTION_HASH_H

struct FunctionHash
{
    FunctionHash(unsigned int hash)
        : hash(hash)
    {
    }

    operator unsigned int() const { return hash; }

    unsigned int hash;
};

#endif // GAME_FUNCTION_HASH_H

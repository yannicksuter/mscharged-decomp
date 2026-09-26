#ifndef _FEINLINEHASHER_H_
#define _FEINLINEHASHER_H_

#include "Game/FE/feFinder.h"
#include "NL/nlString.h"

inline InlineHasher::InlineHasher(unsigned long h)
    : m_Hash(h)
{
}

inline InlineHasher::InlineHasher(const char* string)
    : m_Hash(nlStringLowerHash(string))
{
}

#endif // _FEINLINEHASHER_H_

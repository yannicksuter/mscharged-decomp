#ifndef _FEFINDER_INL_
#define _FEFINDER_INL_

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

#include "Game/FE/feFinder_impl.h"

#endif // _FEFINDER_INL_

#ifndef NL_BASIC_STRING_INL
#define NL_BASIC_STRING_INL

#include "NL/nlBasicString.h"

template <typename CharT, typename Allocator>
inline BasicString<CharT, Allocator>& BasicString<CharT, Allocator>::operator=(BasicString other)
{
    Data* tmp = mData;
    mData = other.mData;
    other.mData = tmp;
    return *this;
}

template <typename CharT, typename Allocator>
inline const CharT* BasicString<CharT, Allocator>::c_str() const
{
    static CharT emptyString = 0;
    return mData ? mData->c_str() : &emptyString;
}

#endif

#ifndef _NLLEXICALCAST_H_
#define _NLLEXICALCAST_H_

#include "NL/nlBasicString.h"
#include "NL/nlPrint.h"

namespace Detail
{
template <typename To, typename From>
struct LexicalCastImpl
{
    static To Do(const From& from);
};
} // namespace Detail

template <typename To, typename From>
To LexicalCast(const From& from)
{
    return Detail::LexicalCastImpl<To, From>::Do(const_cast<From&>(from));
}

namespace Detail
{

template <typename Allocator>
struct LexicalCastImpl<BasicString<char, Allocator>, const char*>
{
    static BasicString<char, Allocator> Do(const char* const& string)
    {
        return BasicString<char, Allocator>(string);
    }
};

template <typename Allocator>
struct LexicalCastImpl<BasicString<char, Allocator>, unsigned long>
{
    static BasicString<char, Allocator> Do(unsigned long value);
};

template <typename Allocator>
struct LexicalCastImpl<BasicString<char, Allocator>, int>
{
    static BasicString<char, Allocator> Do(int value);
};

template <typename Allocator>
struct LexicalCastImpl<BasicString<char, Allocator>, float>
{
    static BasicString<char, Allocator> Do(float value);
};

template <typename To>
struct LexicalCastImpl<To, char>
{
    static To Do(char value);
};

template <>
inline NLString LexicalCastImpl<NLString, char>::Do(char value)
{
    char buffer[0x40];
    nlSNPrintf(buffer, sizeof(buffer), "%c", value);
    return NLString(buffer);
}
} // namespace Detail

#endif // _NLLEXICALCAST_H_

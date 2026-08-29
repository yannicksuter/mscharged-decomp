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
    static BasicString<char, Allocator> Do(const char* const& s)
    {
        return BasicString<char, Allocator>(s);
    }
};

template <typename Allocator>
struct LexicalCastImpl<BasicString<char, Allocator>, unsigned long>
{
    static BasicString<char, Allocator> Do(unsigned long t);
};

template <typename Allocator>
struct LexicalCastImpl<BasicString<char, Allocator>, int>
{
    static BasicString<char, Allocator> Do(int t);
};

template <typename Allocator>
struct LexicalCastImpl<BasicString<char, Allocator>, float>
{
    static BasicString<char, Allocator> Do(float t);
};

template <typename Allocator>
struct LexicalCastImpl<BasicString<char, Allocator>, bool>
{
    static BasicString<char, Allocator> Do(bool t);
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

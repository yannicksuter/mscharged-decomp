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
inline To LexicalCast(const From& from)
{
    return Detail::LexicalCastImpl<To, From>::Do(const_cast<From&>(from));
}

namespace Detail
{

template <typename To>
struct LexicalCastImpl<To, int>
{
    static To Do(int t);
};

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

template <typename To, typename From, int N>
struct LexicalCastImpl<To, From[N]>
{
    static To Do(const From (&f)[N])
    {
        return To(f);
    }
};

} // namespace Detail

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;

template <>
inline WideBasicString Detail::LexicalCastImpl<WideBasicString, const unsigned short*>::Do(
    const unsigned short* const& f)
{
    return WideBasicString(f);
}

template <>
inline WideBasicString LexicalCast<WideBasicString, const unsigned short*>(
    const unsigned short* const& from)
{
    return Detail::LexicalCastImpl<WideBasicString, const unsigned short*>::Do(from);
}

#endif // _NLLEXICALCAST_H_

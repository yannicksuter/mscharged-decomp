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
    static BasicString<char, Allocator> Do(const char* s)
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

template <typename Allocator>
inline BasicString<char, Allocator> LexicalCastImpl<BasicString<char, Allocator>, int>::Do(int t)
{
    char s[0x40];
    nlSNPrintf(s, 0x40, "%i", t);
    return BasicString<char, Allocator>(s);
}

template <typename Allocator>
inline BasicString<char, Allocator> LexicalCastImpl<BasicString<char, Allocator>, float>::Do(float t)
{
    char s[0x40];
    nlSNPrintf(s, 0x40, "%f", t);
    return BasicString<char, Allocator>(s);
}

template <typename Allocator>
inline BasicString<char, Allocator> LexicalCastImpl<BasicString<char, Allocator>, bool>::Do(bool t)
{
    if (t)
    {
        return BasicString<char, Allocator>("true");
    }
    return BasicString<char, Allocator>("false");
}

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

template <>
inline NLString LexicalCastImpl<NLString, char*>::Do(char* const& f)
{
    return NLString(f);
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

namespace Detail
{
template <typename Allocator>
struct LexicalCastImpl<BasicString<unsigned short, Allocator>, const char*>
{
    static BasicString<unsigned short, Allocator> Do(const char* f)
    {
        unsigned short buffer[256];
        nlStrToWcs(f, buffer, 256);
        return BasicString<unsigned short, Allocator>(buffer);
    }
};

template <typename To, typename Allocator>
struct LexicalCastImpl<To, BasicString<char, Allocator> >
{
    static To Do(const BasicString<char, Allocator>& f)
    {
        return LexicalCast<To>(f.c_str());
    }
};

template <typename Allocator>
struct LexicalCastImpl<BasicString<unsigned short, Allocator>, int>
{
    static BasicString<unsigned short, Allocator> Do(int t)
    {
        return LexicalCast<BasicString<unsigned short, Allocator> >(
            LexicalCast<BasicString<char, StringAllocator_801CBA50> >(t));
    }
};
} // namespace Detail

namespace Detail
{
template <typename Allocator>
struct LexicalCastImpl<BasicString<unsigned short, Allocator>, BasicString<unsigned short, Allocator> >
{
    static BasicString<unsigned short, Allocator> Do(const BasicString<unsigned short, Allocator>& f)
    {
        return f;
    }
};

template <typename Allocator>
struct LexicalCastImpl<BasicString<unsigned short, Allocator>, const unsigned short*>
{
    static BasicString<unsigned short, Allocator> Do(const unsigned short* const& f)
    {
        return BasicString<unsigned short, Allocator>(f);
    }
};

} // namespace Detail

#endif // _NLLEXICALCAST_H_

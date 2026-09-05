#ifndef NL_STRING_IMPL_H
#define NL_STRING_IMPL_H

// Out-of-line string templates. Units that instantiate one include this file
// after their code so the instantiation is emitted with the unit's deferred
// weak functions instead of being expanded or placed after its first caller;
// every other unit calls the retained copy through the NL/nlString.h
// declaration.

template <typename CharT>
CharT* nlStrNCat(CharT* dest, const CharT* a, const CharT* b, unsigned long maxsize)
{
    unsigned long n = 0;
    while (*a)
    {
        dest[n] = *a++;
        if (++n >= maxsize)
        {
            dest[maxsize - 1] = 0;
            return dest;
        }
    }
    while (*b)
    {
        dest[n] = *b++;
        if (++n >= maxsize)
        {
            dest[maxsize - 1] = 0;
            return dest;
        }
    }
    dest[n] = 0;
    return dest;
}

#endif // NL_STRING_IMPL_H

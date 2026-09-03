#ifndef NL_STRING_TMPL_H
#define NL_STRING_TMPL_H

template <typename CharT>
CharT nlToUpper(CharT value);

template <typename CharT>
inline unsigned long nlStrLen(const CharT* str)
{
    unsigned long n = 0;
    if (str)
    {
        while (str[n])
            n++;
    }
    return n;
}

template <typename CharT>
inline CharT* nlStrNCpy(CharT* str1, const CharT* str2, unsigned long len)
{
    unsigned long n;
    int p;
    unsigned long c;
    n = len - 1;
    p = 0;
    goto test;
loop:
    p++;
test:
    if (n-- == 0)
        goto done;
    if (sizeof(CharT) == 1)
        c = *(const unsigned char*)&str2[p];
    else
        c = str2[p];
    if ((str1[p] = c) != 0)
        goto loop;
done:
    str1[p] = '\0';
    return str1;
}

template <typename CharT>
inline CharT nlToLower(CharT c)
{
    if ((c >= 0x41) && (c <= 0x5A))
    {
        c = (CharT)(c | 0x20);
    }
    return c;
}

template <typename CharT>
inline CharT* nlToLower(CharT* str)
{
    CharT* cp = str;
    while (*cp)
    {
        *cp = nlToLower<CharT>(*cp);
        cp++;
    }
    return str;
}

template <typename CharT>
inline int nlStrCmp(const CharT* a, const CharT* b)
{
    CharT c1;
    CharT c2;

    do
    {
        c1 = *a++;
        c2 = *b++;
    } while (c1 != 0 && c2 != 0 && c1 == c2);

    return c1 - c2;
}

template <typename CharT>
inline int nlStrICmp(const CharT* lhs, const CharT* rhs)
{
    CharT left;
    CharT right;
    do
    {
        left = *lhs++;
        if (left >= 0x61 && left <= 0x7A)
        {
            left = (CharT)(left & 0x5F);
        }
        right = *rhs++;
        if (right >= 0x61 && right <= 0x7A)
        {
            right = (CharT)(right & 0x5F);
        }
    } while (left != 0 && right != 0 && left == right);
    return left - right;
}

template <typename CharT>
inline int nlStrNCmp(const CharT* lhs, const CharT* rhs, unsigned long count)
{
    CharT left;
    CharT right;
    do
    {
        left = *lhs++;
        right = *rhs++;
    } while (--count != 0 && left != 0 && right != 0 && left == right);
    return left - right;
}

template <typename CharT>
inline CharT* nlStrChr(const CharT* str, CharT c)
{
    const CharT* cp = str;
    while (*cp != 0)
    {
        if (*cp == c)
        {
            return (CharT*)cp;
        }
        cp++;
    }
    return 0;
}

template <typename CharT>
CharT nlToUpper(CharT value)
{
    if (value >= 0x61 && value <= 0x7A)
    {
        return (CharT)(value & 0x5F);
    }
    return value;
}

#endif // NL_STRING_TMPL_H

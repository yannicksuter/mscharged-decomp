#ifndef NL_STRING_TMPL_H
#define NL_STRING_TMPL_H

template <typename CharT>
CharT nlToUpper(CharT value);

template <typename CharT>
inline unsigned long nlStrLen(const CharT* str)
{
    unsigned long length = 0;
    if (str != 0)
    {
        while (str[length] != 0)
        {
            ++length;
        }
    }
    return length;
}

template <typename CharT>
inline CharT nlToLower(CharT value)
{
    if (value >= 0x41 && value <= 0x5A)
    {
        value = (CharT)(value | 0x20);
    }
    return value;
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
CharT nlToUpper(CharT value)
{
    if (value >= 0x61 && value <= 0x7A)
    {
        return (CharT)(value & 0x5F);
    }
    return value;
}

#endif // NL_STRING_TMPL_H

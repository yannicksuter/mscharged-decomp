#ifndef NL_FONT_INL
#define NL_FONT_INL

#include "NL/nlFont.h"
#include "NL/nlBasicString.h"

inline unsigned long nlFont::GetStringWidth(const BasicString<unsigned short, Detail::TempStringAllocator>& Text, bool SingleLine, unsigned long Width, bool WordWrap) const
{
    FontCharString fontText(Text.c_str(), this, (unsigned short*)0);
    return GetStringWidth(fontText, SingleLine, Width, WordWrap);
}

inline unsigned long nlFont::fn_80305278(const BasicString<unsigned short, Detail::TempStringAllocator>& Text, unsigned long Width, bool WordWrap) const
{
    FontCharString fontText(Text.c_str(), this, (unsigned short*)0);
    return fn_80305278(fontText, Width, WordWrap);
}

#endif // NL_FONT_INL

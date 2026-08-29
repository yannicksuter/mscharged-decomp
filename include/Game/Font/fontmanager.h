#ifndef _FONTMANAGER_H_
#define _FONTMANAGER_H_

#include "NL/nlDLListContainer.h"
#include "NL/nlFont.h"
#include "NL/nlSingleton.h"

class FontManager : public nlSingleton<FontManager>
{
public:
    FontManager();
    virtual ~FontManager();

    nlFont* GetFontByHashID(unsigned long hashID);
    bool LoadFont(const char* bundlePath, const char* fontName, const char* fontFileName);

    /* 0x04 */ nlDLListSlotPool<nlFont*> m_fonts;
    /* 0x20 */ unsigned long field_0x20;
};

#endif // _FONTMANAGER_H_

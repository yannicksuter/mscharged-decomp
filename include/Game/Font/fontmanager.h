#ifndef _FONTMANAGER_H_
#define _FONTMANAGER_H_

#include "NL/nlDLListContainer.h"
#include "NL/nlSingleton.h"

class nlFont;
class GLResourcePool;

class FontManager : public nlSingleton<FontManager>
{
public:
    FontManager();
    virtual ~FontManager();

    nlFont* GetFontByHashID(unsigned long uHashID);
    bool IsLoadingComplete() const;
    bool LoadFont(const char* szBundleFilename, const char* szFileName, const char* szFileFontName);
    void SetResourcePool(GLResourcePool* pResourcePool);

    static void BundleOpenComplete(void*, unsigned long, unsigned long uParam);
    static void FontDescriptionLoadComplete(void* buffer, unsigned long, unsigned long uParam);
    static void TextureLoadComplete(void* buffer, unsigned long uReadSize, unsigned long uParam);

    /* 0x04 */ nlDLListSlotPool<nlFont*> m_fonts;
    /* 0x20 */ GLResourcePool* m_pResourcePool;
};

#endif // _FONTMANAGER_H_

#include "Game/Font/fontmanager.h"

#include "Game/GameInfo.h"
#include "Game/main.h"
#include "NL/MemAlloc.h"
#include "NL/nlLocalization.h"
#include "NL/nlPrint.h"

extern "C" bool fn_80306EF4(FontManager*);

static inline void LoadFonts()
{
    const char* TEXT_FONT_NAME = "fot-rodinprob18";
    const char* HEADING_FONT_NAME = "Scratchy36";
    char langprefix[4] = "eur";

    if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
    {
        langprefix[0] = 'j';
        langprefix[1] = 'p';
        langprefix[2] = 'n';
    }

    char textfontbundlename[64];
    char textfontfilename[64];
    char headingfontbundlename[64];
    char headingfontfilename[64];

    nlSNPrintf(textfontbundlename, 64, "art/fe/fonts/%sfonttext18.res", langprefix);
    nlSNPrintf(textfontfilename, 64, "fe/fonts/%sfonttext18", langprefix);
    nlSNPrintf(headingfontbundlename, 64, "art/fe/fonts/%sfontheading36.res", langprefix);
    nlSNPrintf(headingfontfilename, 64, "fe/fonts/%sfontheading36", langprefix);
    FontManager::Instance()->LoadFont(textfontbundlename, textfontfilename, TEXT_FONT_NAME);
    FontManager::Instance()->LoadFont(headingfontbundlename, headingfontfilename, HEADING_FONT_NAME);
}

static inline void LoadFontsJapaneseInGame()
{
    const char* TEXT_FONT_NAME = "fot-rodinprob18";
    const char* HEADING_FONT_NAME = "Scratchy36";
    char textfontbundlename[64];
    char textfontfilename[64];
    char headingfontbundlename[64];
    char headingfontfilename[64];
    char langprefix[4] = "jpn";

    nlSNPrintf(textfontbundlename, 64, "art/fe/fonts/%sfonttextingame18.res", langprefix);
    nlSNPrintf(textfontfilename, 64, "fe/fonts/%sfonttextingame18", langprefix);
    nlSNPrintf(headingfontbundlename, 64, "art/fe/fonts/%sfontheadingingame36.res", langprefix);
    nlSNPrintf(headingfontfilename, 64, "fe/fonts/%sfontheadingingame36", langprefix);
    FontManager::Instance()->LoadFont(textfontbundlename, textfontfilename, TEXT_FONT_NAME);
    FontManager::Instance()->LoadFont(headingfontbundlename, headingfontfilename, HEADING_FONT_NAME);
}

static inline void LoadFontsJapanese101()
{
    const char* TEXT_FONT_NAME = "fot-rodinprob18";
    const char* HEADING_FONT_NAME = "cepoitalic24";
    char textfontbundlename[64];
    char textfontfilename[64];
    char headingfontbundlename[64];
    char headingfontfilename[64];
    char langprefix[4] = "jpn";

    nlSNPrintf(textfontbundlename, 64, "art/fe/fonts/%sfonttext10118.res", langprefix);
    nlSNPrintf(textfontfilename, 64, "art/fe/fonts/%sfonttext10118", langprefix);
    nlSNPrintf(headingfontbundlename, 64, "art/fe/fonts/%sfontheading10124.res", langprefix);
    nlSNPrintf(headingfontfilename, 64, "art/fe/fonts/%sfontheading10124", langprefix);
    FontManager::Instance()->LoadFont(textfontbundlename, textfontfilename, TEXT_FONT_NAME);
    FontManager::Instance()->LoadFont(headingfontbundlename, headingfontfilename, HEADING_FONT_NAME);
}

extern "C" bool fn_8020078C()
{
    if (g_pLocalization->m_pFile == 0)
    {
        return false;
    }
    return fn_80306EF4(FontManager::Instance());
}

extern "C" bool fn_80200380(bool* inGame)
{
    g_pLocalization->Load(g_Language, false, &VirtualAllocator);

    if (*inGame)
    {
        if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
        {
            if (GameInfoManager::Instance()->unknown_0x122)
            {
                LoadFontsJapanese101();
            }
            else
            {
                LoadFontsJapaneseInGame();
            }
        }
        else
        {
            LoadFonts();
        }
    }
    else
    {
        LoadFonts();
    }

    if (g_pLocalization->m_pFile == 0)
    {
        return false;
    }
    return fn_80306EF4(FontManager::Instance());
}

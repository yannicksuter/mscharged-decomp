#include "Game/Font/FontLoading.h"
#include "Game/Font/fontmanager.h"

#include "Game/GameInfo.h"
#include "Game/main.h"
#include "NL/MemAlloc.h"
#include "NL/nlLocalization.h"
#include "NL/nlPrint.h"

struct FontLanguagePrefix
{
    char value[4];
};

static FontLanguagePrefix sDefaultLanguagePrefix = { "eur" };
static FontLanguagePrefix sJapaneseInGameLanguagePrefix = { "jpn" };
static FontLanguagePrefix sJapanese101LanguagePrefix = { "jpn" };

bool gLoadInGameFonts;

static inline void LoadFonts()
{
    const char* TEXT_FONT_NAME = "fot-rodinprob18";
    const char* HEADING_FONT_NAME = "Scratchy36";
    FontLanguagePrefix langprefix = sDefaultLanguagePrefix;

    if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
    {
        langprefix.value[0] = 'j';
        langprefix.value[1] = 'p';
        langprefix.value[2] = 'n';
    }

    char textfontbundlename[64];
    char textfontfilename[64];
    char headingfontbundlename[64];
    char headingfontfilename[64];

    nlSNPrintf(textfontbundlename, 64, "art/fe/fonts/%sfonttext18.res", langprefix.value);
    nlSNPrintf(textfontfilename, 64, "fe/fonts/%sfonttext18", langprefix.value);
    nlSNPrintf(headingfontbundlename, 64, "art/fe/fonts/%sfontheading36.res", langprefix.value);
    nlSNPrintf(headingfontfilename, 64, "fe/fonts/%sfontheading36", langprefix.value);
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
    FontLanguagePrefix langprefix = sJapaneseInGameLanguagePrefix;

    nlSNPrintf(textfontbundlename, 64, "art/fe/fonts/%sfonttextingame18.res", langprefix.value);
    nlSNPrintf(textfontfilename, 64, "fe/fonts/%sfonttextingame18", langprefix.value);
    nlSNPrintf(headingfontbundlename, 64, "art/fe/fonts/%sfontheadingingame36.res", langprefix.value);
    nlSNPrintf(headingfontfilename, 64, "fe/fonts/%sfontheadingingame36", langprefix.value);
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
    FontLanguagePrefix langprefix = sJapanese101LanguagePrefix;

    nlSNPrintf(textfontbundlename, 64, "art/fe/fonts/%sfonttext10118.res", langprefix.value);
    nlSNPrintf(textfontfilename, 64, "art/fe/fonts/%sfonttext10118", langprefix.value);
    nlSNPrintf(headingfontbundlename, 64, "art/fe/fonts/%sfontheading10124.res", langprefix.value);
    nlSNPrintf(headingfontfilename, 64, "art/fe/fonts/%sfontheading10124", langprefix.value);
    FontManager::Instance()->LoadFont(textfontbundlename, textfontfilename, TEXT_FONT_NAME);
    FontManager::Instance()->LoadFont(headingfontbundlename, headingfontfilename, HEADING_FONT_NAME);
}

bool UpdateFontLoading(bool*)
{
    if (g_pLocalization->m_pFile == 0)
    {
        return false;
    }
    return FontManager::Instance()->IsLoadingComplete();
}

bool BeginFontLoading(bool* inGame)
{
    g_pLocalization->Load(g_Language, false, &VirtualAllocator);

    if (*inGame)
    {
        if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
        {
            if (GameInfoManager::Instance()->mIsInStrikers101Mode)
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
    return FontManager::Instance()->IsLoadingComplete();
}

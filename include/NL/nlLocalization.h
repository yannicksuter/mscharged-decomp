#ifndef _NLLOCALIZATION_H_
#define _NLLOCALIZATION_H_

#include "types.h"

struct LOCHeader
{
    char Thumbprint[4];
    unsigned long Version;
    unsigned long Language;
    unsigned long StringCount;
    unsigned long Flags;
};

class nlLocalization
{
public:
    struct StringLookup
    {
        unsigned long hash;
        unsigned long StringOffset;

        operator unsigned long() const { return hash; }
    };

    // Charged inserts the two North American variants between UKEnglish and
    // the debug entries; the seven shared identifier hashes are unchanged.
    enum nlLanguage
    {
        LangEnglish = 0,
        LangFrench = 1,
        LangGerman = 2,
        LangSpanish = 3,
        LangItalian = 4,
        LangJapanese = 5,
        LangUKEnglish = 6,
        LangNAFrench = 7,
        LangNASpanish = 8,
        LangLongestStrings = 9,
        LangBob = 10,
        LangEnd = 11,
    };

    unsigned char Load(nlLanguage Language, bool ingameloc, void* queue);
    static void Initialize();

    LOCHeader* m_pFile;
    StringLookup* m_LookupTable;
    unsigned short* m_FirstString;
    nlLanguage m_CurrentLanguage;

    static char* LanguageName[];
    static const unsigned long LanguageId[];
    static const char Thumbprint[5];
};

extern nlLocalization* g_pLocalization;
extern const unsigned short LocalizationTableNotFound[];
extern const unsigned short MissingLocString[];

#endif // _NLLOCALIZATION_H_

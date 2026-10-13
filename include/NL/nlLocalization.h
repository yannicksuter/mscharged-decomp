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
        bool operator==(const unsigned long& key) const { return hash == key; }
    };

    // Charged inserts the two North American variants between UKEnglish and
    // the debug entries. The Korean release also inserts Korean after Japanese.
    enum nlLanguage
    {
        LangEnglish = 0,
        LangFrench = 1,
        LangGerman = 2,
        LangSpanish = 3,
        LangItalian = 4,
        LangJapanese = 5,
#if defined(VERSION_R4QK01)
        LangKorean,
#endif
        LangUKEnglish,
        LangNAFrench,
        LangNASpanish,
        LangLongestStrings,
        LangBob,
        LangEnd,
    };

    unsigned char Load(nlLanguage Language, bool ingameloc, void* allocator);
    static void Initialize();
    nlLanguage GetCurrentLanguage() const;
    const unsigned short* GetString(const char* name) const;


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

const unsigned short* GetString(nlLocalization* localization, const char* id);

#endif // _NLLOCALIZATION_H_

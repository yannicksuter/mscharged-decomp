#include "NL/nlLocalization.h"

#include <string.h>

#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"

extern const unsigned short LocalizationTableNotFound[] = L"Localization Table Not Found";
extern const unsigned short MissingLocString[] = L"missing loc string";

const unsigned long nlLocalization::LanguageId[] = {
    0x7A947B29,
    0xA93C2035,
    0xAAAD26B9,
    0xB482A4B5,
    0xBC0FCCA1,
    0x95F1D726,
    0x5F2F5E69,
    0x30D469C4,
    0x2F242024,
    0x983D29BB,
    0x00012332,
};

char* nlLocalization::LanguageName[] = {
    "English",
    "French",
    "German",
    "Spanish",
    "Italian",
    "Japanese",
    "UKEnglish",
    "NAFrench",
    "NASpanish",
    "Longest",
    "Bob",
};

// Charged spells the thumbprint as a five-byte string rather than the
// predecessor's char[4] initialiser list.
const char nlLocalization::Thumbprint[5] = "NLOC";

nlLocalization* g_pLocalization;

// Charged spells the file thumbprint as a string literal, so it is five bytes
// in read-only small data rather than the predecessor's char[4] initialiser.


static void OnTableLoaded(LOCHeader* pFile, unsigned long, nlLocalization* pLocalization)
{
    pLocalization->m_pFile = pFile;

    if (memcmp(pFile, nlLocalization::Thumbprint, 4) != 0 || pFile->Version != 1
        || pFile->Language != nlLocalization::LanguageId[pLocalization->m_CurrentLanguage])
    {
        nlFree(pFile);
        pLocalization->m_pFile = 0;
        pLocalization->m_LookupTable = 0;
        pLocalization->m_FirstString = 0;
        return;
    }

    pLocalization->m_LookupTable = (nlLocalization::StringLookup*)(pFile + 1);
    pLocalization->m_FirstString =
        (unsigned short*)(&pLocalization->m_LookupTable[pLocalization->m_pFile->StringCount]);
}

void nlLocalization::Initialize()
{
    nlLocalization* pLocalization = (nlLocalization*)nlMalloc(sizeof(nlLocalization), 8, false);
    if (pLocalization != 0)
    {
        pLocalization->m_pFile = 0;
        pLocalization->m_LookupTable = 0;
        pLocalization->m_FirstString = 0;
    }
    g_pLocalization = pLocalization;
}

unsigned char nlLocalization::Load(nlLanguage Language, bool ingameloc, void* allocator)
{
    char Filename[64] = { 0 };

    m_LookupTable = 0;
    m_FirstString = 0;
    m_CurrentLanguage = Language;

    if (ingameloc)
    {
        nlSNPrintf(Filename, 64, "art/fe/%s_game.loc", LanguageName[Language]);
    }
    else
    {
        nlSNPrintf(Filename, 64, "art/fe/%s.loc", LanguageName[Language]);
    }

    int result;
    if (allocator != 0)
    {
        result = nlLoadEntireFileAsync(Filename, (LoadAsyncCallback)OnTableLoaded, this, 32, AllocateStart, 0, 0, (MemoryAllocator*)allocator);
    }
    else
    {
        result = nlLoadEntireFileAsync(Filename, (LoadAsyncCallback)OnTableLoaded, this, 32, AllocateStart, 0, 0, 0);
    }

    return result != 0;
}

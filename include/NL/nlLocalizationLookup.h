#ifndef NL_LOCALIZATION_LOOKUP_H
#define NL_LOCALIZATION_LOOKUP_H

#include "NL/nlAlgorithm.h"
#include "NL/nlLocalization.h"
#include "NL/nlString.h"

inline const unsigned short* nlLocalization::GetString(const char* string) const
{
    unsigned long hash = nlStringLowerHash(string);
    if (m_LookupTable == 0)
        return LocalizationTableNotFound;
    StringLookup* found = nlBSearch<StringLookup, unsigned long>(hash, m_LookupTable, m_pFile->StringCount);
    if (found != 0)
        return m_FirstString + found->StringOffset;
    return MissingLocString;
}

inline const unsigned short* LookupLocString(const char* id)
{
    return g_pLocalization->GetString(id);
}

#endif // NL_LOCALIZATION_LOOKUP_H

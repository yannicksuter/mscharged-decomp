#ifndef NL_LOCALIZATION_LOOKUP_H
#define NL_LOCALIZATION_LOOKUP_H

#include "NL/nlAlgorithm.h"
#include "NL/nlLocalization.h"
#include "NL/nlString.h"

inline const unsigned short* LookupLocString(const char* id)
{
    nlLocalization* localization = g_pLocalization;
    unsigned long hash = nlStringLowerHash(id);
    if (localization->m_LookupTable == 0)
    {
        return LocalizationTableNotFound;
    }

    nlLocalization::StringLookup* lookup
        = nlBSearch<nlLocalization::StringLookup, unsigned long>(
            hash, localization->m_LookupTable, (int)localization->m_pFile->StringCount);
    if (lookup != 0)
    {
        return localization->m_FirstString + lookup->StringOffset;
    }

    return MissingLocString;
}

#endif // NL_LOCALIZATION_LOOKUP_H

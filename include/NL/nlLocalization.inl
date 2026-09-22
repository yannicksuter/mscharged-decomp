#ifndef NL_NL_LOCALIZATION_INL
#define NL_NL_LOCALIZATION_INL

#include "NL/nlLocalization.h"

inline nlLocalization::nlLanguage nlLocalization::GetCurrentLanguage() const
{
    return m_CurrentLanguage;
}

#endif // NL_NL_LOCALIZATION_INL

#ifndef MSL_WCTYPE_H
#define MSL_WCTYPE_H

#include "internal/locale.h"
#include "locale.h"
#include <wchar_t.h>

inline int iswdigit(wint_t value)
{
    return value < 0 || value >= 256
        ? 0
        : _current_locale.ctype_cmpt_ptr->wctype_map_ptr[value] & (1 << 3);
}

inline int iswupper(wint_t value)
{
    return value < 0 || value >= 256
        ? 0
        : _current_locale.ctype_cmpt_ptr->wctype_map_ptr[value] & (1 << 9);
}

#endif // MSL_WCTYPE_H

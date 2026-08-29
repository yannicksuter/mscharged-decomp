#ifndef MSL_CTYPE_H
#define MSL_CTYPE_H

#include "internal/locale.h"

#ifdef __cplusplus
extern "C" {
#endif

#define __MSL_ISALPHA_MAP  (1 << 0)
#define __MSL_ISBLANK_MAP  (1 << 1)
#define __MSL_ISCNTRL_MAP  (1 << 2)
#define __MSL_ISDIGIT_MAP  (1 << 3)
#define __MSL_ISGRAPH_MAP  (1 << 4)
#define __MSL_ISLOWER_MAP  (1 << 5)
#define __MSL_ISPRINT_MAP  (1 << 6)
#define __MSL_ISPUNCT_MAP  (1 << 7)
#define __MSL_ISSPACE_MAP  (1 << 8)
#define __MSL_ISUPPER_MAP  (1 << 9)
#define __MSL_ISXDIGIT_MAP (1 << 10)

inline int isalpha(int value)
{
    return value < 0 || value >= 256
        ? 0
        : _current_locale.ctype_cmpt_ptr->ctype_map_ptr[value] & __MSL_ISALPHA_MAP;
}

inline int isdigit(int value)
{
    return value < 0 || value >= 256
        ? 0
        : _current_locale.ctype_cmpt_ptr->ctype_map_ptr[value] & __MSL_ISDIGIT_MAP;
}

inline int isspace(int value)
{
    return value < 0 || value >= 256
        ? 0
        : _current_locale.ctype_cmpt_ptr->ctype_map_ptr[value] & __MSL_ISSPACE_MAP;
}

inline int isxdigit(int value)
{
    return value < 0 || value >= 256
        ? 0
        : _current_locale.ctype_cmpt_ptr->ctype_map_ptr[value] & __MSL_ISXDIGIT_MAP;
}

inline int tolower(int value)
{
    return value < 0 || value >= 256
        ? value
        : _current_locale.ctype_cmpt_ptr->lower_map_ptr[value];
}

inline int toupper(int value)
{
    return value < 0 || value >= 256
        ? value
        : _current_locale.ctype_cmpt_ptr->upper_map_ptr[value];
}

#ifdef __cplusplus
}
#endif

#endif // MSL_CTYPE_H

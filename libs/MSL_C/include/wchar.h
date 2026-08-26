#ifndef MSL_WCHAR_H
#define MSL_WCHAR_H

#include <size_t.h>

#ifdef __cplusplus
extern "C" {

/* wchar_t is a keyword only in C++; declaring this unguarded makes
   <wchar.h> invalid for every C translation unit that includes it. */
size_t wcslen(const wchar_t* s);

}
#endif

#endif // MSL_WCHAR_H

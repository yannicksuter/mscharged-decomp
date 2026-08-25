#ifndef MSL_STDIO_H
#define MSL_STDIO_H

#include <stdarg.h>
#include <size_t.h>

#ifdef __cplusplus
extern "C" {
#endif

int sprintf(char* s, const char* format, ...);
int snprintf(char* s, size_t n, const char* format, ...);
int vprintf(const char* format, va_list arguments);
int vsprintf(char* s, const char* format, va_list arguments);
int vsnprintf(char* s, size_t n, const char* format, va_list arguments);
#ifdef __cplusplus
/* wchar_t is a keyword only in C++; declaring this unguarded makes <stdio.h>
   invalid for every C translation unit that includes it. */
int vswprintf(wchar_t* s, size_t n, const wchar_t* format, va_list arguments);
#endif

#ifdef __cplusplus
}
#endif

#endif // MSL_STDIO_H

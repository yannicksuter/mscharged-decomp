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
int vswprintf(wchar_t* s, size_t n, const wchar_t* format, va_list arguments);

#ifdef __cplusplus
}
#endif

#endif // MSL_STDIO_H

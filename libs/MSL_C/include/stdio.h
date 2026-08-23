#ifndef MSL_STDIO_H
#define MSL_STDIO_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

int vprintf(const char* format, va_list arguments);

#ifdef __cplusplus
}
#endif

#endif // MSL_STDIO_H

#ifndef RVL_SDK_NAND_LOGGING_H
#define RVL_SDK_NAND_LOGGING_H
#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*NANDLoggingCallback)(BOOL result);

BOOL NANDLoggingAddMessageAsync(NANDLoggingCallback callback, const char* format, ...);

#ifdef __cplusplus
}
#endif
#endif

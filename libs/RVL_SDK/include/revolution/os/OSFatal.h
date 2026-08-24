#ifndef REVOLUTION_OS_FATAL_H
#define REVOLUTION_OS_FATAL_H

#include <revolution/gx.h>

#ifdef __cplusplus
extern "C" {
#endif

void OSFatal(GXColor textColor, GXColor bgColor, const char* message);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_FATAL_H

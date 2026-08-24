#ifndef REVOLUTION_DB_H
#define REVOLUTION_DB_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void DBInit(void);
void __DBExceptionDestinationAux(void);
void __DBExceptionDestination(void);
BOOL __DBIsExceptionMarked(u8 exception);
void DBPrintf(const char* message, ...);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_DB_H

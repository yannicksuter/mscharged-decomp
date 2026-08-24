#ifndef REVOLUTION_SC_H
#define REVOLUTION_SC_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SC_STATUS_BUSY 1

void SCInit(void);
u32 SCCheckStatus(void);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_SC_H

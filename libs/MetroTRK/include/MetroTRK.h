#ifndef METROTRK_H
#define METROTRK_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void InitMetroTRK(void);
void InitMetroTRK_BBA(void);
void InitMetroTRKCommTable(s32 type);
void EnableMetroTRKInterrupts(void);

#ifdef __cplusplus
}
#endif

#endif  // METROTRK_H

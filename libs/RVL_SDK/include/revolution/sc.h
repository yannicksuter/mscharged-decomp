#ifndef REVOLUTION_SC_H
#define REVOLUTION_SC_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SC_STATUS_BUSY 1

typedef enum {
    SC_LANG_JP,
    SC_LANG_EN,
    SC_LANG_DE,
    SC_LANG_FR,
    SC_LANG_SP,
    SC_LANG_IT,
    SC_LANG_NL,
    SC_LANG_ZH_S,
    SC_LANG_ZH_T,
    SC_LANG_KR,
    SC_LANG_MAX
} SCLanguage;

u8 SCGetLanguage(void);

void SCInit(void);
u32 SCCheckStatus(void);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_SC_H

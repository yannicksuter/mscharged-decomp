#ifndef MSL_ANSI_FP_H
#define MSL_ANSI_FP_H

#include <float.h>
#include <math.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SIGDIGLEN 36

typedef struct decimal {
    char sign;
    char unk1;
    s16 exp;
    struct {
        u8 length;
        u8 text[SIGDIGLEN];
        u8 unk41;
    } sig;
} decimal;

double __dec2num(const decimal* d);

#ifdef __cplusplus
}
#endif

#endif // MSL_ANSI_FP_H

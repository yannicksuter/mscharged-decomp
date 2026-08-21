/*************************************************************************
 * Open Dynamics Engine, Copyright (C) 2001-2004 Russell L. Smith.
 * Distributed under the BSD-style license in LICENSES/ODE-BSD.txt.
 *************************************************************************/

#ifndef ODE_ERROR_H
#define ODE_ERROR_H

typedef struct __va_list_struct
{
    char gpr;
    char fpr;
    char reserved[2];
    char* input_arg_area;
    char* reg_save_area;
} __va_list[1];

typedef __va_list va_list;

extern void __builtin_va_info(void*);
#define va_start(ap, fmt) ((void)fmt, __builtin_va_info(&ap))
#define va_end(ap)        ((void)0)

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void dMessageFunction(int errnum, const char* msg, va_list ap);

    void dDebug(int num, const char* msg, ...);
    void dMessage(int num, const char* msg, ...);

#ifdef __cplusplus
}
#endif

#endif

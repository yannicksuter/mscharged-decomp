#ifndef REVOLUTION_TYPES_H
#define REVOLUTION_TYPES_H

#include <decomp.h>

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef signed long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;

typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;
typedef volatile s8 vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

typedef float f32;
typedef double f64;
typedef volatile f32 vf32;
typedef volatile f64 vf64;

typedef int BOOL;
typedef volatile BOOL vBOOL;

typedef int UNKWORD;
typedef void UNKTYPE;
#ifndef __cplusplus
typedef int bool;
typedef unsigned short wchar_t;

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

typedef unsigned long uintptr_t;
typedef unsigned long int byte4_t;

#define ALIGN(x) __attribute__((aligned(x)))
#define PACKED __attribute__((packed))

#define DEFAULT_ALIGN 32
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

typedef void (*funcptr_t)(void);

int __abs(int value);

#endif  // REVOLUTION_TYPES_H

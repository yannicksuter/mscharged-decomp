#ifndef DECOMP_H
#define DECOMP_H

#define ROUNDUP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#define ROUNDDOWN(x, align) ((x) & ~((align) - 1))
#define PTR_ROUNDUP(x, align) ((void*)ROUNDUP((unsigned long)(x), (align)))

#define __CONCAT(x, y) x##y
#define CONCAT(x, y) __CONCAT(x, y)

#define ROUND_UP(x, align) ROUNDUP((x), (align))
#define ROUND_DOWN_PTR(x, align) ((void*)ROUNDDOWN((unsigned long)(x), (align)))

#define CLAMP(low, high, x) \
    ((x) > (high) ? (high) : ((x) < (low) ? (low) : (x)))

#define DECOMP_DONT_INLINE __attribute__((never_inline))
#define DECOMP_FORCEACTIVE(module, ...)                       \
    void fake_function(...);                                 \
    void CONCAT(FORCEACTIVE##module, __LINE__)(void);         \
    void CONCAT(FORCEACTIVE##module, __LINE__)(void) {        \
        fake_function(__VA_ARGS__);                           \
    }

#ifdef __MWERKS__
#define AT_ADDRESS(address) : (address)
#else
#define AT_ADDRESS(address)
#endif

#endif  // DECOMP_H

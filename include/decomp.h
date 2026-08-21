#ifndef DECOMP_H
#define DECOMP_H

#define ROUNDUP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#define ROUNDDOWN(x, align) ((x) & ~((align) - 1))
#define PTR_ROUNDUP(x, align) ((void*)ROUNDUP((unsigned long)(x), (align)))

#ifdef __MWERKS__
#define AT_ADDRESS(address) : (address)
#else
#define AT_ADDRESS(address)
#endif

#endif  // DECOMP_H

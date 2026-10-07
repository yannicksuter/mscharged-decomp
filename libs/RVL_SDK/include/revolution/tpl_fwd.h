#ifndef REVOLUTION_TPL_FWD_H
#define REVOLUTION_TPL_FWD_H

#include <revolution/tpl/TPLTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

void TPLBind(TPLPalette* pal);
TPLDescriptor* TPLGet(TPLPalette* pal, unsigned long id);

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_TPL_FWD_H

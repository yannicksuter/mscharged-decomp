#ifndef RVL_SDK_GX_LIGHT_FWD_H
#define RVL_SDK_GX_LIGHT_FWD_H

#include <revolution/gx/GXPublicTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

void GXSetChanAmbColor(GXChannelID channel, GXColor color);
void GXSetChanMatColor(GXChannelID channel, GXColor color);

#ifdef __cplusplus
}
#endif

#endif

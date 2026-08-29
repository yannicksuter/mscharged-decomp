#ifndef RVL_FACE_LIBRARY_ICON_H
#define RVL_FACE_LIBRARY_ICON_H
#include <revolution/types.h>

#include <revolution/gx.h>
#include <revolution/mtx.h>

#include <RVLFaceLib/RFL_MiddleDatabase.h>
#include <RVLFaceLib/RFL_Types.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct RFLIconSetting {
    u16 width;             // at 0x0
    u16 height;            // at 0x2
    RFLIconBGType bgType;  // at 0x4
    GXColor bgColor;   // at 0x8
    BOOL drawXluOnly;  // at 0xC
} RFLIconSetting;

RFLErrcode RFLMakeIcon(void* buf, RFLDataSource source, RFLMiddleDB* middleDB,
                       u16 index, RFLExpression expression,
                       const RFLIconSetting* setting);

void RFLSetIconDrawDoneCallback(RFLCallback callback);

#ifdef __cplusplus
}
#endif
#endif

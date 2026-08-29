#ifndef RVL_FACE_LIBRARY_INTERNAL_DEFAULT_DATABASE_H
#define RVL_FACE_LIBRARY_INTERNAL_DEFAULT_DATABASE_H
#include <revolution/types.h>

#include <RVLFaceLib/RFLi_Types.h>
#ifdef __cplusplus
extern "C" {
#endif

#define RFL_DEFAULT_DATA_MAX 6

RFLiCharData* RFLiGetDefaultDataPtr(u16 index);
void RFLiGetDefaultData(RFLiCharInfo* info, u16 index);

#ifdef __cplusplus
}
#endif
#endif

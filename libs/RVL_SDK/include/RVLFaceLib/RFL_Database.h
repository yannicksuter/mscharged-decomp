#ifndef RVL_FACE_LIBRARY_DATABASE_H
#define RVL_FACE_LIBRARY_DATABASE_H
#include <revolution/types.h>

#include <RVLFaceLib/RFL_Types.h>
#ifdef __cplusplus
extern "C" {
#endif

BOOL RFLIsAvailableOfficialData(u16 index);
u16 RFLGetAvailableOfficialDataNum(void);
BOOL RFLSearchOfficialData(const RFLCreateID* id, u16* index);
RFLErrcode RFLGetStoreData(RFLStoreData* data, RFLDataSource source,
                           u16 index);

#ifdef __cplusplus
}
#endif
#endif

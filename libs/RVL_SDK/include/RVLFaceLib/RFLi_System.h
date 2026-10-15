#ifndef RVL_FACE_LIBRARY_INTERNAL_SYSTEM_H
#define RVL_FACE_LIBRARY_INTERNAL_SYSTEM_H
#include <revolution/types.h>

#include <RVLFaceLib/RFL_System.h>
#include <RVLFaceLib/RFLi_Controller.h>
#include <RVLFaceLib/RFLi_Database.h>
#include <RVLFaceLib/RFLi_HiddenDatabase.h>
#include <RVLFaceLib/RFLi_NANDAccess.h>
#include <RVLFaceLib/RFLi_NANDLoader.h>
#include <RVLFaceLib/RFLi_Types.h>
#if defined(VERSION_R4QK01)
#include <RVLFaceLib/RFLi_Model.h>
#endif

#include <revolution/mem.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct RFLiManager {
    void* workBuffer;                      // at 0x0
    MEMiHeapHead* rootHeap;                // at 0x4
    MEMiHeapHead* systemHeap;              // at 0x8
    MEMiHeapHead* tmpHeap;                 // at 0xC
    RFLiDBManager dbMgr;                   // at 0x10
    RFLiLoader loader;                     // at 0xC0
    RFLiHDBManager hdbMgr;                 // at 0x16C
    RFLiCtrlBufManager ctrlMgr;            // at 0x1AAC
    BOOL working;                          // at 0x1B34 (Korea 0x1B20)
    BOOL deluxeTex;                        // at 0x1B38 (Korea 0x1B24)
    u8 brokenType;                         // at 0x1B3C (Korea 0x1B28)
    RFLErrcode lastErrCode;                // at 0x1B40 (Korea 0x1B2C)
    RFLErrcode beforeCloseErr;             // at 0x1B44 (Korea 0x1B30)
    s32 lastReason;                        // at 0x1B48 (Korea 0x1B34)
    s32 beforeCloseReason;                 // at 0x1B4C (Korea 0x1B38)
    RFLiAccessInfo info[RFLiFileType_Max]; // at 0x1B50 (Korea 0x1B40)
#if defined(VERSION_R4QK01)
    RFLiCallback systemCb;                 // at 0x1F00
    u8 UNK_0x1F04[8];
    RFLCallback modelDrawCb;               // at 0x1F0C
    RFLCallback iconDrawCb;                // at 0x1F10
    RFLiCoordinateData coordinateData;      // at 0x1F14
#else
    RFLiCallback systemCb;                 // at 0x1F10
    RFLCallback iconDrawCb;                // at 0x1F14
    RFLCallback modelDrawCb;               // at 0x1F18
    char UNK_0x1F1C[0x4];
#endif
} RFLiManager;

RFLErrcode RFLiBootLoadAsync(void);
void* RFLiAlloc(u32 size);
void* RFLiAlloc32(u32 size);
void RFLiFree(void* block);
RFLiDBManager* RFLiGetDBManager(void);
RFLiHDBManager* RFLiGetHDBManager(void);
RFLiLoader* RFLiGetLoader(void);
BOOL RFLiGetWorking(void);
void RFLiSetWorking(BOOL working);
RFLiManager* RFLiGetManager(void);
BOOL RFLiGetUseDeluxTex(void);
RFLiAccessInfo* RFLiGetAccInfo(RFLiFileType type);
RFLiCtrlBufManager* RFLiGetCtrlBufManager(void);
s32 RFLiGetLastReason(void);
void RFLiSetFileBroken(RFLiFileBrokenType type);
BOOL RFLiNotFoundError(void);
BOOL RFLiNeedRepairError(void);
BOOL RFLiCriticalError(void);

#ifdef __cplusplus
}
#endif
#endif

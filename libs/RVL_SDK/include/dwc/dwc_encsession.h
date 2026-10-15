#pragma once

#include <dwc/dwc_ranksession.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    enum
    {
        DWCi_ENC_SERVER_RELEASE,
        DWCi_ENC_SERVER_DEBUG,
        DWCi_ENC_SERVER_TEST
    };

    void DWCi_EncSessionInitialize(int server, const char* initdata);
    DWCiRankingSessionState DWCi_EncSessionProcess(void);
#if defined(VERSION_R4QK01)
    void DWCi_EncSessionCancel(void);
#endif
    void DWCi_EncSessionShutdown(void);
    void* DWCi_EncSessionGetResponse(u32* size);
    DWCiRankingSessionResult DWCi_EncSessionGetAsync(const char* url, s32 pid,
        void* data, u32 size, BOOL (*callback)(u32* data, int size));

#ifdef __cplusplus
}
#endif

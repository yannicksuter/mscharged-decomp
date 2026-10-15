#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(VERSION_R4QK01)
    typedef void (*DWCUserRecvCallback)(unsigned char aid,
        unsigned char* buffer, int size, void* param);
#else
    typedef void (*DWCUserRecvCallback)(unsigned char aid,
        unsigned char* buffer, int size);
#endif

    int DWC_SendUnreliable(unsigned char aid, const void* buffer, int size);
    int DWC_SetRecvBuffer(unsigned char aid, void* buffer, int size);
#if defined(VERSION_R4QK01)
    int DWC_SetUserRecvCallback(DWCUserRecvCallback callback, void* param);
#else
    int DWC_SetUserRecvCallback(DWCUserRecvCallback callback);
#endif

#ifdef __cplusplus
}
#endif

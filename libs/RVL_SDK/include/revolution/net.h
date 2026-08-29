#ifndef RVL_SDK_PUBLIC_NET_H
#define RVL_SDK_PUBLIC_NET_H
#ifdef __cplusplus
extern "C" {
#endif

#include <revolution/net/NETVersion.h>
#include <revolution/net/NETDigest.h>
#include <revolution/net/nettime.h>

int NETGetWirelessMacAddress(void* data);
int NETGetStartupErrorCode(int);

#ifdef __cplusplus
}
#endif
#endif

#include <revolution/ncd.h>
#include <revolution/net_fwd.h>

int NETGetWirelessMacAddress(void* data)
{
    return NCDiGetWirelessMacAddress(data);
}

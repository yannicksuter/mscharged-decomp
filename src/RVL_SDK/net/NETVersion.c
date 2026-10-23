#pragma force_active on

#include <revolution/net/NETVersion.h>

#if defined(VERSION_R4QK01)
const char* NETRexPPCVersionPrintableString = "<< REX-PPC 2.4.255.0 (RevoEX-2.4) REL 090609111526 >>";
#else
const char* NETRexPPCVersionPrintableString = "<< REX-PPC 1.0.0.0 (RevoEX-1.0) REL 070309140556 >>";
#endif

const char* NETGetRexPPCVersionPrintable(void)
{
    return NETRexPPCVersionPrintableString;
}

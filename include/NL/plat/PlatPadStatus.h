#ifndef NL_PLAT_PLATPADSTATUS_H
#define NL_PLAT_PLATPADSTATUS_H

#include <revolution/wpad/WPAD.h>
#include <revolution/kpad/KPAD.h>

struct WiiRemotePadStatus
{
    WPADStatus wpad;
    KPADStatus kpad;
}; // size 0xB0

struct WiiFreestylePadStatus
{
    WPADFSStatus wpad;
    KPADStatus kpad;
}; // size 0xB8

struct WiiClassicPadStatus
{
    WPADCLStatus wpad;
    KPADStatus kpad;
}; // size 0xBC

union PlatPadStatus
{
    WiiRemotePadStatus core;
    WiiFreestylePadStatus freestyle;
    WiiClassicPadStatus classic;
};


#endif // NL_PLAT_PLATPADSTATUS_H

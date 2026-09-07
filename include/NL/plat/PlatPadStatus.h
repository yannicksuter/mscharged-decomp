#ifndef NL_PLAT_PLATPADSTATUS_H
#define NL_PLAT_PLATPADSTATUS_H

#include <revolution/wpad/WPAD.h>
#include <revolution/kpad/KPAD.h>

struct PlatPadStatus_80375EC8
{
    WPADStatus wpad;
    KPADStatus kpad;
}; // size 0xB0

struct PlatPadStatus_80375ED4
{
    WPADFSStatus wpad;
    KPADStatus kpad;
}; // size 0xB8

struct PlatPadStatus_80375EE0
{
    WPADCLStatus wpad;
    KPADStatus kpad;
}; // size 0xBC

union PlatPadStatus
{
    PlatPadStatus_80375EC8 core;
    PlatPadStatus_80375ED4 freestyle;
    PlatPadStatus_80375EE0 classic;
};

struct PlatPadManager;

extern "C"
{
    extern PlatPadManager* g_pPlatPadManager;
    PlatPadStatus_80375EC8* fn_80375EC8(PlatPadManager* manager, int channel);
    PlatPadStatus_80375ED4* fn_80375ED4(PlatPadManager* manager, int channel);
    PlatPadStatus_80375EE0* fn_80375EE0(PlatPadManager* manager, int channel);
}

#endif // NL_PLAT_PLATPADSTATUS_H

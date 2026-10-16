#include <dwc/dwc_init.h>

#include <dwc/dwc_auth_interface.h>
#include <dwc/dwc_cf.h>
#include <dwc/dwc_nonport.h>
#include <dwc/dwc_report.h>
#include <dwc/dwci_memfunc.h>
#include <gamespy/common/gsMemory.h>
#include <gamespy/common/gsPlatformSocket.h>
#include <gamespy/gstats/gstats.h>
#include <revolution/os/OS.h>
#include <string.h>

static u32 dwci_gamecode = 0;
static BOOL s_initialized = FALSE;

int DWC_Init(DWC_AuthServer authSvr, const char* gameName, u32 gameCode,
             DWCAllocEx allocator, DWCFreeEx freeer) {
  OSRegisterVersion(
      "<< RVL_SDK - DWC \trelease build: May 14 2009 19:45:15 (0x4302_145) >>");

  DWCi_SetMemFunc(allocator, freeer);
  if (!DWCi_Auth_InitInterface(authSvr)) {
    DWC_Printf(8, "Failed to initialize auth interface.\n");
    return -1;
  }

  dwci_gamecode = gameCode;
  gsiMemoryCallbacksSet(&DWCi_GsMalloc, &DWCi_GsFree, &DWCi_GsRealloc,
                        &DWCi_GsMemalign);
  strcpy(gcd_gamename, gameName);

  if (authSvr == DWC_SVR_DEV) {
    strcpy(StatsServerHostname, "sdkdev.gamespy.com");
  } else {
    strcpy(StatsServerHostname, "gamestats.gs.nintendowifi.net");
  }

  s_initialized = TRUE;
  DWCi_Np_GetConsoleId();
  return 0;
}

void DWC_Shutdown() {
  gsiMemoryCallbacksSet(&DWCi_GsMalloc, &DWCi_GsFree, &DWCi_GsRealloc,
                        &DWCi_GsMemalign);
  gethostbyname("clear");
  DWCi_CfReset();
  s_initialized = FALSE;
}

u32 DWCi_GetGamecode() {
  return dwci_gamecode;
}

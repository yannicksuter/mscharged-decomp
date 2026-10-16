#include <dwc/dwc_report.h>

#include <revolution/types.h>
#include <stdarg.h>
#if defined(VERSION_R4QK01)
#include <revolution/os/OSError.h>
#define DWC_REPORT_PRINTF OSReport
#else
#include <stdio.h>
#define DWC_REPORT_PRINTF printf
#endif

static u32 sReportLevel = 0;

void DWC_SetReportLevel(u32 level) {
    sReportLevel = level;
}

void DWC_Printf(u32 id, const char* format, ...) {
    va_list args;
    if ((id & sReportLevel) == 0) return;
    switch (id) {
        case 0x1:   DWC_REPORT_PRINTF("DWC_INFO     :"); break;
        case 0x2:   DWC_REPORT_PRINTF("++DWC_ERROR  :"); break;
        case 0x4:   DWC_REPORT_PRINTF("DWC_DEBUG    :"); break;
        case 0x8:   DWC_REPORT_PRINTF("DWC_WARN     :"); break;
        case 0x10:  DWC_REPORT_PRINTF("DWC_ACHECK   :"); break;
        case 0x20:  DWC_REPORT_PRINTF("DWC_LOGIN    :"); break;
        case 0x40:  DWC_REPORT_PRINTF("DWC_MATCH_NN :"); break;
        case 0x80:  DWC_REPORT_PRINTF("DWC_MATCH_GT2:"); break;
        case 0x100: DWC_REPORT_PRINTF("DWC_TRANSPORT:"); break;
        case 0x200: DWC_REPORT_PRINTF("DWC_QR2_REQ  :"); break;
        case 0x400: DWC_REPORT_PRINTF("DWC_SB_UPDATE:"); break;
        case 0x8000:DWC_REPORT_PRINTF("DWC_SEND     :"); break;
        case 0x10000:   DWC_REPORT_PRINTF("DWC_RECV     :"); break;
        case 0x20000:   DWC_REPORT_PRINTF("DWC_UPDATE_SV:"); break;
        case 0x40000:   DWC_REPORT_PRINTF("DWC_CONNECTINET:"); break;
        case 0x1000000: DWC_REPORT_PRINTF("DWC_AUTH     :"); break;
        case 0x2000000: DWC_REPORT_PRINTF("DWC_AC       :"); break;
        case 0x4000000: DWC_REPORT_PRINTF("DWC_BM       :"); break;
        case 0x8000000: DWC_REPORT_PRINTF("DWC_UTIL     :"); break;
        case 0x10000000:DWC_REPORT_PRINTF("DWC_OPTION_CF:"); break;
#if defined(VERSION_R4QK01)
        case 0x20000000:DWC_REPORT_PRINTF("DWC_OPTION_CONNTEST:"); break;
#endif
        case 0x80000000:DWC_REPORT_PRINTF("DWC_GAMESPY  :"); break;

        default: DWC_REPORT_PRINTF("DWC_UNKNOWN  :"); break;
    }
    va_start(args, format);
#if defined(VERSION_R4QK01)
    OSVReport(format, args);
#else
    vprintf(format, args);
#endif
    va_end(args);
}

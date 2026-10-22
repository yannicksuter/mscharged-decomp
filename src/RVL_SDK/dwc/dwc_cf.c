#include <dwc/dwc_cf.h>
#include <dwc/dwc_memfunc.h>

typedef struct DWCiCfEntry {
    u8 unknown[0x18];
    struct DWCiCfEntry* next;
} DWCiCfEntry;

static DWCiCfEntry* cfList = NULL;

int DWCi_CfReset(void) {
    while (cfList != NULL) {
        DWCiCfEntry* next = cfList->next;
        DWC_Free(DWC_ALLOCTYPE_OPTION_CF, cfList, 0);
        cfList = next;
    }

    return 0;
}

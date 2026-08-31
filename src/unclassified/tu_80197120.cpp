#include "NL/nlSlotPool.h"

#define DEFINE_EVENT_POOL(address, size)       \
    struct UnidentifiedEventPoolData_##address \
    {                                          \
        unsigned char data[size];              \
    };                                         \
    extern "C" SlotPool<UnidentifiedEventPoolData_##address> lbl_##address(16, 16)

DEFINE_EVENT_POOL(80571258, 0x20);
DEFINE_EVENT_POOL(80571280, 0x08);
DEFINE_EVENT_POOL(805712A8, 0x08);
DEFINE_EVENT_POOL(805712D0, 0x0C);
DEFINE_EVENT_POOL(805712F8, 0x08);
DEFINE_EVENT_POOL(80571320, 0x08);
DEFINE_EVENT_POOL(80571348, 0x1C);
DEFINE_EVENT_POOL(80571370, 0x18);
DEFINE_EVENT_POOL(80571398, 0x10);
DEFINE_EVENT_POOL(805713C0, 0x14);
DEFINE_EVENT_POOL(805713E8, 0x10);
DEFINE_EVENT_POOL(80571410, 0x24);
DEFINE_EVENT_POOL(80571438, 0x14);
DEFINE_EVENT_POOL(80571460, 0x14);
DEFINE_EVENT_POOL(80571488, 0x24);
DEFINE_EVENT_POOL(805714B0, 0x24);
DEFINE_EVENT_POOL(805714D8, 0x18);
DEFINE_EVENT_POOL(80571500, 0x08);
DEFINE_EVENT_POOL(80571528, 0x08);
DEFINE_EVENT_POOL(80571550, 0x08);
DEFINE_EVENT_POOL(80571578, 0x08);
DEFINE_EVENT_POOL(805715A0, 0x08);
DEFINE_EVENT_POOL(805715C8, 0x08);
DEFINE_EVENT_POOL(805715F0, 0x08);
DEFINE_EVENT_POOL(80571618, 0x14);
DEFINE_EVENT_POOL(80571640, 0x08);
DEFINE_EVENT_POOL(80571668, 0x28);
DEFINE_EVENT_POOL(80571690, 0x10);
DEFINE_EVENT_POOL(805716B8, 0x18);
DEFINE_EVENT_POOL(805716E0, 0x1C);
DEFINE_EVENT_POOL(80571708, 0x1C);
DEFINE_EVENT_POOL(80571730, 0x10);
DEFINE_EVENT_POOL(80571758, 0x1C);
DEFINE_EVENT_POOL(80571780, 0x10);
DEFINE_EVENT_POOL(805717A8, 0x08);
DEFINE_EVENT_POOL(805717D0, 0x10);
DEFINE_EVENT_POOL(805717F8, 0x20);
DEFINE_EVENT_POOL(80571820, 0x04);
DEFINE_EVENT_POOL(80571848, 0x14);
DEFINE_EVENT_POOL(80571870, 0x0C);
DEFINE_EVENT_POOL(80571898, 0x08);
DEFINE_EVENT_POOL(805718C0, 0x04);
DEFINE_EVENT_POOL(805718E8, 0x04);
DEFINE_EVENT_POOL(80571910, 0x08);
DEFINE_EVENT_POOL(80571938, 0x24);
DEFINE_EVENT_POOL(80571960, 0x14);
DEFINE_EVENT_POOL(80571988, 0x08);
DEFINE_EVENT_POOL(805719B0, 0x0C);
DEFINE_EVENT_POOL(805719D8, 0x0C);
DEFINE_EVENT_POOL(80571A00, 0x04);

#undef DEFINE_EVENT_POOL

extern "C" void fn_80197120()
{
    lbl_80571258.FreeBlocks();
    lbl_80571280.FreeBlocks();
    lbl_805712A8.FreeBlocks();
    lbl_805712D0.FreeBlocks();
    lbl_805712F8.FreeBlocks();
    lbl_80571320.FreeBlocks();
    lbl_80571348.FreeBlocks();
    lbl_80571370.FreeBlocks();
    lbl_80571398.FreeBlocks();
    lbl_805713C0.FreeBlocks();
    lbl_805713E8.FreeBlocks();
    lbl_80571410.FreeBlocks();
    lbl_80571438.FreeBlocks();
    lbl_80571460.FreeBlocks();
    lbl_80571488.FreeBlocks();
    lbl_805714B0.FreeBlocks();
    lbl_805714D8.FreeBlocks();
    lbl_80571500.FreeBlocks();
    lbl_80571528.FreeBlocks();
    lbl_80571550.FreeBlocks();
    lbl_80571578.FreeBlocks();
    lbl_805715A0.FreeBlocks();
    lbl_805715C8.FreeBlocks();
    lbl_805715F0.FreeBlocks();
    lbl_80571618.FreeBlocks();
    lbl_80571640.FreeBlocks();
    lbl_80571668.FreeBlocks();
    lbl_80571690.FreeBlocks();
    lbl_805716B8.FreeBlocks();
    lbl_805716E0.FreeBlocks();
    lbl_80571708.FreeBlocks();
    lbl_80571730.FreeBlocks();
    lbl_80571758.FreeBlocks();
    lbl_80571780.FreeBlocks();
    lbl_805717A8.FreeBlocks();
    lbl_805717D0.FreeBlocks();
    lbl_805717F8.FreeBlocks();
    lbl_80571820.FreeBlocks();
    lbl_80571848.FreeBlocks();
    lbl_80571870.FreeBlocks();
    lbl_80571898.FreeBlocks();
    lbl_805718C0.FreeBlocks();
    lbl_805718E8.FreeBlocks();
    lbl_80571910.FreeBlocks();
    lbl_80571938.FreeBlocks();
    lbl_80571960.FreeBlocks();
    lbl_80571988.FreeBlocks();
    lbl_805719B0.FreeBlocks();
    lbl_805719D8.FreeBlocks();
    lbl_80571A00.FreeBlocks();
}

#include "revolution/hbm/nw4hbm/snd/NandSoundArchive.h"

#include "revolution/hbm/HBMAssert.hpp"

namespace nw4hbm
{
namespace snd
{

void NandSoundArchive::Close()
{
    if (mOpen)
    {
        s32 result = NANDClose(&mFileInfo);
        NW4HBMCheckMessage_Line(result == NAND_RESULT_OK, 141, "Failed close mcs file. ErrID=%d\n", result);
        mOpen = false;
    }

    Shutdown();
}

} // namespace snd
} // namespace nw4hbm

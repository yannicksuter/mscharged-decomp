#include "revolution/hbm/nw4hbm/snd/DvdSoundArchive.h"

namespace nw4hbm
{
namespace snd
{

void DvdSoundArchive::Close()
{
    DVDClose(&mFileInfo);
    mOpen = false;
    Shutdown();
}

} // namespace snd
} // namespace nw4hbm

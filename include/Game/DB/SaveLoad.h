#ifndef GAME_DB_SAVE_LOAD_H
#define GAME_DB_SAVE_LOAD_H

#include "types.h"

struct SaveFileHeader
{
    /* 0x0 */ u32 Size;
    /* 0x4 */ u32 CRC;
}; // size 0x8

void LoadMemoryCardIconData();
extern bool SaveEnabled;
extern bool SaveError;
extern bool NormalSaveLoaded;
extern bool InOperation;

namespace SaveLoad
{
inline bool CardBusy()
{
    return InOperation;
}
void StartSave(bool online);
void StartLoad(bool online);
u32 GetSaveBlockSize();
void AllocateBannerBuffer();
void FreeBannerBuffer();
} // namespace SaveLoad

#endif // GAME_DB_SAVE_LOAD_H

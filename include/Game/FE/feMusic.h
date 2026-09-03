#ifndef _FEMUSIC_H_
#define _FEMUSIC_H_

#include "types.h"

namespace FEMusic
{
void SetUnidentifiedMode_801FC29C(bool value);
void SetEnabled_801FC2A4(bool value);
bool IsEnabled_801FC2AC();
void StartStreamIfDifferent(int idx);
void StopStream();
} // namespace FEMusic

#endif // _FEMUSIC_H_

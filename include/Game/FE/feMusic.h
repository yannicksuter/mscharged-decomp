#ifndef _FEMUSIC_H_
#define _FEMUSIC_H_

namespace FEMusic
{
void SetInGame(bool value);
void SetEnabled(bool value);
bool IsEnabled();
void StartStreamIfDifferent(int idx);
void StopStream();
void PauseStream();
void ResumeStream();
bool IsPlayingCupResultStream();
} // namespace FEMusic

#endif // _FEMUSIC_H_

#ifndef GAME_SYS_MOVIE_H
#define GAME_SYS_MOVIE_H

bool MoviePlay();
bool MovieStop();
bool MovieStart(const char* szFilename, bool bSound, bool bLoopMovie, bool bMono);

bool MovieInit();
bool MovieQuit();
void SetSyncedDecode(bool value);
void MovieRenderTick();
bool IsMovieActive();
bool IsMovieFinished();
void ClearMovieFinished();
unsigned int GetMovieFrame();

#endif // GAME_SYS_MOVIE_H

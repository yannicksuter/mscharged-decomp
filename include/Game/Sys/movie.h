#ifndef GAME_SYS_MOVIE_H
#define GAME_SYS_MOVIE_H

bool MoviePlay();
bool MovieStop();
#if defined(VERSION_R4QP01_REV1)
bool MovieStart(const char* szFilename, bool bSound, bool bLoopMovie);
#else
bool MovieStart(const char* szFilename, bool bSound, bool bLoopMovie, bool bMono);
#endif

bool MovieInit();
bool MovieQuit();
void SetSyncedDecode(bool value);
void MovieRenderTick();
bool IsMovieActive();
bool IsMovieFinished();
void ClearMovieFinished();
unsigned int GetMovieFrame();

#endif // GAME_SYS_MOVIE_H

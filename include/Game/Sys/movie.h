#ifndef GAME_SYS_MOVIE_H
#define GAME_SYS_MOVIE_H

bool MoviePlay();
bool MovieStop();
bool MovieStart(const char* szFilename, bool bSound, bool bLoopMovie, bool bMono);

bool fn_80370E20();
bool fn_80370E64();
void SetSyncedDecode(bool value);
void fn_80371254();
bool IsMovieActive();
bool IsMovieFinished();
void ClearMovieFinished();
unsigned int GetMovieFrame();

#endif // GAME_SYS_MOVIE_H

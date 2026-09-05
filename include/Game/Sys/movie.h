#ifndef GAME_SYS_MOVIE_H
#define GAME_SYS_MOVIE_H

bool MoviePlay();
bool MovieStop();
bool MovieStart(const char* szFilename, bool bSound, bool bLoopMovie, bool bMono);

bool fn_80370E20();
bool fn_80370E64();
void fn_80370E90(bool value);
void fn_80371254();
bool fn_803713C4();
bool fn_803713CC();
void fn_803713D4();
unsigned int fn_803713E0();

#endif // GAME_SYS_MOVIE_H

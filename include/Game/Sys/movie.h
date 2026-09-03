#ifndef GAME_SYS_MOVIE_H
#define GAME_SYS_MOVIE_H

bool MoviePlay();
bool MovieStop();
bool MovieStart(const char* szFilename, bool bSound, bool bLoopMovie, bool bMono);

extern "C" bool fn_80370E20();
extern "C" bool fn_80370E64();
extern "C" void fn_80370E90(bool value);
extern "C" void fn_80371254();
extern "C" bool fn_803713C4();
extern "C" bool fn_803713CC();
extern "C" void fn_803713D4();
extern "C" unsigned int fn_803713E0();

#endif // GAME_SYS_MOVIE_H

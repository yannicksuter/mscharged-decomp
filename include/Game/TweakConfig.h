#ifndef GAME_TWEAK_CONFIG_H
#define GAME_TWEAK_CONFIG_H

void LoadTweakConfigFile(const char* fileName, const char* category, bool reload);
void LoadTweakConfigBuffer(void* context, char* data, unsigned long size, const char* category);

#endif // GAME_TWEAK_CONFIG_H

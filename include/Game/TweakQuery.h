#ifndef GAME_TWEAK_QUERY_H
#define GAME_TWEAK_QUERY_H

float GetTweakFloat(const char* path, float defaultValue);
int GetTweakInt(const char* path, int defaultValue);
bool GetTweakBool(const char* path, bool defaultValue);
const char* GetTweakString(const char* path, const char* defaultValue);
int TweakExists(const char* path);

#endif // GAME_TWEAK_QUERY_H

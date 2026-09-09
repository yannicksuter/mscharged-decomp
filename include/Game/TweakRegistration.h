#ifndef GAME_TWEAK_REGISTRATION_H
#define GAME_TWEAK_REGISTRATION_H

class TweakEntry;
class TweakNode;
class TweakValueBase;
struct TweakPendingValue;

void InitializeTweakRegistry(int fromEnd, unsigned char pushState,
    unsigned int* stringPoolSizes);
void ResetDynamicTweaks();

int IsTweakRegistryInitialized(void);
TweakEntry* GetTweakRoot(void);
void QueueTweakValue(TweakPendingValue*, TweakValueBase*, const char*);
TweakEntry* FindOrCreateTweakPath(TweakEntry*, const char*, int);
void AddTweakValue(TweakEntry*, TweakValueBase*);
TweakNode* FindTweakChild(TweakEntry*, const char*);

#endif // GAME_TWEAK_REGISTRATION_H

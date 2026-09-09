#ifndef GAME_RENDER_STADIUM_LOADING_H
#define GAME_RENDER_STADIUM_LOADING_H

class BasicStadium;
class DrawableObject;
struct glModel;

struct StadiumModelEntry
{
    /* 0x00 */ int mType;
    /* 0x04 */ const char* mResourceName;
    /* 0x08 */ int mNumInstances;
    /* 0x0C */ DrawableObject** mInstances;
    /* 0x10 */ int mCaptain;
    /* 0x14 */ int mSidekick;
    /* 0x18 */ int mStadium;
};

struct StadiumLoadResult
{
    void* mData;
    unsigned long mSize;
    bool mProcessed;
};

void BeginLoadStadium(const char* path, bool skipGameplayModels);
bool IsStadiumResourceDataLoaded();
void BeginLoadStadiumTemporaryResources();
void SetStadiumBannerTextures();
void SetWorldNPCsVisible(bool visible);
bool FinishLoadStadiumResources();
void BeginLoadStadiumEffects();
bool FinishLoadStadiumEffects();
void DestroyStadium();
void UpdateStadium(float fDeltaT);
bool IsStadiumWorldLoaded();

void BeginLoadTournamentTrophy();
bool IsTournamentTrophyLoaded();
void FinishLoadTournamentTrophy();

DrawableObject* FindStadiumDrawableObject(unsigned long uHashID);
DrawableObject* GetBallRenderObject(unsigned int index);
bool ShouldLoadStadiumModel(const StadiumModelEntry* entry);
bool CreateStadiumModelInstances(int index, glModel* models, unsigned long numModels);
bool CreatePowerupDrawables(glModel* models, unsigned long numModels);
void OnStadiumResourceLoaded(void* data, unsigned long size, void* userData);
void OnStadiumTemporaryResourceLoaded(void* data, unsigned long size, void* userData);
void OnStadiumModelResourceLoaded(void* data, unsigned long size, void* userData);
void OnStadiumEffectsLoaded(void* data, unsigned long size, void* userData);

extern BasicStadium* pBasicStadiumInstance;
extern StadiumModelEntry gStadiumModelEntries[22];
extern char gStadiumName[32];
extern char gStadiumResourcePath[256];
extern bool gSkipGameplayModels;
extern void* gStadiumResourceData;
extern unsigned long gStadiumResourceDataSize;
extern bool gStadiumResourceDataLoaded;
extern void* gStadiumTemporaryData;
extern unsigned long gStadiumTemporaryDataSize;
extern bool gStadiumWorldLoaded;
extern void* gStadiumEffectsData;
extern unsigned int gStadiumEffectsRequest;
extern void* gStadiumNonResidentEffectsData;
extern bool gStadiumNonResidentEffectsRequested;
extern void* gStadiumLoadBuffers[2];
extern StadiumLoadResult gStadiumModelLoadResults[2][22];

#endif // GAME_RENDER_STADIUM_LOADING_H

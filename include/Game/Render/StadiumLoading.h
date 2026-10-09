#ifndef GAME_RENDER_STADIUM_LOADING_H
#define GAME_RENDER_STADIUM_LOADING_H

class BasicStadium;
class DrawableObject;
struct glModel;
class nlVector3;
class ImpostorModel;

enum eStadiumModelID
{
    STADIUM_MODEL_BALL = 0,
    STADIUM_MODEL_BULLET_BILL = 1,
    STADIUM_MODEL_HAMMER = 2,
    STADIUM_MODEL_YOSHI_EGG = 3,
    STADIUM_MODEL_BIRDO_EGG = 4,
    STADIUM_MODEL_KOOPA_SHELL = 5,
    STADIUM_MODEL_DAISY_FIST = 6,
    STADIUM_MODEL_FLYING_CAMERA = 7,
    STADIUM_MODEL_THWOMP = 8,
    STADIUM_MODEL_NUMBER_0 = 9,
    STADIUM_MODEL_NUMBER_1 = 10,
    STADIUM_MODEL_NUMBER_2 = 11,
    STADIUM_MODEL_NUMBER_3 = 12,
    STADIUM_MODEL_NUMBER_4 = 13,
    STADIUM_MODEL_NUMBER_5 = 14,
    STADIUM_MODEL_NUMBER_6 = 15,
    STADIUM_MODEL_NUMBER_7 = 16,
    STADIUM_MODEL_NUMBER_8 = 17,
    STADIUM_MODEL_NUMBER_9 = 18,
    STADIUM_MODEL_NUMBER_DASH = 19,
    STADIUM_MODEL_NUMBER_COLON = 20,
    STADIUM_MODEL_POWERUPS = 21,
};

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

char* fn_802772C4();
void fn_802772D0(const char* name, bool stadiumViewer);

void BeginLoadStadium(const char* path, bool skipGameplayModels);
bool FinishLoadStadium(bool stadiumViewer);
void StadiumScreenToWorldPosition(nlVector3& result, float screenX, float screenY, float distance);
bool ShouldRenderStadiumNPC(ImpostorModel* model);
bool IsStadiumResourceDataLoaded();
void BeginLoadStadiumTemporaryResources();
void SetStadiumBannerTextures();
void SetWorldNPCsVisible(bool visible);
bool FinishLoadStadiumResources();
void BeginLoadStadiumEffects();
bool FinishLoadStadiumEffects();
void DestroyStadium();
void UpdateStadium(float fDeltaT);
void fn_80277BB0();
bool IsStadiumWorldLoaded();
float GetStadiumTime();

void BeginLoadTournamentTrophy();
bool IsTournamentTrophyLoaded();
void FinishLoadTournamentTrophy();

DrawableObject* FindStadiumDrawableObject(unsigned long uHashID);
void fn_802772A4(DrawableObject* pObject);
DrawableObject* GetBallRenderObject(unsigned int index);
DrawableObject** GetNumberRenderObjects();
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

extern "C" bool gDisableHighRange;

extern "C" bool lbl_806E1960;

#endif // GAME_RENDER_STADIUM_LOADING_H

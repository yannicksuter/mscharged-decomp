#ifndef GAME_RENDER_IMPOSTOR_MANAGER_H
#define GAME_RENDER_IMPOSTOR_MANAGER_H

#include "NL/nlDLListContainer.h"
#include "types.h"

class Impostor;
class ImpostorCharacter;

class GLView;
class GLResourcePool;
class nlVector3;
struct GLMemoryRequirement;

class ImpostorManager
{
public:
    ImpostorManager();
    ~ImpostorManager();

    static ImpostorManager* GetInstance();
    static void SetSpritesInvalid();
    static float GetImpostorSizeScale();
    static void SetImpostorSizeScale(float scale);

    void Initialize(GLView* registry, int capacity,
        const GLMemoryRequirement* config, int numRequirements, bool flag);
    void Uninitialize();
    void ResetImpostors();
    void ResetSpriteSlots();
    void InvalidateCapture();
    Impostor* AllocImpostor(int* outIndex);
    int GetNumImpostors();
    void Render(void* target, bool skipCapture);
    void AddCharacter(ImpostorCharacter* character);
    void UpdateCharacters(float dt, const char* unidentified);
    void UpdateAnimations(float dt);
    void UpdateSprites();
    void UpdatePositions(const nlVector3* direction, const nlVector3* up);
    void StaggerAnimations();
    void SetEnabled(bool enable);
    void SetUpdatePeriod(int period);
    u32 CalculateRenderChecksum();

    /* 0x00 */ u8 mEnabled;
    /* 0x01 */ u8 mUnidentified001[3];
    /* 0x04 */ Impostor* mImpostors;
    /* 0x08 */ int mNumUsed;
    /* 0x0C */ int mCapacity;
    /* 0x10 */ void* mUnidentified010;
    /* 0x14 */ nlDLListSlotPool<ImpostorCharacter*> mCharacters;
    /* 0x30 */ GLView* mParentView;
    /* 0x34 */ u8 mInitialized;
    /* 0x35 */ u8 mUnidentified035;
    /* 0x36 */ u8 mUnidentified036;
    /* 0x37 */ u8 mUnidentified037;
    /* 0x38 */ GLResourcePool* mResources[2];
    /* 0x40 */ unsigned long mResourceMarkers[2];
    /* 0x48 */ int mCurrentResource;
    /* 0x4C */ u8 mUnidentified04C;
    /* 0x4D */ u8 mUnidentified04D[3];
    /* 0x50 */ u32 mLastRenderChecksum;
    /* 0x54 */ int mFrameCount;
    /* 0x58 */ u8 mCaptured;
    /* 0x59 */ u8 mUnidentified059;
}; // size: 0x5C

#endif // GAME_RENDER_IMPOSTOR_MANAGER_H

#ifndef GAME_RENDER_IMPOSTOR_CHARACTER_H
#define GAME_RENDER_IMPOSTOR_CHARACTER_H

#include "Game/Render/ImpostorModel.h"
#include "Game/SAnim.h"
#include "Game/TweakValue.h"
#include "NL/gl/glTarget.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlList.h"
#include "NL/nlMath.h"
#include "types.h"

class AnimRetarget;
class cPN_SAnimController;
class cPoseAccumulator;
class cPoseNode;
class cSAnim;
class cSHierarchy;
template <typename T>
class cInventory;
class GLView;
class GLSkinMesh;
class Impostor;
class ImpostorCharacter;
class GLResourcePool;
class ImpostorView;
struct glModel;
class GLCompactColourMeshWriter;

#include "Game/Render/ImpostorSprite.h"

u16 QuantizeImpostorAngle(u16 target, int count);

struct ImpostorCharacterParams
{
    /* 0x00 */ int mWidth;
    /* 0x04 */ int mHeight;
    /* 0x08 */ u8 mUnidentified008;
    /* 0x09 */ u8 mUseIntensityAlpha;
    /* 0x0A */ u16 mBaseAngle;
}; // size: 0x0C

class ImpostorCharacter
{
public:
    ImpostorCharacter(const char* name, int budget, int numAngles,
        int numTextures, const ImpostorCharacterParams* params);
    ~ImpostorCharacter();

    // These four are inline in the header: the retail vtable for
    // ImpostorCharacter is emitted by this class's translation unit, which is
    // only possible when they are skipped as CodeWarrior's key function. Their
    // retained out-of-line copies live at 0x801A505C..0x801A508C.
    virtual void SetScale(float scale) { mfScale = scale; }
    virtual float GetScale() { return mfScale; }
    virtual float GetCameraDistance() { return mfCameraDistance; }
    virtual float GetCameraLookatZ() { return mfCameraLookatZ; }
    virtual void SetAnimationTime(int index, float phase) = 0;
    virtual void EvaluatePose(int texture) = 0;
    virtual void Render(GLView* target, int texture) = 0;
    virtual void UpdateAnimation(float dt) = 0;
    virtual void PlayAnimation(
        float dt, const char* unidentified) = 0;
    virtual void UpdateView(const nlVector3* direction, const nlVector3* up);

    void Acquire(Impostor* impostor);
    void EnableSprites(bool enable);
    void ReleaseSprites();
    void RegisterSprites(GLView* registry);
    void UpdateSprites(int period, int slot);

    /* 0x04 */ int mNumAngles;
    /* 0x08 */ int mNumTextures;
    /* 0x0C */ u8 mUnidentified00C;
    /* 0x0D */ u8 mUnidentified00D[3];
    /* 0x10 */ nlDLListSlotPool<ImpostorSprite*> mSprites;
    /* 0x2C */ int mWidth;
    /* 0x30 */ int mHeight;
    /* 0x34 */ u8 mUnidentified034;
    /* 0x35 */ u8 mUseIntensityAlpha;
    /* 0x36 */ u16 mBaseAngle;
    /* 0x38 */ const char* mName;
    /* 0x3C */ TweakFloatBinding mfScale;
    /* 0x4C */ TweakFloatBinding mfCameraLookatZ;
    /* 0x5C */ TweakFloatBinding mfCameraDistance;
}; // size: 0x6C

// One animated model per texture set.
class AnimatedImpostorCharacter : public ImpostorCharacter
{
public:
    AnimatedImpostorCharacter(const char* name,
        ImpostorModel* model, void* animations, int budget,
        int numAngles, int numTextures, const ImpostorCharacterParams* params);
    virtual ~AnimatedImpostorCharacter();

    virtual void SetAnimationTime(int index, float phase);
    virtual void EvaluatePose(int texture);
    virtual void Render(GLView* target, int texture);
    virtual void UpdateAnimation(float dt);
    virtual void PlayAnimation(float dt, const char* unidentified);

    /* 0x6C */ ImpostorModel** mModels;
    /* 0x70 */ int mNumModels;
}; // size: 0x74

// Captures the crowd as a single sprite.
class ImpostorCluster : public ImpostorCharacter
{
public:
    ImpostorCluster(const char* name, int budget,
        const ImpostorCharacterParams* params);

    virtual void SetAnimationTime(int index, float phase);
    virtual void EvaluatePose(int texture);
    virtual void Render(GLView* target, int texture);
    virtual void UpdateAnimation(float dt);
    virtual void PlayAnimation(
        float dt, const char* unidentified);
    virtual void UpdateView(
        const nlVector3* direction, const nlVector3* up);

    unsigned long GetTexture();

    /* 0x6C */ const char* mName;
}; // size: 0x70


#endif // GAME_RENDER_IMPOSTOR_CHARACTER_H

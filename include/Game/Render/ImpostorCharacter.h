#ifndef GAME_RENDER_IMPOSTOR_CHARACTER_H
#define GAME_RENDER_IMPOSTOR_CHARACTER_H

#include "Game/TweakValue.h"
#include "NL/nlDLListContainer.h"
#include "types.h"

class Impostor;
class ImpostorCharacter;
struct UnidentifiedMesh_802D7AEC;

// Sprite-mesh unit at 0x802D4290; its own translation unit is not yet
// reconstructed, so it stays an address-named placeholder with the members
// the impostor units are proven to touch.
class ImpostorSprite_802D4290
{
public:
    ImpostorSprite_802D4290(ImpostorCharacter* character, int texture,
        int budget, int width, int height);
    ~ImpostorSprite_802D4290();

    /* 0x00 */ u8 mUnidentified000[0x44];
    /* 0x44 */ int mUnidentified044;
    /* 0x48 */ int mUnidentified048;
    /* 0x4C */ u8 mUnidentified04C[0x8];
    /* 0x54 */ void* mUnidentified054;
    /* 0x58 */ u8 mUnidentified058[0x10];
    /* 0x68 */ UnidentifiedMesh_802D7AEC* mUnidentified068;
    /* 0x6C */ u8 mUnidentified06C[0xC];
    /* 0x78 */ u8 mUnidentified078;
    /* 0x79 */ u8 mUnidentified079;
    /* 0x7A */ u16 mAngle;
    /* 0x7C */ u8 mUnidentified07C[0x8];
    /* 0x84 */ int mUnidentified084;
    /* 0x88 */ u8 mUnidentified088;
}; // size: 0x8C

// Per-view animated model instance owned by the 0x802DAEE0 unit.
class ImpostorModel_802DAEE0
{
public:
    virtual ~ImpostorModel_802DAEE0();
    virtual void UnidentifiedVirtual0C(void*, int) = 0;
};

// Object referenced through the sprite's 0x68 pointer; only the registry
// back-pointer at 0x54 is proven.
struct UnidentifiedMesh_802D7AEC
{
    /* 0x00 */ u8 mUnidentified000[0x54];
    /* 0x54 */ void* mUnidentified054;
};

struct UnidentifiedRegistryNode_802D7AEC
{
    /* 0x00 */ UnidentifiedRegistryNode_802D7AEC* mNext;
    /* 0x04 */ void* mTarget;
}; // size: 0x08

struct UnidentifiedRegistry_802D7AEC
{
    /* 0x00 */ u8 mUnidentified000[8];
    /* 0x08 */ UnidentifiedRegistryNode_802D7AEC* mHead;
    /* 0x0C */ UnidentifiedRegistryNode_802D7AEC* mTail;
};

struct ImpostorCharacterParams
{
    /* 0x00 */ int mWidth;
    /* 0x04 */ int mHeight;
    /* 0x08 */ u8 mUnidentified008;
    /* 0x09 */ u8 mUnidentified009;
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
    virtual void UnidentifiedVirtual18(int index, float phase) = 0;
    virtual void UnidentifiedVirtual1C(int texture) = 0;
    virtual void UnidentifiedVirtual20(void* target, int texture) = 0;
    virtual void UnidentifiedVirtual24(float dt) = 0;
    virtual void UnidentifiedVirtual28(
        float dt, const char* unidentified) = 0;
    virtual void UnidentifiedVirtual2C(void* unidentified0, void* unidentified1);

    void Acquire(Impostor* impostor);
    void EnableSprites(bool enable);
    void ReleaseSprites();
    void RegisterSprites(void* registry);
    void UpdateSprites(int period, int slot);

    /* 0x04 */ int mNumAngles;
    /* 0x08 */ int mNumTextures;
    /* 0x0C */ u8 mUnidentified00C;
    /* 0x0D */ u8 mUnidentified00D[3];
    /* 0x10 */ nlDLListSlotPool<ImpostorSprite_802D4290*> mSprites;
    /* 0x2C */ int mWidth;
    /* 0x30 */ int mHeight;
    /* 0x34 */ u8 mUnidentified034;
    /* 0x35 */ u8 mUnidentified035;
    /* 0x36 */ u16 mBaseAngle;
    /* 0x38 */ const char* mName;
    /* 0x3C */ TweakValue_804F4DC8 mfScale;
    /* 0x4C */ TweakValue_804F4DC8 mfCameraLookatZ;
    /* 0x5C */ TweakValue_804F4DC8 mfCameraDistance;
}; // size: 0x6C

// Concrete implementation backed by one animated model per texture set; the
// class name is not recoverable from the stripped binary.
class ImpostorCharacterImpl_8052E9B8 : public ImpostorCharacter
{
public:
    ImpostorCharacterImpl_8052E9B8(const char* name,
        ImpostorModel_802DAEE0* model, void* animations, int budget,
        int numAngles, int numTextures, const ImpostorCharacterParams* params);
    virtual ~ImpostorCharacterImpl_8052E9B8();

    virtual void UnidentifiedVirtual18(int index, float phase);
    virtual void UnidentifiedVirtual1C(int texture);
    virtual void UnidentifiedVirtual20(void* target, int texture);
    virtual void UnidentifiedVirtual24(float dt);
    virtual void UnidentifiedVirtual28(float dt, const char* unidentified);

    /* 0x6C */ ImpostorModel_802DAEE0** mModels;
    /* 0x70 */ int mNumModels;
}; // size: 0x74

#endif // GAME_RENDER_IMPOSTOR_CHARACTER_H

#ifndef GAME_RENDER_IMPOSTOR_CHARACTER_H
#define GAME_RENDER_IMPOSTOR_CHARACTER_H

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
class ResourceInterface_802CC094;
class UnidentifiedImpostorView_802D4290;
struct glModel;
struct State_802A7C90;

// Sprite-mesh unit at 0x802D4290. The retail binary does not preserve the
// original class name, so the address-qualified placeholder is retained.
class ImpostorSprite_802D4290
{
public:
    ImpostorSprite_802D4290(ImpostorCharacter* character, int texture,
        int budget, int width, int height);
    ~ImpostorSprite_802D4290();

    /* 0x00 */ bool mUnidentified000;
    /* 0x01 */ char mName[0x40];
    /* 0x41 */ u8 mUnidentified041[3];
    /* 0x44 */ int mUnidentified044;
    /* 0x48 */ int mUnidentified048;
    /* 0x4C */ ImpostorCharacter* mUnidentified04C;
    /* 0x50 */ int mUnidentified050;
    /* 0x54 */ int mUnidentified054;
    /* 0x58 */ int* mUnidentified058;
    /* 0x5C */ int mUnidentified05C;
    /* 0x60 */ int mUnidentified060;
    /* 0x64 */ UnidentifiedImpostorView_802D4290* mUnidentified064;
    /* 0x68 */ GLView* mUnidentified068;
    /* 0x6C */ GLRenderPair mUnidentified06C;
    /* 0x74 */ State_802A7C90* mUnidentified074;
    /* 0x78 */ u8 mUnidentified078;
    /* 0x79 */ u8 mUnidentified079;
    /* 0x7A */ u16 mAngle;
    /* 0x7C */ float mUnidentified07C;
    /* 0x80 */ int* mUnidentified080;
    /* 0x84 */ int mUnidentified084;
    /* 0x88 */ u8 mUnidentified088;
    /* 0x89 */ u8 mUnidentified089[3];
}; // size: 0x8C

extern "C" void fn_802D4480(
    ImpostorSprite_802D4290* sprite, const char* name);
extern "C" void fn_802D4484(ImpostorSprite_802D4290* sprite,
    const nlVector3* direction, const nlVector3* up);
extern "C" void fn_802D47F8(ImpostorSprite_802D4290* sprite);
extern "C" void fn_802D4874(ImpostorSprite_802D4290* sprite);
extern "C" void fn_802D4898(ImpostorSprite_802D4290* sprite);
extern "C" void fn_802D48E4(
    ImpostorSprite_802D4290* sprite, const char* name);
extern "C" int fn_802D4AEC(ImpostorSprite_802D4290* sprite,
    GLView* target, Impostor* impostors, bool cached, bool skipCapture);
extern "C" void fn_802D5034(ImpostorSprite_802D4290* sprite);
extern "C" void fn_802D5040(ImpostorSprite_802D4290* sprite);
extern "C" void fn_802D5078(ImpostorSprite_802D4290* sprite, int slot);
extern "C" unsigned long fn_802D50A4(ImpostorSprite_802D4290* sprite);
extern "C" bool fn_802D50D8(ImpostorSprite_802D4290* sprite, int slot);
extern "C" void fn_802D5110(ImpostorSprite_802D4290* sprite);
extern "C" int fn_802D536C(ImpostorSprite_802D4290* sprite);

// Per-view animated model instance owned by the 0x802DAEE0 unit.
class ImpostorModel_802DAEE0
{
public:
    ImpostorModel_802DAEE0(cSHierarchy& hierarchy, unsigned long modelID,
        cInventory<cSAnim>* animations,
        ResourceInterface_802CC094* resource);
    ImpostorModel_802DAEE0(cSHierarchy& hierarchy, unsigned long modelID,
        ResourceInterface_802CC094* resource);
    virtual ~ImpostorModel_802DAEE0();
    virtual void UnidentifiedVirtual0C(
        GLView* opaqueView, GLView* translucentView);
    virtual void UnidentifiedVirtual10(float dt);
    virtual void UnidentifiedVirtual14(GLView* opaqueView,
        GLView* translucentView, const cPoseAccumulator& poseAccumulator,
        const nlMatrix4* worldMatrix);

    /* 0x04 */ nlMatrix4 mWorldMatrix;
    /* 0x44 */ cPN_SAnimController* mAnimController;
    /* 0x48 */ cPoseAccumulator* mPoseAccumulator;
    /* 0x4C */ GLSkinMesh* mSkinMesh;
    /* 0x50 */ cPoseNode* mPoseTree;
    /* 0x54 */ unsigned long mModelID;
    /* 0x58 */ cSHierarchy* mHierarchy;
    /* 0x5C */ bool mVisible;
    /* 0x5D */ bool mTexturesResolved;
    /* 0x5E */ u8 mPadding05E[2];
    /* 0x60 */ glModel* mLastModel;
    /* 0x64 */ cInventory<cSAnim>* mAnimations;
    /* 0x68 */ unsigned long mOriginalTexture;
    /* 0x6C */ unsigned long mReplacementTexture;
    /* 0x70 */ unsigned long mResolvedTexture;
    /* 0x74 */ void (*mModelCallback)(
        ImpostorModel_802DAEE0*, glModel*);
}; // size: 0x78

extern "C" ImpostorModel_802DAEE0* fn_802DB0AC(
    const ImpostorModel_802DAEE0* model,
    ResourceInterface_802CC094* resource);
extern "C" void fn_802DB22C(
    ImpostorModel_802DAEE0* model, float dt);
extern "C" void fn_802DB26C(ImpostorModel_802DAEE0* model);
extern "C" void fn_802DB2B8(
    ImpostorModel_802DAEE0* model, float time);
extern "C" void fn_802DB4EC(
    ImpostorModel_802DAEE0* model, unsigned long texture);
extern "C" void fn_802DB528(ImpostorModel_802DAEE0* model,
    const char* name, float blendTime, ePlayMode playMode);
extern "C" void fn_802DB6CC(ImpostorModel_802DAEE0* model,
    cSAnim& anim, ePlayMode playMode, const AnimRetarget* retarget);
extern "C" void fn_802DB79C(ImpostorModel_802DAEE0* model);

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
    /* 0x3C */ TweakValueImpl_804F4DC8 mfScale;
    /* 0x4C */ TweakValueImpl_804F4DC8 mfCameraLookatZ;
    /* 0x5C */ TweakValueImpl_804F4DC8 mfCameraDistance;
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

// Single-sprite crowd cluster implementation at 0x802D818C. The descriptive
// portion of this placeholder follows the retained Cluster tweak namespace;
// the stripped binary does not preserve the original C++ class name.
class ImpostorCluster_8052EA20 : public ImpostorCharacter
{
public:
    ImpostorCluster_8052EA20(const char* name, int budget,
        const ImpostorCharacterParams* params);

    virtual void UnidentifiedVirtual18(int index, float phase);
    virtual void UnidentifiedVirtual1C(int texture);
    virtual void UnidentifiedVirtual20(void* target, int texture);
    virtual void UnidentifiedVirtual24(float dt);
    virtual void UnidentifiedVirtual28(
        float dt, const char* unidentified);
    virtual void UnidentifiedVirtual2C(
        void* unidentified0, void* unidentified1);

    /* 0x6C */ const char* mUnidentified06C;
}; // size: 0x70

extern "C" unsigned long fn_802D81EC(
    ImpostorCluster_8052EA20* character);

#endif // GAME_RENDER_IMPOSTOR_CHARACTER_H

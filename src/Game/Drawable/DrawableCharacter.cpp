#include "Game/Drawable/DrawableCharacter.h"

#include "Game/BasicStadium.h"
#include "Game/CharacterEffects.h"
#include "Game/AI/HeadTrack.h"
#include "Game/PoseAccumulator.h"
#include "Game/Render/SkinAnimatedMovableNPC.h"
#include "NL/gl/glState.h"

extern "C" void* fn_802CDF0C();

#include "NL/nlMemory.h"
#include "NL/nlString.h"

#pragma cpp_extensions on

void* operator new(unsigned long, void*);

struct UnidentifiedStaticState
{
    UnidentifiedStaticState()
        : value(0)
    {
    }

    void* value;
};

template <typename T>
struct UnidentifiedStaticStorage
{
    static UnidentifiedStaticState state;
};

struct UnidentifiedStaticTag;

template <typename T>
UnidentifiedStaticState UnidentifiedStaticStorage<T>::state;

struct RotAccum
{
    nlQuaternion q;
    float quatAccumulatedWeight;
    u16 rotAroundZ;
    u16 padding;
    float rotAroundZAccumulatedWeight;
    u8 identity;
    u8 tail[3];
};

struct LightObject
{
    char _000[0x2C];
    int intervalIndex;
    float height;
    float radius;
};

struct PoseNode
{
    virtual void Reserved0() = 0;
    virtual void Reserved1() = 0;
    virtual void Evaluate(float, cPoseAccumulator*) = 0;
};

struct Character
{
    nlVector3& GetJointPosition(int) const;
    int GetBallJointIndex() const { return ballJointIndex; }

    char _000[0x1C];
    int characterType;
    char _020[4];
    int characterClass;
    char _028[8];
    nlVector3 position;
    char _03C[0x0C];
    nlVector3 velocity;
    char _054[0x0E];
    u16 facingDirection;
    char _064[0x3C];
    union
    {
        float height;
        float scale;
    };
    char _0A4[0x18];
    cPoseAccumulator* sourcePoseAccumulator;
    void* object;
    void* unknownC4;
    void* animationController;
    char _0CC[8];
    cHeadTrack* headTrack;
    int headJointIndex;
    int bip01JointIndex;
    char _0E0[0x10];
    int specialState;
    char _0F4[0x0C];
    u32 normalTexture;
    u32 swapTexture;
    u32 alternateTexture;
    ResolvedTexture resolvedNormalTexture;
    ResolvedTexture resolvedSwapTexture;
    ResolvedTexture resolvedAlternateTexture;
    char _118[4];
    LightObject* shadowLight;
    int partitionIndex;
    char _124[0x40];
    float damage1;
    float damage2;
    int damageType;
    char _170[8];
    float blendAmount;
    u8 shadowEnabled;
    u8 alternateView;
    u8 flag2;
    u8 flag3;
    u8 megaEnabled;
    u8 flag5;
    u8 flag6;
    u8 _183;
    nlVector4 megaBasis;
    nlVector3 megaTranslation;
    float megaScale;
    char _1A4[4];
    union
    {
        float state1A8;
        float megaBlend;
    };
    char _1AC[4];
    EffectsTexturing* effectsTexturing;
    char _1B4[0x144];
    int ballJointIndex;
    char _2FC[0x18];
    void* teamOrPlayer;
};

struct SkinMesh
{
    virtual void Reserved0() = 0;
    virtual void Reserved1() = 0;
    virtual void* FinishPreparing() = 0;
    virtual void Reserved3() = 0;
    virtual void PrepareToRender() = 0;

    int unknown04;
    int unknown08;
    int enabled;
};

struct PacketUserData
{
    struct TextureSlot
    {
        u32 texture;
        u16 textureFrame;

        void SetTexture(u32 newTexture, u16 newTextureFrame)
        {
            texture = newTexture;
            textureFrame = newTextureFrame;
        }
    };

    char _000[0x20];
    union
    {
        struct
        {
            u32 texture;
            u16 textureFrame;
        };
        TextureSlot textureSlots[1];
    };
};

struct ModelPacket
{
    char _000[0x1C];
    u32 raster;
    PacketUserData* userData;
    char _024[0x0C];
};

struct Model
{
    int unknown;
    int packetCount;
    ModelPacket* packets;
};

union Colour
{
    u32 value;
    struct
    {
        u8 r;
        u8 g;
        u8 b;
        u8 a;
    };
};

struct ProjectedShadowParams
{
    nlVector4 light;
    nlVector3 position;
    float radius;
    Model* model;
    float height;
    float scalar;
    int partitionIndex;
    int visibleInterval;
    int invisibleInterval;
};

struct Camera
{
    virtual void Reserved0() = 0;
    virtual void Reserved1() = 0;
    virtual void Reserved2() = 0;
    virtual void Reserved3() = 0;
    virtual void Reserved4() = 0;
    virtual void Reserved5() = 0;
    virtual void* GetView() = 0;

    char _004[0x88];
    nlVector3 position;
};

struct CameraManager
{
    char _000[0x50];
    Camera* camera;
};

struct TaskManager
{
    void* vtable;
    char _004[4];
    u32 state;
};

extern "C" cPoseAccumulator* fn_8030A9D0(cPoseAccumulator*, cSHierarchy*, bool);
extern "C" void fn_8030B1C0(cPoseAccumulator*);
extern "C" void fn_8030B318(cPoseAccumulator*, const nlMatrix4*);
extern "C" void fn_8030B9C8(cPoseAccumulator*, const nlMatrix4*);
extern "C" void fn_8030BD18(cPoseAccumulator*, int, const nlQuaternion*, bool, float);
extern "C" void fn_8030BE68(cPoseAccumulator*, int, u16, float);
extern "C" nlMatrix4* fn_8030C2F0(cPoseAccumulator*, int);
extern "C" int fn_8030C374(cPoseAccumulator*);
extern "C" void fn_8030C380(cPoseAccumulator*, int, void (*)(void*), void*, int);
extern "C" int fn_8030CC1C(cSHierarchy*, u32);
extern "C" void fn_8017BF84(void*);
extern "C" void fn_8030AE14(cPoseAccumulator*, bool);
extern "C" cPoseAccumulator* fn_8030ABF8(
    cPoseAccumulator*, cPoseAccumulator*);
extern "C" cPoseAccumulator* fn_8030AD14(
    cPoseAccumulator*, cPoseAccumulator*);
extern "C" bool fn_8001C534(Character*, bool);
extern "C" SkinMesh* fn_8001C550(Character*, int);
extern "C" void fn_8001C574(Character*);
extern "C" void fn_8001D6F4(Character*, cPoseAccumulator*, int);
extern "C" void fn_8001EFCC(Character*, SkinMesh*, Model*);
extern "C" Character* fn_800A6954();
extern "C" void fn_800A6A94(void*);
extern "C" bool fn_80182120();
extern "C" void fn_80182EC8(int);
extern "C" int fn_800FC748(int);
extern "C" int fn_80183DEC(const nlVector3*);
extern "C" int fn_80184AE8(int);
extern "C" bool fn_80184B5C(ProjectedShadowParams*);
extern "C" void fn_80185130(ProjectedShadowParams*);
extern "C" void fn_8018595C(ProjectedShadowParams*);
extern "C" CameraManager* fn_8027261C();
extern "C" void fn_8027267C(int);
extern "C" void fn_80273A4C(int, Model*, int);
extern "C" bool fn_80277238();
extern "C" Model* fn_802CC360(void*, int, int);
extern "C" void fn_802CC458(ModelPacket*, u32, u32);
extern "C" void fn_802CC4FC(
    ModelPacket*, u32, const ResolvedTexture&);
extern "C" void fn_802CC628(ModelPacket*, u32, float);
extern "C" void fn_802CC6C0(ModelPacket*, u32, u32);
extern "C" float fn_802CC758(ModelPacket*, u32);
extern "C" u32 fn_802CC7E4(ModelPacket*, u32);
extern "C" bool fn_802CC8FC(ModelPacket*, u32);
extern "C" ResolvedTexture fn_802CE1B8(void*, u32);
extern "C" int fn_802DD1EC(void*, const nlVector3*, float);

int lbl_80511298[3] = { 2, 2, 2 };
int lbl_805112A4[3] = { 4, 4, 4 };

__declspec(weak) char CharacterAlphaValueName[] = "alphaValue";
__declspec(weak) char CharacterLeftPropJointName[] = "bip01 l prop";
__declspec(weak) char CharacterRightPropJointName[] = "bip01 r prop";
__declspec(weak) char CharacterSpineJointName[] = "bip01 spine1";
__declspec(weak) char CharacterBlendAmountName[] = "blendAmount";
__declspec(weak) char CharacterBlackTextureName[] = "global/black";
__declspec(weak) char CharacterShadowLevelName[] = "shadowLevel";
__declspec(weak) char CharacterMegaBlendName[] = "megaBlend";
__declspec(weak) char CharacterDamage1EnabledName[] = "damage1Enabled";
__declspec(weak) char CharacterScorchTextureName[] = "global/scorch";
__declspec(weak) char CharacterDamage2EnabledName[] = "damage2Enabled";
__declspec(weak) char CharacterLightTextureName[] = "global/lightramp";
__declspec(weak) char CharacterWhiteTextureName[] = "global/white";

float lbl_806DCB48 = 1.0f;
u8 lbl_806DCB4C = 1;
float lbl_806DCB50 = 0.25f;
float lbl_806DCB54 = -0.22f;
float lbl_806DCB58 = -0.16f;
float lbl_806DCB5C = -98.0f;
float lbl_806DCB60 = -132.0f;
float lbl_806DCB64 = 60.0f;
float lbl_806DCB68 = 0.26f;
float lbl_806DCB6C = -0.26f;
float lbl_806DCB70 = 0.12f;
float lbl_806DCB74 = 60.0f;
float lbl_806DCB78 = 130.0f;
float lbl_806DCB7C = 28.0f;
int lbl_806DCB80 = 8;
int lbl_806DCB84 = 11;
float lbl_806DCB88 = 0.25f;
float lbl_806DCB8C = 1.175f;
float lbl_806DCB90 = 1.125f;

u32 lbl_806E1388 = glGetTexture(CharacterLightTextureName);
u32 lbl_806E138C = glGetTexture(CharacterBlackTextureName);
u32 lbl_806E1390 = glGetTexture(CharacterWhiteTextureName);
int lbl_806E1394;
int lbl_806E1398;
int lbl_806E139C;
float lbl_806E13A0;
float lbl_806E13A4;
float lbl_806E13A8;
u8 lbl_806E13AC;
u8 lbl_806E13AD;
u8 lbl_806E13AE;
u8 lbl_806E13AF;
u8 lbl_806E13B0;
u8 lbl_806E13B1;
u8 lbl_806E13B2;
int lbl_806E13B4;
u8 lbl_806E13B8;
Character* DrawableCharacter::renderOnlyCharacter;
bool DrawableCharacter::renderOpposingGoalie;
u8 lbl_806E13C1;
u8 lbl_806E13C2;

extern int lbl_806E0F54;
extern int lbl_80573CA8[];
extern TaskManager* m_pInstance__13nlTaskManager;
extern u32 lbl_806E1F0C;
extern u32 lbl_806E1F10;

static inline float DegreesToRadians(float degrees)
{
    return (3.1415927f * degrees) / 180.0f;
}

static inline double BoolAsDouble(bool value)
{
    return value ? 1.0f : 0.0f;
}

static inline float Lerp(float lhs, float rhs, float weight, float one)
{
    float lhsWeight = one - weight;
    float rhsWeighted = weight * rhs;
    return lhsWeight * lhs + rhsWeighted;
}

static inline void VectorLerp(
    nlVector3& output,
    const nlVector3& lhs,
    const nlVector3& rhs,
    float weight)
{
    float lhsWeight = 1.0f - weight;
    output.x = lhsWeight * lhs.x + weight * rhs.x;
    output.y = lhsWeight * lhs.y + weight * rhs.y;
    output.z = lhsWeight * lhs.z + weight * rhs.z;
}

static inline void SetMatrixTranslation(nlMatrix4& matrix, const nlVector3& translation)
{
    matrix.e2[3][0] = translation.x;
    matrix.e2[3][1] = translation.y;
    matrix.e2[3][2] = translation.z;
    matrix.e2[3][3] = 1.0f;
}

static inline nlVector3& GetMatrixTranslation(nlMatrix4& matrix)
{
    return *(nlVector3*)&matrix.e2[3][0];
}

static inline void SetMatrixTranslation(
    nlMatrix4& matrix,
    float x,
    float y,
    float z,
    float w)
{
    matrix.e2[3][0] = x;
    matrix.e2[3][1] = y;
    matrix.e2[3][2] = z;
    matrix.e2[3][3] = w;
}

static inline void BlendTranslationAccum(
    VectorAccum& output,
    const VectorAccum& lhs,
    const VectorAccum& rhs,
    float weight,
    float one,
    float zero)
{
    output.accumulatedWeight = one;
    if (lhs.identity && rhs.identity)
    {
        output.identity = true;
        output.t.x = zero;
        output.t.y = zero;
        output.t.z = zero;
    }
    else
    {
        output.identity = false;
        output.t.x = Lerp(lhs.t.x, rhs.t.x, weight, one);
        output.t.y = Lerp(lhs.t.y, rhs.t.y, weight, one);
        output.t.z = Lerp(lhs.t.z, rhs.t.z, weight, one);
    }
}

static inline void BlendScaleAccum(
    VectorAccum& output,
    const VectorAccum& lhs,
    const VectorAccum& rhs,
    float weight,
    float one)
{
    output.accumulatedWeight = one;
    if (lhs.identity && rhs.identity)
    {
        output.identity = true;
        output.t.x = one;
        output.t.y = one;
        output.t.z = one;
    }
    else
    {
        output.identity = false;
        output.t.x = Lerp(lhs.t.x, rhs.t.x, weight, one);
        output.t.y = Lerp(lhs.t.y, rhs.t.y, weight, one);
        output.t.z = Lerp(lhs.t.z, rhs.t.z, weight, one);
    }
}

static inline void BuildCharacterMatrices(DrawableCharacter* drawable, cPoseAccumulator* accumulator)
{
    nlMatrix4 matrix;
    nlMakeRotationMatrixZ(matrix, 0.0000958738f * (float)drawable->facingDirection);
    SetMatrixTranslation(matrix, drawable->position);
    if (drawable->character != 0)
    {
        fn_8030C380(
            accumulator, *(int*)((char*)drawable->character + 0xD8),
            fn_8017BF84, drawable, 0);
    }
    fn_8030B318(accumulator, &matrix);
    if (drawable->character != 0)
    {
        fn_8030C380(
            accumulator, *(int*)((char*)drawable->character + 0xD8), 0, 0, 0);
    }
}

static inline void BuildNpcMatrices(DrawableCharacter* drawable)
{
    nlMatrix4 matrix;
    nlMakeRotationMatrixZ(matrix, 0.0000958738f * (float)drawable->facingDirection);
    SetMatrixTranslation(matrix, drawable->position);
    fn_8030B9C8(drawable->poseAccumulator, &matrix);
}

DrawableCharacter::DrawableCharacter()
{
    visible = true;
    useObject = false;
    flag2 = true;
    flag3 = true;
    megaEnabled = false;
    flag5 = true;
    flag6 = true;
    typeIsOne = false;
    facingDirection = 0;
    headSpin = 0;
    headTilt = 0;
    height = 0.0f;
    scale = 1.0f;
    blendAmount = 1.0f;
    state40 = 0.0f;
    shadowLevel = 1.0f;
    object = 0;
    poseAccumulator = 0;
    effectsTexturing = 0;
    character = 0;
    damage1 = 0.0f;
    damage2 = 0.0f;
    damageType = 0;
    savedScorchTexture = -1;
    scorchTexture = 0;
    position.x = 0.0f;
    position.y = 0.0f;
    position.z = 0.0f;
    bip01Position.x = 0.0f;
    bip01Position.y = 0.0f;
    bip01Position.z = 0.0f;
    headPosition.x = 0.0f;
    headPosition.y = 0.0f;
    headPosition.z = 0.0f;
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    velocity.z = 0.0f;
    megaBasis.z = 0.0f;
    megaBasis.y = 0.0f;
    megaBasis.x = 0.0f;
    megaBasis.w = 1.0f;
    megaTranslation.x = 0.0f;
    megaTranslation.y = 0.0f;
    megaTranslation.z = -10.0f;
    megaScale = 1.0f;
}

DrawableCharacter::~DrawableCharacter()
{
    fn_8030AE14(poseAccumulator, true);
}

void DrawableCharacter::Free()
{
    fn_8030AE14(poseAccumulator, true);
    poseAccumulator = 0;
}

void* DrawableCharacter::GetAnimController() const
{
    return character->animationController;
}

void DrawableCharacter::Grab(Character& source)
{
    character = &source;
    position = source.position;
    bip01Position = source.GetJointPosition(source.bip01JointIndex);
    headPosition = source.GetJointPosition(source.headJointIndex);
    height = bip01Position.z;
    scale = source.height;
    flag2 = source.flag2;
    flag3 = source.flag3;
    megaEnabled = source.megaEnabled;
    if (megaEnabled)
    {
        megaTranslation = source.megaTranslation;
        megaBasis = source.megaBasis;
        megaScale = source.megaScale;
    }
    flag5 = source.flag5;
    flag6 = source.flag6;
    typeIsOne = source.characterType == 1;
    blendAmount = source.blendAmount;
    state40 = source.state1A8;
    shadowLevel = (float)BoolAsDouble(source.shadowEnabled != 0);
    shadowLevel = shadowLevel * blendAmount;
    velocity = source.velocity;
    facingDirection = source.facingDirection;
    headSpin = (u16)(int)source.headTrack->m_fHeadSpin;
    headTilt = (u16)(int)source.headTrack->m_fHeadTilt;
    visible = true;
    useObject = source.sourcePoseAccumulator->m_bUseObject;
    damage1 = source.damage1;
    damage2 = source.damage2;
    damageType = source.damageType;
    if (!useObject)
    {
        object = source.object;
    }

    if (poseAccumulator == 0)
    {
        cPoseAccumulator* accumulator =
            (cPoseAccumulator*)nlMalloc(0x7C, 8, false);
        if (accumulator != 0)
        {
            accumulator = fn_8030ABF8(
                accumulator, source.sourcePoseAccumulator);
        }
        poseAccumulator = accumulator;
    }
    else
    {
        fn_8030AD14(poseAccumulator, source.sourcePoseAccumulator);
    }

    if (lbl_806E13B0 == 0 && megaEnabled)
    {
        nlMatrix4 matrix;
        nlQuatToMatrix(matrix, *(const nlQuaternion*)&megaBasis, true);
        for (int row = 0; row < 3; ++row)
        {
            for (int column = 0; column < 3; ++column)
            {
                matrix.e2[row][column] *= megaScale;
            }
        }
        matrix.e2[3][0] = megaTranslation.x;
        matrix.e2[3][1] = megaTranslation.y;
        matrix.e2[3][2] = megaTranslation.z;
        matrix.e2[3][3] = 1.0f;

        poseAccumulator->m_pNodeMatrices[lbl_806E1398] = matrix;
    }

    EffectsTexturing* texturing = source.effectsTexturing;
    if (texturing == 0)
    {
        texturing = fxGetTexturing(eFXTex_Nothing);
    }
    effectsTexturing = texturing;
}

void DrawableCharacter::HeadTrackCallback(
    u32 context, u32, cPoseAccumulator* accumulator, u32 headNodeIndex, int)
{
    DrawableCharacter* drawable = (DrawableCharacter*)context;
    CalcHeadTrackMatrix(drawable->headSpin, drawable->headTilt,
        accumulator, headNodeIndex);
}

void DrawableCharacter::BuildNodeMatrices(cPoseAccumulator* accumulator)
{
    nlMatrix4 matrix;
    nlMakeRotationMatrixZ(matrix, 0.0000958738f * (float)facingDirection);
    matrix.e2[3][0] = position.x;
    matrix.e2[3][1] = position.y;
    matrix.e2[3][2] = position.z;
    matrix.e2[3][3] = 1.0f;

    if (character != 0)
    {
        fn_8030C380(
            accumulator, character->headJointIndex,
            fn_8017BF84, this, 0);
    }
    fn_8030B318(accumulator, &matrix);
    if (character != 0)
    {
        fn_8030C380(
            accumulator, character->headJointIndex, 0, 0, 0);
    }
}

void DrawableCharacter::BuildNpcMatrix()
{
    nlMatrix4 matrix;
    nlMakeRotationMatrixZ(matrix, 0.0000958738f * (float)facingDirection);
    matrix.e2[3][0] = position.x;
    matrix.e2[3][1] = position.y;
    matrix.e2[3][2] = position.z;
    matrix.e2[3][3] = 1.0f;
    fn_8030B9C8(poseAccumulator, &matrix);
}

void DrawableCharacter::Render(Character& source)
{
    if (!visible)
    {
        return;
    }

    bool special = false;
    if (typeIsOne
        || (lbl_806E13AC != 0 && fn_8001C534(&source, true)))
    {
        special = true;
    }

    if (special)
    {
        fn_8001D6F4(&source, poseAccumulator, 1);
    }

    if (!special || lbl_806DCB4C != 0)
    {
        if (m_pInstance__13nlTaskManager->state == 2)
        {
            fn_8001D6F4(&source, poseAccumulator, 2);
        }
        else
        {
            fn_8001D6F4(&source, poseAccumulator, 0);
        }
    }
    fn_8001D6F4(&source, poseAccumulator, 3);

    Character* renderOnly = renderOnlyCharacter;
    if (renderOnly == 0 || renderOnly == &source
        || (renderOpposingGoalie
            && (fn_800A6A94(renderOnly->teamOrPlayer),
                &source == fn_800A6954())))
    {
        if (special)
        {
            if (lbl_806DCB4C != 0)
            {
                SendToGl(source, 1);
            }
            SendToGl(source, 2);
        }
        else
        {
            SendToGl(source, 0);
        }
    }

    fn_8001C574(&source);
}

void DrawableCharacter::SendToGl(Character& source, int renderPass)
{
    SkinMesh* skinMesh;
    int characterClass = source.characterClass;
    int view = lbl_806DCB84;
    if (lbl_80573CA8[0] == 1)
    {
        view = 13;
        if (source.alternateView)
        {
            view = 11;
        }
    }
    if (characterClass == 5 && source.state1A8 > 0.0f)
    {
        view = 15;
    }

    if (renderPass != 2)
    {
        if (m_pInstance__13nlTaskManager->state == 2)
        {
            skinMesh = fn_8001C550(&source, 2);
        }
        else
        {
            skinMesh = fn_8001C550(&source, 0);
        }
    }
    else
    {
        skinMesh = fn_8001C550(&source, 1);
    }

    if (lbl_806E13C1)
    {
        fn_80182EC8(2);
    }
    else if (lbl_806E13C2 || fn_80182120())
    {
        fn_80182EC8(1);
    }
    else
    {
        fn_80182EC8(0);
    }

    bool isVisible;
    if (m_pInstance__13nlTaskManager->state == 0x10)
    {
        isVisible = true;
    }
    else if (fn_80277238())
    {
        float radius;
        if (characterClass == 3)
        {
            radius = 3.5f;
        }
        else
        {
            radius = 2.5f;
        }
        void* viewObject = fn_8027261C()->camera->GetView();
        isVisible = fn_802DD1EC(viewObject, &bip01Position, radius) != 0;
    }
    else
    {
        isVisible = true;
    }

    if (!isVisible)
    {
        return;
    }

    skinMesh->enabled = 1;
    if (lbl_806E13AD
        || (m_pInstance__13nlTaskManager->state & 0x18) != 0
        || (m_pInstance__13nlTaskManager->state & 8) != 0
        || (m_pInstance__13nlTaskManager->state & 0x20000) != 0)
    {
        skinMesh->enabled = 0;
    }
    skinMesh->PrepareToRender();
    void* sourceModel = skinMesh->FinishPreparing();

    Model* model = fn_802CC360(sourceModel, 0, 0);
    ApplyDamageEffects(source, model, renderPass);
    bool attachEffects = false;
    ApplyMaterialEffects(
        source, model, (eCharacterRenderPass)renderPass, &attachEffects);
    if (attachEffects)
    {
        fn_8001EFCC(&source, skinMesh, model);
    }

    static u32 alphaValueHash = nlStringLowerHash(CharacterAlphaValueName);
    if (characterClass == 10)
    {
        int packetCount = model->packetCount;
        if ((lbl_806E13B1 || !flag5)
            && lbl_806E13B4 < packetCount)
        {
            fn_802CC628(
                model->packets + lbl_806E13B4,
                alphaValueHash, 0.0f);
        }
        if ((lbl_806E13B2 || !flag6)
            && lbl_806DCB80 < packetCount)
        {
            fn_802CC628(
                model->packets + lbl_806DCB80,
                alphaValueHash, 0.0f);
        }
    }

    fn_80273A4C(view, model, 0);
    if (characterClass == 5)
    {
        view = 13;
    }

    SkinMesh* shadowMesh = fn_8001C550(&source, 3);
    if (shadowMesh != skinMesh)
    {
        shadowMesh->enabled = 1;
        if (lbl_806E13AD
            || (m_pInstance__13nlTaskManager->state & 0x18) != 0
            || (m_pInstance__13nlTaskManager->state & 8) != 0
            || (m_pInstance__13nlTaskManager->state & 0x20000) != 0)
        {
            shadowMesh->enabled = 0;
        }
        shadowMesh->PrepareToRender();
        sourceModel = shadowMesh->FinishPreparing();
        model = fn_802CC360(sourceModel, 0, 0);
    }
    RenderCharacterShadow(source, model, view);
}

void DrawableCharacter::Grab(SkinAnimatedMovableNPC& npc)
{
    position = npc.mv3Position;
    height = npc.mpPoseAccumulator->GetNodeMatrix(0).e2[3][2];
    facingDirection = npc.maFacingDirection;
    object = npc.mpPoseTree;
    visible = npc.mbIsVisible;

    if (poseAccumulator == 0)
    {
        cPoseAccumulator* accumulator =
            (cPoseAccumulator*)nlMalloc(0x7C, 8, false);
        if (accumulator != 0)
        {
            accumulator = fn_8030ABF8(accumulator, npc.mpPoseAccumulator);
        }
        poseAccumulator = accumulator;
    }
    else
    {
        fn_8030AD14(poseAccumulator, npc.mpPoseAccumulator);
    }
}

void DrawableCharacter::Render(SkinAnimatedMovableNPC& npc)
{
    if (!visible)
    {
        return;
    }

    u16 angleValue = ((volatile DrawableCharacter*)this)->facingDirection;
    nlMatrix4 worldMatrix;
    nlMakeRotationMatrixZ(worldMatrix, 0.0000958738f * (float)angleValue);
    worldMatrix.SetRow_(3, position);
    npc.mbIsVisible = visible;
    npc.RenderFromReplay(*poseAccumulator, &worldMatrix);
}

#pragma opt_findoptimalunrollfactor off
#pragma opt_unroll_count 1
void DrawableCharacter::Blend(
    float* blendFactors,
    DrawableCharacter& lhs,
    DrawableCharacter& rhs)
{
    float identityZero;
    float identityOne;
    const float rhsWeight = *blendFactors;
    const float lhsWeight = 1.0f - rhsWeight;

    visible = lhs.visible && rhs.visible;
    bool normalBlend = false;
    bool specialCharacter = false;
    character = lhs.character;

    if (character != 0)
    {
        if (character->specialState == 2)
        {
            if (character->characterClass == 13)
            {
                specialCharacter = true;
                if (lbl_806E1394 <= 0)
                {
                    cSHierarchy* hierarchy = lhs.poseAccumulator->m_pHierarchy;
                    u32 hash = nlStringLowerHash(CharacterLeftPropJointName);
                    lbl_806E1394 = fn_8030CC1C(hierarchy, hash);
                    hierarchy = lhs.poseAccumulator->m_pHierarchy;
                    hash = nlStringLowerHash(CharacterRightPropJointName);
                    lbl_806E1398 = fn_8030CC1C(hierarchy, hash);
                    hierarchy = lhs.poseAccumulator->m_pHierarchy;
                    hash = nlStringLowerHash(CharacterSpineJointName);
                    lbl_806E139C = fn_8030CC1C(hierarchy, hash);
                }

                megaEnabled = rhs.megaEnabled;
                if (megaEnabled)
                {
                    megaTranslation = rhs.megaTranslation;
                    megaBasis = rhs.megaBasis;
                    megaScale = rhs.megaScale;
                }
                flag2 = rhs.flag2;
                flag3 = rhs.flag3;
            }
            else
            {
                megaEnabled = false;
                flag2 = true;
                flag3 = true;
            }
            flag5 = rhs.flag5;
            flag6 = rhs.flag6;
        }

        damage1 = lhs.damage1;
        damage2 = lhs.damage2;
        damageType = lhs.damageType;
        VectorLerp(
            bip01Position, lhs.bip01Position, rhs.bip01Position, rhsWeight);
        VectorLerp(
            headPosition, lhs.headPosition, rhs.headPosition, rhsWeight);
        velocity = lhs.velocity;
        scale = lhs.scale * lhsWeight + rhsWeight * rhs.scale;
        blendAmount = lhs.blendAmount * lhsWeight + rhsWeight * rhs.blendAmount;
        typeIsOne = lhs.typeIsOne;
        state40 = lhs.state40 * lhsWeight + rhsWeight * rhs.state40;
        shadowLevel = lhs.shadowLevel * lhsWeight + rhsWeight * rhs.shadowLevel;
        effectsTexturing = lhs.effectsTexturing;

        if (!lhs.useObject && !rhs.useObject)
        {
            int headSpinOffset =
                (short)(rhsWeight * (float)(short)(rhs.headSpin - lhs.headSpin));
            headSpin = lhs.headSpin + headSpinOffset;
            normalBlend = true;
            int headTiltOffset =
                (short)(rhsWeight * (float)(short)(rhs.headTilt - lhs.headTilt));
            headTilt = lhs.headTilt + headTiltOffset;
            object = 0;
        }
    }
    else
    {
        object = 0;
        normalBlend = true;
        scale = 1.0f;
    }

    float positionLhsWeight = 1.0f - rhsWeight;
    position.x = positionLhsWeight * lhs.position.x + rhsWeight * rhs.position.x;
    position.y = positionLhsWeight * lhs.position.y + rhsWeight * rhs.position.y;
    position.z = positionLhsWeight * lhs.position.z + rhsWeight * rhs.position.z;
    int facingOffset =
        (short)(rhsWeight * (float)(short)(rhs.facingDirection - lhs.facingDirection));
    facingDirection = lhs.facingDirection + facingOffset;
    height = lhs.scale * lhsWeight + rhsWeight * rhs.scale;

    if (poseAccumulator == 0)
    {
        cPoseAccumulator* accumulator =
            (cPoseAccumulator*)nlMalloc(sizeof(cPoseAccumulator), 8, false);
        if (accumulator != 0)
        {
            accumulator = fn_8030A9D0(
                accumulator, lhs.poseAccumulator->m_pHierarchy, false);
        }
        poseAccumulator = accumulator;
    }

    poseAccumulator->m_Scale = scale;
    fn_8030B1C0(poseAccumulator);

    for (int morphIndex = 0; morphIndex < 20; ++morphIndex)
    {
        float& lhsMorphWeight =
            lhs.poseAccumulator->m_MorphWeights[morphIndex];
        poseAccumulator->m_MorphWeights[morphIndex] +=
            lhsMorphWeight * lhsWeight;
        float& rhsMorphWeight =
            rhs.poseAccumulator->m_MorphWeights[morphIndex];
        poseAccumulator->m_MorphWeights[morphIndex] +=
            rhsMorphWeight * rhsWeight;
    }

    if (normalBlend)
    {
        identityOne = 1.0f;
        identityZero = 0.0f;
        for (int i = 0; i < fn_8030C374(poseAccumulator); ++i)
        {
            RotAccum& lhsRot = lhs.poseAccumulator->m_pRotations[i];
            RotAccum& rhsRot = rhs.poseAccumulator->m_pRotations[i];
            float lhsRotAroundZWeight =
                lhsRot.rotAroundZAccumulatedWeight * lhsWeight;
            float rhsRotAroundZWeight = rhsRot.rotAroundZAccumulatedWeight * rhsWeight;
            fn_8030BE68(
                poseAccumulator, i, lhsRot.rotAroundZ,
                lhsRotAroundZWeight);
            fn_8030BE68(poseAccumulator, i, rhsRot.rotAroundZ, rhsRotAroundZWeight);
            float lhsQuaternionWeight =
                lhsRot.quatAccumulatedWeight * lhsWeight;
            float rhsQuaternionWeight = rhsRot.quatAccumulatedWeight * rhsWeight;
            fn_8030BD18(
                poseAccumulator, i, &lhsRot.q,
                false, lhsQuaternionWeight);
            fn_8030BD18(
                poseAccumulator, i, &rhsRot.q, false, rhsQuaternionWeight);

            BlendTranslationAccum(
                poseAccumulator->m_pTranslations[i],
                lhs.poseAccumulator->m_pTranslations[i],
                rhs.poseAccumulator->m_pTranslations[i],
                rhsWeight,
                identityOne,
                identityZero);
            BlendScaleAccum(
                poseAccumulator->m_pScales[i],
                lhs.poseAccumulator->m_pScales[i],
                rhs.poseAccumulator->m_pScales[i],
                rhsWeight,
                identityOne);
        }

        BuildCharacterMatrices(this, poseAccumulator);
    }
    else
    {
        if (!lhs.useObject)
        {
            BuildCharacterMatrices(this, lhs.poseAccumulator);
        }
        else if (!rhs.useObject)
        {
            BuildCharacterMatrices(this, rhs.poseAccumulator);
        }

        identityZero = 1.0f;
        identityOne = 0.0f;
        for (int i = 0; i < fn_8030C374(poseAccumulator); ++i)
        {
            nlQuatNLerp(
                poseAccumulator->m_pQuaternions[i],
                lhs.poseAccumulator->m_pQuaternions[i],
                rhs.poseAccumulator->m_pQuaternions[i],
                rhsWeight);
            BlendTranslationAccum(
                poseAccumulator->m_pTranslations[i],
                lhs.poseAccumulator->m_pTranslations[i],
                rhs.poseAccumulator->m_pTranslations[i],
                rhsWeight,
                identityZero,
                identityOne);
            BlendScaleAccum(
                poseAccumulator->m_pScales[i],
                lhs.poseAccumulator->m_pScales[i],
                rhs.poseAccumulator->m_pScales[i],
                rhsWeight,
                identityZero);
        }

        BuildNpcMatrices(this);
    }

    if (specialCharacter)
    {
        if (!lbl_806E13B0 && megaEnabled)
        {
            nlMatrix4 matrix;
            nlQuatToMatrix(matrix, *(const nlQuaternion*)&megaBasis, true);
            for (int row = 0; row < 3; ++row)
            {
                for (int column = 0; column < 3; ++column)
                {
                    matrix.e2[row][column] *= megaScale;
                }
            }
            SetMatrixTranslation(matrix, megaTranslation);
            poseAccumulator->m_pNodeMatrices[lbl_806E1398] = matrix;
        }
        else if (lbl_806E13AF || !flag3)
        {
            nlMatrix4 rotation;
            nlMakeRotationMatrixEulerAngles(
                rotation,
                DegreesToRadians(lbl_806DCB5C),
                DegreesToRadians(lbl_806DCB60),
                DegreesToRadians(lbl_806DCB64));
            float translationW;
            float translationX;
            float translationY;
            float translationZ;
            translationZ = lbl_806DCB58;
            translationY = lbl_806DCB54;
            translationX = lbl_806DCB50;
            translationW = 1.0f;
            rotation.e2[3][0] = translationX;
            rotation.e2[3][1] = translationY;
            rotation.e2[3][2] = translationZ;
            rotation.e2[3][3] = translationW;
            nlMatrix4 matrix;
            nlMultMatrices(
                matrix, rotation,
                *fn_8030C2F0(poseAccumulator, lbl_806E139C));
            poseAccumulator->m_pNodeMatrices[lbl_806E1398] = matrix;
        }

        if (lbl_806E13AE || !flag2)
        {
            nlMatrix4 rotation;
            nlMakeRotationMatrixEulerAngles(
                rotation,
                DegreesToRadians(lbl_806DCB74),
                DegreesToRadians(lbl_806DCB78),
                DegreesToRadians(lbl_806DCB7C));
            float translationW;
            float translationX;
            float translationY;
            float translationZ;
            translationZ = lbl_806DCB70;
            translationY = lbl_806DCB6C;
            translationX = lbl_806DCB68;
            translationW = 1.0f;
            rotation.e2[3][0] = translationX;
            rotation.e2[3][1] = translationY;
            rotation.e2[3][2] = translationZ;
            rotation.e2[3][3] = translationW;
            nlMatrix4 matrix;
            nlMultMatrices(
                matrix, rotation,
                *fn_8030C2F0(poseAccumulator, lbl_806E139C));
            poseAccumulator->m_pNodeMatrices[lbl_806E1394] = matrix;
        }
    }
}
#pragma opt_findoptimalunrollfactor reset
#pragma opt_unroll_count reset

void DrawableCharacter::EvaluateFrom(
    PoseNode& poseNode,
    const nlVector3& offset,
    u16 facingAngle,
    float poseScale)
{
    static const float initialOne = 1.0f;
    static const float initialZero = 0.0f;

    position = offset;
    nlVec3Set(velocity, initialZero, initialZero, initialZero);
    facingDirection = facingAngle;
    headSpin = 0;
    headTilt = 0;
    height = initialZero;
    scale = poseScale;
    flag2 = false;
    flag3 = false;
    megaEnabled = false;
    flag5 = false;
    flag6 = false;
    typeIsOne = false;
    blendAmount = initialOne;
    state40 = initialZero;

    float currentDamage1 = character->damage1;
    damage1 = currentDamage1;
    float currentDamage2 = character->damage2;
    damage2 = currentDamage2;
    damageType = character->damageType;
    shadowLevel = initialOne;

    poseAccumulator->m_Scale = poseScale;
    poseAccumulator->InitAccumulators(poseScale, currentDamage2);
    poseNode.Evaluate(1.0f, poseAccumulator);
    effectsTexturing = fxGetTexturing(eFXTex_Nothing);

    cPoseAccumulator* accumulator = poseAccumulator;
    nlMatrix4 matrix;
    nlMakeRotationMatrixZ(matrix, 0.0000958738f * (float)facingDirection);
    matrix.e2[3][0] = position.x;
    matrix.e2[3][1] = position.y;
    matrix.e2[3][2] = position.z;
    matrix.e2[3][3] = 1.0f;

    if (character != 0)
    {
        fn_8030C380(
            accumulator, character->headJointIndex,
            fn_8017BF84, this, 0);
    }
    fn_8030B318(accumulator, &matrix);
    if (character != 0)
    {
        fn_8030C380(
            accumulator, character->headJointIndex, 0, 0, 0);
    }

    nlMatrix4& bip01Matrix =
        poseAccumulator->GetNodeMatrix(character->bip01JointIndex);
    bip01Position = GetMatrixTranslation(bip01Matrix);
    nlMatrix4& headMatrix =
        poseAccumulator->GetNodeMatrix(character->headJointIndex);
    headPosition = GetMatrixTranslation(headMatrix);
}

#pragma schedule off
nlVector3 DrawableCharacter::GetBallPosition() const
{
    Character* player = *(Character* volatile*)&character;
    nlMatrix4& matrix =
        poseAccumulator->GetNodeMatrix(player->ballJointIndex);
    return GetMatrixTranslation(matrix);
}
#pragma schedule reset

nlQuaternion DrawableCharacter::GetBallOrientation()
{
    nlQuaternion result;
    nlMatrix4 matrix;
    int ballJointIndex = character->ballJointIndex;

    if (1.0f != scale)
    {
        nlMatrix4* source =
            fn_8030C2F0(poseAccumulator, ballJointIndex);
        matrix = *source;

        float reciprocalLength = nlRecipSqrt(
            nlVec3LengthSquared(*(nlVector3*)&matrix.e2[0][0]), true);
        nlVec3Scale(*(nlVector3*)&matrix.e2[0][0], reciprocalLength);

        reciprocalLength = nlRecipSqrt(
            nlVec3LengthSquared(*(nlVector3*)&matrix.e2[1][0]), true);
        nlVec3Scale(*(nlVector3*)&matrix.e2[1][0], reciprocalLength);

        reciprocalLength = nlRecipSqrt(
            nlVec3LengthSquared(*(nlVector3*)&matrix.e2[2][0]), true);
        nlVec3Scale(*(nlVector3*)&matrix.e2[2][0], reciprocalLength);

        nlMatrixToQuat(result, matrix);
    }
    else
    {
        nlMatrix4* source =
            fn_8030C2F0(poseAccumulator, ballJointIndex);
        nlMatrixToQuat(result, *source);
    }

    return result;
}

void DrawableCharacter::RenderOnlyOneCharacter(
    Character& source, bool goalie)
{
    renderOnlyCharacter = &source;
    renderOpposingGoalie = goalie;
}

void DrawableCharacter::RenderAllCharacters()
{
    renderOnlyCharacter = 0;
    renderOpposingGoalie = false;
}

Character* DrawableCharacter::OnlyRenderingOneCharacter()
{
    return renderOnlyCharacter;
}

static inline void ApplyTexture(
    Model* model, u32 texture, ResolvedTexture resolvedTexture)
{
    for (ModelPacket* packet = model->packets;
         packet < model->packets + model->packetCount;
         ++packet)
    {
        if (texture != fn_802CC7E4(packet, lbl_806E1F0C))
        {
            fn_802CC458(packet, lbl_806E1F0C, texture);
            ResolvedTexture packetTexture = resolvedTexture;
            fn_802CC4FC(packet, lbl_806E1F0C, packetTexture);
        }
    }
}

void DrawableCharacter::ApplyMaterialEffects(
    const Character& source,
    Model* model,
    eCharacterRenderPass renderPass,
    bool* attachEffects)
{
    static u32 blendAmountHash = nlStringLowerHash(CharacterBlendAmountName);

    EffectsTexturing* texturing = effectsTexturing;
    int characterClass = source.characterClass;

    if (texturing != 0 && texturing->m_uTexture == 0xFFFFFFFF)
    {
        texturing = 0;
    }
    if (texturing != 0 && texturing->m_bDetail)
    {
        texturing = 0;
    }

    if (texturing != 0)
    {
        for (ModelPacket* packet = model->packets;
             packet < model->packets + model->packetCount;
             ++packet)
        {
            u32& raster = packet->raster;
            if (texturing->m_eBlendMode != GLB_None)
            {
                glSetRasterState(
                    raster, GLS_AlphaBlend, (unsigned long)texturing->m_eBlendMode);
            }

            if (texturing->m_bDetail)
            {
                fn_802CC458(
                    packet, lbl_806E1F10, texturing->m_uTexture);
                ResolvedTexture texture = texturing->m_ResolvedTexture;
                fn_802CC4FC(packet, lbl_806E1F10, texture);
                fn_802CC628(
                    packet, blendAmountHash, lbl_806DCB88);
            }
            else
            {
                fn_802CC458(
                    packet, lbl_806E1F0C, texturing->m_uTexture);
                ResolvedTexture texture = texturing->m_ResolvedTexture;
                fn_802CC4FC(packet, lbl_806E1F0C, texture);
            }
        }
    }
    else
    {
        u32 texture = source.normalTexture;
        if (renderPass == CRP_Default)
        {
            if (texture != source.swapTexture)
            {
                for (ModelPacket* packet = model->packets;
                     packet < model->packets + model->packetCount;
                     ++packet)
                {
                    if (texture
                        == fn_802CC7E4(packet, lbl_806E1F0C))
                    {
                        fn_802CC458(
                            packet, lbl_806E1F0C,
                            source.swapTexture);
                        ResolvedTexture packetTexture =
                            source.resolvedSwapTexture;
                        fn_802CC4FC(
                            packet, lbl_806E1F0C, packetTexture);
                    }
                }
            }
            *attachEffects = true;
        }
        else if (renderPass == CRP_Scorch)
        {
            if (scorchTexture == 0)
            {
                scorchTexture = glGetTexture(CharacterBlackTextureName);
                void* textureManager = fn_802CDF0C();
                resolvedScorchTexture =
                    fn_802CE1B8(textureManager, scorchTexture);
            }
            ApplyTexture(
                model, scorchTexture, resolvedScorchTexture);
        }
        else if (renderPass == CRP_Alternate)
        {
            if (characterClass != 0)
            {
                ApplyTexture(
                    model,
                    source.alternateTexture,
                    source.resolvedAlternateTexture);
            }
        }
    }
}

#pragma opt_common_subs off
void DrawableCharacter::ApplyDamageEffects(
    const Character& source, Model* model, int renderPass)
{
    DrawableCharacter* self = this;
    ModelPacket* packet;
    u32 shadowColourValue;
    u32 damageTexture;
    static u32 shadowLevelHash = nlStringLowerHash(CharacterShadowLevelName);

    int shadowAlpha = fn_80183DEC(&self->bip01Position);
    float fade = 1.0f;
    if (self->bip01Position.z > fade)
    {
        if (self->bip01Position.z > 10.0f)
        {
            fade = 0.0f;
        }
        else
        {
            fade = (10.0f - self->bip01Position.z) / 9.0f;
        }
    }

    Colour shadowColour;
    u8 channel = 0xFF - (int)((float)(0xFF - shadowAlpha) * fade);
    shadowColour.a = 1;
    shadowColour.r = channel;
    shadowColour.g = channel;
    shadowColour.b = channel;
    shadowColourValue = shadowColour.value;

    packet = model->packets;
    while (packet < model->packets + model->packetCount)
    {
        fn_802CC6C0(packet, shadowLevelHash, shadowColourValue);
        packet = (ModelPacket*)((char*)packet + 0x30);
    }

    static u32 blackHash = nlStringLowerHash(CharacterAlphaValueName);
    const float one = 1.0f;
    float blackAmount =
        one != lbl_806DCB48 ? lbl_806DCB48 : self->blendAmount;
    if (blackAmount != one)
    {
        for (packet = model->packets;
             packet < model->packets + model->packetCount;
             packet = (ModelPacket*)((char*)packet + 0x30))
        {
            fn_802CC628(packet, blackHash, blackAmount);
        }
    }

    static u32 megaBlendHash = nlStringLowerHash(CharacterMegaBlendName);
    const float zero = 0.0f;
    float megaAmount =
        zero != lbl_806E13A8 ? lbl_806E13A8 : source.megaBlend;
    if (megaAmount != zero)
    {
        for (packet = model->packets;
             packet < model->packets + model->packetCount;
             packet = (ModelPacket*)((char*)packet + 0x30))
        {
            if (fn_802CC8FC(packet, megaBlendHash)
                && fn_802CC758(packet, megaBlendHash) >= zero)
            {
                fn_802CC628(packet, megaBlendHash, megaAmount);
            }
        }
    }

    if (renderPass == 1)
    {
        for (packet = model->packets;
             packet < model->packets + model->packetCount;
             packet = (ModelPacket*)((char*)packet + 0x30))
        {
            glSetRasterState(packet->raster, (eGLState)6, 2);
        }
    }

    static u32 damage1EnabledHash = nlStringLowerHash(CharacterDamage1EnabledName);
    bool damage1Enabled = false;
    if (lbl_806E13A0 > 0.0f || self->damage1 > 0.0f)
    {
        damage1Enabled = true;
    }
    if (damage1Enabled)
    {
        int characterIndex = fn_800FC748(lbl_806E0F54);
        damageTexture = glGetTexture(CharacterScorchTextureName);
        bool useDamageTexture = false;
        if (self->damageType != 1
            && (self->damageType == 2
                || characterIndex == 0
                || characterIndex == 5
                || characterIndex == 7
                || characterIndex == 9
                || characterIndex == 0x10
                || characterIndex == 8
                || characterIndex == 0xE
                || characterIndex == 0xB
                || characterIndex == 0xF
                || characterIndex == 1
                || characterIndex == 6))
        {
            useDamageTexture = true;
        }
        if (self->damage2 > 0.0f)
        {
            useDamageTexture = true;
        }

        for (packet = model->packets;
             packet < model->packets + model->packetCount;
             packet = (ModelPacket*)((char*)packet + 0x30))
        {
            if (fn_802CC8FC(packet, damage1EnabledHash))
            {
                fn_802CC6C0(packet, damage1EnabledHash, 1);
                if (useDamageTexture)
                {
                    PacketUserData* userData = packet->userData;
                    if (self->savedScorchTexture == 0xFFFFFFFF)
                    {
                        self->savedScorchTexture = userData->texture;
                    }
                    userData->texture = damageTexture;
                    userData->textureFrame = 0xFFFF;
                }
                else if (self->savedScorchTexture != 0xFFFFFFFF)
                {
                    packet->userData->textureSlots[0].SetTexture(
                        self->savedScorchTexture, 0xFFFF);
                    self->savedScorchTexture = 0xFFFFFFFF;
                }
            }
        }
    }

    static u32 damage2EnabledHash = nlStringLowerHash(CharacterDamage2EnabledName);
    bool damage2Enabled = false;
    if (lbl_806E13A4 > 0.0f || self->damage2 > 0.0f)
    {
        damage2Enabled = true;
    }
    if (damage2Enabled)
    {
        for (packet = model->packets;
             packet < model->packets + model->packetCount;
             packet = (ModelPacket*)((char*)packet + 0x30))
        {
            if (fn_802CC8FC(packet, damage2EnabledHash))
            {
                fn_802CC6C0(packet, damage2EnabledHash, 1);
            }
        }
    }
}
#pragma opt_common_subs on

void DrawableCharacter::RenderCharacterShadow(
    const Character& source, void* skinModel, int renderContext)
{
    DrawableCharacter* drawable = this;
    ProjectedShadowParams params;
    Camera* camera;
    LightObject* light;
    float height;
    float radius;
    int intervalIndex;
    float blackAmount;
    float currentShadowLevel = drawable->shadowLevel;

    if (lbl_806E13B8 || currentShadowLevel < 0.001f)
    {
        return;
    }

    camera = reinterpret_cast<Camera*>(BasicStadium::GetCurrentStadium());
    blackAmount = 1.0f != lbl_806DCB48
        ? lbl_806DCB48
        : drawable->blendAmount;

    static u32 blackHash = nlStringLowerHash(CharacterAlphaValueName);
    params.scalar = 1.0f;
    light = source.shadowLight;
    float lightRadius = light->radius;
    float lightHeight = light->height;
    intervalIndex = light->intervalIndex;
    radius = lbl_806DCB8C * lightRadius;
    height = lbl_806DCB90 * lightHeight;
    float one = 1.0f;
    float characterScale = source.scale;
    nlVec4Set(
        params.light,
        camera->position.x,
        camera->position.y,
        camera->position.z,
        one);
    params.position = drawable->bip01Position;
    params.radius = characterScale * radius;
    params.height = characterScale * height;
    params.model = 0;
    params.scalar = currentShadowLevel;
    params.partitionIndex = source.partitionIndex;

    if (m_pInstance__13nlTaskManager->state == 2)
    {
        params.visibleInterval = lbl_80511298[intervalIndex];
        params.invisibleInterval = lbl_805112A4[intervalIndex];
    }
    else
    {
        params.visibleInterval = 1;
        params.invisibleInterval = 1;
    }

    if (fn_80184B5C(&params))
    {
        params.model = fn_802CC360(skinModel, 0, 0);
        if (1.0f != blackAmount)
        {
            for (ModelPacket* packet = params.model->packets;
                 packet < params.model->packets + params.model->packetCount;
                 ++packet)
            {
                fn_802CC628(packet, blackHash, 1.0f);
            }
        }
        fn_80185130(&params);
    }

    fn_8027267C(renderContext);
    int oldContext = ((int (*)())fn_80184AE8)();
    fn_8018595C(&params);
    fn_80184AE8(oldContext);
}

bool DrawableCharacter::NoShadowCallback()
{
    return false;
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;

#ifndef _DRAWABLECHARACTER_H_
#define _DRAWABLECHARACTER_H_

#include "Game/CharacterEffects.h"
#include "NL/nlMath.h"

struct Character;
struct Model;
struct PoseNode;
class cCharacter;
class cPoseAccumulator;
class cPN_SAnimController;
class SkinAnimatedMovableNPC;

enum eCharacterRenderPass
{
    CRP_Default = 0,
    CRP_Scorch = 1,
    CRP_Alternate = 2,
};

class DrawableCharacter
{
public:
    DrawableCharacter();
    ~DrawableCharacter();

    void Free();
    cPN_SAnimController& GetAnimController() const;
    void Grab(Character&);
    static void HeadTrackCallback(u32, u32, cPoseAccumulator*, u32, int);
    void BuildNodeMatrices(cPoseAccumulator*);
    void BuildNpcMatrix();
    void Render(Character&);
    void SendToGl(Character&, int);
    void Grab(SkinAnimatedMovableNPC&);
    void Render(SkinAnimatedMovableNPC&);
    void Blend(float*, DrawableCharacter&, DrawableCharacter&);
    void EvaluateFrom(PoseNode&, const nlVector3&, u16, float);
    nlVector3 GetBallPosition() const;
    nlQuaternion GetBallOrientation();

    static void RenderOnlyOneCharacter(Character&, bool);
    static void RenderAllCharacters();
    static cCharacter* OnlyRenderingOneCharacter();

    void ApplyMaterialEffects(
        const Character&, Model*, eCharacterRenderPass, bool*);
    void ApplyDamageEffects(const Character&, Model*, int);
    void RenderCharacterShadow(const Character&, void*, int);
    static bool NoShadowCallback();

    u8 visible;
    u8 useObject;
    u8 flag2;
    u8 flag3;
    u8 megaEnabled;
    u8 flag5;
    u8 flag6;
    bool typeIsOne;
    u16 facingDirection;
    u16 headSpin;
    u16 headTilt;
    char _00E[2];
    nlVector3 position;
    nlVector3 bip01Position;
    nlVector3 headPosition;
    float height;
    float scale;
    float blendAmount;
    float state40;
    float shadowLevel;
    nlVector3 velocity;
    void* object;
    cPoseAccumulator* poseAccumulator;
    EffectsTexturing* effectsTexturing;
    Character* character;
    nlVector4 megaBasis;
    nlVector3 megaTranslation;
    float megaScale;
    float damage1;
    float damage2;
    u8 damageType;
    char _08D[3];
    u32 savedScorchTexture;
    u32 scorchTexture;
    ResolvedTexture resolvedScorchTexture;

    static Character* renderOnlyCharacter;
    static bool renderOpposingGoalie;
    static bool sCameraRelativeLighting;
};

#endif // _DRAWABLECHARACTER_H_

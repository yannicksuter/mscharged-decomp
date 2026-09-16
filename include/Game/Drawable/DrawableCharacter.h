#ifndef _DRAWABLECHARACTER_H_
#define _DRAWABLECHARACTER_H_

#include "Game/CharacterEffects.h"
#include "NL/nlMath.h"

struct glModel;
class cPoseNode;
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
    template <typename T>
    void Replay(T& frame);
    DrawableCharacter();
    ~DrawableCharacter();

    void Free();
    cPN_SAnimController& GetAnimController() const;
    void Grab(cCharacter& source);
    void BuildNodeMatrices(cPoseAccumulator* accumulator);
    void BuildNpcMatrix();
    void Render(cCharacter& source);
    void SendToGl(cCharacter& source, int renderPass);
    void Grab(SkinAnimatedMovableNPC& npc);
    void Render(SkinAnimatedMovableNPC& npc);
    void Blend(float* blendFactors, DrawableCharacter& lhs, DrawableCharacter& rhs);
    void EvaluateFrom(const cPoseNode& poseNode, const nlVector3& offset,
        unsigned short facingAngle, float poseScale);
    nlVector3 GetBallPosition();
    nlQuaternion GetBallOrientation();

    static void RenderOnlyOneCharacter(cCharacter& character, bool renderOpposingGoalieToo);
    static void RenderAllCharacters();
    static cCharacter* OnlyRenderingOneCharacter();

    void ApplyMaterialEffects(const cCharacter& source, glModel* model,
        eCharacterRenderPass renderPass, bool* attachEffects);
    void ApplyDamageEffects(const cCharacter& source, glModel* model, int renderPass);
    void RenderCharacterShadow(const cCharacter& source, glModel* model, int view);

    bool visible;
    bool useObject;
    bool flag2;
    bool flag3;
    bool megaEnabled;
    bool flag5;
    bool flag6;
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
    cPoseNode* object;
    cPoseAccumulator* poseAccumulator;
    EffectsTexturing* effectsTexturing;
    cCharacter* character;
    nlQuaternion megaBasis;
    nlVector3 megaTranslation;
    float megaScale;
    float damage1;
    float damage2;
    u8 damageType;
    char _08D[3];
    u32 savedScorchTexture;
    u32 scorchTexture;
    ResolvedTexture resolvedScorchTexture;

    static unsigned char sShadowRenderingDisabled;
    static cCharacter* spRenderOnlyThisCharacter;
    static bool sbRenderOpposingGoalieToo;
    static bool sSTSLighting;
    static bool sCameraRelativeLighting;
};


extern int g_nCharacterView;

#endif // _DRAWABLECHARACTER_H_

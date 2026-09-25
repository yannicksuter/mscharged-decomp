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


class SaveFrame;
class LoadFrame;
class cPN_Blender;
class cPN_Feather;
class cPN_SingleAxisBlender;
class cPN_8030E550;

void nlBreak();

template <int N>
void Replayable(LoadFrame& frame, char typeId, cPoseNode*& poseNode)
{
    if (N == 0 || frame.mInterval == N)
    {
        if (typeId == 0)
        {
            cPN_Blender* blender = new cPN_Blender;
            blender->Replay(frame);
            poseNode = blender;
        }
        else if (typeId == 1)
        {
            cPN_Feather* feather = new cPN_Feather;
            feather->Replay(frame);
            poseNode = feather;
        }
        else if (typeId == 2)
        {
            cPN_SAnimController* controller = new cPN_SAnimController;
            controller->Replay(frame);
            poseNode = controller;
        }
        else if (typeId == 3)
        {
            cPN_SingleAxisBlender* singleAxis = new cPN_SingleAxisBlender;
            singleAxis->Replay(frame);
            poseNode = singleAxis;
        }
        else if (typeId == 4)
        {
            cPN_8030E550* node = new cPN_8030E550;
            node->Replay(frame);
            poseNode = node;
        }
    }
}

template <int N>
void Replayable(SaveFrame& frame, char typeId, cPoseNode*& poseNode)
{
    if (N == 0 || frame.mInterval == N)
    {
        if (typeId < 0 || typeId >= 5)
            nlBreak();

        if (typeId == 0)
        {
            cPN_Blender* pn = (cPN_Blender*)poseNode;
            pn->Replay(frame);
        }
        else if (typeId == 1)
        {
            cPN_Feather* pn = (cPN_Feather*)poseNode;
            pn->Replay(frame);
        }
        else if (typeId == 2)
        {
            cPN_SAnimController* pn = (cPN_SAnimController*)poseNode;
            pn->Replay(frame);
        }
        else if (typeId == 3)
        {
            cPN_SingleAxisBlender* pn = (cPN_SingleAxisBlender*)poseNode;
            pn->Replay(frame);
        }
        else if (typeId == 4)
        {
            cPN_8030E550* pn = (cPN_8030E550*)poseNode;
            pn->Replay(frame);
        }
    }
}

extern "C" bool fn_8019464C(cCharacter* character);
extern "C" bool fn_80194660(cCharacter* character);
extern "C" bool fn_80194674(cCharacter* character);

template <typename T>
struct ReplayFrameTraits;
template <int Min, int Max, int Bits>
class FloatCompressor;
class UnidentifiedQuaternionCompressor;
template <int N, typename FrameType, typename T>
void ReplayablePolymorphic(FrameType& frame, T*& ptr);

template <typename T>
void DrawableCharacter::Replay(T& frame)
{
    bool usePoseAccumulator = false;
    Replayable<1>(frame, visible);
    Replayable<1>(frame, facingDirection);
    Replayable<1>(frame, FloatCompressor<-128, 128, 8>(position.x));
    Replayable<1>(frame, FloatCompressor<-128, 128, 8>(position.y));
    Replayable<1>(frame, FloatCompressor<-128, 128, 8>(position.z));
    if (character != 0)
    {
        Replayable<1>(frame, useObject);
        usePoseAccumulator = useObject;
        Replayable<1>(frame, FloatCompressor<0, 1, 8>(damage1));
        Replayable<1>(frame, FloatCompressor<0, 1, 8>(damage2));
        Replayable<1>(frame, damageType);
        Replayable<1>(frame, FloatCompressor<-128, 128, 8>(bip01Position.x));
        Replayable<1>(frame, FloatCompressor<-128, 128, 8>(bip01Position.y));
        Replayable<1>(frame, FloatCompressor<-128, 128, 8>(bip01Position.z));
        Replayable<1>(frame, FloatCompressor<-128, 128, 8>(headPosition.x));
        Replayable<1>(frame, FloatCompressor<-128, 128, 8>(headPosition.y));
        Replayable<1>(frame, FloatCompressor<-128, 128, 8>(headPosition.z));
        Replayable<1>(frame, FloatCompressor<-512, 512, 8>(velocity.x));
        Replayable<1>(frame, FloatCompressor<-512, 512, 8>(velocity.y));
        Replayable<1>(frame, FloatCompressor<-512, 512, 8>(velocity.z));
        Replayable<1>(frame, (unsigned long&)effectsTexturing);
        Replayable<1>(frame, FloatCompressor<0, 7, 13>(scale));
        if (ReplayFrameTraits<T>::IsLoadFrame)
            poseAccumulator->fn_801949E4(scale);
        Replayable<1>(frame, FloatCompressor<0, 1, 7>(blendAmount));
        Replayable<1>(frame, FloatCompressor<0, 7, 5>(state40));
        Replayable<1>(frame, FloatCompressor<0, 1, 7>(shadowLevel));
        Replayable<1>(frame, typeIsOne);
        if (fn_8019464C(character))
        {
            cCharacter* current = character;
            if (fn_80194660(current))
            {
                Replayable<1>(frame, megaEnabled);
                if (megaEnabled)
                {
                    Replayable<1>(frame, FloatCompressor<-128, 128, 8>(megaTranslation.x));
                    Replayable<1>(frame, FloatCompressor<-128, 128, 8>(megaTranslation.y));
                    Replayable<1>(frame, FloatCompressor<-128, 128, 8>(megaTranslation.z));
                    Replayable<1>(frame, UnidentifiedQuaternionCompressor(megaBasis));
                    Replayable<1>(frame, FloatCompressor<0, 7, 5>(megaScale));
                }
                else
                {
                    Replayable<1>(frame, flag3);
                }
                Replayable<1>(frame, flag2);
            }
            else if (fn_80194674(current))
            {
                Replayable<1>(frame, flag5);
                Replayable<1>(frame, flag6);
            }
        }
        if (!usePoseAccumulator && frame.GetInterval() == 1)
        {
            unsigned short headAngles;
            if (!ReplayFrameTraits<T>::IsLoadFrame)
                headAngles = (headSpin >> 8) | (headTilt != 0);
            Replayable<1>(frame, headAngles);
            if (ReplayFrameTraits<T>::IsLoadFrame)
            {
                headSpin = (headAngles & 0xFF) << 8;
                headTilt = headAngles & 0xFF00;
            }
        }
    }
    if (!usePoseAccumulator)
    {
        ReplayablePolymorphic<1>(frame, object);
        if (ReplayFrameTraits<T>::IsLoadFrame && frame.GetInterval() == 1)
        {
            poseAccumulator->InitAccumulators();
            object->Evaluate(1.0f, poseAccumulator);
            BuildNodeMatrices(poseAccumulator);
            delete object;
            object = 0;
        }
    }
    else if (frame.GetInterval() == 1)
    {
        Replayable<1>(frame, *poseAccumulator);
        if (ReplayFrameTraits<T>::IsLoadFrame)
            BuildNpcMatrix();
    }
}

extern int g_nCharacterView;

#endif // _DRAWABLECHARACTER_H_

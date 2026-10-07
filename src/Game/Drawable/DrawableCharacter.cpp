#include "NL/nlDLListContainer.inl"
#include "Game/Drawable/DrawableCharacter.h"
#include "Game/Render/StadiumLoading.h"
#include "Game/GameObjectLighting.h"
#include "Game/Character.h"
#include "Game/Player.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/Render/PeachPhoto.h"

#include "Game/BasicStadium.h"
#include "Game/CharacterEffects.h"
#include "Game/AI/HeadTrack.h"
#include "Game/GameInfo.h"
#include "Game/GL/ShaderSkinMesh.h"
#include "Game/PoseAccumulator.h"
#include "Game/PoseNode.h"
#include "Game/Render/RLView.h"
#include "Game/Render/RenderShadow.h"
#include "Game/Render/SkinAnimatedNPC.h"
#include "Game/Render/WorldNPC.h"
#include "Game/SHierarchy.h"
#include "Game/Team.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"

#include "NL/gl/glMaterialParameters.h"
#include "NL/glx/GXCharacterDamageMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "NL/gl/glTexture.h"
#include "Game/UnidentifiedStaticStorage.h"

static int g_nOnscreenUpdate[3] = { 2, 2, 2 };
static int g_nOffscreenUpdate[3] = { 4, 4, 4 };

float g_fAlphaValueOverride = 1.0f;
u8 g_bRenderScorchPass = 1;
float g_fRightPropSpineOffsetX = 0.25f;
float g_fRightPropSpineOffsetY = -0.22f;
float g_fRightPropSpineOffsetZ = -0.16f;
float g_fRightPropSpinePitch = -98.0f;
float g_fRightPropSpineYaw = -132.0f;
float g_fRightPropSpineRoll = 60.0f;
float g_fLeftPropSpineOffsetX = 0.26f;
float g_fLeftPropSpineOffsetY = -0.26f;
float g_fLeftPropSpineOffsetZ = 0.12f;
float g_fLeftPropSpinePitch = 60.0f;
float g_fLeftPropSpineYaw = 130.0f;
float g_fLeftPropSpineRoll = 28.0f;
int g_nPacketBIndex = 8;
int g_nCharacterView = eCLV_Characters;
float g_fDetailBlendAmount = 0.25f;
static float g_fRadiusScale = 1.175f;

static unsigned long LightTexture = glGetTexture("global/lightramp");
static unsigned long BlackTexture = glGetTexture("global/black");
static unsigned long WhiteTexture = glGetTexture("global/white");
int g_nLeftPropJointIndex;
int g_nRightPropJointIndex;
int g_nSpine1JointIndex;
float g_fForceDamage1;
float g_fForceDamage2;
float g_fMegaBlendOverride;
u8 g_bForceShockModel;
u8 g_bForceSoftwareSkinning;
u8 g_bForceLeftPropOnSpine;
u8 g_bForceRightPropOnSpine;
u8 g_bIgnoreFrozenHammerTransform;
u8 g_bForceHidePacketA;
u8 g_bForceHidePacketB;
int g_nPacketAIndex;

unsigned char DrawableCharacter::sShadowRenderingDisabled;
cCharacter* DrawableCharacter::spRenderOnlyThisCharacter = 0;
bool DrawableCharacter::sbRenderOpposingGoalieToo = false;
bool DrawableCharacter::sSTSLighting = false;
bool DrawableCharacter::sCameraRelativeLighting = false;

static inline void BlendTranslationAccum(
    TransAccum& output,
    const TransAccum& lhs,
    const TransAccum& rhs,
    float weight)
{
    output.fAccumulatedWeight = 1.0f;
    if (lhs.bIdentity && rhs.bIdentity)
    {
        output.bIdentity = true;
        nlVec3Set(output.t, 0.0f, 0.0f, 0.0f);
    }
    else
    {
        output.bIdentity = false;
        nlVecLerp(output.t, lhs.t, rhs.t, weight);
    }
}

static inline void BlendScaleAccum(
    ScaleAccum& output,
    const ScaleAccum& lhs,
    const ScaleAccum& rhs,
    float weight)
{
    output.fAccumulatedWeight = 1.0f;
    if (lhs.bIdentity && rhs.bIdentity)
    {
        output.bIdentity = true;
        nlVec3Set(output.s, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        output.bIdentity = false;
        nlVecLerp(output.s, lhs.s, rhs.s, weight);
    }
}

DrawableCharacter::DrawableCharacter()
{
    visible = true;
    useObject = false;
    leftPropAnimated = true;
    rightPropAnimated = true;
    hammerTransformFrozen = false;
    packetAVisible = true;
    packetBVisible = true;
    useShockModel = false;
    facingDirection = 0;
    headSpin = 0;
    headTilt = 0;
    height = 0.0f;
    scale = 1.0f;
    blendAmount = 1.0f;
    megaBlend = 0.0f;
    shadowLevel = 1.0f;
    object = 0;
    poseAccumulator = 0;
    effectsTexturing = 0;
    character = 0;
    damage1 = 0.0f;
    damage2 = 0.0f;
    damageType = 0;
    savedScorchTexture = 0xFFFFFFFF;
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
    frozenHammerRotation.z = 0.0f;
    frozenHammerRotation.y = 0.0f;
    frozenHammerRotation.x = 0.0f;
    frozenHammerRotation.w = 1.0f;
    frozenHammerTranslation.x = 0.0f;
    frozenHammerTranslation.y = 0.0f;
    frozenHammerTranslation.z = -10.0f;
    frozenHammerScale = 1.0f;
}

DrawableCharacter::~DrawableCharacter()
{
    delete poseAccumulator;
}

void DrawableCharacter::Free()
{
    delete poseAccumulator;
    poseAccumulator = 0;
}

cPN_SAnimController& DrawableCharacter::GetAnimController() const
{
    return *character->m_pCurrentAnimController;
}

void DrawableCharacter::Grab(cCharacter& source)
{
    character = &source;
    position = source.mUnidentified024.m_v3Position;
    bip01Position = source.GetJointPosition(source.m_nBip01JointIndex_0xA4);
    headPosition = source.GetJointPosition(source.m_nHeadJointIndex);
    height = bip01Position.z;
    scale = source.mUnidentified024.m_fPlayerScale;
    leftPropAnimated = source.m_bLeftPropAnimated;
    rightPropAnimated = source.m_bRightPropAnimated;
    hammerTransformFrozen = source.mUnidentified180;
    if (hammerTransformFrozen)
    {
        frozenHammerTranslation = source.mUnidentified194;
        frozenHammerRotation = source.mUnidentified184;
        frozenHammerScale = source.mUnidentified1A0;
    }
    packetAVisible = source.mUnidentified181;
    packetBVisible = source.mUnidentified182;
    useShockModel = source.m_ModelType == 1;
    blendAmount = source.mUnidentified178;
    megaBlend = source.mUnidentified1A8;
    shadowLevel = source.mUnidentified17C ? 1.0f : 0.0f;
    shadowLevel *= blendAmount;
    velocity = source.mUnidentified024.m_v3Velocity;
    facingDirection = source.mUnidentified024.m_aActualFacingDirection;
    headSpin = (unsigned short)source.m_pHeadTrack->m_fHeadSpin;
    headTilt = (unsigned short)source.m_pHeadTrack->m_fHeadTilt;
    visible = true;
    useObject = source.m_pPoseAccumulator->m_bUseObject;
    damage1 = source.GetDirt();
    damage2 = source.GetMinDirt();
    damageType = source.mUnidentified16C;
    if (!useObject)
    {
        object = source.m_pPoseTree;
    }

    if (poseAccumulator == 0)
    {
        cPoseAccumulator* p = (cPoseAccumulator*)nlMalloc(sizeof(cPoseAccumulator), 8, false);
        p = new (p) cPoseAccumulator(*source.m_pPoseAccumulator);
        poseAccumulator = p;
    }
    else
    {
        *poseAccumulator = *source.m_pPoseAccumulator;
    }

    if (g_bIgnoreFrozenHammerTransform == 0 && hammerTransformFrozen)
    {
        nlMatrix4 matrix;
        nlQuatToMatrix(matrix, frozenHammerRotation, true);
        for (int row = 0; row < 3; ++row)
        {
            for (int column = 0; column < 3; ++column)
            {
                matrix.e2[row][column] *= frozenHammerScale;
            }
        }
        matrix.SetTranslation(frozenHammerTranslation);

        poseAccumulator->m_NodeMatrices[g_nRightPropJointIndex] = matrix;
    }

    EffectsTexturing* tex = source.m_pEffectsTexturing;
    if (tex == 0)
    {
        tex = fxGetTexturing(eFXTex_Nothing);
    }
    effectsTexturing = tex;
}

static void DrawableCharacterHeadTrackCallback(unsigned int ctx, unsigned int, cPoseAccumulator* poseAccumulator, unsigned int currentNodeIndex, int)
{
    DrawableCharacter* drawableChar = (DrawableCharacter*)ctx;
    CalcHeadTrackMatrix(drawableChar->headSpin, drawableChar->headTilt, poseAccumulator, currentNodeIndex);
}

void DrawableCharacter::BuildNodeMatrices(cPoseAccumulator* accumulator)
{
    nlMatrix4 worldMatrix;
    float angle = 0.0000958738f * (float)facingDirection;
    nlMakeRotationMatrixZ(worldMatrix, angle);
    worldMatrix.SetTranslation(position);

    if (character != 0)
    {
        accumulator->SetBuildNodeMatrixCallback(character->m_nHeadJointIndex, DrawableCharacterHeadTrackCallback, (unsigned int)this, 0);
    }

    accumulator->BuildNodeMatrices(worldMatrix);

    if (character != 0)
    {
        accumulator->SetBuildNodeMatrixCallback(character->m_nHeadJointIndex, 0, 0, 0);
    }
}

void DrawableCharacter::BuildNpcMatrix()
{
    nlMatrix4 worldMatrix;
    float angle = 0.0000958738f * (float)facingDirection;
    nlMakeRotationMatrixZ(worldMatrix, angle);
    worldMatrix.SetTranslation(position);
    poseAccumulator->BuildNodeMatricesFromQuaternions(worldMatrix);
}

void DrawableCharacter::Render(cCharacter& source)
{
    if (!visible)
    {
        return;
    }

    bool renderShockModel = false;
    if (useShockModel
        || (g_bForceShockModel != 0 && source.fn_8001C534(1)))
    {
        renderShockModel = true;
    }

    if (renderShockModel)
    {
        source.PoseSkinMesh(poseAccumulator, 1);
    }

    if (!renderShockModel || g_bRenderScorchPass != 0)
    {
        if (nlTaskManager::m_pInstance->mCurrentState == 2)
        {
            source.PoseSkinMesh(poseAccumulator, 2);
        }
        else
        {
            source.PoseSkinMesh(poseAccumulator, 0);
        }
    }
    source.PoseSkinMesh(poseAccumulator, 3);

    cCharacter* renderOnly = spRenderOnlyThisCharacter;
    if (renderOnly == 0 || renderOnly == &source
        || (sbRenderOpposingGoalieToo
            && &source == (cCharacter*)((cPlayer*)renderOnly)->m_pTeam->GetOtherTeam()->GetGoalie()))
    {
        if (renderShockModel)
        {
            if (g_bRenderScorchPass != 0)
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

    source.fn_8001C574();
}

void DrawableCharacter::SendToGl(cCharacter& source, int renderPass)
{
    GLSkinMesh* skinMesh;
    int characterClass = source.mUnidentified024.m_eCharacterClass;
    int view = g_nCharacterView;
    if (gPeachPhotoState.state == 1)
    {
        view = eCLV_MoreCharacters;
        if (source.mUnidentified17D)
        {
            view = eCLV_Characters;
        }
    }
    if (characterClass == 5 && source.mUnidentified1A8 > 0.0f)
    {
        view = eCLV_HighRange3D;
    }

    if (renderPass != 2)
    {
        if (nlTaskManager::m_pInstance->mCurrentState == 2)
        {
            skinMesh = source.GetSkinMesh(2);
        }
        else
        {
            skinMesh = source.GetSkinMesh(0);
        }
    }
    else
    {
        skinMesh = source.GetSkinMesh(1);
    }

    if (sSTSLighting)
    {
        SetGameObjectLightingMode(2);
    }
    else if (sCameraRelativeLighting || AlwaysUseCameraRelativeCharacterLighting())
    {
        SetGameObjectLightingMode(1);
    }
    else
    {
        SetGameObjectLightingMode(0);
    }

    bool isVisible;
    if (nlTaskManager::m_pInstance->mCurrentState == 0x10)
    {
        isVisible = true;
    }
    else if (IsStadiumWorldLoaded())
    {
        float fRadius;
        if (characterClass == 3)
        {
            fRadius = 3.5f;
        }
        else
        {
            fRadius = 2.5f;
        }
        isVisible = ClassifySphereInFrustum(
            GetShadowedView()->m_Interface->GetShadowMatrix(), &bip01Position, fRadius) != 0;
    }
    else
    {
        isVisible = true;
    }

    if (!isVisible)
    {
        return;
    }

    skinMesh->m_Unknown0C = 1;
    if (g_bForceSoftwareSkinning
        || (nlTaskManager::m_pInstance->mCurrentState & 0x18) != 0
        || (nlTaskManager::m_pInstance->mCurrentState & 8) != 0
        || (nlTaskManager::m_pInstance->mCurrentState & 0x20000) != 0)
    {
        skinMesh->m_Unknown0C = 0;
    }
    skinMesh->PrepareToRender();
    glModel* pModel = glModelDupNoStreams(skinMesh->GetModel(), false, 0);
    ApplyDamageEffects(source, pModel, renderPass);
    bool attachEffects = false;
    ApplyMaterialEffects(source, pModel, (eCharacterRenderPass)renderPass, &attachEffects);
    if (attachEffects)
    {
        source.PerformBlinking(skinMesh, pModel);
    }

    static u32 alphaValueHash = nlStringLowerHash("alphaValue");
    if (characterClass == 10)
    {
        int numPackets = pModel->numPackets;
        if ((g_bForceHidePacketA || !packetAVisible) && g_nPacketAIndex < numPackets)
        {
            glSetMaterialFloatParameter(&pModel->packets[g_nPacketAIndex], alphaValueHash, 0.0f);
        }
        if ((g_bForceHidePacketB || !packetBVisible) && g_nPacketBIndex < numPackets)
        {
            glSetMaterialFloatParameter(&pModel->packets[g_nPacketBIndex], alphaValueHash, 0.0f);
        }
    }

    AttachModelToLayerView((eCLV)view, pModel, 0);
    if (characterClass == 5)
    {
        view = eCLV_MoreCharacters;
    }

    GLSkinMesh* shadowMesh = source.GetSkinMesh(3);
    if (shadowMesh != skinMesh)
    {
        shadowMesh->m_Unknown0C = 1;
        if (g_bForceSoftwareSkinning
            || (nlTaskManager::m_pInstance->mCurrentState & 0x18) != 0
            || (nlTaskManager::m_pInstance->mCurrentState & 8) != 0
            || (nlTaskManager::m_pInstance->mCurrentState & 0x20000) != 0)
        {
            shadowMesh->m_Unknown0C = 0;
        }
        shadowMesh->PrepareToRender();
        pModel = glModelDupNoStreams(shadowMesh->GetModel(), false, 0);
    }
    RenderCharacterShadow(source, pModel, view);
}

void DrawableCharacter::Grab(SkinAnimatedNPC& npc)
{
    position = npc.mv3Position;
    nlMatrix4& nodeMatrix = npc.mpPoseAccumulator->GetNodeMatrix(0);
    height = nodeMatrix.m43;
    facingDirection = npc.maFacingDirection;
    object = npc.mpPoseTree;
    visible = npc.mbIsVisible;

    if (poseAccumulator == 0)
    {
        cPoseAccumulator* p = (cPoseAccumulator*)nlMalloc(sizeof(cPoseAccumulator), 8, false);
        p = new (p) cPoseAccumulator(*npc.mpPoseAccumulator);
        poseAccumulator = p;
    }
    else
    {
        *poseAccumulator = *npc.mpPoseAccumulator;
    }
}

void DrawableCharacter::Render(SkinAnimatedNPC& npc)
{
    if (!visible)
    {
        return;
    }

    nlMatrix4 worldMatrix;
    float angle = 0.0000958738f * (float)facingDirection;
    nlMakeRotationMatrixZ(worldMatrix, angle);

    worldMatrix.SetRow_(3, position);

    npc.mbIsVisible = visible;
    npc.RenderFromReplay(*poseAccumulator, &worldMatrix);
}

void DrawableCharacter::Blend(const float* blendFactors, DrawableCharacter& lhs, DrawableCharacter& rhs)
{
    const float rhsWeight = *blendFactors;
    const float lhsWeight = 1.0f - rhsWeight;

    visible = lhs.visible && rhs.visible;
    bool normalBlend = false;
    bool isHammerBro = false;
    character = lhs.character;

    if (character != 0)
    {
        if (character->m_eClassType == 2)
        {
            if (character->mUnidentified024.m_eCharacterClass == 13)
            {
                isHammerBro = true;
                if (g_nLeftPropJointIndex <= 0)
                {
                    cSHierarchy* hierarchy = lhs.poseAccumulator->m_BaseSHierarchy;
                    g_nLeftPropJointIndex = hierarchy->GetNodeIndexByID(nlStringLowerHash("bip01 l prop"));
                    hierarchy = lhs.poseAccumulator->m_BaseSHierarchy;
                    g_nRightPropJointIndex = hierarchy->GetNodeIndexByID(nlStringLowerHash("bip01 r prop"));
                    hierarchy = lhs.poseAccumulator->m_BaseSHierarchy;
                    g_nSpine1JointIndex = hierarchy->GetNodeIndexByID(nlStringLowerHash("bip01 spine1"));
                }

                hammerTransformFrozen = rhs.hammerTransformFrozen;
                if (hammerTransformFrozen)
                {
                    frozenHammerTranslation = rhs.frozenHammerTranslation;
                    frozenHammerRotation = rhs.frozenHammerRotation;
                    frozenHammerScale = rhs.frozenHammerScale;
                }
                leftPropAnimated = rhs.leftPropAnimated;
                rightPropAnimated = rhs.rightPropAnimated;
            }
            else
            {
                hammerTransformFrozen = false;
                leftPropAnimated = true;
                rightPropAnimated = true;
            }
            packetAVisible = rhs.packetAVisible;
            packetBVisible = rhs.packetBVisible;
        }

        damage1 = lhs.damage1;
        damage2 = lhs.damage2;
        damageType = lhs.damageType;
        nlVecLerp(bip01Position, lhs.bip01Position, rhs.bip01Position, rhsWeight);
        nlVecLerp(headPosition, lhs.headPosition, rhs.headPosition, rhsWeight);
        velocity = lhs.velocity;
        scale = lhs.scale * lhsWeight + rhsWeight * rhs.scale;
        blendAmount = lhs.blendAmount * lhsWeight + rhsWeight * rhs.blendAmount;
        useShockModel = lhs.useShockModel;
        megaBlend = lhs.megaBlend * lhsWeight + rhsWeight * rhs.megaBlend;
        shadowLevel = lhs.shadowLevel * lhsWeight + rhsWeight * rhs.shadowLevel;
        effectsTexturing = lhs.effectsTexturing;

        if (!lhs.useObject && !rhs.useObject)
        {
            int headSpinOffset = (short)(rhsWeight * (float)(short)(rhs.headSpin - lhs.headSpin));
            headSpin = lhs.headSpin + headSpinOffset;
            normalBlend = true;
            int headTiltOffset = (short)(rhsWeight * (float)(short)(rhs.headTilt - lhs.headTilt));
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

    nlVecLerp(position, lhs.position, rhs.position, rhsWeight);
    int facingOffset = (short)(rhsWeight * (float)(short)(rhs.facingDirection - lhs.facingDirection));
    facingDirection = lhs.facingDirection + facingOffset;
    height = lhs.scale * lhsWeight + rhsWeight * rhs.scale;

    if (poseAccumulator == 0)
    {
        poseAccumulator = new (nlMalloc(sizeof(cPoseAccumulator), 8, false)) cPoseAccumulator(lhs.poseAccumulator->m_BaseSHierarchy, false);
    }

    poseAccumulator->m_Scale = scale;
    poseAccumulator->InitAccumulators();

    for (int i = 0; i < 20; i++)
    {
        float lhsMorphWeight = lhs.poseAccumulator->m_MorphWeights[i];
        poseAccumulator->m_MorphWeights[i] += lhsMorphWeight * lhsWeight;
        float rhsMorphWeight = rhs.poseAccumulator->m_MorphWeights[i];
        poseAccumulator->m_MorphWeights[i] += rhsMorphWeight * rhsWeight;
    }

    if (normalBlend)
    {
        for (int i = 0; i < poseAccumulator->GetNumNodes(); i++)
        {
            RotAccum& lhsRot = lhs.poseAccumulator->m_rot[i];
            RotAccum& rhsRot = rhs.poseAccumulator->m_rot[i];
            float lhsRotAroundZWeight = lhsRot.rotAroundZAccumulatedWeight * lhsWeight;
            float rhsRotAroundZWeight = rhsRot.rotAroundZAccumulatedWeight * rhsWeight;
            poseAccumulator->BlendRotAroundZ(i, lhsRot.rotAroundZ, lhsRotAroundZWeight);
            poseAccumulator->BlendRotAroundZ(i, rhsRot.rotAroundZ, rhsRotAroundZWeight);
            float lhsQuatWeight = lhsRot.quatAccumulatedWeight * lhsWeight;
            float rhsQuatWeight = rhsRot.quatAccumulatedWeight * rhsWeight;
            poseAccumulator->BlendRot(i, &lhsRot.q, lhsQuatWeight, false);
            poseAccumulator->BlendRot(i, &rhsRot.q, rhsQuatWeight, false);

            BlendTranslationAccum(
                poseAccumulator->m_trans[i],
                lhs.poseAccumulator->m_trans[i],
                rhs.poseAccumulator->m_trans[i],
                rhsWeight);
            BlendScaleAccum(
                poseAccumulator->m_scale[i],
                lhs.poseAccumulator->m_scale[i],
                rhs.poseAccumulator->m_scale[i],
                rhsWeight);
        }

        BuildNodeMatrices(poseAccumulator);
    }
    else
    {
        if (!lhs.useObject)
        {
            BuildNodeMatrices(lhs.poseAccumulator);
        }
        else if (!rhs.useObject)
        {
            BuildNodeMatrices(rhs.poseAccumulator);
        }

        for (int i = 0; i < poseAccumulator->GetNumNodes(); i++)
        {
            nlQuatNLerp(
                poseAccumulator->GetNodeQuaternion(i),
                lhs.poseAccumulator->GetNodeQuaternion(i),
                rhs.poseAccumulator->GetNodeQuaternion(i),
                rhsWeight);
            BlendTranslationAccum(
                poseAccumulator->m_trans[i],
                lhs.poseAccumulator->m_trans[i],
                rhs.poseAccumulator->m_trans[i],
                rhsWeight);
            BlendScaleAccum(
                poseAccumulator->m_scale[i],
                lhs.poseAccumulator->m_scale[i],
                rhs.poseAccumulator->m_scale[i],
                rhsWeight);
        }

        BuildNpcMatrix();
    }

    if (isHammerBro)
    {
        if (!g_bIgnoreFrozenHammerTransform && hammerTransformFrozen)
        {
            nlMatrix4 matrix;
            nlQuatToMatrix(matrix, frozenHammerRotation, true);
            for (int row = 0; row < 3; ++row)
            {
                for (int column = 0; column < 3; ++column)
                {
                    matrix.e2[row][column] *= frozenHammerScale;
                }
            }
            matrix.SetTranslation(frozenHammerTranslation);
            poseAccumulator->m_NodeMatrices[g_nRightPropJointIndex] = matrix;
        }
        else if (g_bForceRightPropOnSpine || !rightPropAnimated)
        {
            nlMatrix4 rotation;
            nlMakeRotationMatrixEulerAngles(
                rotation,
                DegreesToRadians(g_fRightPropSpinePitch),
                DegreesToRadians(g_fRightPropSpineYaw),
                DegreesToRadians(g_fRightPropSpineRoll));
            rotation.SetRow4_(3, g_fRightPropSpineOffsetX, g_fRightPropSpineOffsetY, g_fRightPropSpineOffsetZ, 1.0f);
            nlMatrix4 matrix;
            nlMultMatrices(matrix, rotation, poseAccumulator->GetNodeMatrix(g_nSpine1JointIndex));
            poseAccumulator->m_NodeMatrices[g_nRightPropJointIndex] = matrix;
        }

        if (g_bForceLeftPropOnSpine || !leftPropAnimated)
        {
            nlMatrix4 rotation;
            nlMakeRotationMatrixEulerAngles(
                rotation,
                DegreesToRadians(g_fLeftPropSpinePitch),
                DegreesToRadians(g_fLeftPropSpineYaw),
                DegreesToRadians(g_fLeftPropSpineRoll));
            rotation.SetRow4_(3, g_fLeftPropSpineOffsetX, g_fLeftPropSpineOffsetY, g_fLeftPropSpineOffsetZ, 1.0f);
            nlMatrix4 matrix;
            nlMultMatrices(matrix, rotation, poseAccumulator->GetNodeMatrix(g_nSpine1JointIndex));
            poseAccumulator->m_NodeMatrices[g_nLeftPropJointIndex] = matrix;
        }
    }
}

void DrawableCharacter::EvaluateFrom(const cPoseNode& poseNode, const nlVector3& offset, unsigned short facingAngle, float poseScale)
{
    position = offset;
    nlVec3Set(velocity, 0.0f, 0.0f, 0.0f);
    facingDirection = facingAngle;
    headSpin = 0;
    headTilt = 0;
    height = 0.0f;
    scale = poseScale;
    leftPropAnimated = false;
    rightPropAnimated = false;
    hammerTransformFrozen = false;
    packetAVisible = false;
    packetBVisible = false;
    useShockModel = false;
    blendAmount = 1.0f;
    megaBlend = 0.0f;
    damage1 = character->GetDirt();
    damage2 = character->GetMinDirt();
    damageType = character->mUnidentified16C;
    shadowLevel = 1.0f;

    poseAccumulator->m_Scale = poseScale;
    poseAccumulator->InitAccumulators();

    poseNode.Evaluate(1.0f, poseAccumulator);

    effectsTexturing = fxGetTexturing(eFXTex_Nothing);

    BuildNodeMatrices(poseAccumulator);

    nlMatrix4& bip01Matrix = poseAccumulator->GetNodeMatrix(character->m_nBip01JointIndex_0xA4);
    bip01Position = bip01Matrix.GetTranslation();

    nlMatrix4& headMatrix = poseAccumulator->GetNodeMatrix(character->m_nHeadJointIndex);
    headPosition = headMatrix.GetTranslation();
}

nlVector3 DrawableCharacter::GetBallPosition()
{
    cPlayer* pPlayer = (cPlayer*)character;
    nlMatrix4& matrix = poseAccumulator->GetNodeMatrix(pPlayer->m_nBallJointIndex);
    return matrix.GetTranslation();
}

nlQuaternion DrawableCharacter::GetBallOrientation()
{
    nlQuaternion result;
    nlMatrix4 matrix;
    int ballJointIndex = ((cPlayer*)character)->m_nBallJointIndex;

    if (1.0f != scale)
    {
        matrix = poseAccumulator->GetNodeMatrix(ballJointIndex);

        float reciprocalLength = nlRecipSqrt(nlVec3LengthSquared(*(nlVector3*)&matrix.e2[0][0]), true);
        nlVec3Scale(*(nlVector3*)&matrix.e2[0][0], reciprocalLength);

        reciprocalLength = nlRecipSqrt(nlVec3LengthSquared(*(nlVector3*)&matrix.e2[1][0]), true);
        nlVec3Scale(*(nlVector3*)&matrix.e2[1][0], reciprocalLength);

        reciprocalLength = nlRecipSqrt(nlVec3LengthSquared(*(nlVector3*)&matrix.e2[2][0]), true);
        nlVec3Scale(*(nlVector3*)&matrix.e2[2][0], reciprocalLength);

        nlMatrixToQuat(result, matrix);
    }
    else
    {
        nlMatrixToQuat(result, poseAccumulator->GetNodeMatrix(ballJointIndex));
    }

    return result;
}

void DrawableCharacter::RenderOnlyOneCharacter(cCharacter& character, bool renderOpposingGoalieToo)
{
    spRenderOnlyThisCharacter = &character;
    sbRenderOpposingGoalieToo = renderOpposingGoalieToo;
}

void DrawableCharacter::RenderAllCharacters()
{
    spRenderOnlyThisCharacter = 0;
    sbRenderOpposingGoalieToo = false;
}

cCharacter* DrawableCharacter::OnlyRenderingOneCharacter()
{
    return spRenderOnlyThisCharacter;
}

static inline void ApplyTexture(glModel* model, unsigned long texture, ResolvedTexture resolvedTexture)
{
    for (glModelPacket* packet = model->packets; packet < model->packets + model->numPackets; packet++)
    {
        if (texture != glGetMaterialUnsignedParameter(packet, gDiffuseTextureSemantic))
        {
            glSetMaterialTextureParameter(packet, gDiffuseTextureSemantic, texture);
            unsigned long packetTexture = resolvedTexture.value;
            glSetMaterialTextureIndexParameter(packet, gDiffuseTextureSemantic, &packetTexture);
        }
    }
}

void DrawableCharacter::ApplyMaterialEffects(const cCharacter& source, glModel* model, eCharacterRenderPass renderPass, bool* attachEffects)
{
    static u32 blendAmountHash = nlStringLowerHash("blendAmount");

    EffectsTexturing* fxtex = effectsTexturing;
    int characterClass = source.mUnidentified024.m_eCharacterClass;
    u32 texture;
    glModelPacket* pPacket;

    if (fxtex != 0 && fxtex->m_uTexture == 0xFFFFFFFF)
    {
        fxtex = 0;
    }
    if (fxtex != 0 && fxtex->m_bDetail)
    {
        fxtex = 0;
    }

    if (fxtex != 0)
    {
        for (glModelPacket* pFxPacket = model->packets; pFxPacket < model->packets + model->numPackets; pFxPacket++)
        {
            unsigned int& raster = pFxPacket->rasterState;
            if (fxtex->m_eBlendMode != GLB_None)
            {
                glSetRasterState(raster, GLS_AlphaBlend, fxtex->m_eBlendMode);
            }

            if (fxtex->m_bDetail)
            {
                glSetMaterialTextureParameter(pFxPacket, gDetailTextureSemantic, fxtex->m_uTexture);
                unsigned long resolvedTexture = fxtex->m_ResolvedTexture.value;
                glSetMaterialTextureIndexParameter(pFxPacket, gDetailTextureSemantic, &resolvedTexture);
                glSetMaterialFloatParameter(pFxPacket, blendAmountHash, g_fDetailBlendAmount);
            }
            else
            {
                glSetMaterialTextureParameter(pFxPacket, gDiffuseTextureSemantic, fxtex->m_uTexture);
                unsigned long resolvedTexture = fxtex->m_ResolvedTexture.value;
                glSetMaterialTextureIndexParameter(pFxPacket, gDiffuseTextureSemantic, &resolvedTexture);
            }
        }
    }
    else
    {
        texture = source.mUnidentified100;
        if (renderPass == CRP_Default)
        {
            if (texture != source.mUnidentified104)
            {
                for (pPacket = model->packets; pPacket < model->packets + model->numPackets; pPacket++)
                {
                    if (texture == glGetMaterialUnsignedParameter(pPacket, gDiffuseTextureSemantic))
                    {
                        glSetMaterialTextureParameter(pPacket, gDiffuseTextureSemantic, source.mUnidentified104);
                        unsigned long resolvedTexture = source.mUnidentified110.value;
                        glSetMaterialTextureIndexParameter(pPacket, gDiffuseTextureSemantic, &resolvedTexture);
                    }
                }
            }
            *attachEffects = true;
        }
        else if (renderPass == CRP_Scorch)
        {
            if (scorchTexture == 0)
            {
                scorchTexture = glGetTexture("global/black");
                resolvedScorchTexture.value = glGetTextureManager()->GetTextureIndex(scorchTexture);
            }
            ApplyTexture(model, scorchTexture, resolvedScorchTexture);
        }
        else if (renderPass == CRP_Shock)
        {
            if (characterClass != 0)
            {
                ApplyTexture(model, source.mUnidentified108, source.mUnidentified114);
            }
        }
    }
}

void DrawableCharacter::ApplyDamageEffects(const cCharacter& source, glModel* model, int renderPass)
{
    glModelPacket* pPacket;
    static u32 shadowLevelHash = nlStringLowerHash("shadowLevel");

    int shadowAlpha = GetShadowLookupLevel(&bip01Position);
    float fade = 1.0f;
    if (bip01Position.z > fade)
    {
        if (bip01Position.z > 10.0f)
        {
            fade = 0.0f;
        }
        else
        {
            fade = (10.0f - bip01Position.z) / 9.0f;
        }
    }

    nlColour shadowColour;
    u8 channel = 0xFF - (int)((float)(0xFF - shadowAlpha) * fade);
    shadowColour.c[3] = 1;
    shadowColour.c[0] = channel;
    shadowColour.c[1] = channel;
    shadowColour.c[2] = channel;
    unsigned long shadowColourValue = *(u32*)&shadowColour;

    pPacket = model->packets;
    while (pPacket < model->packets + model->numPackets)
    {
        glSetMaterialUnsignedParameter(pPacket, shadowLevelHash, shadowColourValue);
        pPacket++;
    }

    static u32 alphaValueHash = nlStringLowerHash("alphaValue");
    float alphaValue = 1.0f != g_fAlphaValueOverride ? g_fAlphaValueOverride : blendAmount;
    if (alphaValue != 1.0f)
    {
        for (pPacket = model->packets; pPacket < model->packets + model->numPackets; pPacket++)
        {
            glSetMaterialFloatParameter(pPacket, alphaValueHash, alphaValue);
        }
    }

    static u32 megaBlendHash = nlStringLowerHash("megaBlend");
    float megaAmount = 0.0f != g_fMegaBlendOverride ? g_fMegaBlendOverride : source.mUnidentified1A8;
    if (megaAmount != 0.0f)
    {
        for (pPacket = model->packets; pPacket < model->packets + model->numPackets; pPacket++)
        {
            if (glHasMaterialParameter(pPacket, megaBlendHash)
                && glGetMaterialFloatParameter(pPacket, megaBlendHash) >= 0.0f)
            {
                glSetMaterialFloatParameter(pPacket, megaBlendHash, megaAmount);
            }
        }
    }

    if (renderPass == 1)
    {
        for (pPacket = model->packets; pPacket < model->packets + model->numPackets; pPacket++)
        {
            glSetRasterState(pPacket->rasterState, GLS_Culling, 2);
        }
    }

    static u32 damage1EnabledHash = nlStringLowerHash("damage1Enabled");
    bool damage1Enabled = false;
    if (g_fForceDamage1 > 0.0f || damage1 > 0.0f)
    {
        damage1Enabled = true;
    }
    if (damage1Enabled)
    {
        int stadium = GameInfoManager::Instance()->GetStadium();
        unsigned long damageTexture = glGetTexture("global/scorch");
        bool useDamageTexture = false;
        if (damageType != 1
            && (damageType == 2
                || stadium == 0
                || stadium == 5
                || stadium == 7
                || stadium == 9
                || stadium == 0x10
                || stadium == 8
                || stadium == 0xE
                || stadium == 0xB
                || stadium == 0xF
                || stadium == 1
                || stadium == 6))
        {
            useDamageTexture = true;
        }
        if (damage2 > 0.0f)
        {
            useDamageTexture = true;
        }

        for (pPacket = model->packets; pPacket < model->packets + model->numPackets; pPacket++)
        {
            if (glHasMaterialParameter(pPacket, damage1EnabledHash))
            {
                glSetMaterialUnsignedParameter(pPacket, damage1EnabledHash, 1);
                if (useDamageTexture)
                {
                    glTextureBinding& damageBinding = static_cast<GXCharacterDamageParameters*>(pPacket->materialParameters)->damage1Texture;
                    if (savedScorchTexture == 0xFFFFFFFF)
                    {
                        savedScorchTexture = damageBinding.texture;
                    }
                    damageBinding.texture = damageTexture;
                    damageBinding.textureIndex = 0xFFFF;
                }
                else if (savedScorchTexture != 0xFFFFFFFF)
                {
                    glTextureBinding& damageBinding = static_cast<GXCharacterDamageParameters*>(pPacket->materialParameters)->damage1Texture;
                    damageBinding.texture = savedScorchTexture;
                    damageBinding.textureIndex = 0xFFFF;
                    savedScorchTexture = 0xFFFFFFFF;
                }
            }
        }
    }

    static u32 damage2EnabledHash = nlStringLowerHash("damage2Enabled");
    bool damage2Enabled = false;
    if (g_fForceDamage2 > 0.0f || damage2 > 0.0f)
    {
        damage2Enabled = true;
    }
    if (damage2Enabled)
    {
        for (pPacket = model->packets; pPacket < model->packets + model->numPackets; pPacket++)
        {
            if (glHasMaterialParameter(pPacket, damage2EnabledHash))
            {
                glSetMaterialUnsignedParameter(pPacket, damage2EnabledHash, 1);
            }
        }
    }
}

void DrawableCharacter::RenderCharacterShadow(const cCharacter& source, glModel* model, int view)
{
    ProjectedShadowParams params;
    BasicStadium* stadium;
    const CharacterInfo* info;
    float fHeight;
    float fRadius;
    int characterSizeIndex;
    float alphaValue;
    float fScalar = shadowLevel;

    if (sShadowRenderingDisabled || fScalar < 0.001f)
    {
        return;
    }

    stadium = BasicStadium::GetCurrentStadium();
    alphaValue = 1.0f != g_fAlphaValueOverride ? g_fAlphaValueOverride : blendAmount;

    static u32 alphaValueHash = nlStringLowerHash("alphaValue");
    static float s_fHeightFudge = 1.125f;
    params.fScalar = 1.0f;
    info = source.mUnidentified11C;
    float shadowRadius = info->unknown_0x30.unknown_0x4;
    float shadowHeight = info->unknown_0x30.unknown_0x0;
    characterSizeIndex = info->unknown_0x2C;
    fRadius = g_fRadiusScale * shadowRadius;
    fHeight = s_fHeightFudge * shadowHeight;
    float characterScale = source.mUnidentified024.m_fPlayerScale;
    nlVec4Set(
        params.vLight,
        stadium->m_shadowLightPosition.x,
        stadium->m_shadowLightPosition.y,
        stadium->m_shadowLightPosition.z,
        1.0f);
    params.vPosition = bip01Position;
    params.fRadius = characterScale * fRadius;
    params.fHeight = characterScale * fHeight;
    params.pModel = 0;
    params.fScalar = fScalar;
    params.nPartitionIndex = source.mUnidentified120;

    if (nlTaskManager::m_pInstance->mCurrentState == 2)
    {
        params.nVisibleInterval = g_nOnscreenUpdate[characterSizeIndex];
        params.nInvisibleInterval = g_nOffscreenUpdate[characterSizeIndex];
    }
    else
    {
        params.nVisibleInterval = 1;
        params.nInvisibleInterval = 1;
    }

    if (ShouldShadowBeUpdated(params))
    {
        params.pModel = glModelDupNoStreams(model, false, 0);
        if (1.0f != alphaValue)
        {
            for (glModelPacket* pPacket = params.pModel->packets; pPacket < params.pModel->packets + params.pModel->numPackets; pPacket++)
            {
                glSetMaterialFloatParameter(pPacket, alphaValueHash, 1.0f);
            }
        }
        RenderCharacterIntoTexture(params);
    }

    RLView* oldView = SetCharacterShadowView(GetLayerView((eCLV)view));
    RenderProjectedShadow(params);
    SetCharacterShadowView(oldView);
}

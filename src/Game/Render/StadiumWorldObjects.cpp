#include "Game/Render/StadiumWorldObjects.h"

#include "Game/BasicStadium.h"
#include "Game/Drawable/DrawableObj.h"
#include "Game/World/WorldDrawable.h"
#include "Game/Camera/CameraMan.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/GameInfo.h"
#include "Game/GameObjectLighting.h"
#include "Game/FE/feCupFlow.h"
#include "Game/Render/AttackSideIndicators.h"
#include "Game/Render/Frustum.h"
#include "Game/Render/Presentation.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/Render/ShadowVolume.h"
#include "Game/AI/Fielder.h"
#include "Game/Team.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/gl/glMaterialParameters.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTexture.h"
#include "NL/gl/glTextureManager.h"
#include "NL/gl/glView.h"
#include "Game/Debug/ShapeRender.h"
#include "NL/nlColour.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "NL/nlstring_tmpl.h"

// Proxy objects of the stadium hierarchies that carry the team banner
// material; matching one marks the drawable for the banner texture swap.
static const char* sBannerProxyObjects[] = {
    "proxy object15/root object01",
    "proxy object15/balloonbase",
    "proxy object15/balloonbase2",
    "proxy object15/balloon3",
    "proxy object15/balloonbase4",
    "proxy object15/balloon5",
    "proxy object15/balloon6",
    "proxy object15/balloon7",
    "proxy object15/balloonbase8",
    "proxy object15/balloon9",
    "proxy object14/root object01",
    "proxy object14/string",
    "proxy object14/nub",
    "proxy object14/balloon01",
    "proxy object14/balloon02",
    "proxy object14/balloon03",
    "proxy object14/string02",
    "proxy object14/balloon04",
    "proxy object17/root object01",
    "proxy object17/object28",
    "proxy object17/geosphere128",
    "proxy object17/object29",
    "proxy object17/object30",
    "proxy object17/object31",
    "proxy object17/object32",
    "proxy object05/root object01",
    "proxy object05/viceriot_body",
    "proxy object05/tube1",
    "proxy object05/pipebase",
    "proxy object05/pipe01",
    "proxy object05/lid1",
    "proxy object05/lid2",
    "proxy object05/lid3",
    "proxy object05/effectemitter01",
    "proxy object05/thigh3",
    "proxy object05/leg2",
    "proxy object05/foot2",
    "proxy object05/thigh2",
    "proxy object05/leg1",
    "proxy object05/foot1",
    "proxy object05/thigh1",
    "proxy object05/leg3",
    "proxy object05/foot03",
    "proxy object05/tube2",
    "proxy object05/tube3",
    "proxy object05/armbase",
    "proxy object05/piston1",
    "proxy object05/piston2",
    "proxy object05/arm1",
    "proxy object05/clawjoint",
    "proxy object05/clawbase",
    "proxy object05/claw1",
    "proxy object05/claw2",
    "proxy object05/armpivot",
    "proxy object05/viceriot_body",
    "proxy object05/tube1",
    "proxy object05/pipebase",
    "proxy object05/pipe01",
    "proxy object05/lid1",
    "proxy object05/lid2",
    "proxy object05/lid3",
    "proxy object05/thigh3",
    "proxy object05/leg2",
    "proxy object05/foot2",
    "proxy object05/thigh2",
    "proxy object05/leg1",
    "proxy object05/foot1",
    "proxy object05/thigh1",
    "proxy object05/leg3",
    "proxy object05/foot03",
    "proxy object05/tube2",
    "proxy object05/tube3",
    "proxy object05/armbase",
    "proxy object05/piston1",
    "proxy object05/piston2",
    "proxy object05/arm1",
    "proxy object05/clawjoint",
    "proxy object05/clawbase",
    "proxy object05/claw1",
    "proxy object05/claw2",
    "proxy object05/armpivot",
    "proxy object06/root object01",
    "proxy object06/viceriot_body",
    "proxy object06/tube1",
    "proxy object06/pipebase",
    "proxy object06/pipe01",
    "proxy object06/lid1",
    "proxy object06/lid2",
    "proxy object06/lid3",
    "proxy object06/effectemitter01",
    "proxy object06/thigh3",
    "proxy object06/leg2",
    "proxy object06/foot2",
    "proxy object06/thigh2",
    "proxy object06/leg1",
    "proxy object06/foot1",
    "proxy object06/thigh1",
    "proxy object06/leg3",
    "proxy object06/foot03",
    "proxy object06/tube2",
    "proxy object06/tube3",
    "proxy object06/armbase",
    "proxy object06/piston1",
    "proxy object06/piston2",
    "proxy object06/arm1",
    "proxy object06/clawjoint",
    "proxy object06/clawbase",
    "proxy object06/claw1",
    "proxy object06/claw2",
    "proxy object06/armpivot",
    "proxy object06/viceriot_body",
    "proxy object06/tube1",
    "proxy object06/pipebase",
    "proxy object06/pipe01",
    "proxy object06/lid1",
    "proxy object06/lid2",
    "proxy object06/lid3",
    "proxy object06/thigh3",
    "proxy object06/leg2",
    "proxy object06/foot2",
    "proxy object06/thigh2",
    "proxy object06/leg1",
    "proxy object06/foot1",
    "proxy object06/thigh1",
    "proxy object06/leg3",
    "proxy object06/foot03",
    "proxy object06/tube2",
    "proxy object06/tube3",
    "proxy object06/armbase",
    "proxy object06/piston1",
    "proxy object06/piston2",
    "proxy object06/arm1",
    "proxy object06/clawjoint",
    "proxy object06/clawbase",
    "proxy object06/claw1",
    "proxy object06/claw2",
    "proxy object06/armpivot"
};

// The banner texture the swap installs, its index in the texture manager and
// the stadium texture it replaces.
// The captain's character index sits where cCharacter currently models its
// movement state, so it is read positionally until that layout is resolved.
static inline int GetCaptainCharacter(cFielder* captain)
{
    return *(const int*)((const u8*)captain + 0x24);
}

static unsigned long sStadiumBannerTexture;
static unsigned long sTeamBannerTexture;
static unsigned long sTeamBannerTextureIndex;
static bool sShowObjectBounds;
static bool sForceBannerHidden;
static bool sAlternateBannerVisibility;
static bool sAlternateBannerVisible;

/**
 * Address/Size: 0x80279AC8 | size: 0xCC
 */
void StadiumWorldDrawable::Initialize(WorldObjectLoadContext* context)
{
    WorldDrawable::Initialize(context);

    for (unsigned int i = 0; i < 0x89; i++)
    {
        unsigned long objectHash = nlStringHash(sBannerProxyObjects[i]);
        if (objectHash == m_uHashID)
            m_uFlags |= 2;
    }

    unsigned long taskState = nlTaskManager::m_pInstance->mCurrentState;
    if (taskState == 2 || taskState == 0x18 || taskState == 0x200000 || taskState == 0x800000)
        UpdateModelMaterials(m_pModel);

    sTeamBannerTexture = 0;
    sStadiumBannerTexture = 0;
}

/**
 * Address/Size: 0x80279B94 | size: 0x1E8
 *
 * Replaces the stadium banner texture of every packet of the model with the
 * banner of the two captains taking part.
 */
void StadiumWorldDrawable::UpdateModelMaterials(glModel* model)
{
    if (sTeamBannerTexture == 0)
    {
        char stadiumTexture[64];
        nlSNPrintf(stadiumTexture, sizeof(stadiumTexture), "_%s/mario_banners",
            GetStadiumName(
                nlSingleton<GameInfoManager>::Instance()->GetStadium()));
        sStadiumBannerTexture = glGetTexture(stadiumTexture);

        int firstCharacter;
        int secondCharacter;
        if (nlTaskManager::m_pInstance->mCurrentState > 0x10)
        {
            firstCharacter = nlSingleton<GameInfoManager>::Instance()->GetTeam(0);
            secondCharacter = nlSingleton<GameInfoManager>::Instance()->GetTeam(1);
            firstCharacter = GetCharacterIndexFromCaptain(firstCharacter);
            secondCharacter = GetCharacterIndexFromCaptain(secondCharacter);
        }
        else
        {
            firstCharacter = GetCaptainCharacter(g_pTeams[0]->GetCaptain());
            secondCharacter = GetCaptainCharacter(g_pTeams[1]->GetCaptain());
        }

        const CharacterInfo& firstCharacterInfo = GetCharacterInfo(firstCharacter);
        const CharacterInfo& secondCharacterInfo = GetCharacterInfo(secondCharacter);
        const char* characterName = GetCharacterInfo(firstCharacter).mName;
        char bannerTexture[64];
        if (NeedsAlternateColour(firstCharacterInfo, secondCharacterInfo))
        {
            nlSNPrintf(bannerTexture, sizeof(bannerTexture),
                "%s/%s_banners_alt", characterName, characterName);
        }
        else
        {
            nlSNPrintf(bannerTexture, sizeof(bannerTexture),
                "%s/%s_banners", characterName, characterName);
        }
        sTeamBannerTexture = glGetTexture(bannerTexture);
        sTeamBannerTextureIndex = glGetTextureManager()->GetTextureIndex(
            sTeamBannerTexture);
    }

    for (glModelPacket* packet = model->packets;
         packet < model->packets + model->numPackets; packet++)
    {
        if (sStadiumBannerTexture
            == glGetMaterialUnsignedParameter(packet, gDiffuseTextureSemantic))
        {
            glSetMaterialTextureParameter(
                packet, gDiffuseTextureSemantic, sTeamBannerTexture);
            unsigned long index = sTeamBannerTextureIndex;
            glSetMaterialTextureIndexParameter(
                packet, gDiffuseTextureSemantic, &index);
        }
    }
}

/**
 * Address/Size: 0x80279D7C | size: 0x4
 */
void StadiumWorldDrawable::ReleaseResources()
{
}

/**
 * Address/Size: 0x80279D80 | size: 0x54
 */
bool StadiumWorldDrawable::IsVisibleInFrustum(const nlVector4* planes) const
{
    if (m_pAnimController != 0)
        return WorldDrawable::IsVisibleInFrustum(planes);
    return ClassifyBoxInFrustum(
               planes, &m_boundsMin, &m_boundsMax, 0)
        != 0;
}

/**
 * Address/Size: 0x80279DD4 | size: 0xB4
 */
void StadiumWorldDrawable::Draw()
{
    if ((m_uFlags & 8) != 0)
    {
        WorldDrawable::DrawToView((GLView*)GetLayerView(eCLV_NoFog));
    }
    else
    {
        if ((m_uFlags & 0x37) != 0)
            UpdateBlend();
        if (0.0f != GetBlend())
            WorldDrawable::Draw();
    }

    if (sShowObjectBounds && m_pAnimController == 0)
    {
        nlColour colour;
        nlColourSet(colour, 0xFF, 0xFF, 0xFF, 0xFF);
        g_ShapeRenderer.DrawWireBox(m_boundsMin,
            m_boundsMax, colour);
    }
}


// Every blend update clamps into [0, 1] the same way.
static inline void SetObjectBlend(
    StadiumWorldDrawable* object, float blend)
{
    object->m_fBlend = blend;
    if (object->GetBlend() < 0.0f)
        object->m_fBlend = 0.0f;
    if (object->GetBlend() > 1.0f)
        object->m_fBlend = 1.0f;
}

/**
 * Address/Size: 0x80279E88 | size: 0x1CC
 */
void StadiumWorldDrawable::UpdateBlend()
{
    unsigned long flags = m_uFlags;
    if ((flags & 0x10) != 0
        && nlTaskManager::m_pInstance->mCurrentState == 0x10)
    {
        SetObjectBlend(this, 0.0f);
        return;
    }

    if ((flags & 0x20) != 0)
    {
        unsigned long taskState = nlTaskManager::m_pInstance->mCurrentState;
        if (taskState == 0x10 || taskState == 8 || taskState == 0x20000)
        {
            SetObjectBlend(this, 0.0f);
            return;
        }
    }

    if (cCameraManager::m_pBeginFrameCameraType == eCameraType_Animated)
    {
        SetObjectBlend(this, 1.0f);
        return;
    }

    bool hide = sForceBannerHidden;
    int cameraType = cCameraManager::m_pBeginFrameCameraType;
    if (cameraType == eCameraType_Gameplay
        || cameraType == eCameraType_ShootToScore
        || cameraType == eCameraType_Goal
        || !GetPresentation()->mUnidentified164)
    {
        hide = true;
    }

    if (sAlternateBannerVisibility)
    {
        bool visible = !sAlternateBannerVisible;
        sAlternateBannerVisible = visible;
        if (visible)
            hide = false;
    }

    if ((m_uFlags & 2) != 0 && hide)
        SetObjectBlend(this, 0.0f);
    else
        SetObjectBlend(this, 1.0f);
}

/**
 * Address/Size: 0x8027A054 | size: 0x70
 */
void StadiumLight::Initialize(WorldObjectLoadContext*)
{
    if (m_fIntensity <= 0.0f)
    {
        nlMatrix4* transform = GetWorldMatrix();
        BasicStadium* stadium = BasicStadium::GetCurrentStadium();
        stadium->m_shadowLightPosition = *(nlVector3*)&transform->m41;
    }
    else
    {
        PrepareStadiumLight(this);
    }
}

/**
 * Address/Size: 0x8027A0C4 | size: 0x4
 */
void StadiumLight::ReleaseResources()
{
}

/**
 * Address/Size: 0x8027A0C8 | size: 0x34
 */
void StadiumAttackSideIndicator::Initialize(WorldObjectLoadContext* context)
{
    WorldDrawable::Initialize(context);
    RegisterAttackSideIndicator(this);
}

/**
 * Address/Size: 0x8027A0FC | size: 0x4
 */
void StadiumAttackSideIndicator::ReleaseResources()
{
}

/**
 * Address/Size: 0x8027A100 | size: 0x14
 */
void StadiumAttackSideIndicator::Draw()
{
    if (m_nVisible != 0)
        WorldDrawable::Draw();
}

/**
 * Address/Size: 0x8027A114 | size: 0x4
 */
void StadiumToggleDrawable::Initialize(WorldObjectLoadContext* context)
{
    WorldDrawable::Initialize(context);
}

/**
 * Address/Size: 0x8027A118 | size: 0x4
 */
void StadiumToggleDrawable::ReleaseResources()
{
}

/**
 * Address/Size: 0x8027A11C | size: 0x14
 */
void StadiumToggleDrawable::Draw()
{
    if (m_nVisible != 0)
        WorldDrawable::Draw();
}

/**
 * Address/Size: 0x8027A130 | size: 0x6C
 */
void StadiumShadowVolumeDrawable::Initialize(WorldObjectLoadContext* context)
{
    WorldDrawable::Initialize(context);

    glModel* model = m_pModel;
    for (int modelIndex = 0; modelIndex < 2; modelIndex++)
    {
        m_pShadowModels[modelIndex]
            = glModelDupNoStreams(model, true, glGetCurrentResourcePool());
    }
}

/**
 * Address/Size: 0x8027A19C | size: 0x4
 */
void StadiumShadowVolumeDrawable::ReleaseResources()
{
}

/**
 * Address/Size: 0x8027A1A0 | size: 0x68
 */
void StadiumShadowVolumeDrawable::Draw()
{
    AttachShadowVolumeModels(m_pShadowModels[0], m_pShadowModels[1],
        (GLView*)GetLayerView(eCLV_ShadowVolume), (GLView*)GetLayerView(eCLV_ShadowVolume));
    ShowLayerView(eCLV_ShadowVolume);
    ShowLayerView(eCLV_ShadowVolumeBlend);
}

/**
 * Address/Size: 0x8027A208 | size: 0x3C
 */
void StadiumHighRangeDrawable::Initialize(WorldObjectLoadContext* context)
{
    WorldDrawable::Initialize(context);
    GameInfoManager* info = nlSingleton<GameInfoManager>::Instance();
    if (info->mCurrentMode != -1)
        SetStadiumHasHighRangeDrawables(info->GetStadium(), true);
}

/**
 * Address/Size: 0x8027A244 | size: 0x4
 */
void StadiumHighRangeDrawable::ReleaseResources()
{
}

/**
 * Address/Size: 0x8027A248 | size: 0x80
 */
void StadiumHighRangeDrawable::Draw()
{
    if ((m_uFlags & 0x37) != 0)
        UpdateBlend();

    if (0.0f != GetBlend())
    {
        if ((m_uFlags & 8) != 0)
        {
            ((GLView*)GetLayerView(eCLV_HighRange3DNoFog))
                ->AttachModel(m_pModel, 0);
        }
        else
        {
            ((GLView*)GetLayerView(eCLV_HighRange3D))
                ->AttachModel(m_pModel, 0);
        }
    }
}

/**
 * Address/Size: 0x8027A2C8 | size: 0x34
 */
void StadiumCupTrophyDrawable::Initialize(WorldObjectLoadContext* context)
{
    WorldDrawable::Initialize(context);
    RegisterCupTrophy(this);
}

/**
 * Address/Size: 0x8027A2FC | size: 0x4
 */
void StadiumCupTrophyDrawable::ReleaseResources()
{
}

/**
 * Address/Size: 0x8027A300 | size: 0x18
 */
void StadiumCupTrophyDrawable::Draw()
{
    if (0.0f != GetOpacity())
        WorldDrawable::Draw();
}

/**
 * Address/Size: 0x8027A318 | size: 0x4
 */
void WorldDrawable::UpdateModelMaterials(glModel*)
{
}

StadiumLight::~StadiumLight()
{
}

StadiumAttackSideIndicator::~StadiumAttackSideIndicator()
{
}

StadiumToggleDrawable::~StadiumToggleDrawable()
{
}

StadiumShadowVolumeDrawable::~StadiumShadowVolumeDrawable()
{
}

StadiumHighRangeDrawable::~StadiumHighRangeDrawable()
{
}

StadiumCupTrophyDrawable::~StadiumCupTrophyDrawable()
{
}

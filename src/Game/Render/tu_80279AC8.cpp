#include "Game/Render/tu_80279AC8.h"

#include "Game/BasicStadium.h"
#include "Game/Drawable/DrawableObj.h"
#include "Game/Camera/CameraMan.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/GameInfo.h"
#include "Game/Render/AttackSideIndicators.h"
#include "Game/Render/Frustum.h"
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

class GLView;

extern "C"
{
    void fn_80343DE4(StadiumWorldObject_80279AC8* object, void* context);
    nlMatrix4* fn_80343B14(StadiumWorldObject_80279AC8* object);
    bool fn_80343B34(StadiumWorldObject_80279AC8* object, const nlVector4* planes);
    void fn_80343C00(StadiumWorldObject_80279AC8* object);
    void fn_80343C14(StadiumWorldObject_80279AC8* object, GLView* view);
    void fn_80341EE8(StadiumWorldObject_80279AC8* object, const nlMatrix4* transform);
    void fn_80182168(StadiumWorldObject_80279AC8* object);
    void fn_802092A4(StadiumGoalObject_8027A2C8* object);
    void fn_802BC678(const ShapeRender* renderer, const nlVector3& boundsMin,
        const nlVector3& boundsMax, const nlColour& colour);
    void* GetPresentation();
    void fn_80279E88(StadiumWorldObject_80279AC8* object);
}

void* fn_80273A14(eCLV layer);

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
static bool sForceBlendOn;
static bool sBlendOverride;
static bool sBlendOverrideConsumed;

/**
 * Address/Size: 0x80279AC8 | size: 0xCC
 */
extern "C" void fn_80279AC8(
    StadiumWorldObject_80279AC8* object, void* context)
{
    fn_80343DE4(object, context);

    for (unsigned int i = 0; i < 0x89; i++)
    {
        unsigned long hash = nlStringHash(sBannerProxyObjects[i]);
        if (hash == object->m_uHashID)
            object->m_uFlags |= 2;
    }

    unsigned long state = nlTaskManager::m_pInstance->mCurrentState;
    if (state == 2 || state == 0x18 || state == 0x200000 || state == 0x800000)
        object->V7(object->m_pModel);

    sTeamBannerTexture = 0;
    sStadiumBannerTexture = 0;
}

/**
 * Address/Size: 0x80279B94 | size: 0x1E8
 *
 * Replaces the stadium banner texture of every packet of the model with the
 * banner of the two captains taking part.
 */
extern "C" void fn_80279B94(
    StadiumWorldObject_80279AC8* object, glModel* model)
{
    if (sTeamBannerTexture == 0)
    {
        char stadiumTexture[64];
        nlSNPrintf(stadiumTexture, sizeof(stadiumTexture), "_%s/mario_banners",
            GetStadiumName(
                nlSingleton<GameInfoManager>::Instance()->GetStadium()));
        sStadiumBannerTexture = glGetTexture(stadiumTexture);

        int first;
        int second;
        if (nlTaskManager::m_pInstance->mCurrentState > 0x10)
        {
            first = GetCharacterIndexFromCaptain(
                nlSingleton<GameInfoManager>::Instance()->GetTeam(0));
            second = GetCharacterIndexFromCaptain(
                nlSingleton<GameInfoManager>::Instance()->GetTeam(1));
        }
        else
        {
            first = GetCaptainCharacter(g_pTeams[0]->GetCaptain());
            second = GetCaptainCharacter(g_pTeams[1]->GetCaptain());
        }

        const char* name = GetCharacterInfo(first).mName;
        char bannerTexture[64];
        if (NeedsAlternateColour(
                GetCharacterInfo(first), GetCharacterInfo(second)))
        {
            nlSNPrintf(bannerTexture, sizeof(bannerTexture),
                "banners/%s_%s_b", name, name);
        }
        else
        {
            nlSNPrintf(bannerTexture, sizeof(bannerTexture),
                "banners/%s_%s", name, name);
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
extern "C" void fn_80279D7C(StadiumWorldObject_80279AC8*)
{
}

/**
 * Address/Size: 0x80279D80 | size: 0x54
 */
extern "C" bool fn_80279D80(
    StadiumWorldObject_80279AC8* object, const nlVector4* planes)
{
    if (object->m_pAnimController != 0)
        return fn_80343B34(object, planes);
    return ClassifyBoxInFrustum(
               planes, &object->m_boundsMin, &object->m_boundsMax, 0)
        != 0;
}

/**
 * Address/Size: 0x80279DD4 | size: 0xB4
 */
extern "C" void fn_80279DD4(StadiumWorldObject_80279AC8* object)
{
    if ((object->m_uFlags & 8) != 0)
    {
        fn_80343C14(object, (GLView*)GetLayerView((eCLV)5));
    }
    else
    {
        if ((object->m_uFlags & 0x37) != 0)
            fn_80279E88(object);
        if (0.0f != object->GetBlend())
            fn_80343C00(object);
    }

    if (sShowObjectBounds && object->m_pAnimController == 0)
    {
        nlColour colour;
        nlColourSet(colour, 0xFF, 0xFF, 0xFF, 0xFF);
        fn_802BC678(&g_ShapeRenderer, object->m_boundsMin,
            object->m_boundsMax, colour);
    }
}


// Every blend update clamps into [0, 1] the same way.
static inline void SetObjectBlend(
    StadiumWorldObject_80279AC8* object, float blend)
{
    object->m_fBlend = blend;
    if (blend < 0.0f)
        object->m_fBlend = 0.0f;
    if (object->GetBlend() > 1.0f)
        object->m_fBlend = 1.0f;
}

/**
 * Address/Size: 0x80279E88 | size: 0x1CC
 */
extern "C" void fn_80279E88(StadiumWorldObject_80279AC8* object)
{
    unsigned long flags = object->m_uFlags;
    if ((flags & 0x10) != 0
        && nlTaskManager::m_pInstance->mCurrentState == 0x10)
    {
        SetObjectBlend(object, 0.0f);
        return;
    }

    if ((flags & 0x20) != 0)
    {
        unsigned long state = nlTaskManager::m_pInstance->mCurrentState;
        if (state == 0x10 || state == 8 || state == 0x20000)
        {
            SetObjectBlend(object, 0.0f);
            return;
        }
    }

    if (cCameraManager::m_pBeginFrameCameraType == 9)
    {
        SetObjectBlend(object, 1.0f);
        return;
    }

    bool hide = sForceBlendOn;
    int camera = cCameraManager::m_pBeginFrameCameraType;
    if (camera == 1 || camera == 7 || camera == 6
        || *((bool*)GetPresentation() + 0x164))
    {
        hide = true;
    }

    if (sBlendOverride)
    {
        bool consumed = sBlendOverrideConsumed;
        sBlendOverrideConsumed = consumed == 0;
        if (consumed)
            hide = false;
    }

    if ((object->m_uFlags & 2) != 0 && hide)
        SetObjectBlend(object, 0.0f);
    else
        SetObjectBlend(object, 1.0f);
}

/**
 * Address/Size: 0x8027A054 | size: 0x70
 */
extern "C" void fn_8027A054(StadiumWorldObject_80279AC8* object)
{
    if (object->m_fLightRange <= 0.0f)
    {
        nlMatrix4* transform = object->GetWorldMatrix();
        BasicStadium* stadium = BasicStadium::GetCurrentStadium();
        stadium->m_shadowLightPosition = *(nlVector3*)&transform->m41;
    }
    else
    {
        fn_80182168(object);
    }
}

/**
 * Address/Size: 0x8027A0C4 | size: 0x4
 */
extern "C" void fn_8027A0C4(StadiumWorldObject_80279AC8*)
{
}

/**
 * Address/Size: 0x8027A0C8 | size: 0x34
 */
extern "C" void fn_8027A0C8(
    StadiumWorldObject_80279AC8* object, void* context)
{
    fn_80343DE4(object, context);
    RegisterAttackSideIndicator((DrawableObject*)object);
}

/**
 * Address/Size: 0x8027A0FC | size: 0x4
 */
extern "C" void fn_8027A0FC(StadiumWorldObject_80279AC8*)
{
}

/**
 * Address/Size: 0x8027A100 | size: 0x14
 */
extern "C" void fn_8027A100(StadiumWorldObject_80279AC8* object)
{
    if (object->m_pLayerModels[1] != 0)
        fn_80343C00(object);
}

/**
 * Address/Size: 0x8027A114 | size: 0x4
 */
extern "C" void fn_8027A114(
    StadiumWorldObject_80279AC8* object, void* context)
{
    fn_80343DE4(object, context);
}

/**
 * Address/Size: 0x8027A118 | size: 0x4
 */
extern "C" void fn_8027A118(StadiumWorldObject_80279AC8*)
{
}

/**
 * Address/Size: 0x8027A11C | size: 0x14
 */
extern "C" void fn_8027A11C(StadiumWorldObject_80279AC8* object)
{
    if (object->m_pLayerModels[0] != 0)
        fn_80343C00(object);
}

/**
 * Address/Size: 0x8027A130 | size: 0x6C
 */
extern "C" void fn_8027A130(
    StadiumWorldObject_80279AC8* object, void* context)
{
    fn_80343DE4(object, context);

    glModel* source = object->m_pModel;
    for (int i = 0; i < 2; i++)
    {
        object->m_pLayerModels[i]
            = glModelDupNoStreams(source, true, glGetCurrentResourcePool());
    }
}

/**
 * Address/Size: 0x8027A19C | size: 0x4
 */
extern "C" void fn_8027A19C(StadiumWorldObject_80279AC8*)
{
}

/**
 * Address/Size: 0x8027A1A0 | size: 0x68
 */
extern "C" void fn_8027A1A0(StadiumWorldObject_80279AC8* object)
{
    AttachShadowVolumeModels(object->m_pLayerModels[0], object->m_pLayerModels[1],
        (GLView*)GetLayerView((eCLV)0x14), (GLView*)GetLayerView((eCLV)0x14));
    fn_80273A14((eCLV)0x14);
    fn_80273A14((eCLV)0x15);
}

/**
 * Address/Size: 0x8027A208 | size: 0x3C
 */
extern "C" void fn_8027A208(
    StadiumWorldObject_80279AC8* object, void* context)
{
    fn_80343DE4(object, context);
    GameInfoManager* info = nlSingleton<GameInfoManager>::Instance();
    if (info->mCurrentMode != -1)
        SetStadiumUnknown0x2C(info->GetStadium(), true);
}

/**
 * Address/Size: 0x8027A244 | size: 0x4
 */
extern "C" void fn_8027A244(StadiumWorldObject_80279AC8*)
{
}

/**
 * Address/Size: 0x8027A248 | size: 0x80
 */
extern "C" void fn_8027A248(StadiumWorldObject_80279AC8* object)
{
    if ((object->m_uFlags & 0x37) != 0)
        fn_80279E88(object);

    if (0.0f != object->GetBlend())
    {
        if ((object->m_uFlags & 8) != 0)
        {
            ((GLView*)GetLayerView((eCLV)0x10))
                ->AttachModel(object->m_pModel, 0);
        }
        else
        {
            ((GLView*)GetLayerView((eCLV)0xF))
                ->AttachModel(object->m_pModel, 0);
        }
    }
}

/**
 * Address/Size: 0x8027A2C8 | size: 0x34
 */
extern "C" void fn_8027A2C8(
    StadiumGoalObject_8027A2C8* object, void* context)
{
    fn_80343DE4(object, context);
    fn_802092A4(object);
}

/**
 * Address/Size: 0x8027A2FC | size: 0x4
 */
extern "C" void fn_8027A2FC(StadiumWorldObject_80279AC8*)
{
}

/**
 * Address/Size: 0x8027A300 | size: 0x18
 */
extern "C" void fn_8027A300(StadiumGoalObject_8027A2C8* object)
{
    if (object->m_fCupTrophyOpacity != 0.0f)
        fn_80343C00(object);
}

/**
 * Address/Size: 0x8027A318 | size: 0x4
 */
extern "C" void fn_8027A318(StadiumWorldObject_80279AC8*)
{
}

StadiumLightObject_8027A054::~StadiumLightObject_8027A054()
{
}

StadiumIndicatorObject_8027A0C8::~StadiumIndicatorObject_8027A0C8()
{
}

StadiumEffectObject_8027A11C::~StadiumEffectObject_8027A11C()
{
}

StadiumLayerObject_8027A130::~StadiumLayerObject_8027A130()
{
}

StadiumCrowdObject_8027A208::~StadiumCrowdObject_8027A208()
{
}

StadiumGoalObject_8027A2C8::~StadiumGoalObject_8027A2C8()
{
}

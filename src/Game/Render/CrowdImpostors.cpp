#include "Game/Render/ImpostorLighting.h"
#include "NL/gl/glMemory.h"
#include "Game/Camera/CameraMan.h"
#include "NL/gl/glTexture.h"
#include "Game/BasicStadium.h"
#include "Game/World/WorldObjectLoadContext.h"
#include "Game/Drawable/DrawableObj.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/GameInfo.h"
#include "Game/Render/ImpostorManager.h"
#include "Game/Render/RLView.h"
#include "Game/Render/CrowdImpostorManager.h"
#include "Game/TweakRegistry.h"
#include "Game/UnidentifiedStaticStorage.h"

#include "Game/TweakConfig.h"
#include "NL/gl/glTextureManager.h"
#include "NL/gl/glMaterialParameters.h"
#include "NL/nlMath.h"
#include "NL/nlDLRing.h"
#include "NL/nlString.h"
#include "Game/Render/CrowdImpostors.h"
#include "NL/nlstring_tmpl.h"

static CrowdCharacterDefinition sCrowdCharacterDefinitions[36] = {
    { "birdo", "art/animation/crowdbirdo.sanim.zlib",
        "art/animation/crowdbirdo.shier", "crowdbirdo",
        "Art/Characters/Crowdbirdo/crowdbirdo.rlt",
        "Art/Characters/Crowdbirdo/crowdbirdo.rlg" },
    { "birdoblack", "art/animation/crowdbirdoblack.sanim.zlib",
        "art/animation/crowdbirdoblack.shier", "crowdbirdoblack",
        "Art/Characters/Crowdbirdoblack/crowdbirdoblack.rlt",
        "Art/Characters/Crowdbirdoblack/crowdbirdoblack.rlg" },
    { "birdoblue", "art/animation/crowdbirdoblue.sanim.zlib",
        "art/animation/crowdbirdoblue.shier", "crowdbirdoblue",
        "Art/Characters/Crowdbirdoblue/crowdbirdoblue.rlt",
        "Art/Characters/Crowdbirdoblue/crowdbirdoblue.rlg" },
    { "birdogreen", "art/animation/crowdbirdogreen.sanim.zlib",
        "art/animation/crowdbirdogreen.shier", "crowdbirdogreen",
        "Art/Characters/Crowdbirdogreen/crowdbirdogreen.rlt",
        "Art/Characters/Crowdbirdogreen/crowdbirdogreen.rlg" },
    { "birdolightblue", "art/animation/crowdbirdolightblue.sanim.zlib",
        "art/animation/crowdbirdolightblue.shier", "crowdbirdolightblue",
        "Art/Characters/Crowdbirdolightblue/crowdbirdolightblue.rlt",
        "Art/Characters/Crowdbirdolightblue/crowdbirdolightblue.rlg" },
    { "birdoorange", "art/animation/crowdbirdoorange.sanim.zlib",
        "art/animation/crowdbirdoorange.shier", "crowdbirdoorange",
        "Art/Characters/Crowdbirdoorange/crowdbirdoorange.rlt",
        "Art/Characters/Crowdbirdoorange/crowdbirdoorange.rlg" },
    { "birdopink", "art/animation/crowdbirdopink.sanim.zlib",
        "art/animation/crowdbirdopink.shier", "crowdbirdopink",
        "Art/Characters/Crowdbirdopink/crowdbirdopink.rlt",
        "Art/Characters/Crowdbirdopink/crowdbirdopink.rlg" },
    { "birdored", "art/animation/crowdbirdored.sanim.zlib",
        "art/animation/crowdbirdored.shier", "crowdbirdored",
        "Art/Characters/Crowdbirdored/crowdbirdored.rlt",
        "Art/Characters/Crowdbirdored/crowdbirdored.rlg" },
    { "birdowhite", "art/animation/crowdbirdowhite.sanim.zlib",
        "art/animation/crowdbirdowhite.shier", "crowdbirdowhite",
        "Art/Characters/Crowdbirdowhite/crowdbirdowhite.rlt",
        "Art/Characters/Crowdbirdowhite/crowdbirdowhite.rlg" },
    { "birdoyellow", "art/animation/crowdbirdoyellow.sanim.zlib",
        "art/animation/crowdbirdoyellow.shier", "crowdbirdoyellow",
        "Art/Characters/Crowdbirdoyellow/crowdbirdoyellow.rlt",
        "Art/Characters/Crowdbirdoyellow/crowdbirdoyellow.rlg" },
    { "shyguy", "art/animation/crowdshyguy.sanim.zlib",
        "art/animation/crowdshyguy.shier", "crowdshyguy",
        "Art/Characters/Crowdshyguy/crowdshyguy.rlt",
        "Art/Characters/Crowdshyguy/crowdshyguy.rlg" },
    { "shyguyblack", "art/animation/crowdshyguyblack.sanim.zlib",
        "art/animation/crowdshyguyblack.shier", "crowdshyguyblack",
        "Art/Characters/Crowdshyguyblack/crowdshyguyblack.rlt",
        "Art/Characters/Crowdshyguyblack/crowdshyguyblack.rlg" },
    { "shyguyblue", "art/animation/crowdshyguyblue.sanim.zlib",
        "art/animation/crowdshyguyblue.shier", "crowdshyguyblue",
        "Art/Characters/Crowdshyguyblue/crowdshyguyblue.rlt",
        "Art/Characters/Crowdshyguyblue/crowdshyguyblue.rlg" },
    { "shyguygreen", "art/animation/crowdshyguygreen.sanim.zlib",
        "art/animation/crowdshyguygreen.shier", "crowdshyguygreen",
        "Art/Characters/Crowdshyguygreen/crowdshyguygreen.rlt",
        "Art/Characters/Crowdshyguygreen/crowdshyguygreen.rlg" },
    { "shyguylightblue", "art/animation/crowdshyguylightblue.sanim.zlib",
        "art/animation/crowdshyguylightblue.shier", "crowdshyguylightblue",
        "Art/Characters/Crowdshyguylightblue/crowdshyguylightblue.rlt",
        "Art/Characters/Crowdshyguylightblue/crowdshyguylightblue.rlg" },
    { "shyguypink", "art/animation/crowdshyguypink.sanim.zlib",
        "art/animation/crowdshyguypink.shier", "crowdshyguypink",
        "Art/Characters/Crowdshyguypink/crowdshyguypink.rlt",
        "Art/Characters/Crowdshyguypink/crowdshyguypink.rlg" },
    { "shyguyred", "art/animation/crowdshyguyred.sanim.zlib",
        "art/animation/crowdshyguyred.shier", "crowdshyguyred",
        "Art/Characters/Crowdshyguyred/crowdshyguyred.rlt",
        "Art/Characters/Crowdshyguyred/crowdshyguyred.rlg" },
    { "shyguywhite", "art/animation/crowdshyguywhite.sanim.zlib",
        "art/animation/crowdshyguywhite.shier", "crowdshyguywhite",
        "Art/Characters/Crowdshyguywhite/crowdshyguywhite.rlt",
        "Art/Characters/Crowdshyguywhite/crowdshyguywhite.rlg" },
    { "shyguyyellow", "art/animation/crowdshyguyyellow.sanim.zlib",
        "art/animation/crowdshyguyyellow.shier", "crowdshyguyyellow",
        "Art/Characters/Crowdshyguyyellow/crowdshyguyyellow.rlt",
        "Art/Characters/Crowdshyguyyellow/crowdshyguyyellow.rlg" },
    { "pianta", "art/animation/crowdpianta.sanim.zlib",
        "art/animation/crowdpianta.shier", "crowdpianta",
        "Art/Characters/Crowdpianta/crowdpianta.rlt",
        "Art/Characters/Crowdpianta/crowdpianta.rlg" },
    { "piantablue", "art/animation/crowdpiantablue.sanim.zlib",
        "art/animation/crowdpiantablue.shier", "crowdpiantablue",
        "Art/Characters/Crowdpiantablue/crowdpiantablue.rlt",
        "Art/Characters/Crowdpiantablue/crowdpiantablue.rlg" },
    { "piantapink", "art/animation/crowdpiantapink.sanim.zlib",
        "art/animation/crowdpiantapink.shier", "crowdpiantapink",
        "Art/Characters/Crowdpiantapink/crowdpiantapink.rlt",
        "Art/Characters/Crowdpiantapink/crowdpiantapink.rlg" },
    { "piantayellow", "art/animation/crowdpiantayellow.sanim.zlib",
        "art/animation/crowdpiantayellow.shier", "crowdpiantayellow",
        "Art/Characters/Crowdpiantayellow/crowdpiantayellow.rlt",
        "Art/Characters/Crowdpiantayellow/crowdpiantayellow.rlg" },
    { "toad", "art/animation/crowdtoad.sanim.zlib",
        "art/animation/crowdtoad.shier", "crowdtoad",
        "Art/Characters/Crowdtoad/crowdtoad.rlt",
        "Art/Characters/Crowdtoad/crowdtoad.rlg" },
    { "toadblue", "art/animation/crowdtoadblue.sanim.zlib",
        "art/animation/crowdtoadblue.shier", "crowdtoadblue",
        "Art/Characters/Crowdtoadblue/crowdtoadblue.rlt",
        "Art/Characters/Crowdtoadblue/crowdtoadblue.rlg" },
    { "toadgreen", "art/animation/crowdtoadgreen.sanim.zlib",
        "art/animation/crowdtoadgreen.shier", "crowdtoadgreen",
        "Art/Characters/Crowdtoadgreen/crowdtoadgreen.rlt",
        "Art/Characters/Crowdtoadgreen/crowdtoadgreen.rlg" },
    { "toadpink", "art/animation/crowdtoadpink.sanim.zlib",
        "art/animation/crowdtoadpink.shier", "crowdtoadpink",
        "Art/Characters/Crowdtoadpink/crowdtoadpink.rlt",
        "Art/Characters/Crowdtoadpink/crowdtoadpink.rlg" },
    { "toadred", "art/animation/crowdtoadred.sanim.zlib",
        "art/animation/crowdtoadred.shier", "crowdtoadred",
        "Art/Characters/Crowdtoadred/crowdtoadred.rlt",
        "Art/Characters/Crowdtoadred/crowdtoadred.rlg" },
    { "toadyellow", "art/animation/crowdtoadyellow.sanim.zlib",
        "art/animation/crowdtoadyellow.shier", "crowdtoadyellow",
        "Art/Characters/Crowdtoadyellow/crowdtoadyellow.rlt",
        "Art/Characters/Crowdtoadyellow/crowdtoadyellow.rlg" },
    { "boo", "art/animation/crowdboo.sanim.zlib",
        "art/animation/crowdboo.shier", "crowdboo",
        "Art/Characters/Crowdboo/crowdboo.rlt",
        "Art/Characters/Crowdboo/crowdboo.rlg" },
    { "goomba", "art/animation/crowdgoomba.sanim.zlib",
        "art/animation/crowdgoomba.shier", "crowdgoomba",
        "Art/Characters/Crowdgoomba/crowdgoomba.rlt",
        "Art/Characters/Crowdgoomba/crowdgoomba.rlg" },
    { "koopa", "art/animation/crowdkoopa.sanim.zlib",
        "art/animation/crowdkoopa.shier", "crowdkoopa",
        "Art/Characters/Crowdkoopa/crowdkoopa.rlt",
        "Art/Characters/Crowdkoopa/crowdkoopa.rlg" },
    { "penguin", "art/animation/crowdpenguin.sanim.zlib",
        "art/animation/crowdpenguin.shier", "crowdpenguin",
        "Art/Characters/Crowdpenguin/crowdpenguin.rlt",
        "Art/Characters/Crowdpenguin/crowdpenguin.rlg" },
    { "delfin", "art/animation/crowddelfin.sanim.zlib",
        "art/animation/crowddelfin.shier", "crowddelfin",
        "Art/Characters/Crowddelfin/crowddelfin.rlt",
        "Art/Characters/Crowddelfin/crowddelfin.rlg" },
    { "delfina", "art/animation/crowddelfina.sanim.zlib",
        "art/animation/crowddelfina.shier", "crowddelfina",
        "Art/Characters/Crowddelfina/crowddelfina.rlt",
        "Art/Characters/Crowddelfina/crowddelfina.rlg" },
    { "monkey", "art/animation/crowdmonkey.sanim.zlib",
        "art/animation/crowdmonkey.shier", "crowdmonkey",
        "Art/Characters/Crowdmonkey/crowdmonkey.rlt",
        "Art/Characters/Crowdmonkey/crowdmonkey.rlg" },
};

class CrowdImpostorCharacter
    : public AnimatedImpostorCharacter
{
public:
    CrowdImpostorCharacter(const char* name,
        ImpostorModel* model, void* animations, int budget,
        int numAngles, int numTextures,
        const ImpostorCharacterParams* params)
        : AnimatedImpostorCharacter(name, model, animations, budget,
            numAngles, numTextures, params)
    {
    }
    virtual ~CrowdImpostorCharacter();
};

static const char* sCrowdCharactersToLoadPath = "/Render/Crowd/CharactersToLoad";
static const char sAlternateCrowdListSuffix[] = "Alt";
static const char sDefaultCrowdListSuffix[] = "";
static const char sCrowdIdleAnimation[] = "idle";
static const char sCrowdExcitedAnimation[] = "excited";
static float sCrowdExcitedThreshold = 200.0f;
static float sCrowdIdleThreshold = 100.0f;
static const char sCrowdCharacterListFormat[] = "ini/CrowdCharacterLists/%s%s.ini";
static const double sCrowdViewVectorMinLengthSq = 0.001;
static const float sCrowdZero = 0.0f;
static const float sMaxCrowdSidelineCullingDistance = 50.0f;
static const float sCrowdSidelineCullingDistanceStep = 0.05f;
static const float sAlternateCrowdViewBudgetScale = 750.0f;
static const float sDefaultMaxCrowdSize = 10000.0f;
static const float sCrowdImpostorSizeScale = 1.5f;
static const double sCrowdIntegerConversionBias = 4503599627370496.0;
static const float sCrowdLayoutTransformW = 0.96f;
static const float sCrowdLayoutXScale = 12.0f;
static const float sCrowdLayoutRotationX = 2.0f;
static const float sCrowdLayoutSize = 5.0f;
static const float sCrowdExcitedBlendTime = 2.0f;
static const float sCrowdIdleBlendTime = 5.0f;
static GLMemoryRequirement sCrowdResourceRequirements[2]
    = { { GLM_Header, 0x8000 }, { GLM_VertexData, 0 } };
static GLMemoryRequirement sEmptyCrowdResourceRequirements[2]
    = { { GLM_Header, 0x400 }, { GLM_VertexData, 0x400 } };
static const char sMaxCrowdSizePath[] = "user/CrowdMax";
static const char sCrowdClusterName[] = "impostorCluster";
static const char sViceCrowdModelName[] = "_vice/crowd_testanim";
static const char sViceNightCrowdModelName[] = "_vicenight/crowd_testanim";
static const char sUndergroundCrowdModelName[] = "_underground/crowd_testanim";
static const char sWastelandsCrowdModelName[] = "_wastelands/crowd_testanim";
static CrowdLayoutObject* sCrowdLayoutObject;
static CrowdModelArray sCrowdModels(36);
static CrowdCharacterArray sCrowdCharacters(36);
static CrowdDefinitionArray sCrowdLoadDefinitions(8);
static int sNumCrowdCharacters;
static bool sCullCrowdInDebugCamera;
static CrowdSidelineFilter* sCrowdSidelineFilter;
static bool sCrowdViewBudgetConfigured;
static bool sCrowdImpostorsExcited;
static ImpostorCluster* sCrowdCluster;
static bool sLockCrowdImpostorAnimation;
CrowdModelCollection gCrowdModelCollection;

void UpdateImpostorPositions()
{
    nlVector3 viewVector;
    nlVector3 upVector = { 0.0f, 0.0f, 1.0f };

    cCameraManager::GetViewVector(viewVector);
    if (viewVector.GetLengthSq3D() > sCrowdViewVectorMinLengthSq
        && upVector.GetLengthSq3D() > sCrowdViewVectorMinLengthSq)
    {
        ImpostorManager::GetInstance()->UpdatePositions(
            &viewVector, &upVector);
    }
}

void LoadCrowdCharacterList()
{
    sNumCrowdCharacters = 0;

    int captain = GameInfoManager::Instance()->GetTeam(0);
    const char* teamName;
    const CharacterInfo& team
        = GetCharacterInfo(GetCharacterIndexFromCaptain(captain));
    int opponentCaptain = GameInfoManager::Instance()->GetTeam(1);
    const CharacterInfo& opponent
        = GetCharacterInfo(GetCharacterIndexFromCaptain(opponentCaptain));

    teamName = team.mName;
    bool alternateColour = NeedsAlternateColour(team, opponent);
    char fileName[256];
    nlSNPrintf(fileName, sizeof(fileName), sCrowdCharacterListFormat, teamName,
        alternateColour ? sAlternateCrowdListSuffix : sDefaultCrowdListSuffix);

    LoadTweakConfigFile(fileName, sCrowdCharactersToLoadPath, true);
    TweakEntry* entry
        = FindOrCreateTweakPath(GetTweakRoot(), sCrowdCharactersToLoadPath, true);
    for (TweakNode* node = entry->m_ChildHead; node != 0;
        node = node->m_Next)
    {
        const char* name = GetTweakNodeName(node);
        for (int i = 0; i < 36; ++i)
        {
            if (nlStrICmp(name, sCrowdCharacterDefinitions[i].mName) == 0)
            {
                sCrowdLoadDefinitions.data[sNumCrowdCharacters]
                    = sCrowdCharacterDefinitions[i];
                ++sNumCrowdCharacters;
            }
        }
    }

    gCrowdModelCollection.Initialize(sCrowdLoadDefinitions.data, sNumCrowdCharacters);
}

void InitializeCrowdImpostors(bool alternateView)
{
    CrowdSidelineFilter* tweak = new (8, false) CrowdSidelineFilter;
    tweak->mSidelineCullingDistance.BindWithDefault("mfSidelineCullingDistance",
        sCrowdZero, "/Render/Crowd", true, sCrowdZero,
        sMaxCrowdSidelineCullingDistance, sCrowdSidelineCullingDistanceStep);
    sCrowdSidelineFilter = tweak;

    int crowdMax = GetTweakInt(sMaxCrowdSizePath, 10000);
    if (!sCrowdViewBudgetConfigured)
    {
        unsigned int scaled = (unsigned int)(crowdMax << 10);
        sCrowdResourceRequirements[1].mSize = (unsigned int)(
            sAlternateCrowdViewBudgetScale * ((float)scaled / sDefaultMaxCrowdSize));
        sCrowdViewBudgetConfigured = true;
    }

    const GLMemoryRequirement* requirements = sEmptyCrowdResourceRequirements;
    if (alternateView)
    {
        requirements = sCrowdResourceRequirements;
    }
    ImpostorManager::GetInstance()->Initialize(
        GetLayerView(eCLV_ImpostorTexture), crowdMax, requirements, 2, false);
    ImpostorManager::GetInstance()->SetImpostorSizeScale(sCrowdImpostorSizeScale);

    int budget = 20000 / sNumCrowdCharacters;
    ImpostorCharacterParams params;
    params.mWidth = 64;
    params.mHeight = 64;
    params.mUnidentified008 = false;
    params.mUseIntensityAlpha = false;
    params.mBaseAngle = 0;
    for (int i = 0; i < sNumCrowdCharacters; ++i)
    {
        sCrowdModels.data[i] = gCrowdModelCollection.mModels[i];
        CrowdImpostorCharacter* character
            = new (8, false) CrowdImpostorCharacter(
                sCrowdLoadDefinitions.data[i % sNumCrowdCharacters].mName,
                sCrowdModels.data[i],
                (void*)sCrowdIdleAnimation, budget, 4, 2, &params);
        sCrowdCharacters.data[i] = character;
        GetCrowdImpostorManager()->AddCharacter( character);
    }

    sCrowdImpostorsExcited = false;
    CreateCrowdLayoutObject();

    ImpostorCharacterParams clusterParams;
    clusterParams.mWidth = 128;
    clusterParams.mHeight = 128;
    clusterParams.mUnidentified008 = false;
    clusterParams.mUseIntensityAlpha = false;
    clusterParams.mBaseAngle = 0;
    void* clusterMemory = nlMalloc(0x70, 8, false);
    ImpostorCluster* clusterCharacter
        = (ImpostorCluster*)clusterMemory;
    if (clusterCharacter != 0)
    {
        clusterCharacter = new (clusterCharacter) ImpostorCluster(
            sCrowdClusterName, 10, &clusterParams);
    }
    sCrowdCluster = clusterCharacter;
    u32 firstHash = nlStringLowerHash(sViceCrowdModelName);
    unsigned long cluster = sCrowdCluster->GetTexture();
    SetCrowdModelTexture(firstHash, cluster);
    SetCrowdModelTexture(nlStringLowerHash(sViceNightCrowdModelName), cluster);
    SetCrowdModelTexture(nlStringLowerHash(sUndergroundCrowdModelName), cluster);
    SetCrowdModelTexture(nlStringLowerHash(sWastelandsCrowdModelName), cluster);

    GetCrowdImpostorManager()->GenerateCrowd( 0);
    GetCrowdImpostorManager()->AddVisibilityFilter( sCrowdSidelineFilter);
    UpdateImpostorLighting();
    ImpostorManager::GetInstance()->StaggerAnimations();
}

void UninitializeCrowdImpostors()
{
    ImpostorManager::GetInstance()->ResetImpostors();
    for (int i = 0; i < sNumCrowdCharacters; ++i)
    {
        delete sCrowdModels.data[i];
        delete sCrowdCharacters.data[i];
    }
    sNumCrowdCharacters = 0;

    delete sCrowdCluster;
    GetCrowdImpostorManager()->Clear();
    ImpostorManager::GetInstance()->Uninitialize();

    if (sCrowdLayoutObject != 0)
    {
        delete sCrowdLayoutObject;
        sCrowdLayoutObject = 0;
    }
    if (sCrowdSidelineFilter != 0)
    {
        delete sCrowdSidelineFilter;
        sCrowdSidelineFilter = 0;
    }
}

void CreateCrowdLayoutObject()
{
    nlMatrix4 transform;
    transform.SetIdentity();
    nlMakeRotationMatrixX(transform, sCrowdLayoutRotationX);

    float distance = sCrowdLayoutSize;
    float zero = sCrowdZero;
    transform.m41 = sCrowdLayoutXScale * -distance;
    transform.m42 = zero;
    transform.m43 = zero;
    transform.m44 = sCrowdLayoutTransformW;

    BasicStadium* stadium = BasicStadium::GetCurrentStadium();
    WorldObjectLoadContext* context
        = new (8, false) WorldObjectLoadContext(stadium);
    sCrowdLayoutObject = new (8, false) CrowdLayoutObject;
    sCrowdLayoutObject->Initialize();
    sCrowdLayoutObject->mStartWidth = distance;
    sCrowdLayoutObject->mEndWidth = distance;
    sCrowdLayoutObject->mLength = distance;
    sCrowdLayoutObject->mEndOffset = zero;
    sCrowdLayoutObject->SetWorldMatrix(transform);
    GetCrowdImpostorManager()->AddObject( sCrowdLayoutObject, 1);
    delete context;
}

void SetCrowdModelTexture(u32 hash, unsigned long texture)
{
    nlDLListIterator<WorldListObject0_80340AC8*> iterator
        = BasicStadium::GetCurrentStadium()->m_objectList0.Begin();
    unsigned long textureIndex = glGetTextureManager()->GetTextureIndex(texture);

    for (; iterator.hasNext(); iterator.next())
    {
        DrawableObject* pObject = (DrawableObject*)*iterator;
        if (pObject->m_uRenderLayer == 0x10002)
        {
            glModel* pGlModel = pObject->m_pModel;
            for (glModelPacket* pPacket = pGlModel->packets;
                 pPacket < pGlModel->packets + pGlModel->numPackets;
                 ++pPacket)
            {
                unsigned long packetTexture
                    = glGetMaterialUnsignedParameter(
                        pPacket, gDiffuseTextureSemantic);
                if (packetTexture == hash)
                {
                    glSetMaterialTextureParameter(pPacket, gDiffuseTextureSemantic, texture);
                    unsigned long resolvedTexture = textureIndex;
                    glSetMaterialTextureIndexParameter(pPacket, gDiffuseTextureSemantic, &resolvedTexture);
                }
            }
        }
    }
}

void SetCrowdImpostorsExcited()
{
    sCrowdImpostorsExcited = true;
    ImpostorManager::GetInstance()->UpdateCharacters(
        sCrowdExcitedBlendTime, sCrowdExcitedAnimation);
    ImpostorManager::GetInstance()->StaggerAnimations();
}

void SetCrowdImpostorsIdle()
{
    sCrowdImpostorsExcited = false;
    ImpostorManager::GetInstance()->UpdateCharacters(
        sCrowdIdleBlendTime, sCrowdIdleAnimation);
    ImpostorManager::GetInstance()->StaggerAnimations();
}

void UpdateCrowdImpostorAnimation(float value)
{
    if (!sLockCrowdImpostorAnimation)
    {
        if (sCrowdImpostorsExcited)
        {
            if (value < sCrowdIdleThreshold)
            {
                sCrowdImpostorsExcited = false;
                ImpostorManager::GetInstance()->UpdateCharacters(
                    sCrowdIdleBlendTime, sCrowdIdleAnimation);
                ImpostorManager::GetInstance()->StaggerAnimations();
            }
        }
        else if (value > sCrowdExcitedThreshold)
        {
            sCrowdImpostorsExcited = true;
            ImpostorManager::GetInstance()->UpdateCharacters(
                sCrowdExcitedBlendTime, sCrowdExcitedAnimation);
            ImpostorManager::GetInstance()->StaggerAnimations();
        }
    }
}

CrowdImpostorCharacter::~CrowdImpostorCharacter()
{
}


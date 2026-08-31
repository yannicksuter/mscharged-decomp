#include "Game/Camera/CameraMan.h"
#include "Game/BasicStadium.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/GameInfo.h"
#include "Game/Render/ImpostorManager.h"
#include "Game/TweakRegistry.h"
#include "Game/UnidentifiedStaticStorage.h"

#include "NL/nlMath.h"
#include "NL/nlString.h"
#include "unclassified/tu_801A4188.h"

static CrowdCharacterDefinition_801A4188 lbl_80513C80[36] = {
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

class UnidentifiedRenderObject_801A4188
{
public:
    virtual ~UnidentifiedRenderObject_801A4188();
    virtual void UnidentifiedVirtual0C();
    virtual void UnidentifiedVirtual10();
    virtual void SetTransform(const nlMatrix4& transform);

    u8 mUnidentified004[0x5C];
    float mUnidentified060;
    float mUnidentified064;
    float mUnidentified068;
    float mUnidentified06C;
    u8 mUnidentified070[0x10];
};

struct UnidentifiedStadiumOwner_801A4188
{
    UnidentifiedStadiumOwner_801A4188(BasicStadium* stadium)
    {
        mUnidentified000 = 0;
        mStadium = stadium;
        mUnidentified008 = 0;
        mUnidentified00C = 0;
    }

    void* mUnidentified000;
    BasicStadium* mStadium;
    void* mUnidentified008;
    void* mUnidentified00C;
};

class UnidentifiedCrowdCharacter_801A4188 : public ImpostorCharacter
{
public:
    u32 mUnidentified06C;
};

class ImpostorCharacterImpl_801A4188
    : public ImpostorCharacterImpl_8052E9B8
{
public:
    ImpostorCharacterImpl_801A4188(const char* name,
        ImpostorModel_802DAEE0* model, void* animations, int budget,
        int numAngles, int numTextures,
        const ImpostorCharacterParams* params)
        : ImpostorCharacterImpl_8052E9B8(name, model, animations, budget,
            numAngles, numTextures, params)
    {
    }
    virtual ~ImpostorCharacterImpl_801A4188();
};

class CrowdTweak_801A4188
{
public:
    virtual bool UnidentifiedVirtual08(const Impostor* impostor);

    TweakValue_804F4DC8 value;
};

struct CrowdModelCollection_801A4188
{
    CrowdModelCollection_801A4188();
    ~CrowdModelCollection_801A4188();

    u8 mUnidentified000[0x48];
    ImpostorModel_802DAEE0** models;
    u32 mUnidentified04C;
}; // size: 0x50

static const char* lbl_806DCF68 = "/Render/Crowd/CharactersToLoad";
static const char lbl_806DCF6C[] = "Alt";
static const char lbl_806DCF70[] = "";
static const char lbl_806DCF74[] = "idle";
static const char lbl_806DCF80[] = "excited";
static float lbl_806DCF88 = 200.0f;
static float lbl_806DCF8C = 100.0f;
static const char lbl_80514000[] = "ini/CrowdCharacterLists/%s%s.ini";
static const double lbl_806E5108 = 0.001;
static const float lbl_806E5110 = 0.0f;
static const float lbl_806E5114 = 50.0f;
static const float lbl_806E5118 = 0.05f;
static const float lbl_806E511C = 750.0f;
static const float lbl_806E5120 = 10000.0f;
static const float lbl_806E5124 = 1.5f;
static const double lbl_806E5128 = 4503599627370496.0;
static const float lbl_806E5130 = 0.96f;
static const float lbl_806E5134 = 12.0f;
static const float lbl_806E5138 = 2.0f;
static const float lbl_806E513C = 5.0f;
static const float lbl_806E5140 = 2.0f;
static const float lbl_806E5144 = 5.0f;
static UnidentifiedViewConfig_8052E828 lbl_80514028
    = { 0, 0x8000, 3, 0 };
static UnidentifiedViewConfig_8052E828 lbl_80514038
    = { 0, 0x400, 3, 0x400 };
static const char lbl_80514048[] = "user/CrowdMax";
static const char lbl_80514058[] = "impostorCluster";
static const char lbl_80514068[] = "_vice/crowd_testanim";
static const char lbl_80514080[] = "_vicenight/crowd_testanim";
static const char lbl_8051409C[] = "_underground/crowd_testanim";
static const char lbl_805140B8[] = "_wastelands/crowd_testanim";
static UnidentifiedRenderObject_801A4188* lbl_806E1580;
static ArrayOwner_801A4EC0 lbl_806E1588(36);
static ArrayOwner_801A4F18 lbl_806E1590(36);
static ArrayOwner_801A4F70 lbl_806E1598(8);
static int lbl_806E15A0;
static bool lbl_806E15A4;
static CrowdTweak_801A4188* lbl_806E15A8;
static bool lbl_806E15AC;
static bool lbl_806E15AD;
static UnidentifiedCrowdCharacter_801A4188* lbl_806E15B0;
static bool lbl_806E15B4;
static CrowdModelCollection_801A4188 lbl_805721E8;

extern "C" void fn_802C6CAC(
    const char* fileName, const char* category, bool reload);
extern "C" void fn_802DBDA0(
    void* object, CrowdCharacterDefinition_801A4188* definitions, int count);
extern "C" void* fn_8027267C(int index);
extern "C" void fn_802D932C(void* manager, ImpostorCharacter* character);
extern "C" UnidentifiedCrowdCharacter_801A4188* fn_802D818C(
    void* memory, const char* name, int budget,
    const ImpostorCharacterParams* params);
extern "C" void* fn_802D81EC(
    UnidentifiedCrowdCharacter_801A4188* character);
extern "C" void fn_802D93E8(void* manager, int value);
extern "C" void fn_802D9CD8(void* manager, CrowdTweak_801A4188* tweak);
extern "C" void fn_801A49E4(u32 hash, void* object);
extern "C" void fn_801A51D8();

extern "C" void fn_801A4188()
{
    nlVector3 viewVector;
    nlVector3 upVector = { 0.0f, 0.0f, 1.0f };

    cCameraManager::GetViewVector(viewVector);
    if (viewVector.GetLengthSq3D() > lbl_806E5108
        && upVector.GetLengthSq3D() > lbl_806E5108)
    {
        ImpostorManager::GetInstance()->UpdatePositions(
            &viewVector, &upVector);
    }
}

extern "C" void fn_801A421C()
{
    lbl_806E15A0 = 0;

    int captain = GameInfoManager::Instance()->GetTeam(0);
    const CharacterInfo& team
        = GetCharacterInfo(GetCharacterIndexFromCaptain(captain));
    int opponentCaptain = GameInfoManager::Instance()->GetTeam(1);
    const CharacterInfo& opponent
        = GetCharacterInfo(GetCharacterIndexFromCaptain(opponentCaptain));

    const char* teamName = team.mName;
    bool alternateColour = NeedsAlternateColour(team, opponent);
    char fileName[256];
    nlSNPrintf(fileName, sizeof(fileName), lbl_80514000, teamName,
        alternateColour ? lbl_806DCF6C : lbl_806DCF70);

    fn_802C6CAC(fileName, lbl_806DCF68, true);
    TweakEntry_8052BF00* entry
        = fn_802C4504(fn_802C0E30(), lbl_806DCF68, true);
    for (TweakNode_8052BEB0* node = entry->m_ChildHead; node != 0;
        node = node->m_Next)
    {
        const char* name = fn_802C3FDC(node);
        for (int i = 0; i < 36; ++i)
        {
            if (nlStrICmp(name, lbl_80513C80[i].mUnidentified00) == 0)
            {
                lbl_806E1598.data[lbl_806E15A0]
                    = lbl_80513C80[i];
                ++lbl_806E15A0;
            }
        }
    }

    fn_802DBDA0(&lbl_805721E8, lbl_806E1598.data, lbl_806E15A0);
}

extern "C" void* fn_802D8BB4();
extern "C" void fn_801A48A8();

extern "C" void fn_801A43E0(bool alternateView)
{
    CrowdTweak_801A4188* tweak = new (8, false) CrowdTweak_801A4188;
    bool found = tweak->value.fn_802C4FEC("mfSidelineCullingDistance",
        lbl_806E5110, "/Render/Crowd", true, lbl_806E5114,
        lbl_806E5118);
    if (!found)
    {
        tweak->value = tweak->value.GetDefaultValue();
        tweak->value = lbl_806E5110;
    }
    lbl_806E15A8 = tweak;

    int crowdMax = fn_802C2BE8(lbl_80514048, 10000);
    if (!lbl_806E15AC)
    {
        unsigned int scaled = (unsigned int)(crowdMax << 10);
        lbl_80514028.mUnidentified0C = (unsigned int)(
            lbl_806E511C * ((float)scaled / lbl_806E5120));
        lbl_806E15AC = true;
    }

    const UnidentifiedViewConfig_8052E828* viewConfig = &lbl_80514038;
    if (alternateView)
    {
        viewConfig = &lbl_80514028;
    }
    ImpostorManager::GetInstance()->Initialize(
        fn_8027267C(0), crowdMax, viewConfig, 2, false);
    ImpostorManager::GetInstance()->SetImpostorSizeScale(lbl_806E5124);

    int budget = 20000 / lbl_806E15A0;
    ImpostorCharacterParams params;
    params.mWidth = 64;
    params.mHeight = 64;
    params.mUnidentified008 = false;
    params.mUnidentified009 = false;
    params.mBaseAngle = 0;
    for (int i = 0; i < lbl_806E15A0; ++i)
    {
        lbl_806E1588.data[i] = lbl_805721E8.models[i];
        ImpostorCharacterImpl_801A4188* character
            = new (8, false) ImpostorCharacterImpl_801A4188(
                lbl_806E1598.data[i % lbl_806E15A0].mUnidentified00,
                lbl_806E1588.data[i],
                (void*)lbl_806DCF74, budget, 4, 2, &params);
        lbl_806E1590.data[i] = character;
        fn_802D932C(fn_802D8BB4(), character);
    }

    lbl_806E15AD = false;
    fn_801A48A8();

    ImpostorCharacterParams clusterParams;
    clusterParams.mWidth = 128;
    clusterParams.mHeight = 128;
    clusterParams.mUnidentified008 = false;
    clusterParams.mUnidentified009 = false;
    clusterParams.mBaseAngle = 0;
    void* clusterMemory = nlMalloc(0x70, 8, false);
    UnidentifiedCrowdCharacter_801A4188* clusterCharacter
        = (UnidentifiedCrowdCharacter_801A4188*)clusterMemory;
    if (clusterCharacter != 0)
    {
        clusterCharacter = fn_802D818C(
            clusterCharacter, lbl_80514058, 10, &clusterParams);
    }
    lbl_806E15B0 = clusterCharacter;
    u32 firstHash = nlStringLowerHash(lbl_80514068);
    void* cluster = fn_802D81EC(lbl_806E15B0);
    fn_801A49E4(firstHash, cluster);
    fn_801A49E4(nlStringLowerHash(lbl_80514080), cluster);
    fn_801A49E4(nlStringLowerHash(lbl_8051409C), cluster);
    fn_801A49E4(nlStringLowerHash(lbl_805140B8), cluster);

    fn_802D93E8(fn_802D8BB4(), 0);
    fn_802D9CD8(fn_802D8BB4(), lbl_806E15A8);
    fn_801A51D8();
    ImpostorManager::GetInstance()->StaggerAnimations();
}

extern "C" void fn_802D9708();
extern "C" void fn_802D8930(UnidentifiedRenderObject_801A4188* object);
extern "C" void fn_802D911C(void* manager,
    UnidentifiedRenderObject_801A4188* object, int enabled);

extern "C" void fn_801A4734()
{
    ImpostorManager::GetInstance()->ResetImpostors();
    for (int i = 0; i < lbl_806E15A0; ++i)
    {
        delete lbl_806E1588.data[i];
        delete lbl_806E1590.data[i];
    }
    lbl_806E15A0 = 0;

    delete lbl_806E15B0;
    fn_802D8BB4();
    fn_802D9708();
    ImpostorManager::GetInstance()->Uninitialize();

    if (lbl_806E1580 != 0)
    {
        delete lbl_806E1580;
        lbl_806E1580 = 0;
    }
    if (lbl_806E15A8 != 0)
    {
        delete lbl_806E15A8;
        lbl_806E15A8 = 0;
    }
}

extern "C" void fn_801A48A8()
{
    nlMatrix4 transform;
    transform.SetIdentity();
    nlMakeRotationMatrixX(transform, lbl_806E5138);

    float distance = lbl_806E513C;
    float zero = lbl_806E5110;
    transform.m41 = lbl_806E5134 * -distance;
    transform.m42 = zero;
    transform.m43 = zero;
    transform.m44 = lbl_806E5130;

    BasicStadium* stadium = BasicStadium::GetCurrentStadium();
    UnidentifiedStadiumOwner_801A4188* stadiumOwner
        = new (8, false) UnidentifiedStadiumOwner_801A4188(stadium);
    lbl_806E1580 = new (8, false) UnidentifiedRenderObject_801A4188;
    fn_802D8930(lbl_806E1580);
    lbl_806E1580->mUnidentified060 = distance;
    lbl_806E1580->mUnidentified064 = distance;
    lbl_806E1580->mUnidentified068 = distance;
    lbl_806E1580->mUnidentified06C = zero;
    lbl_806E1580->SetTransform(transform);
    fn_802D911C(fn_802D8BB4(), lbl_806E1580, 1);
    delete stadiumOwner;
}

extern "C" void fn_801A4B0C()
{
    lbl_806E15AD = true;
    ImpostorManager::GetInstance()->UpdateCharacters(
        lbl_806E5140, lbl_806DCF80);
    ImpostorManager::GetInstance()->StaggerAnimations();
}

extern "C" void fn_801A4B48()
{
    lbl_806E15AD = false;
    ImpostorManager::GetInstance()->UpdateCharacters(
        lbl_806E5144, lbl_806DCF74);
    ImpostorManager::GetInstance()->StaggerAnimations();
}

extern "C" void fn_801A4B84(float value)
{
    if (!lbl_806E15B4)
    {
        if (lbl_806E15AD)
        {
            if (value < lbl_806DCF8C)
            {
                lbl_806E15AD = false;
                ImpostorManager::GetInstance()->UpdateCharacters(
                    lbl_806E5144, lbl_806DCF74);
                ImpostorManager::GetInstance()->StaggerAnimations();
            }
        }
        else if (value > lbl_806DCF88)
        {
            lbl_806E15AD = true;
            ImpostorManager::GetInstance()->UpdateCharacters(
                lbl_806E5140, lbl_806DCF80);
            ImpostorManager::GetInstance()->StaggerAnimations();
        }
    }
}

ImpostorCharacterImpl_801A4188::~ImpostorCharacterImpl_801A4188()
{
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;

#include "Game/Task/GameRenderTask.h"

#include "types.h"

#include "Game/BasicStadium.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Character.h"
#include "Game/CharacterTemplate.h"
#include "Game/Debug/ShapeRender.h"
#include "Game/Drawable/DrawableCharacter.h"
#include "Game/Drawable/DrawableNetMesh.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Effects/PhotoFlashEffect.h"
#include "Game/FE/feRender.h"
#include "Game/GL/UnidentifiedMeshWriter_802A195C.h"
#include "Game/Game.h"
#include "Game/GameObjectLighting.h"
#include "Game/HBMManager_8024795C.h"
#include "Game/NisPlayer.h"
#include "Game/ObjectBlur.h"
#include "Game/Render/AttackSideIndicators.h"
#include "Game/Render/CrowdManager.h"
#include "Game/Render/ElectricFence.h"
#include "Game/Render/FlareHandler.h"
#include "Game/Render/ImpostorManager.h"
#include "Game/Render/Indicators.h"
#include "Game/Render/Jumbotron.h"
#include "Game/Render/Nis.h"
#include "Game/Render/RLView.h"
#include "Game/Render/RenderShadow.h"
#include "Game/Render/ShootToScoreArrow.h"
#include "Game/Render/ShootToScoreMeter.h"
#include "Game/Render/depthoffield.h"
#include "Game/Render/tu_802D88F4.h"
#include "Game/Render/tu_802DCDB4.h"
#include "Game/ReplayManager.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/MemAlloc.h"
#include "NL/gl/gl.h"
#include "NL/gl/glDraw2.h"
#include "NL/gl/glFont.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTexture.h"
#include "NL/gl/glView.h"
#include "NL/glx/glxMemory.h"
#include "NL/glx/glxSend.h"
#include "NL/nlColour.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "NL/nlTicker.h"
#include "unclassified/tu_80188884.h"
#include "unclassified/tu_801A4188.h"
#include "unclassified/tu_801A6824.h"
#include "unclassified/tu_801A6AAC.h"
#include "unclassified/tu_801AD15C.h"
#include "unclassified/tu_801AE530.h"
#include "unclassified/tu_801B369C.h"
#include "unclassified/tu_802B7798.h"
#include "unclassified/tu_8037091C.h"

#include <string.h>

extern const nlVector3 lbl_804DCBE8;
extern void* lbl_806E14F8;

GameRenderTask* GameRenderTask::sInstance;

u8 g_bRenderWorldEffects = 1;
static u8 g_bTexelDensity;
static u8 g_bRestartWarble;
static u8 g_bWarblePreview;
static u8 g_bWarbleShow;
static float g_fWarbleBlobScale;
static float g_fWarbleDuration = 0.375f;
static float lbl_806DC7E0 = 3.0f;
static float lbl_806DC7E4 = 12.0f;
static float lbl_806DC7E8 = 32.0f;
static float lbl_806DC7EC = 32.0f;
static float lbl_806DC7F0 = 0.8125f;
static float lbl_806E101C;
bool g_bRenderWorld = true;
static bool lbl_806DC7F5 = true;
static bool g_bUpdateImpostorSprites = true;
static u8 g_bMemoryOnScreen;
static u8 g_bShowEmissionLights;

static void WarbleTest(float fDeltaT)
{
    glPoly2 p;

    if (g_bRestartWarble)
    {
        g_bRestartWarble = 0;
        ResetWarble(&gWarble);

        WarbleConfiguration config;
        nlZeroMemory(&config, sizeof(config));
        config.view = GetLayerView(eCLV_Unshadowed);
        config.position = g_pCharacters[0]->m_v3Position;
        config.position.z = (1.0f / 128.0f)
            + reinterpret_cast<UnidentifiedStadiumShadowData*>(
                  BasicStadium::GetCurrentStadium())
                  ->shadowHeight;
        config.blobScale = g_fWarbleBlobScale;
        config.duration = g_fWarbleDuration;
        config.values18[0] = lbl_806DC7E0;
        config.values18[1] = lbl_806DC7E4;
        config.values18[2] = lbl_806DC7E8;
        config.values18[3] = lbl_806DC7EC;
        config.values18[4] = lbl_806DC7F0;
        config.values18[5] = lbl_806E101C;
        SetWarbleInstance(&gWarble, new (8, false) WarbleInstance(config));
    }

    nlColour white = { 0xFF, 0xFF, 0xFF, 0xFF };
    glSetDefaultState(false);
    glSetCurrentTexture(glGetTexture("target/warble"), GLTT_Diffuse);

    p.SetupRectangle(512.0f, 64.0f, 100.0f, 86.0f, 0.0f);
    p.SetColour(white);

    if (g_bWarblePreview)
    {
        p.Attach(GetLayerView(eCLV_FrontEnd), 0, 0);
    }

    u32 showTex = glGetTexture(g_bWarbleShow ? "target/offset" : "target/colour");
    glSetCurrentTexture(showTex, GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 3);
    glSetCurrentTextureState(glHandleizeTextureState());

    p.FullCoverage(white, 0.0f);
    p.Attach(GetLayerView(eCLV_WarbleBlend), 0, 0);
}

static int sTriStripIndices[4] = { 3, 0, 2, 1 };
static int sQuadIndices[4] = { 0, 1, 2, 3 };

static void RenderImageQuad(GLView* view, unsigned long texture,
    const UnidentifiedTextureState* pExtraTextureStates, const nlVector2* positions,
    const nlVector2* texcoords)
{
    UnidentifiedMeshWriter_802A195C writer;
    int* indices;
    int primitive;

    if (glHasQuads())
    {
        indices = sQuadIndices;
        primitive = 3;
    }
    else
    {
        indices = sTriStripIndices;
        primitive = 1;
    }

    if (writer.Begin(4, primitive, 0))
    {
        memcpy((u8*)writer.model->packets->unknown20 + 8, pExtraTextureStates, 0x10);

        UnidentifiedTextureState* state
            = (UnidentifiedTextureState*)writer.model->packets->unknown20;
        state->texture = texture;
        state->textureIndex = 0xFFFF;
        state->SetWrapS(1);
        state->SetWrapT(1);
        state->unknown07 = 0;

        for (int n = 0; n < 2; n++)
        {
            int i0 = indices[0];
            writer.Texcoord(texcoords[i0]);
            writer.Vertex(positions[i0].x, positions[i0].y, 0.0f);
            int i1 = indices[1];
            writer.Texcoord(texcoords[i1]);
            writer.Vertex(positions[i1].x, positions[i1].y, 0.0f);
            indices += 2;
        }

        if (writer.End())
        {
            view->AttachModel(writer.model, 0);
        }
    }
}

void InstallImageRenderCallback()
{
    g_pfnRenderImage = RenderImageQuad;
}

void GameRenderTask::Run(float fDeltaT)
{
    mValue_20 += fDeltaT;

    if (gpHBMManager != 0 && gpHBMManager->mActive && gpHBMManager->mReady)
    {
        return;
    }

    glx_Fog(true);
    ImpostorManager::GetInstance()->InvalidateCapture();
    DrawableNetMesh::Reset();

    if (UnidentifiedManager_80188928::Instance() != 0)
    {
        UnidentifiedManager_80188928::Instance()->Update(fDeltaT);
    }

    if (NisPlayer::Instance()->WorldIsFrozen())
    {
        void* save14F8;
        GLView* save19D8;
        int saveView;
        void* saveStadium068;
        void* saveStadium06C;
        GLView* saveShapeView;
        GLView* save132C;
        u32 save1330;
        RLView* pipView;
        RLView* pipAlphaView;
        void* saveContext;
        RLView* saveShadowView;

        pipView = GetLayerView(eCLV_PictureInPicture);
        pipAlphaView = GetLayerView(eCLV_PictureInPictureAlpha);
        saveStadium068 = BasicStadium::GetCurrentStadium()->mUnidentified068;
        saveStadium06C = BasicStadium::GetCurrentStadium()->mUnidentified06C;
        save14F8 = lbl_806E14F8;
        lbl_806E14F8 = pipView;
        saveShapeView = g_ShapeRenderer.m_eView;
        g_ShapeRenderer.m_eView = pipView;
        save132C = g_pNetMeshView;
        save1330 = g_NetMeshInvisiblePlaneView;
        BasicStadium::GetCurrentStadium()->mUnidentified068 = pipView;
        BasicStadium::GetCurrentStadium()->mUnidentified06C = pipAlphaView;
        save19D8 = g_pNisRenderView;
        saveView = g_nCharacterView;
        g_pNetMeshView = pipAlphaView;
        g_NetMeshInvisiblePlaneView = save1330;
        g_pNisRenderView = pipView;
        g_nCharacterView = eCLV_PictureInPicture;
        saveShadowView = SetCharacterShadowView(pipView);

        NisPlayer::Instance()->Render(0);

        if (NisPlayer::Instance()->mUnidentified34338 == 4)
        {
            ReplayManager::Instance()->RenderSnapshotAt(0.0f);
        }
        else
        {
            saveContext = EmissionManager::Instance()->mContext;
            EmissionManager::Instance()->SetContext(pipAlphaView);
            EmissionManager::Instance()->Render();
            EmissionManager::Instance()->SetContext(saveContext);
            RenderFrame(fDeltaT, true);
        }

        g_pNisRenderView = save19D8;
        lbl_806E14F8 = save14F8;
        SetCharacterShadowView(saveShadowView);
        g_nCharacterView = saveView;
        g_pNetMeshView = save132C;
        g_NetMeshInvisiblePlaneView = save1330;
        g_ShapeRenderer.m_eView = saveShapeView;
        BasicStadium::GetCurrentStadium()->mUnidentified068 = saveStadium068;
        BasicStadium::GetCurrentStadium()->mUnidentified06C = saveStadium06C;

        NisPlayer::Instance()->Render(1);
        RenderFrame(0.0f, false);
    }
    else
    {
        NisPlayer::Instance()->Render(0);
        RenderFrame(fDeltaT, false);
        gpNumberDisplay->Render();
    }
}

static inline void MemoryOnScreen()
{
    float x0;
    float x1;
    float y0;
    float y1;

    nlColour bg = { 0x3A, 0x6E, 0xA5, 0xFF };
    nlColour fg = { 0x00, 0x00, 0x00, 0xFF };

    int savedFont = glFontSetFont(0);

    glFontVirtualPosToScreenCoordPos(0.0f, -2.0f, x0, y0);
    glFontVirtualPosToScreenCoordPos(0.0f, 1.0f, x1, y1);

    g_ShapeRenderer.DrawRectangle2D(0.0f, y0 - 4.0f, 640.0f, 8.0f + (y1 - y0), -0.5f, bg, -1);

    glFontBegin(false);
    glFontPrintf(GetDebugFontView(), -1, -2, fg, "main : %u free, %u block",
        VirtualAllocator.TotalFreeMemory() >> 10, VirtualAllocator.LargestFreeBlock() >> 10);
    glFontPrintf(GetDebugFontView(), -1, -1, fg, "rc : %u free", glx_GetFreeMemory() >> 10);
    glFontPrintf(GetDebugFontView(), -1, 0, fg, "intl : %u free, %u block",
        StandardAllocator.TotalFreeMemory() >> 10, StandardAllocator.LargestFreeBlock() >> 10);
    glFontEnd();

    glFontSetFont(savedFont);
}

void GameRenderTask::RenderFrame(float fDeltaT, bool bPictureInPicture)
{
    static unsigned int lastTick;
    static s8 init;
    static float prevTime;

    glPoly2 p;
    float w;
    float h;

    bool bCaptainShot = false;
    if (nlTaskManager::m_pInstance->mCurrentState == 2 && g_pGame->mUnidentified040)
    {
        bCaptainShot = true;
    }

    if (!bPictureInPicture)
    {
        switch (nlTaskManager::m_pInstance->mCurrentState)
        {
        case 8:
        case 0x10:
            if (gPeachPhotoState.state == 1)
            {
                EndPeachPhoto(&gPeachPhotoState, true);
            }
            break;
        default:
            RenderPeachPhoto(&gPeachPhotoState);
            break;
        }
    }

    NisPlayer::Instance()->fn_8027E5D0();

    if (g_bShowEmissionLights)
    {
        for (int lightIndex = 0; lightIndex < GetEmissionManager()->GetNumLights(); lightIndex++)
        {
            EffectsLight* light = GetEmissionManager()->GetLight(lightIndex);
            nlColour lightColour;
            lightColour.c[0] = light->m_Colour.c[0];
            lightColour.c[1] = light->m_Colour.c[1];
            lightColour.c[2] = light->m_Colour.c[2];
            lightColour.c[3] = 0x40;
            g_ShapeRenderer.DrawSphere(light->m_v3Position, lightColour, light->m_fRadius);
        }
    }

    if (g_bMemoryOnScreen)
    {
        MemoryOnScreen();
    }

    UpdateGameObjectLighting();
    ClearCharacterShadowsUpdated();

    if (!g_bRenderWorld)
    {
        return;
    }

    bool bFrameLocked = GetFixedUpdateTask()->mfFrameLockTime > 0.0f;
    if (bFrameLocked)
    {
        FixedUpdateTask::DecrementFrameLock(fDeltaT);
    }

    if (!bCaptainShot)
    {
        WorldNPCManager_802DD4F0* worldNPCs = gpWorldNPCManager;
        worldNPCs->Render(GetLayerView(eCLV_MoreCharacters));
    }

    u32 whiteTexture = glGetTexture("global/white");

    float dt = (nlTaskManager::m_pInstance->mCurrentState == 1) ? 0.0f : fDeltaT;

    Jumbotron::instance.Update(dt);
    CrowdManager::instance.Update(dt);
    FlareHandler::instance.Render();
    UpdateWarble(&gWarble, dt);
    RenderWarble(&gWarble);
    UpdateAttackSideIndicators();

    for (int charIndex = 0; charIndex < 10; charIndex++)
    {
        if (g_pCharacters[charIndex] != 0)
        {
            g_pCharacters[charIndex]->UpdateBlinking(dt);
        }
    }

    if (g_bTexelDensity)
    {
        if (glTextureLoad(whiteTexture))
        {
            w = (float)(s32)glTextureGetWidth();
            h = (float)(s32)glTextureGetHeight();
            p.SetupRectangle(24.0f, 24.0f, w, h, 0.0f);

            WorldDarkening::SetPolyColour(p, 0xFF, 0xFF, 0xFF, 0xFF);

            glSetDefaultState(false);
            glSetCurrentTexture(whiteTexture, GLTT_Diffuse);
            glSetTextureState(GLTS_DiffuseWrap, 3);
            glSetCurrentTextureState(glHandleizeTextureState());
            p.Attach(GetLayerView(eCLV_FrontEnd), 0, 0);
        }
    }

    if (g_bRenderWorldEffects)
    {
        RenderWorldNPCs();
    }
    UpdateHighRange();

    if (nlTaskManager::m_pInstance->mCurrentState != 4)
    {
        ReplayManager::Instance()->RenderSnapshotAt(fDeltaT);
    }
    else
    {
        nlVector3 up = lbl_804DCBE8;
        cCameraManager::m_UpVectorStack[cCameraManager::m_UpVectorStackSize] = up;
    }

    u32 currState = nlTaskManager::m_pInstance->mCurrentState;
    if (currState == 2
        || (currState == 1 && nlTaskManager::m_pInstance->mPreviousState == 2))
    {
        if (g_pGame->mUnidentified040)
        {
            float indicatorDt;
            if (currState == 2)
            {
                indicatorDt = fDeltaT;
            }
            else
            {
                indicatorDt = 0.0f;
            }
            UpdateAndRenderMegaBallIndicators(indicatorDt);
        }
        else
        {
            float indicatorDt;
            if (currState == 2)
            {
                indicatorDt = fDeltaT;
            }
            else
            {
                indicatorDt = 0.0f;
            }
            UpdateAndRenderIndicators(indicatorDt);
        }

        ShootToScoreMeter::instance.UpdateAndRender(fDeltaT);
        gScreenOverlay_801A6824.UpdateAndRender(fDeltaT);
    }
    else if (currState == 0x10)
    {
        if (!init)
        {
            lastTick = nlGetTicker();
            init = 1;
        }

        u32 tick = nlGetTicker();
        nlGetTickerDifference(lastTick, tick);
        lastTick = tick;
    }

    WorldDarkening::Instance().Update(fDeltaT);
    WorldDarkening::Instance().Render();
    RenderShadowVolumeBlend(GetLayerView(eCLV_ShadowVolumeBlend));
    DepthOfFieldManager::instance.Update();

    if ((nlTaskManager::m_pInstance->mCurrentState & 0x00020008) == 0)
    {
        BlurManager::Update(fDeltaT);
    }

    if (glTextureLoad(glGetTexture("target/warble")))
    {
        WarbleTest(fDeltaT);
    }

    if (g_bRenderWorldEffects)
    {
        PhotoFlash::Render(fDeltaT);
        UpdateElectricFence(fDeltaT);
    }

    if (g_bRenderWorldEffects)
    {
        if (ImpostorManager::GetInstance()->mEnabled)
        {
            UpdateImpostorPositions();
            UpdateCrowdVisibility(GetCrowdImpostorManager(),
                GetLayerView(bPictureInPicture ? eCLV_PictureInPicture : eCLV_ImpostorOut));

            if (ImpostorManager::GetInstance()->mUnidentified037)
            {
                ReleaseCrowdImpostors(GetCrowdImpostorManager());
            }

            bool paused = nlTaskManager::m_pInstance->mCurrentState == 0x10;
            ImpostorManager::GetInstance()->mUnidentified04C = g_bUpdateImpostorSprites && !paused;
            ImpostorManager::GetInstance()->UpdateSprites();

            if (ImpostorManager::GetInstance()->mUnidentified037)
            {
                UpdateCrowdVisibility(GetCrowdImpostorManager(),
                    GetLayerView(bPictureInPicture ? eCLV_PictureInPicture : eCLV_ImpostorOut));
            }

            ImpostorManager::GetInstance()->Render(
                GetLayerView(bPictureInPicture ? eCLV_PictureInPicture : eCLV_ImpostorOut), false);
        }
    }
}

void SetRenderWorldEffects(unsigned int enabled)
{
    if (enabled == g_bRenderWorldEffects)
    {
        return;
    }
    g_bRenderWorldEffects = enabled;
    if (enabled == 0)
    {
        EmissionManager* manager = EmissionManager::Instance();
        manager->mRenderPersistentOnly = 1;
    }
    else
    {
        EmissionManager* manager = EmissionManager::Instance();
        manager->mRenderPersistentOnly = 0;
    }
}

bool fn_80115EB0()
{
    return lbl_806DC7F5;
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;

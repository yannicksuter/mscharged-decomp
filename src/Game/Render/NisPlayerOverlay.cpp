#include "Game/Render/NisPlayerOverlay.h"

#include "Game/NisPlayer.h"
#include "Game/Camera/CameraMan.h"
#include "Game/PoseAccumulator.h"
#include "Game/Render/ImpostorModel.h"
#include "Game/Render/RLView.h"
#include "NL/gl/glDraw2.h"
#include "NL/gl/glDraw3.h"
#include "NL/gl/glState.h"
#include "NL/nlColour.h"
#include "NL/nlString.h"
#include "NL/platvmath.h"

#include "Game/UnidentifiedStaticStorage.h"

u32 g_NisOverlayCheckerTexture = glGetTexture("global/checkers");
u8 g_HolotronOpaque;
u8 g_HolotronDoubleDraw;
u32 g_HolotronBoneHash = nlStringLowerHash("Holotron_Bone01");

static bool sHolotronFacesCamera = true;
static bool sTrackHolotronCamera = true;
static float sHolotronWidth = 22.0f;
static float sHolotronHeight = 17.0f;
static float sHolotronVerticalOffset = 7.0f;
static int sHolotronRed = 0x80;
static int sHolotronGreen = 0xFF;
static int sHolotronBlue = 0xFF;

static inline void CopyCameraRotation(nlMatrix4& rotation, const nlMatrix4& view)
{
    rotation.m11 = view.m11;
    rotation.m12 = view.m12;
    rotation.m13 = view.m13;
    rotation.m14 = 0.0f;
    rotation.m21 = view.m21;
    rotation.m22 = view.m22;
    rotation.m23 = view.m23;
    rotation.m24 = 0.0f;
    rotation.m31 = view.m31;
    rotation.m32 = view.m32;
    rotation.m33 = view.m33;
    rotation.m34 = 0.0f;
    rotation.m41 = 0.0f;
    rotation.m42 = 0.0f;
    rotation.m43 = 0.0f;
    rotation.m44 = 1.0f;
}

/**
 * Offset/Address/Size: 0x0 | 0x8028400C | size: 0x14
 */
NisPlayerNoOverlay::NisPlayerNoOverlay(NisPlayer* player)
{
    mPlayer = player;
}

/**
 * Offset/Address/Size: 0x14 | 0x80284020 | size: 0x4
 */
void NisPlayerNoOverlay::Render()
{
}

/**
 * Offset/Address/Size: 0x18 | 0x80284024 | size: 0x14
 */
NisPlayerPIPOverlay::NisPlayerPIPOverlay(NisPlayer* player)
{
    mPlayer = player;
}

/**
 * Offset/Address/Size: 0x2C | 0x80284038 | size: 0xB8
 */
void NisPlayerPIPOverlay::Render()
{
    glSetDefaultState(false);
    glSetTextureState(GLTS_DiffuseWrap, 3);
    glSetCurrentTexture(glGetTexture("target/pip"), GLTT_Diffuse);
    glSetRasterState(GLS_DepthTest, 0);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetCurrentTextureState(glHandleizeTextureState());

    glPoly2 poly;
    poly.SetupRectangle(360.0f, 270.0f, 240.0f, 150.0f, 0.0f);

    nlColour colour;
    nlColourSet(colour, 0xFF, 0xFF, 0xFF, 0xFF);
    poly.SetColour(colour);
    poly.Attach(GetLayerView(eCLV_FrontEnd), 0, 0);
}

/**
 * Offset/Address/Size: 0xE4 | 0x802840F0 | size: 0x14
 */
NisPlayerHolotronOverlay::NisPlayerHolotronOverlay(NisPlayer* player)
{
    mPlayer = player;
}

/**
 * Offset/Address/Size: 0xF8 | 0x80284104 | size: 0x588
 */
void NisPlayerHolotronOverlay::Render()
{
    glQuad3 quad;
    nlMatrix4 transform;

    glSetDefaultState(true);
    glSetRasterState(GLS_DepthTest, 1);
    glSetRasterState(GLS_DepthWrite, 0);
    if (!g_HolotronOpaque)
    {
        glSetRasterState(GLS_AlphaBlend, 2);
    }
    glSetRasterState(GLS_Culling, 0);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetCurrentTexture(glGetTexture("target/pip"), GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 0);
    glSetCurrentTextureState(glHandleizeTextureState());

    nlMakeRotationMatrixX(transform, 1.5707964f);
    if (sHolotronFacesCamera)
    {
        nlTransposeMatrix(transform, cCameraManager::PeekCamera()->GetViewMatrix());
        if (sTrackHolotronCamera)
        {
            mPlayer->mUseViewMatrixOverride = true;
            if (!mPlayer->mInitialCameraRotationCached)
            {
                nlMatrix4 initialRotation;
                CopyCameraRotation(initialRotation, mPlayer->mCamera[0].GetViewMatrix());
                nlInvertMatrix(mPlayer->mInitialCameraRotationInverse, initialRotation);
                mPlayer->mInitialCameraRotationCached = true;
            }

            nlVector3 headPosition;
            mPlayer->mPlaying[1]->GetMainCharacterHeadPosition(headPosition);
            cAnimCamera* secondaryCamera = mPlayer->GetSecondaryCamera();
            nlMatrix4 cameraInverse;
            nlMatrix4 cameraAdjustment;
            nlMatrix4 translation;
            nlMatrix4 rotation;
            nlMatrix4 viewOverride;
            nlInvertMatrix(cameraInverse, cCameraManager::PeekCamera()->GetViewMatrix());
            translation.SetIdentity();
            nlVec3Scale(headPosition, -1.0f);
            translation.SetTranslation(headPosition);
            CopyCameraRotation(rotation, mPlayer->mCamera[0].GetViewMatrix());
            nlMultMatrices(rotation, mPlayer->mInitialCameraRotationInverse);
            nlMultMatrices(cameraAdjustment, translation, rotation);
            nlVec3Scale(headPosition, -1.0f);
            translation.SetTranslation(headPosition);
            nlMultMatrices(cameraAdjustment, translation);
            nlMultMatrices(viewOverride, cameraAdjustment, secondaryCamera->GetViewMatrix());
            mPlayer->mViewMatrixOverride = viewOverride;
        }
    }

    ImpostorModel* holotron = mPlayer->mPlaying[0]->FindImpostor("holotron");
    nlVector3 position = holotron->mPoseAccumulator->GetNodeMatrixByHashID(g_HolotronBoneHash).GetTranslation();
    position.z += sHolotronHeight + sHolotronVerticalOffset;
    transform.SetTranslation(position);
    quad.SetupRotatedRectangle(sHolotronWidth, sHolotronHeight, transform, false, true);
    if (g_HolotronOpaque)
    {
        quad.SetColour(0xFF, 0xFF, 0xFF, 0xFF);
    }
    else
    {
        quad.SetColour(sHolotronRed, sHolotronGreen, sHolotronBlue, 0x80);
    }
    glAttachQuad3((eGLView)GetLayerView(eCLV_Particles), 1, &quad);
    if (g_HolotronDoubleDraw)
    {
        glAttachQuad3((eGLView)GetLayerView(eCLV_Particles), 1, &quad);
    }
    glSetDefaultState(false);
}

/**
 * Offset/Address/Size: 0x680 | 0x8028468C | size: 0x10
 */
void SetHolotronDimensions(float verticalOffset, float width, float height)
{
    sHolotronVerticalOffset = verticalOffset;
    sHolotronWidth = width;
    sHolotronHeight = height;
}

/**
 * Offset/Address/Size: 0x690 | 0x8028469C | size: 0x8
 */
void SetHolotronCameraTrackingEnabled(bool enabled)
{
    sTrackHolotronCamera = enabled;
}

/**
 * Offset/Address/Size: 0x698 | 0x802846A4 | size: 0x14
 */
NisPlayerCameraSwapOverlay::NisPlayerCameraSwapOverlay(NisPlayer* player)
{
    mPlayer = player;
}

/**
 * Offset/Address/Size: 0x6AC | 0x802846B8 | size: 0x4
 */
void NisPlayerCameraSwapOverlay::Render()
{
}

/**
 * Offset/Address/Size: 0x6B0 | 0x802846BC | size: 0x28
 */
int NisPlayerCameraSwapOverlay::Update(float dt)
{
    mPlayer->SwapCameras();
    return 1;
}

/**
 * Offset/Address/Size: 0x6D8 | 0x802846E4 | size: 0x4C
 */
NisPlayerPIPExpandOverlay::NisPlayerPIPExpandOverlay(NisPlayer* player, float duration)
{
    mPlayer = player;
    mDuration = duration;
    Reset();
}

/**
 * Offset/Address/Size: 0x724 | 0x80284730 | size: 0xC
 */
void NisPlayerPIPExpandOverlay::Reset()
{
    mTime = 0.0f;
}

/**
 * Offset/Address/Size: 0x730 | 0x8028473C | size: 0xF8
 */
void NisPlayerPIPExpandOverlay::Render()
{
    glSetDefaultState(false);
    glSetTextureState(GLTS_DiffuseWrap, 3);
    glSetCurrentTexture(glGetTexture("target/pip"), GLTT_Diffuse);
    glSetRasterState(GLS_DepthTest, 0);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetCurrentTextureState(glHandleizeTextureState());

    float u;
    float t = mTime / mDuration;
    u = 1.0f - t;

    glPoly2 poly;
    poly.SetupRectangle(360.0f * u, 270.0f * u, 640.0f * t + 240.0f * u,
        480.0f * t + 150.0f * u, 0.0f);

    nlColour colour;
    nlColourSet(colour, 0xFF, 0xFF, 0xFF, 0xFF);
    poly.SetColour(colour);
    poly.Attach(GetLayerView(eCLV_FrontEnd), 0, 0);
}

/**
 * Offset/Address/Size: 0x828 | 0x80284834 | size: 0x4C
 */
int NisPlayerPIPExpandOverlay::Update(float dt)
{
    mTime += dt;
    if (mTime <= mDuration)
    {
        return 3;
    }

    mPlayer->SwapCameras();
    return 0;
}

/**
 * Offset/Address/Size: 0x874 | 0x80284880 | size: 0x8
 */
int NisPlayerPIPExpandOverlay::GetOverlayType()
{
    return 1;
}

/**
 * Offset/Address/Size: 0x87C | 0x80284888 | size: 0x8
 */
int NisPlayerCameraSwapOverlay::GetOverlayType()
{
    return 1;
}

/**
 * Offset/Address/Size: 0x884 | 0x80284890 | size: 0x8
 */
int NisPlayerHolotronOverlay::GetOverlayType()
{
    return 4;
}

/**
 * Offset/Address/Size: 0x88C | 0x80284898 | size: 0x8
 */
int NisPlayerHolotronOverlay::Update(float dt)
{
    return 4;
}

/**
 * Offset/Address/Size: 0x894 | 0x802848A0 | size: 0x8
 */
int NisPlayerPIPOverlay::GetOverlayType()
{
    return 1;
}

/**
 * Offset/Address/Size: 0x89C | 0x802848A8 | size: 0x8
 */
int NisPlayerPIPOverlay::Update(float dt)
{
    return 1;
}

/**
 * Offset/Address/Size: 0x8A4 | 0x802848B0 | size: 0x8
 */
int NisPlayerNoOverlay::GetOverlayType()
{
    return 0;
}

/**
 * Offset/Address/Size: 0x8AC | 0x802848B8 | size: 0x8
 */
int NisPlayerNoOverlay::Update(float dt)
{
    return 0;
}

NisPlayerNoOverlay::~NisPlayerNoOverlay()
{
}

NisPlayerPIPOverlay::~NisPlayerPIPOverlay()
{
}

NisPlayerHolotronOverlay::~NisPlayerHolotronOverlay()
{
}

NisPlayerCameraSwapOverlay::~NisPlayerCameraSwapOverlay()
{
}

NisPlayerPIPExpandOverlay::~NisPlayerPIPExpandOverlay()
{
}

#include "Game/Render/NisPlayerOverlay.inl"

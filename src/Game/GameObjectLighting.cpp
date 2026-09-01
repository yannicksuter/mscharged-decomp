#include <revolution/gx/GXLight.h>
#include <revolution/gx/GXTev.h>
#include <revolution/gx/GXTransform.h>
#include <revolution/mtx/mtx.h>

#include "Game/GameObjectLighting.h"

#include "Game/Camera/CameraMan.h"
#include "Game/Drawable/DrawableCharacter.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glTexture.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialCrystalTweaks.h"
#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxMatrix.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/platvmath.h"
#include "unclassified/Lookup_801A537C.h"

struct StadiumLightingParams
{
    /* 0x00 */ u32 lightRamp;
    /* 0x04 */ s32 rampStartR;
    /* 0x08 */ s32 rampStartG;
    /* 0x0C */ s32 rampStartB;
    /* 0x10 */ s32 rampEndR;
    /* 0x14 */ s32 rampEndG;
    /* 0x18 */ s32 rampEndB;
    /* 0x1C */ f32 keyLightIntensity;
    /* 0x20 */ f32 fillLightIntensity;
    /* 0x24 */ f32 inGameKeyIntensity;
    /* 0x28 */ f32 inGameFillIntensity;
    /* 0x2C */ f32 inGameKeyRotYDeg;
    /* 0x30 */ f32 inGameKeyRotZDeg;
    /* 0x34 */ f32 inGameFillRotYDeg;
    /* 0x38 */ f32 inGameFillRotZDeg;
    /* 0x3C */ u32 unknown3C;
}; // total size: 0x40

struct GameObjectLight
{
    /* 0x00 */ bool enabled;
    /* 0x01 */ u8 unknown01;
    /* 0x02 */ u8 unknown02;
    /* 0x03 */ u8 _pad03;
    /* 0x04 */ f32 intensity;
    /* 0x08 */ f32 unknown08;
    /* 0x0C */ f32 unknown0C;
    /* 0x10 */ nlVector3 worldPosition;
    /* 0x1C */ u8 unknown1C[4];
    /* 0x20 */ f32 unknown20;
}; // total size: 0x24

struct GameObjectLightArray
{
    GameObjectLight lights[2];
}; // total size: 0x48

extern "C" {
extern StadiumLightingParams gStadiumGameObjectLightingParams;
extern GameObjectLightArray lbl_80570AF8;
extern u8 lbl_806DCC68;
extern s32 lbl_806E1428;
extern bool gAlwaysUseCameraRelativeCharacterLighting;
extern PlatTexture* g_pGameObjectLightRamp;
extern u32 lbl_806DCC60;
extern u32 lbl_806DCC6C;
extern u32 lbl_806DCC4C;
extern u32 lbl_806DCC50;
extern u32 lbl_806DCC54;
extern s32 lbl_806DCC5C;
extern bool lbl_806DCC58;
extern bool lbl_806DCC59;
extern bool lbl_806DCC5A;
extern u32 lbl_806DCC70;
extern s32 lbl_806DCC74;
extern f32 lbl_806DCC44;
extern bool lbl_806DC7D8;
extern GLView* lbl_806E143C;
extern Lookup_801A537C* lbl_806E1420;
extern bool lbl_806E1413;
extern bool lbl_806E1440;
extern const u32 lbl_804DCD00[6];
extern const u32 lbl_804DCD18[6];
extern const nlVector3 lbl_804DCD30;
extern const nlVector3 lbl_804DCD3C;
extern const u32 lbl_806E4D10[2];
extern const u8 lbl_806E4D1B;
extern const u8 lbl_806E4D1F;
extern const f32 lbl_806E4CD0;
extern const f32 lbl_806E4CD4;
extern const f32 lbl_806E4CD8;
extern const f32 lbl_806E4D20;
extern const f32 lbl_806E4D24;
extern const f32 lbl_806E4D28;
extern const f32 lbl_806E4D2C;
extern const f32 lbl_806E4D30;
extern const u8 lbl_806E4D34;
extern const u8 lbl_806E4D35;
extern const u8 lbl_806E4D36;
extern const u8 lbl_806E4D37;
extern const u8 lbl_806E4D3B;
extern const u8 lbl_806E4D3C;
extern const u8 lbl_806E4D3D;
extern const u8 lbl_806E4D3E;
extern const u8 lbl_806E4D3F;
extern const f32 lbl_806E4D40;

GameObjectLight* fn_8018230C(s32, bool);

extern Mtx lbl_80511490;
extern nlMatrix4 lbl_80570C18;
extern GXMaterialFloatTweak_804F4190 lbl_80570968;
extern GXMaterialFloatTweak_804F4190 lbl_80570988;
extern GXMaterialFloatTweak_804F4190 lbl_805709A8;
extern GXMaterialFloatTweak_804F4190 lbl_805709C8;

bool fn_80183C54();

nlColour fn_80183C9C(const nlVector2* arg0, bool arg1)
{
    if (!fn_80183C54())
    {
        nlColour var0;
        nlColourSet(var0, 0xFF, 0xFF, 0xFF, 0xFF);
        return var0;
    }

    if (lbl_806DCC5C <= 0xFF)
    {
        nlColour var0;
        nlColourSet(var0, 0xFF, 0xFF, 0xFF, 0xFF);
        return var0;
    }

    f32 var0 = arg0->x * lbl_80570968.value;
    var0 += lbl_805709A8.value;
    f32 var1 = arg0->y * lbl_80570988.value;
    var1 += lbl_805709C8.value;
    var0 = lbl_806E4D2C * var0 + lbl_806E4D2C;
    var1 = lbl_806E4D40 * var1 + lbl_806E4D2C;
    var0 *= (f32)lbl_806E1420->mWidth;
    var1 *= (f32)lbl_806E1420->mHeight;
    return lbl_806E1420->fn_801A5760(var0, var1, arg1);
}

bool fn_80183C54()
{
    bool enabled;

    if (!lbl_806DCC58)
        return false;

    if (lbl_806DCC6C == (u32)-1)
        return false;

    enabled = false;
    if (lbl_806E1420 != 0)
        enabled = lbl_806DC7D8;

    return enabled;
}

void fn_80183BF4(const nlMatrix4* matrix)
{
    if (fn_80183C54())
        nlInvertMatrix(lbl_80570C18, *matrix);
}

void fn_80183B40(u32 matrix)
{
    if (!fn_80183C54())
        return;

    if (matrix == (u32)-1)
    {
        lbl_806DCC70 = -1;
    }
    else if (matrix != lbl_806DCC70)
    {
        lbl_806DCC70 = matrix;

        nlMatrix4 transform;
        glGetMatrix(matrix, transform);

        Mtx textureMatrix;
        glxCopyMatrix(textureMatrix, transform);
        GXLoadTexMtxImm(textureMatrix, 0x36, GX_MTX3x4);
    }
}

void fn_80183A98()
{
    if (fn_80183C54() && lbl_806DCC74 >= 0)
    {
        gxSetNumTexGens(gxGetNumTexGens() - 1);
        gxSetNumTevStages(gxGetNumTevStages() - 1);
        gxSetTexCoordGen(lbl_806DCC74, 1, lbl_806DCC74 + 4, 0x3C);
        lbl_806DCC74 = -1;
    }
}

void fn_801837DC(s32 arg0, u32 arg1)
{
    if (!fn_80183C54())
        return;

    u32 numTevStages;
    u32 numTexGens;
    numTexGens = gxGetNumTexGens();
    numTevStages = gxGetNumTevStages();
    gxSetNumTexGens(numTexGens + 1);
    gxSetNumTevStages(numTevStages + 1);
    lbl_806DCC74 = numTexGens;

    if (arg1 != 0)
    {
        gxSetTevOrder(numTevStages, 0xFF, 0xFF, 0xFF);
        gxSetTevColourIn(numTevStages, 15, 0, 6, 15);
        gxSetTevAlphaIn(numTevStages, 7, 7, 7, 0);

        GXColor colour = *(GXColor*)&arg1;
        GXSetTevColor(GX_TEVREG2, colour);
    }
    else
    {
        gxSetTevOrder(numTevStages, numTexGens, numTexGens, 0xFF);
        if (arg0 && lbl_806DCC5A)
            gxSetTexCoordGen(numTexGens, 1, 0, 0, false, 0x76);
        else
            gxSetTexCoordGen(numTexGens, 1, 0, 0x36, false, 0x76);

        gxSetTevColourIn(numTevStages, 15, 0, 8, 15);
        gxSetTevAlphaIn(numTevStages, 7, 7, 7, 0);

        UnidentifiedTextureState textureState;
        textureState.texture = lbl_806DCC6C;
        textureState.textureIndex = 0xFFFF;
        textureState.flags = 0;
        textureState.unknown07 = 0;
        textureState.SetWrapS(!lbl_806E1413);
        textureState.SetWrapT(!lbl_806E1413);
        fn_8036BE88(numTexGens, &textureState);

        nlMatrix4 transform;
        nlMakeScaleMatrix(transform, lbl_80570968.value,
            lbl_80570988.value, lbl_806E4CD4);
        transform.m41 = lbl_805709A8.value;
        transform.m42 = lbl_805709C8.value;
        transform.m43 = lbl_806E4CD8;
        transform.m44 = lbl_806E4CD4;

        Mtx gxTransform;
        glxCopyMatrix(gxTransform, transform);

        Mtx textureMatrix;
        PSMTXConcat(lbl_80511490, gxTransform, textureMatrix);

        if (arg0 && lbl_806DCC5A)
        {
            Mtx inverse;
            glxCopyMatrix(inverse, lbl_80570C18);
            if (lbl_806DCC59)
                PSMTXConcat(textureMatrix, inverse, textureMatrix);
            else
                PSMTXConcat(inverse, textureMatrix, textureMatrix);
        }

        GXLoadTexMtxImm(textureMatrix, 0x76, GX_MTX3x4);
    }
}

void fn_80183764(u32 textureHandle)
{
    if (textureHandle != (u32)-1 && glTextureLoad(textureHandle))
    {
        lbl_806DCC6C = textureHandle;
        lbl_806E1420 = new (8, false) Lookup_801A537C;
        lbl_806E1420->fn_801A53F0(textureHandle);
    }
    else
    {
        lbl_806DCC6C = -1;
    }
}

void fn_801836FC(s32 arg0)
{
    if (arg0)
    {
        nlColour ambient = {
            (u8)lbl_806DCC4C,
            (u8)lbl_806DCC50,
            (u8)lbl_806DCC54,
            0,
        };
        gxSetChanAmbColour(0, ambient);
    }
    else
    {
        nlColour ambient = {
            0,
            0,
            0,
            0,
        };
        gxSetChanAmbColour(0, ambient);
    }
}

void fn_80183654(s32 arg0)
{
    if (arg0)
    {
        nlColour colour = {
            lbl_806E4D3C,
            lbl_806E4D3D,
            lbl_806E4D3E,
            lbl_806E4D3F,
        };
        gxSetChanMatColour(1, colour);

        nlColour ambient = {
            0,
            0,
            0,
            0,
        };
        gxSetChanAmbColour(1, ambient);

        GXSetChanCtrl(GX_COLOR1, GX_TRUE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT6,
            GX_DF_NONE, GX_AF_SPEC);
    }
    else
    {
        GXSetChanCtrl(GX_COLOR1, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT6,
            GX_DF_NONE, GX_AF_NONE);
    }
}

void fn_801833D0(s32 arg0, GameObjectLight* arg1, f32 arg2, const nlMatrix4& arg3)
{
    if (arg0 < 0 || arg0 >= 2)
        return;

    GXLightObj light;
    nlVector3 viewDir;
    nlVector3 worldDir;
    nlVector3 var0;

    s32 var3 = (s32)(lbl_806E4CD0 * arg1->intensity);
    if (var3 > 255)
        var3 = 255;

    GXColor colour = {
        (u8)var3,
        (u8)var3,
        (u8)var3,
        lbl_806E4D3B,
    };
    GXInitLightColor(&light, colour);

    if (arg1->enabled)
    {
        var0 = arg1->worldPosition;
    }
    else
    {
        float angleY = arg1->unknown08;
        angleY = (lbl_806E4D20 * angleY) / lbl_806E4D24;
        nlVector3 initialDirection = lbl_804DCD3C;
        nlVector3 var1;
        nlVector3 var2;
        nlMatrix4 matY;
        nlMatrix4 matZ;

        nlMakeRotationMatrixY(matY, angleY);
        float angleZ = arg1->unknown0C;
        nlMakeRotationMatrixZ(
            matZ, (lbl_806E4D20 * angleZ) / lbl_806E4D24);
        nlMultDirVectorMatrix(var1, initialDirection, matY);
        nlMultDirVectorMatrix(var2, var1, matZ);
        var1 = var2;

        nlVec3Set(var0, -var1.x, -var1.y, -var1.z);
    }

    nlVector3 origin = {
        0.0f,
        0.0f,
        0.0f,
    };

    float worldY = var0.y - origin.y;
    float worldX = var0.x - origin.x;
    float worldZ = var0.z - origin.z;

    worldDir.x = worldX;
    worldDir.y = worldY;
    worldDir.z = worldZ;

    {
        float lengthSq = worldDir.GetLengthSq3D();
        float recipLength = nlRecipSqrt(lengthSq, true);

        nlVec3Scale(worldDir, recipLength);
    }

    nlMultDirVectorMatrix(viewDir, worldDir, arg3);
    nlVec3Set(viewDir, -viewDir.x, -viewDir.y, -viewDir.z);
    GXInitSpecularDir(&light, viewDir.x, viewDir.y, viewDir.z);

    GXInitLightAttn(&light, lbl_806E4CD8, lbl_806E4CD8, lbl_806E4CD4,
        arg2 * lbl_806E4D2C, lbl_806E4CD8,
        lbl_806E4CD4 - arg2 * lbl_806E4D2C);

    GXLoadLightObjImm(&light, (GXLightID)lbl_806E4D10[arg0]);
}

void fn_801832F4(s32 arg0, s32 arg1, s32 arg2)
{
    if (arg0)
    {
        if (!arg2)
        {
            nlColour colour = {
                lbl_806E4D34,
                lbl_806E4D35,
                lbl_806E4D36,
                lbl_806E4D37,
            };
            gxSetChanMatColour(0, colour);
        }

        s32 var0 = arg1 - 1;
        GXSetChanCtrl(GX_COLOR0, GX_TRUE, GX_SRC_REG, (GXColorSrc)(arg2 != 0),
            (GXLightID)lbl_804DCD18[var0], GX_DF_CLAMP, GX_AF_SPOT);
    }
    else
    {
        s32 var0 = arg1 - 1;
        GXSetChanCtrl(GX_COLOR0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX,
            (GXLightID)lbl_804DCD18[var0], GX_DF_NONE, GX_AF_NONE);
    }
}

void fn_80182F74(s32 lightId, GameObjectLight* pLight, const nlMatrix4& mview)
{
    GXLightObj light;
    nlVector3 viewPos;
    nlVector3 viewDir;
    nlVector3 worldDir;
    nlVector3 var0;

    if (pLight->unknown01)
    {
        s32 var3 = (s32)(lbl_806E4CD0 * pLight->intensity * lbl_806DCC44);
        if (var3 > 255)
            var3 = 255;

        GXColor colour = {
            (u8)((var3 * pLight->unknown1C[0]) >> 8),
            (u8)((var3 * pLight->unknown1C[1]) >> 8),
            (u8)((var3 * pLight->unknown1C[2]) >> 8),
            lbl_806E4D1B,
        };
        GXInitLightColor(&light, colour);
    }
    else
    {
        s32 var3 = (s32)(lbl_806E4CD0 * pLight->intensity * lbl_806DCC44);
        if (var3 > 255)
            var3 = 255;

        GXColor colour = {
            (u8)var3,
            (u8)var3,
            (u8)var3,
            lbl_806E4D1F,
        };
        GXInitLightColor(&light, colour);
    }

    if (pLight->enabled)
    {
        var0 = pLight->worldPosition;
    }
    else
    {
        nlVector3 initialDirection = lbl_804DCD30;
        nlVector3 var1;
        nlVector3 var2;
        nlMatrix4 matZ;
        nlMatrix4 matY;

        nlMakeRotationMatrixY(
            matY, (lbl_806E4D20 * pLight->unknown08) / lbl_806E4D24);
        nlMakeRotationMatrixZ(
            matZ, (lbl_806E4D20 * pLight->unknown0C) / lbl_806E4D24);
        nlMultDirVectorMatrix(var1, initialDirection, matY);
        nlMultDirVectorMatrix(var2, var1, matZ);
        var1 = var2;

        nlVec3Set(var0, -var1.x, -var1.y, -var1.z);
    }

    if (pLight->unknown02)
    {
        nlMultPosVectorMatrix(viewPos, var0, mview);
        GXInitLightPos(&light, viewPos.x, viewPos.y, viewPos.z);
        GXInitLightAttnA(&light, lbl_806E4CD4, lbl_806E4CD8, lbl_806E4CD8);
        GXInitLightDistAttn(
            &light, lbl_806E4D28 * pLight->unknown20, lbl_806E4D2C, GX_DA_STEEP);
    }
    else
    {
        nlVector3 origin = {
            0.0f,
            0.0f,
            0.0f,
        };

        float worldY = var0.y - origin.y;
        float worldX = var0.x - origin.x;
        float worldZ = var0.z - origin.z;

        worldDir.x = worldX;
        worldDir.y = worldY;
        worldDir.z = worldZ;

        if (worldDir.GetLengthSq3D() <= lbl_806E4CD8)
            worldDir.x = lbl_806E4CD4;

        {
            float lengthSq = worldDir.GetLengthSq3D();
            float recipLength = nlRecipSqrt(lengthSq, true);

            nlVec3Scale(worldDir, recipLength);
        }

        nlMultDirVectorMatrix(viewDir, worldDir, mview);
        nlVec3Scale(viewDir, lbl_806E4D30);

        GXInitLightPos(&light, viewDir.x, viewDir.y, viewDir.z);
        GXInitLightAttnA(&light, lbl_806E4CD4, lbl_806E4CD8, lbl_806E4CD8);
        GXInitLightDistAttn(&light, lbl_806E4D30, lbl_806E4CD4, GX_DA_OFF);
    }

    GXLoadLightObjImm(&light, (GXLightID)lbl_804DCD00[lightId]);
}

void fn_80182ED0(s32 arg0, GLView* arg1, bool arg2)
{
    s32 var0;
    nlMatrix4 var1;

    if (arg1 == lbl_806E143C && arg2 == lbl_806E1440)
        return;

    GLViewInterface* var2 = arg1->m_Interface;
    lbl_806E143C = arg1;
    var2->GetViewMatrix(var1);
    lbl_806E1440 = arg2;

    for (var0 = 0; var0 < arg0; var0++)
    {
        GameObjectLight* var3 = fn_8018230C(var0, arg2);
        fn_80182F74(var0, var3, var1);
    }
}

void fn_80182EC8(s32 arg0)
{
    lbl_806E1428 = arg0;
}

void fn_80182EC0(u32 arg0)
{
    lbl_806DCC60 = arg0;
}

u32 fn_80182EB8()
{
    return lbl_806DCC60;
}
}

u32 GetGameObjectLightRamp()
{
    return gStadiumGameObjectLightingParams.lightRamp;
}

void FillInGameObjectLightRamp()
{
    s32 i;
    f32 deltaB;
    f32 deltaG;
    f32 deltaR;
    u8* pTextureData;
    StadiumLightingParams* pRampParams = &gStadiumGameObjectLightingParams;

    deltaR = (f32)(pRampParams->rampEndR - pRampParams->rampStartR);
    deltaG = (f32)(pRampParams->rampEndG - pRampParams->rampStartG);
    deltaB = (f32)(pRampParams->rampEndB - pRampParams->rampStartB);

    pTextureData = (u8*)g_pGameObjectLightRamp->m_LinearData;
    for (i = 0; i < 0x100; i += 8)
    {
        {
            f32 t = (f32)i / 256.0f;
            pTextureData[0] = (u8)(t * deltaR + (f32)pRampParams->rampStartR);
            pTextureData[1] = (u8)(t * deltaG + (f32)pRampParams->rampStartG);
            pTextureData[2] = (u8)(t * deltaB + (f32)pRampParams->rampStartB);
            pTextureData[3] = 0xFF;
        }
        {
            f32 t = (f32)(i + 1) / 256.0f;
            pTextureData[4] = (u8)(t * deltaR + (f32)pRampParams->rampStartR);
            pTextureData[5] = (u8)(t * deltaG + (f32)pRampParams->rampStartG);
            pTextureData[6] = (u8)(t * deltaB + (f32)pRampParams->rampStartB);
            pTextureData[7] = 0xFF;
        }
        {
            f32 t = (f32)(i + 2) / 256.0f;
            pTextureData[8] = (u8)(t * deltaR + (f32)pRampParams->rampStartR);
            pTextureData[9] = (u8)(t * deltaG + (f32)pRampParams->rampStartG);
            pTextureData[10] = (u8)(t * deltaB + (f32)pRampParams->rampStartB);
            pTextureData[11] = 0xFF;
        }
        {
            f32 t = (f32)(i + 3) / 256.0f;
            pTextureData[12] = (u8)(t * deltaR + (f32)pRampParams->rampStartR);
            pTextureData[13] = (u8)(t * deltaG + (f32)pRampParams->rampStartG);
            pTextureData[14] = (u8)(t * deltaB + (f32)pRampParams->rampStartB);
            pTextureData[15] = 0xFF;
        }
        {
            f32 t = (f32)(i + 4) / 256.0f;
            pTextureData[16] = (u8)(t * deltaR + (f32)pRampParams->rampStartR);
            pTextureData[17] = (u8)(t * deltaG + (f32)pRampParams->rampStartG);
            pTextureData[18] = (u8)(t * deltaB + (f32)pRampParams->rampStartB);
            pTextureData[19] = 0xFF;
        }
        {
            f32 t = (f32)(i + 5) / 256.0f;
            pTextureData[20] = (u8)(t * deltaR + (f32)pRampParams->rampStartR);
            pTextureData[21] = (u8)(t * deltaG + (f32)pRampParams->rampStartG);
            pTextureData[22] = (u8)(t * deltaB + (f32)pRampParams->rampStartB);
            pTextureData[23] = 0xFF;
        }
        {
            f32 t = (f32)(i + 6) / 256.0f;
            pTextureData[24] = (u8)(t * deltaR + (f32)pRampParams->rampStartR);
            pTextureData[25] = (u8)(t * deltaG + (f32)pRampParams->rampStartG);
            pTextureData[26] = (u8)(t * deltaB + (f32)pRampParams->rampStartB);
            pTextureData[27] = 0xFF;
        }
        {
            f32 t = (f32)(i + 7) / 256.0f;
            pTextureData[28] = (u8)(t * deltaR + (f32)pRampParams->rampStartR);
            pTextureData[29] = (u8)(t * deltaG + (f32)pRampParams->rampStartG);
            pTextureData[30] = (u8)(t * deltaB + (f32)pRampParams->rampStartB);
            pTextureData[31] = 0xFF;
        }
        pTextureData += 0x20;
    }

    for (i = 1; i < 4; i++)
    {
        memcpy((u8*)g_pGameObjectLightRamp->m_LinearData + (i * 0x400), g_pGameObjectLightRamp->m_LinearData, 0x400);
    }

    g_pGameObjectLightRamp->Swizzle(false);
    g_pGameObjectLightRamp->Prepare();
}

void SetCameraRelativeLightData(void* pLightData)
{
    static nlVector3 keyLightInViewSpace;
    static nlVector3 fillLightInViewSpace;
    static bool initedLightInViewSpace;
    nlVector3 transformedDir;
    nlVector3 viewVec;
    nlMatrix4 viewRotMat;
    nlMatrix4 matZ;
    nlMatrix4 matY;

    if (!initedLightInViewSpace)
    {
        nlVector3 initialDirection = { 1.0f, 0.0f, 0.0f };
        nlVector3 fillDirection;
        nlVector3 keyDirection;

        nlMakeRotationMatrixY(matY, 0.7853982f);
        nlMakeRotationMatrixZ(matZ, -0.69813174f);
        nlMultDirVectorMatrix(keyLightInViewSpace, initialDirection, matY);
        nlMultDirVectorMatrix(keyDirection, keyLightInViewSpace, matZ);
        keyLightInViewSpace = keyDirection;

        nlMakeRotationMatrixY(matY, 0.5235988f);
        nlMakeRotationMatrixZ(matZ, 0.34906587f);
        nlMultDirVectorMatrix(fillLightInViewSpace, initialDirection, matY);
        nlMultDirVectorMatrix(fillDirection, fillLightInViewSpace, matZ);
        fillLightInViewSpace = fillDirection;

        initedLightInViewSpace = true;
    }

    cCameraManager::GetViewVector(viewVec);

    f32 angle = nlATan2f(viewVec.y, viewVec.x);
    u16 u16Angle = (u16)(s32)(angle * 10430.378f);
    f32 radAngle = (f32)u16Angle * 0.0000958738f;

    nlMakeRotationMatrixZ(viewRotMat, radAngle);

    StadiumLightingParams* params = &gStadiumGameObjectLightingParams;
    GameObjectLightArray* pLights = (GameObjectLightArray*)pLightData;

    nlMultDirVectorMatrix(transformedDir, keyLightInViewSpace, viewRotMat);

    pLights->lights[0].enabled = true;
    nlVec3Set(pLights->lights[0].worldPosition, -transformedDir.x, -transformedDir.y, -transformedDir.z);
    pLights->lights[0].intensity = params->keyLightIntensity;

    nlMultDirVectorMatrix(transformedDir, fillLightInViewSpace, viewRotMat);

    pLights->lights[1].enabled = true;
    nlVec3Set(pLights->lights[1].worldPosition, -transformedDir.x, -transformedDir.y, -transformedDir.z);
    pLights->lights[1].intensity = params->fillLightIntensity;
}

void UpdateGameObjectLighting()
{
    if (!lbl_806DCC68)
        return;

    if (!DrawableCharacter::sCameraRelativeLighting && !gAlwaysUseCameraRelativeCharacterLighting && lbl_806E1428 != 1)
        return;

    SetCameraRelativeLightData(&lbl_80570AF8);
}

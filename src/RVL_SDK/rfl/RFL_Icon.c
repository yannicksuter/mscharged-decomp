#include <RVLFaceLib/RVLFaceLibInternal.h>

#include <revolution/gx.h>
#include <revolution/mtx.h>

#include <math.h>

void RFLiMakeIcon(void* buf, const RFLiCharInfo* info, RFLExpression expression,
                  const RFLIconSetting* setting);

RFLErrcode RFLMakeIcon(void* buf, RFLDataSource source, RFLMiddleDB* middleDB,
                       u16 index, RFLExpression expression,
                       const RFLIconSetting* setting) {
    RFLiCharInfo info;

    RFLErrcode err = RFLiPickupCharInfo(&info, source, middleDB, index);
    if (err == RFLErrcode_Success) {
        RFLiMakeIcon(buf, &info, expression, setting);
    }

    return err;
}

void RFLiMakeIcon(void* buf, const RFLiCharInfo* info, RFLExpression expression,
                  const RFLIconSetting* setting) {
    RFLCharModel model;
    void* modelBuf;

    f32 vp[6];

    Mtx viewMtx;

    u32 scissorOffsetX, scissorOffsetY;
    u32 scissorWidth, scissorHeight;

    RFLiCoordinateData coordData;

    {
        RFLiCoordinateData iconCoordData = {1, 2, 0, FALSE, FALSE, FALSE};

        coordData = coordinateData;
        RFLiSetCoordinateData(&iconCoordData);
    }

    {
        RFLResolution resolution;
        u32 expFlag;

        if ((setting->width > 128 || setting->height > 128) &&
            RFLiGetUseDeluxTex()) {
            resolution = RFLResolution_256;
        } else if (setting->width > 64 || setting->height > 64) {
            resolution = RFLResolution_128;
        } else {
            resolution = RFLResolution_64;
        }

        expFlag = 1 << expression;

        modelBuf = RFLiAlloc32(RFLGetModelBufferSize(resolution, expFlag));

        RFLiInitCharModel(&model, (RFLiCharInfo*)info, modelBuf, resolution, expFlag);

        RFLSetExpression(&model, expression);
    }

    {
        GXColor backColor;
        if (setting->bgType == RFLIconBG_Direct) {
            backColor = setting->bgColor;
        } else {
            backColor = RFLGetFavoriteColor(info->personal.color);
        }
        backColor.a = 0;

        GXGetScissor(&scissorOffsetX, &scissorOffsetY, &scissorWidth,
                     &scissorHeight);
        GXSetScissor(0, 0, setting->width, setting->height);

        RFLiSetupCopyTex(GX_TF_RGB5A3, setting->width, setting->height, buf,
                         backColor);
    }

    GXGetViewportv(vp);
    GXSetViewport(0.0f, 0.0f, setting->width, setting->height, 0.0f, 1.0f);

    {
        Mtx44 projMtx;
        f32 fovy;
        f32 aspect;

        aspect = (f32)setting->width / (f32)setting->height;

        if (setting->width < setting->height) {
            fovy = 2 * (57.2957801818848f * atan2f(43.2f / aspect, 500.0f));
        } else {
            fovy = 2 * (57.2957801818848f * atan2f(43.2f, 500.0f));
        }

        C_MTXPerspective(projMtx, fovy, aspect, 500.0f, 700.0f);
        GXSetProjection(projMtx, GX_PERSPECTIVE);
    }

    {
        Vec cameraPos = {0.0f, 34.5f, 600.0f};
        Vec target = {0.0f, 34.5f, 0.0f};
        Vec cameraUp = {0.0f, 1.0f, 0.0f};

        C_MTXLookAt(viewMtx, &cameraPos, &cameraUp, &target);
    }

    {
        GXLightObj light;
        GXColor lightColor = {255, 255, 255, 255};

        GXInitLightColor(&light, lightColor);

        {
            Vec pos = {1600.0f, 1500.0f, 6000.0f};

            PSMTXMultVec(viewMtx, &pos, &pos);
            GXInitLightPos(&light, pos.x, pos.y, pos.z);
        }

        GXLoadLightObjImm(&light, GX_LIGHT0);
        RFLSetMtx(&model, viewMtx);
    }

    {
        RFLDrawSetting drawSetting;
        GXColor ambColor = {160, 160, 160, 255};

        drawSetting.lightEnable = TRUE;
        drawSetting.lightMask = GX_LIGHT0;
        drawSetting.diffuse = GX_DF_CLAMP;
        drawSetting.attn = GX_AF_NONE;
        drawSetting.ambColor = ambColor;
        drawSetting.compLoc = FALSE;
        RFLLoadDrawSetting(&drawSetting);
    }

    if (!setting->drawXluOnly) {
        GXSetColorUpdate(GX_TRUE);
        GXSetAlphaUpdate(GX_TRUE);
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA,
                       GX_LO_COPY);
        RFLDrawOpa(&model);
    }

    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);
    GXSetColorUpdate(GX_TRUE);
    GXSetAlphaUpdate(GX_FALSE);
    RFLDrawXlu(&model);

    GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_INVSRCALPHA, GX_LO_COPY);
    GXSetAlphaUpdate(GX_TRUE);
    GXSetColorUpdate(GX_FALSE);
    RFLDrawXlu(&model);

    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetColorUpdate(GX_TRUE);

    GXDrawDone();
    GXCopyTex(buf, GX_TRUE);

    GXDrawDone();
    GXPixModeSync();

    RFLiFree(modelBuf);

    GXSetViewportv(vp);
    GXSetScissor(scissorOffsetX, scissorOffsetY, scissorWidth, scissorHeight);

    RFLiSetCoordinateData(&coordData);
}

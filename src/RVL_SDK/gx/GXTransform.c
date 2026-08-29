#include <revolution/gx.h>
#include <revolution/mtx.h>

void GXProject(f32 x, f32 y, f32 z, const f32 mtx[3][4], const f32* pm,
               const f32* vp, f32* sx, f32* sy, f32* sz) {
    Vec peye;
    f32 xc, yc, zc, wc;

    peye.x = mtx[0][0] * x + mtx[0][1] * y + mtx[0][2] * z + mtx[0][3];
    peye.y = mtx[1][0] * x + mtx[1][1] * y + mtx[1][2] * z + mtx[1][3];
    peye.z = mtx[2][0] * x + mtx[2][1] * y + mtx[2][2] * z + mtx[2][3];

    if (pm[0] == (f32)GX_PERSPECTIVE) {
        xc = peye.x * pm[1] + peye.z * pm[2];
        yc = peye.y * pm[3] + peye.z * pm[4];
        zc = peye.z * pm[5] + pm[6];
        wc = 1.0f / -peye.z;
    } else {
        xc = peye.x * pm[1] + pm[2];
        yc = peye.y * pm[3] + pm[4];
        zc = peye.z * pm[5] + pm[6];
        wc = 1.0f;
    }

    *sx = xc * vp[2] / 2 * wc + vp[0] + vp[2] / 2;
    *sy = -yc * vp[3] / 2 * wc + vp[1] + vp[3] / 2;
    *sz = zc * (vp[5] - vp[4]) * wc + vp[5];
}

// clang-format off
inline void LoadProjPS(register f32* dst) {
    register f32 ps_0, ps_1, ps_2;
    register GXData* src;

    asm volatile {
        lwz src, gxdt
        psq_l  ps_0,  0  + GXData.proj(src), 0, 0
        psq_l  ps_1,  8  + GXData.proj(src), 0, 0
        psq_l  ps_2,  16 + GXData.proj(src), 0, 0
        psq_st ps_0,  0(dst),                0, 0
        psq_st ps_1,  8(dst),                0, 0
        psq_st ps_2, 16(dst),                0, 0
    }
}
// clang-format on

// clang-format off
inline void WriteProjPS(register volatile void* dst, register const f32* src) {
    register f32 ps_0, ps_1, ps_2;

    asm volatile {
        psq_l  ps_0,  0(src), 0, 0
        psq_l  ps_1,  8(src), 0, 0
        psq_l  ps_2, 16(src), 0, 0
        psq_st ps_0,  0(dst), 0, 0
        psq_st ps_1,  0(dst), 0, 0
        psq_st ps_2,  0(dst), 0, 0
    }
}
// clang-format on

// clang-format off
inline void Copy6Floats(register f32* dst, register const f32* src) {
    register f32 ps_0, ps_1, ps_2;

    asm volatile {
        psq_l  ps_0,  0(src), 0, 0
        psq_l  ps_1,  8(src), 0, 0
        psq_l  ps_2, 16(src), 0, 0
        psq_st ps_0,  0(dst), 0, 0
        psq_st ps_1,  8(dst), 0, 0
        psq_st ps_2, 16(dst), 0, 0
    }
}
// clang-format on

void __GXSetProjection(void) {
    // The write-gather pipe is memory mapped at 0xCC008000; holding its
    // address once keeps the projection store and the type store on the
    // same pointer.
    volatile void* wgpipe = &WGPIPE;

    GX_XF_LOAD_REGS(ARRAY_SIZE(gxdt->proj), GX_XF_REG_PROJECTIONA);
    WriteProjPS(wgpipe, gxdt->proj);
    WGPIPE.i = gxdt->projType;
}

void GXSetProjection(const Mtx44 proj, GXProjectionType type) {
    gxdt->projType = type;

    gxdt->proj[0] = proj[0][0];
    gxdt->proj[2] = proj[1][1];
    gxdt->proj[4] = proj[2][2];
    gxdt->proj[5] = proj[2][3];

    if (type == GX_ORTHOGRAPHIC) {
        gxdt->proj[1] = proj[0][3];
        gxdt->proj[3] = proj[1][3];
    } else {
        gxdt->proj[1] = proj[0][2];
        gxdt->proj[3] = proj[1][2];
    }

    gxdt->gxDirtyFlags |= GX_DIRTY_PROJECTION;
}

void GXSetProjectionv(const f32 proj[GX_PROJECTION_SZ]) {
    gxdt->projType = proj[0] == 0.0f ? GX_PERSPECTIVE : GX_ORTHOGRAPHIC;
    Copy6Floats(gxdt->proj, proj + 1);
    gxdt->gxDirtyFlags |= GX_DIRTY_PROJECTION;
}

void GXGetProjectionv(f32 proj[GX_PROJECTION_SZ]) {
    proj[0] = gxdt->projType != GX_PERSPECTIVE ? 1.0f : 0.0f;
    LoadProjPS(proj + 1);
}

// clang-format off
inline void WriteMTXPS4x3(register volatile void* dst, register const Mtx src) {
    register f32 ps_0, ps_1, ps_2, ps_3, ps_4, ps_5;

    asm volatile {
        psq_l  ps_0,  0(src), 0, 0
        psq_l  ps_1,  8(src), 0, 0
        psq_l  ps_2, 16(src), 0, 0
        psq_l  ps_3, 24(src), 0, 0
        psq_l  ps_4, 32(src), 0, 0
        psq_l  ps_5, 40(src), 0, 0

        psq_st ps_0, 0(dst),  0, 0
        psq_st ps_1, 0(dst),  0, 0
        psq_st ps_2, 0(dst),  0, 0
        psq_st ps_3, 0(dst),  0, 0
        psq_st ps_4, 0(dst),  0, 0
        psq_st ps_5, 0(dst),  0, 0
    }
}
// clang-format on

// clang-format off
inline void WriteMTXPS3x3(register volatile void* dst, register const Mtx src) {
    register f32 ps_0, ps_1, ps_2, ps_3, ps_4, ps_5;

    asm volatile {
        psq_l  ps_0,  0(src), 0, 0
        lfs    ps_1,  8(src)
        psq_l  ps_2, 16(src), 0, 0
        lfs    ps_3, 24(src)
        psq_l  ps_4, 32(src), 0, 0
        lfs    ps_5, 40(src)

        psq_st ps_0, 0(dst),  0, 0
        stfs   ps_1, 0(dst)
        psq_st ps_2, 0(dst),  0, 0
        stfs   ps_3, 0(dst)
        psq_st ps_4, 0(dst),  0, 0
        stfs   ps_5, 0(dst)
    }
}
// clang-format on

// clang-format off
inline void WriteMTXPS4x2(register volatile void* dst, register const Mtx src) {
    register f32 ps_0, ps_1, ps_2, ps_3;

    asm volatile {
        psq_l  ps_0,  0(src), 0, 0
        psq_l  ps_1,  8(src), 0, 0
        psq_l  ps_2, 16(src), 0, 0
        psq_l  ps_3, 24(src), 0, 0

        psq_st ps_0, 0(dst),  0, 0
        psq_st ps_1, 0(dst),  0, 0
        psq_st ps_2, 0(dst),  0, 0
        psq_st ps_3, 0(dst),  0, 0
    }
}
// clang-format on

void GXLoadPosMtxImm(const Mtx mtx, u32 id) {
    // Position matrices are 4x3
    GX_XF_LOAD_REGS(4 * 3 - 1, id * 4 + GX_XF_MEM_POSMTX);
    WriteMTXPS4x3(&WGPIPE, mtx);
}

void GXLoadPosMtxIndx(u16 index, u32 id) {
    // Position matrices are 4x3
    GX_FIFO_LOAD_INDX_A(id * 4 + GX_XF_MEM_POSMTX, 4 * 3 - 1, index);
}

void GXLoadNrmMtxImm(const Mtx mtx, u32 id) {
    // Normal matrices are 3x3
    GX_XF_LOAD_REGS(3 * 3 - 1, id * 3 + GX_XF_MEM_NRMMTX);
    WriteMTXPS3x3(&WGPIPE, mtx);
}

void GXLoadNrmMtxIndx3x3(u16 index, u32 id) {
    // Normal matrices are 3x3
    GX_FIFO_LOAD_INDX_B(id * 3 + GX_XF_MEM_NRMMTX, 3 * 3 - 1, index);
}

void GXSetCurrentMtx(u32 id) {
    GX_XF_SET_MATRIXINDEX0_GEOM(gxdt->matrixIndex0, id);
    gxdt->gxDirtyFlags |= GX_DIRTY_MTX_IDX;
}

void GXLoadTexMtxImm(const Mtx mtx, u32 id, GXTexMtxType type) {
    u32 addr;
    u32 num;
    u32 cmd;

    // Base row address in XF memory
    addr = id >= GX_PTTEXMTX0
               ? (id - GX_PTTEXMTX0) * sizeof(f32) + GX_XF_MEM_DUALTEXMTX
               : id * 4 + (u64)GX_XF_MEM_POSMTX;

    // Number of elements in matrix, as an XF load-register size field
    num = type == GX_MTX2x4 ? (u64)(2 * 4) : 3 * 4;
    num = (num - 1) << 16;

    cmd = addr | num;
    GX_XF_LOAD_REG_HDR(cmd);

    if (type == GX_MTX3x4) {
        WriteMTXPS4x3(&WGPIPE, mtx);
    } else {
        WriteMTXPS4x2(&WGPIPE, mtx);
    }
}

void __GXSetViewport(void) {
    f32 a, b, c, d, e, f;
    f32 near, far;

    a = gxdt->vpSx / 2.0f;
    b = -gxdt->vpSy / 2.0f;
    d = gxdt->vpOx + (gxdt->vpSx / 2.0f) + 342.0f;
    e = gxdt->vpOy + (gxdt->vpSy / 2.0f) + 342.0f;

    near = gxdt->vpNear * gxdt->scaleZ;
    far = gxdt->vpFar * gxdt->scaleZ;

    c = far - near;
    f = far + gxdt->offsetZ;

    GX_XF_LOAD_REGS(6 - 1, GX_XF_REG_SCALEX);
    WGPIPE.f = a;
    WGPIPE.f = b;
    WGPIPE.f = c;
    WGPIPE.f = d;
    WGPIPE.f = e;
    WGPIPE.f = f;
}

void GXSetViewportJitter(f32 ox, f32 oy, f32 sx, f32 sy, f32 near, f32 far,
                         u32 nextField) {
    if (nextField == GX_FIELD_EVEN) {
        oy -= 0.5f;
    }

    gxdt->vpOx = ox;
    gxdt->vpOy = oy;
    gxdt->vpSx = sx;
    gxdt->vpSy = sy;
    gxdt->vpNear = near;
    gxdt->vpFar = far;
    gxdt->gxDirtyFlags |= GX_DIRTY_VIEWPORT;
}

void GXSetViewport(f32 ox, f32 oy, f32 sx, f32 sy, f32 near, f32 far) {
    gxdt->vpOx = ox;
    gxdt->vpOy = oy;
    gxdt->vpSx = sx;
    gxdt->vpSy = sy;
    gxdt->vpNear = near;
    gxdt->vpFar = far;
    gxdt->gxDirtyFlags |= GX_DIRTY_VIEWPORT;
}

void GXGetViewportv(f32 view[GX_VIEWPORT_SZ]) {
    Copy6Floats(view, gxdt->view);
}

void GXSetZScaleOffset(f32 scale, f32 offset) {
    gxdt->offsetZ = (f32)0xFFFFFF * offset;      // ???
    gxdt->scaleZ = 1.0f + (f32)0xFFFFFF * scale; // ???
    gxdt->gxDirtyFlags |= GX_DIRTY_VIEWPORT;
}

void GXSetScissor(u32 x, u32 y, u32 w, u32 h) {
    u32 x1, y1, x2, y2;
    u32 reg;

    x1 = x + 342;
    y1 = y + 342;
    x2 = x1 + w - 1;
    y2 = y1 + h - 1;

    reg = gxdt->scissorTL;
    GX_BP_SET_SCISSORTL_TOP(reg, y1);
    GX_BP_SET_SCISSORTL_LEFT(reg, x1);
    gxdt->scissorTL = reg;

    reg = gxdt->scissorBR;
    GX_BP_SET_SCISSORBR_BOT(reg, y2);
    GX_BP_SET_SCISSORBR_RIGHT(reg, x2);
    gxdt->scissorBR = reg;

    GX_BP_LOAD_REG(gxdt->scissorTL);
    GX_BP_LOAD_REG(gxdt->scissorBR);
    gxdt->lastWriteWasXF = FALSE;
}

void GXGetScissor(u32* x, u32* y, u32* w, u32* h) {
    u32 y2, y1;
    u32 x2, x1;

    x1 = GX_BP_GET_SCISSORTL_LEFT(gxdt->scissorTL);
    y1 = GX_BP_GET_SCISSORTL_TOP(gxdt->scissorTL);
    x2 = GX_BP_GET_SCISSORBR_RIGHT(gxdt->scissorBR);
    y2 = GX_BP_GET_SCISSORBR_BOT(gxdt->scissorBR);

    *x = x1 - 342;
    *y = y1 - 342;
    *w = x2 - x1 + 1;
    *h = y2 - y1 + 1;
}

void GXSetScissorBoxOffset(u32 ox, u32 oy) {
    u32 cmd = 0;
    GX_BP_SET_SCISSOROFFSET_OX(cmd, (ox + 342) / 2);
    GX_BP_SET_SCISSOROFFSET_OY(cmd, (oy + 342) / 2);
    GX_BP_SET_OPCODE(cmd, GX_BP_REG_SCISSOROFFSET);

    GX_BP_LOAD_REG(cmd);
    gxdt->lastWriteWasXF = FALSE;
}

void GXSetClipMode(GXClipMode mode) {
    GX_XF_LOAD_REG(GX_XF_REG_CLIPDISABLE, mode);
    gxdt->lastWriteWasXF = TRUE;
}

void __GXSetMatrixIndex(GXAttr index) {
    // Tex4 and after is stored in XF MatrixIndex1
    if (index < GX_VA_TEX4MTXIDX) {
        GX_CP_LOAD_REG(GX_CP_REG_MATRIXINDEXA, gxdt->matrixIndex0);
        GX_XF_LOAD_REG(GX_XF_REG_MATRIXINDEX0, gxdt->matrixIndex0);
    } else {
        GX_CP_LOAD_REG(GX_CP_REG_MATRIXINDEXB, gxdt->matrixIndex1);
        GX_XF_LOAD_REG(GX_XF_REG_MATRIXINDEX1, gxdt->matrixIndex1);
    }

    gxdt->lastWriteWasXF = TRUE;
}

/**
 * @file sdk_candidate_800B5C5C.c
 * @brief Dolphin SDK GXInit.c ("Feb  7 2003" build), 0x800B5C5C - 0x800B6FE0:
 *        __GXDefaultTexRegionCallback, __GXDefaultTlutRegionCallback,
 *        __GXShutdown, GXInit and __GXInitGX.
 *
 * The unit owns the whole TU's data: .bss gxData + FifoObj
 * (0x803FC860-0x803FCDD8), .sbss __piReg..__memReg, the __GXShutdown
 * statics and resetFuncRegistered (0x8047A978-0x8047A9A0), and the .sdata2
 * pool that starts with `gx` (0x8047C2E0-0x8047C308).
 *
 * GXInit.c is built with the peephole pass off, so the bodies sit inside
 * '#pragma peephole off'.
 */
#include "dolphin/types.h"
#include "dolphin/os/OSTime.h"
#include "dolphin/os/PPCArch.h"

extern u32 VIGetTvFormat(void);

typedef struct GXColor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} GXColor;

typedef struct GXRenderModeObj {
    u32 viTVmode;
    u16 fbWidth;
    u16 efbHeight;
    u16 xfbHeight;
    u16 viXOrigin;
    u16 viYOrigin;
    u16 viWidth;
    u16 viHeight;
    u32 xfbMode;
    u8 field_rendering;
    u8 aa;
    u8 sample_pattern[12][2];
    u8 vfilter[7];
} GXRenderModeObj;

typedef struct GXTlutRegion {
    /* 0x00 */ u8 _00[0x10];
} GXTlutRegion;

typedef struct GXTexRegion {
    u32 unk[4];
} GXTexRegion;

typedef struct GXFifoObj {
    /* 0x00 */ u8* base;
    /* 0x04 */ u8* top;
    /* 0x08 */ u32 size;
    /* 0x0C */ u32 hiWatermark;
    /* 0x10 */ u32 loWatermark;
    /* 0x14 */ void* rdPtr;
    /* 0x18 */ void* wrPtr;
    /* 0x1C */ s32 count;
    /* 0x20 */ u8 wrap;
    /* 0x21 */ u8 _21[0x5F];
} GXFifoObj;

typedef struct GXData {
    /* 0x000 */ u8 _000[0x08];
    /* 0x008 */ u32 cpEnable;
    /* 0x00C */ u32 cpStatus;
    /* 0x010 */ u8 _010[0x0C];
    /* 0x01C */ u32 vatA[8];
    /* 0x03C */ u32 vatB[8];
    /* 0x05C */ u8 _05C[0x20];
    /* 0x07C */ u32 lpSize;
    /* 0x080 */ u8 _080[0x38];
    /* 0x0B8 */ u32 suTs0[8];
    /* 0x0D8 */ u32 suTs1[8];
    /* 0x0F8 */ u32 suScis0;
    /* 0x0FC */ u32 suScis1;
    /* 0x100 */ u32 tref[8];
    /* 0x120 */ u32 iref;
    /* 0x124 */ u32 bpMask;
    /* 0x128 */ u8 _128[0x08];
    /* 0x130 */ u32 tevc[16];
    /* 0x170 */ u32 teva[16];
    /* 0x1B0 */ u32 tevKsel[8];
    /* 0x1D0 */ u32 cmode0;
    /* 0x1D4 */ u32 cmode1;
    /* 0x1D8 */ u32 zmode;
    /* 0x1DC */ u32 peCtrl;
    /* 0x1E0 */ u8 _1E0[0x1C];
    /* 0x1FC */ u32 cpTex;
    /* 0x200 */ u8 _200[0x04];
    /* 0x204 */ u32 genMode;
    /* 0x208 */ GXTexRegion defaultTexRegions[8];
    /* 0x288 */ GXTexRegion defaultTexRegionsCI[4];
    /* 0x2C8 */ u32 nextTexRgn;
    /* 0x2CC */ u32 nextTexRgnCI;
    /* 0x2D0 */ GXTlutRegion defaultTlutRegions[20];
    /* 0x410 */ u8 _410[0x8C];
    /* 0x49C */ u32 texmapId[16];
    /* 0x4DC */ u32 field_4DC;
    /* 0x4E0 */ u32 field_4E0;
    /* 0x4E4 */ u8 _4E4[0x08];
    /* 0x4EC */ u32 perfSel;
    /* 0x4F0 */ u8 inDispList;
    /* 0x4F1 */ u8 dlSaveContext;
    /* 0x4F2 */ u8 tcsManEnab;
    /* 0x4F3 */ u8 dirtyVAT;
    /* 0x4F4 */ u32 dirtyState;
} GXData;

/* .bss 0x803FC860: gxData, then FifoObj at +0x4F8 (one anchor). */
static GXData gxData;
static GXFifoObj FifoObj;
GXData* const gx = &gxData;
u32* __piReg = NULL;
u16* __cpReg = NULL;
u16* __peReg = NULL;
u16* __memReg = NULL;
extern OSTime OSGetTime(void);
extern void GXSetBreakPtCallback(void* callback);
extern void GXSetDrawSyncCallback(void* callback);
extern void GXSetDrawDoneCallback(void* callback);
extern void __GXAbort(void);
extern void __GXInitGX(void);

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
void* fn_800B5C5C(void* object) {
    s32 format;
    u32 count;
    extern u32 GXGetTexObjFmt(void* object);

    format = GXGetTexObjFmt(object);
    if (format != 8 && format != 9 && format != 10) {
        u8* data = (u8*)gx;
        count = *(u32*)(data + 0x2c8);
        *(u32*)(data + 0x2c8) = count + 1;
        return data + ((count & 7) << 4) + 0x208;
    } else {
        u8* data = (u8*)gx;
        count = *(u32*)(data + 0x2cc);
        *(u32*)(data + 0x2cc) = count + 1;
        return data + ((count & 3) << 4) + 0x288;
    }
}

GXTlutRegion* __GXDefaultTlutRegionCallback(u32 index) {
    GXTlutRegion* region;

    if (index >= 20) {
        region = NULL;
    } else {
        region = &gx->defaultTlutRegions[index];
    }
    return region;
}

#define GX_GET_MEM_REG(offset) (*(volatile u16*)((volatile u16*)(__memReg) + (offset)))
#define GX_SET_CP_REG(offset, val) (*(volatile u16*)((volatile u16*)(__cpReg) + (offset)) = val)
#define GX_WRITE_U32(val) (*(volatile u32*)0xCC008000 = (val))

static inline u32 GXReadMEMReg(u32 addrHi, u32 addrLo)
{
    u32 hiStart;
    u32 hiNew;
    u32 lo;

    hiStart = GX_GET_MEM_REG(addrHi);
    do {
        hiNew = hiStart;
        hiStart = GX_GET_MEM_REG(addrHi);
        lo = GX_GET_MEM_REG(addrLo);
    } while (hiStart != hiNew);

    return (hiStart << 16) | lo;
}

s32 __GXShutdown_800C6260(BOOL final)
{
    static u32 peCount;
    static OSTime time;
    static u32 calledOnce;
    u32 newPeCount;
    OSTime newTime;

    if (!final) {
        if (!calledOnce) {
            peCount = GXReadMEMReg(0x27, 0x28);
            time = OSGetTime();
            calledOnce = 1;
            return FALSE;
        }

        newTime = OSGetTime();
        newPeCount = GXReadMEMReg(0x27, 0x28);

        if (newTime - time < 10) {
            return FALSE;
        }

        if (newPeCount != peCount) {
            peCount = newPeCount;
            time = newTime;
            return FALSE;
        }
    } else {
        GXSetBreakPtCallback(NULL);
        GXSetDrawSyncCallback(NULL);
        GXSetDrawDoneCallback(NULL);

        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);

        PPCSync();

        GX_SET_CP_REG(1, 0);
        GX_SET_CP_REG(2, 3);

        ((u8*)gx)[0x4F2] = 1;

        __GXAbort();
    }

    return TRUE;
}

#define GX_WRITE_U8(value)  (*(volatile u8*)0xCC008000 = (value))

#define SET_REG_FIELD(reg, size, shift, val) \
    do { \
        (reg) = ((u32)(reg) & ~(((1 << (size)) - 1) << (shift))) | ((u32)(val) << (shift)); \
    } while (0)
/* GX_SET_REG(reg, x, st, end): big-endian bit numbering, st..end */
#define GX_SET_REG(reg, x, st, end) SET_REG_FIELD(reg, (end) - (st) + 1, 31 - (end), x)
#define GXWGFifo_s8 (*(volatile u8*)0xCC008000)
#define GXWGFifo_s32 (*(volatile u32*)0xCC008000)
#define GX_BP_LOAD_REG(data) GXWGFifo_s8 = 0x61; GXWGFifo_s32 = (data);
#define GX_CP_LOAD_REG(addr, data) GXWGFifo_s8 = 0x08; GXWGFifo_s8 = (addr); GXWGFifo_s32 = (data);
#define GX_XF_LOAD_REG(addr, data) GXWGFifo_s8 = 0x10; GXWGFifo_s32 = (addr); GXWGFifo_s32 = (data);

static inline void EnableWriteGatherPipe(void)
{
    u32 hid2;
    hid2 = PPCMfhid2();
    PPCMtwpar(0x0C008000);
    hid2 |= 0x40000000;
    PPCMthid2(hid2);
}

void* GXInit(void* base, u32 size) {
    static u32 resetFuncRegistered;
    extern const char* __GXVersion;
    extern u8 GXResetFuncInfo_80312AD0[];
    extern void OSRegisterVersion();
    extern void OSRegisterResetFunction();
    extern void GXSetMisc();
    extern void __GXFifoInit();
    extern void GXInitFifoBase();
    extern void GXSetCPUFifo();
    extern void GXSetGPFifo();
    extern void __GXPEInit();
    extern void __GXFlushTextureState();
    extern void GXInitTexCacheRegion();
    extern void GXInitTlutRegion();
    extern void __GXSetTmemConfig();
    u32 i;
    u32 reg;
    u32 freqBase;
    char stack_padding[8];

    OSRegisterVersion(__GXVersion);
    gx->inDispList = 0;
    gx->dlSaveContext = 1;
    gx->tcsManEnab = 1;
    gx->field_4DC = 0;
    gx->field_4E0 = 0;
    GXSetMisc(1, 0);

    __piReg = (u32*)0xCC003000;
    __cpReg = (u16*)0xCC000000;
    __peReg = (u16*)0xCC001000;
    __memReg = (u16*)0xCC004000;
    __GXFifoInit();
    GXInitFifoBase(&FifoObj, base, size);
    GXSetCPUFifo(&FifoObj);
    GXSetGPFifo(&FifoObj);
    if (!resetFuncRegistered) {
        OSRegisterResetFunction(GXResetFuncInfo_80312AD0);
        resetFuncRegistered = 1;
    }
    __GXPEInit();
    EnableWriteGatherPipe();

    gx->genMode = 0;
    GX_SET_REG(gx->genMode, 0, 0, 7);
    gx->bpMask = 255;
    GX_SET_REG(gx->bpMask, 0xF, 0, 7);
    gx->lpSize = 0;
    GX_SET_REG(gx->lpSize, 34, 0, 7);

    for (i = 0; i < 16; i++) {
        gx->tevc[i] = 0;
        gx->teva[i] = 0;
        gx->tref[i / 2] = 0;
        gx->texmapId[i] = 0xFF;
        GX_SET_REG(gx->tevc[i], 0xC0 + i * 2, 0, 7);
        GX_SET_REG(gx->teva[i], 0xC1 + i * 2, 0, 7);
        GX_SET_REG(gx->tevKsel[i / 2], 0xF6 + i / 2, 0, 7);
        GX_SET_REG(gx->tref[i / 2], 0x28 + i / 2, 0, 7);
    }

    gx->iref = 0;
    GX_SET_REG(gx->iref, 0x27, 0, 7);

    for (i = 0; i < 8; i++) {
        gx->suTs0[i] = 0;
        gx->suTs1[i] = 0;
        GX_SET_REG(gx->suTs0[i], 0x30 + i * 2, 0, 7);
        GX_SET_REG(gx->suTs1[i], 0x31 + i * 2, 0, 7);
    }

    GX_SET_REG(gx->suScis0, 0x20, 0, 7);
    GX_SET_REG(gx->suScis1, 0x21, 0, 7);
    GX_SET_REG(gx->cmode0, 0x41, 0, 7);
    GX_SET_REG(gx->cmode1, 0x42, 0, 7);
    GX_SET_REG(gx->zmode, 0x40, 0, 7);
    GX_SET_REG(gx->peCtrl, 0x43, 0, 7);
    GX_SET_REG(gx->cpTex, 0, 23, 24);

    gx->dirtyState = 0;
    gx->dirtyVAT = 0;

    freqBase = *(u32*)0x800000F8 / 500;
    __GXFlushTextureState();
    reg = (freqBase >> 11) | 0x400 | 0x69000000;
    GX_BP_LOAD_REG(reg);
    __GXFlushTextureState();
    reg = (freqBase / 0x1080) | 0x200 | 0x46000000;
    GX_BP_LOAD_REG(reg);

    for (i = 0; i < 8; i++) {
        GX_SET_REG(gx->vatA[i], 1, 1, 1);
        GX_SET_REG(gx->vatB[i], 1, 0, 0);
        do {
            s32 regAddr;
            GX_CP_LOAD_REG(i | 0x80, gx->vatB[i]);
            regAddr = i - 12;
        } while (0);
    }

    {
        u32 reg1 = 0;
        u32 reg2 = 0;

        GX_SET_REG(reg1, 1, 31, 31);
        GX_SET_REG(reg1, 1, 30, 30);
        GX_SET_REG(reg1, 1, 29, 29);
        GX_SET_REG(reg1, 1, 28, 28);
        GX_SET_REG(reg1, 1, 27, 27);
        GX_SET_REG(reg1, 1, 26, 26);
        GX_XF_LOAD_REG(0x1000, reg1);
        GX_SET_REG(reg2, 1, 31, 31);
        GX_XF_LOAD_REG(0x1012, reg2);
    }

    {
        u32 reg = 0;
        GX_SET_REG(reg, 1, 31, 31);
        GX_SET_REG(reg, 1, 30, 30);
        GX_SET_REG(reg, 1, 29, 29);
        GX_SET_REG(reg, 1, 28, 28);
        GX_SET_REG(reg, 0x58, 0, 7);
        GX_BP_LOAD_REG(reg);
    }

    for (i = 0; i < 8; i++) {
        GXInitTexCacheRegion(&gx->defaultTexRegions[i], 0, i * 0x8000, 0, 0x80000 + i * 0x8000, 0);
    }
    for (i = 0; i < 4; i++) {
        GXInitTexCacheRegion(&gx->defaultTexRegionsCI[i], 0, (i * 2 + 8) * 0x8000, 0, (i * 2 + 9) * 0x8000, 0);
    }
    for (i = 0; i < 16; i++) {
        GXInitTlutRegion(&gx->defaultTlutRegions[i], 0xC0000 + 0x2000 * i, 16);
    }
    for (i = 0; i < 4; i++) {
        GXInitTlutRegion(&gx->defaultTlutRegions[i + 16], 0xE0000 + 0x8000 * i, 64);
    }

    __cpReg[3] = 0;
    SET_REG_FIELD(gx->perfSel, 4, 4, 0);
    GX_CP_LOAD_REG(0x20, gx->perfSel);
    GX_XF_LOAD_REG(0x1006, 0);
    GX_BP_LOAD_REG(0x23000000);
    GX_BP_LOAD_REG(0x24000000);
    GX_BP_LOAD_REG(0x67000000);

    __GXSetTmemConfig(0);
    __GXInitGX();
    return &FifoObj;
}

/* __GXInitGX: GXInit.c's last function (0x800B671C). */
void __GXInitGX(void) {
    extern GXData* const gx;
    extern GXRenderModeObj lbl_80312D30;
    extern GXRenderModeObj lbl_80312F4C;
    extern GXRenderModeObj lbl_803130F0;
    extern GXRenderModeObj lbl_80313294;
    extern u8 lbl_80312AB4[];
    extern u8 lbl_803129E4[];
    extern void GXSetCopyClear(GXColor, u32);
    extern void GXSetTexCoordGen2();
    extern void GXSetNumTexGens();
    extern void GXClearVtxDesc();
    extern void GXInvalidateVtxCache();
    extern void GXSetArray();
    extern void GXSetVtxAttrFmtv();
    extern void GXSetLineWidth();
    extern void GXSetPointSize();
    extern void GXEnableTexOffsets();
    extern void GXLoadPosMtxImm();
    extern void GXLoadNrmMtxImm();
    extern void GXSetCurrentMtx();
    extern void GXLoadTexMtxImm();
    extern void GXSetViewport(f32, f32, f32, f32, f32, f32);
    extern void fn_800BD394();
    extern void GXSetCoPlanar();
    extern void GXSetCullMode();
    extern void GXSetClipMode();
    extern void GXSetScissor();
    extern void GXSetScissorBoxOffset();
    extern void GXSetNumChans();
    extern void GXSetChanCtrl();
    extern void GXSetChanAmbColor(u32, GXColor);
    extern void GXSetChanMatColor(u32, GXColor);
    extern void GXInvalidateTexAll();
    extern void GXSetTexRegionCallback();
    extern void GXSetTlutRegionCallback();
    extern void GXSetTevOrder();
    extern void GXSetNumTevStages();
    extern void GXSetTevOp();
    extern void GXSetAlphaCompare();
    extern void GXSetZTexture();
    extern void GXSetTevKColorSel();
    extern void GXSetTevKAlphaSel();
    extern void GXSetTevSwapMode();
    extern void GXSetTevSwapModeTable();
    extern void GXSetTevDirect();
    extern void GXSetNumIndStages();
    extern void GXSetIndTexCoordScale();
    extern void GXSetFog(u32, f32, f32, f32, f32, GXColor);
    extern void GXSetFogRangeAdj();
    extern void GXSetBlendMode();
    extern void GXSetColorUpdate();
    extern void GXSetAlphaUpdate();
    extern void GXSetZMode();
    extern void GXSetZCompLoc();
    extern void GXSetDither();
    extern void GXSetDstAlpha();
    extern void GXSetPixelFmt();
    extern void GXSetFieldMask();
    extern void GXSetFieldMode();
    extern void GXSetDispCopySrc();
    extern void GXSetDispCopyDst();
    extern void GXSetDispCopyYScale(f32);
    extern void GXSetCopyClamp();
    extern void GXSetCopyFilter();
    extern void GXSetDispCopyGamma();
    extern void GXSetDispCopyFrame2Field();
    extern void GXClearBoundingBox();
    extern void GXPokeColorUpdate();
    extern void GXPokeAlphaUpdate();
    extern void GXPokeDither();
    extern void GXPokeBlendMode();
    extern void GXPokeAlphaMode();
    extern void GXPokeAlphaRead();
    extern void GXPokeDstAlpha();
    extern void GXPokeZMode();
    extern void fn_800BD91C();
    extern void fn_800BE30C();

    f32 identity[3][4];
    GXColor clear = { 64, 64, 64, 255 };
    GXColor black = { 0, 0, 0, 0 };
    GXColor white = { 255, 255, 255, 255 };
    GXRenderModeObj* rmode;
    u32 i;

    switch (VIGetTvFormat()) {
    case 0:
        rmode = &lbl_80312D30;
        break;
    case 1:
        rmode = &lbl_803130F0;
        break;
    case 5:
        rmode = &lbl_80313294;
        break;
    case 2:
        rmode = &lbl_80312F4C;
        break;
    default:
        rmode = &lbl_80312D30;
        break;
    }

    GXSetCopyClear(clear, 0xFFFFFF);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    GXSetTexCoordGen2(1, 1, 5, 0x3C, 0, 0x7D);
    GXSetTexCoordGen2(2, 1, 6, 0x3C, 0, 0x7D);
    GXSetTexCoordGen2(3, 1, 7, 0x3C, 0, 0x7D);
    GXSetTexCoordGen2(4, 1, 8, 0x3C, 0, 0x7D);
    GXSetTexCoordGen2(5, 1, 9, 0x3C, 0, 0x7D);
    GXSetTexCoordGen2(6, 1, 10, 0x3C, 0, 0x7D);
    GXSetTexCoordGen2(7, 1, 11, 0x3C, 0, 0x7D);
    GXSetNumTexGens(1);
    GXClearVtxDesc();
    GXInvalidateVtxCache();
    for (i = 9; i <= 24; i++) {
        GXSetArray(i, gx, 0);
    }
    for (i = 0; i < 8; i++) {
        GXSetVtxAttrFmtv(i, lbl_803129E4);
    }
    GXSetLineWidth(6, 0);
    GXSetPointSize(6, 0);
    GXEnableTexOffsets(0, 0, 0);
    GXEnableTexOffsets(1, 0, 0);
    GXEnableTexOffsets(2, 0, 0);
    GXEnableTexOffsets(3, 0, 0);
    GXEnableTexOffsets(4, 0, 0);
    GXEnableTexOffsets(5, 0, 0);
    GXEnableTexOffsets(6, 0, 0);
    GXEnableTexOffsets(7, 0, 0);

    identity[0][0] = 1.0f;
    identity[0][1] = 0.0f;
    identity[0][2] = 0.0f;
    identity[0][3] = 0.0f;
    identity[1][0] = 0.0f;
    identity[1][1] = 1.0f;
    identity[1][2] = 0.0f;
    identity[1][3] = 0.0f;
    identity[2][0] = 0.0f;
    identity[2][1] = 0.0f;
    identity[2][2] = 1.0f;
    identity[2][3] = 0.0f;
    GXLoadPosMtxImm(identity, 0);
    GXLoadNrmMtxImm(identity, 0);
    GXSetCurrentMtx(0);
    GXLoadTexMtxImm(identity, 0x3C, 0);
    GXLoadTexMtxImm(identity, 0x7D, 0);

    GXSetViewport(0.0f, 0.0f, (f32)rmode->fbWidth,
                (f32)rmode->xfbHeight, 0.0f, 1.0f);
    fn_800BD394(lbl_80312AB4);
    GXSetCoPlanar(0);
    GXSetCullMode(2);
    GXSetClipMode(0);
    GXSetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
    GXSetScissorBoxOffset(0, 0);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
    GXSetChanAmbColor(4, black);
    GXSetChanMatColor(4, white);
    GXSetChanCtrl(5, 0, 0, 1, 0, 0, 2);
    GXSetChanAmbColor(5, black);
    GXSetChanMatColor(5, white);
    GXInvalidateTexAll();

    {
        GXData* gxState = gx;
        gxState->nextTexRgn = 0;
        gxState->nextTexRgnCI = 0;
    }
    GXSetTexRegionCallback(fn_800B5C5C);
    GXSetTlutRegionCallback(__GXDefaultTlutRegionCallback);

    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOrder(1, 1, 1, 4);
    GXSetTevOrder(2, 2, 2, 4);
    GXSetTevOrder(3, 3, 3, 4);
    GXSetTevOrder(4, 4, 4, 4);
    GXSetTevOrder(5, 5, 5, 4);
    GXSetTevOrder(6, 6, 6, 4);
    GXSetTevOrder(7, 7, 7, 4);
    GXSetTevOrder(8, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(9, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(10, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(11, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(12, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(13, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(14, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(15, 0xFF, 0xFF, 0xFF);
    GXSetNumTevStages(1);
    GXSetTevOp(0, 3);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetZTexture(0, 0x11, 0);
    for (i = 0; i < 16; i++) {
        GXSetTevKColorSel(i, 6);
        GXSetTevKAlphaSel(i, 0);
        GXSetTevSwapMode(i, 0, 0);
    }
    GXSetTevSwapModeTable(0, 0, 1, 2, 3);
    GXSetTevSwapModeTable(1, 0, 0, 0, 3);
    GXSetTevSwapModeTable(2, 1, 1, 1, 3);
    GXSetTevSwapModeTable(3, 2, 2, 2, 3);
    for (i = 0; i < 16; i++) {
        GXSetTevDirect(i);
    }
    GXSetNumIndStages(0);
    GXSetIndTexCoordScale(0, 0, 0);
    GXSetIndTexCoordScale(1, 0, 0);
    GXSetIndTexCoordScale(2, 0, 0);
    GXSetIndTexCoordScale(3, 0, 0);

    GXSetFog(0, 0.0f, 1.0f, 0.1f, 1.0f, black);
    GXSetFogRangeAdj(0, 0, 0);
    GXSetBlendMode(0, 4, 5, 0);
    GXSetColorUpdate(1);
    GXSetAlphaUpdate(1);
    GXSetZMode(1, 3, 1);
    GXSetZCompLoc(1);
    GXSetDither(1);
    GXSetDstAlpha(0, 0);
    GXSetPixelFmt(0, 0);
    GXSetFieldMask(1, 1);
    GXSetFieldMode(rmode->field_rendering,
                rmode->viHeight == 2 * rmode->xfbHeight ? 1 : 0);

    GXSetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
    GXSetDispCopyDst(rmode->fbWidth, rmode->efbHeight);
    GXSetDispCopyYScale((f32)rmode->xfbHeight / (f32)rmode->efbHeight);
    GXSetCopyClamp(3);
    GXSetCopyFilter(rmode->aa, rmode->sample_pattern, 1, rmode->vfilter);
    GXSetDispCopyGamma(0);
    GXSetDispCopyFrame2Field(0);
    GXClearBoundingBox();

    GXPokeColorUpdate(1);
    GXPokeAlphaUpdate(1);
    GXPokeDither(0);
    GXPokeBlendMode(0, 0, 1, 15);
    GXPokeAlphaMode(7, 0);
    GXPokeAlphaRead(1);
    GXPokeDstAlpha(0, 0);
    GXPokeZMode(1, 7, 1);
    fn_800BD91C(0x23, 0x16);
    fn_800BE30C();
}

#pragma peephole reset

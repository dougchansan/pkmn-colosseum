/**
 * @file gs_range_800E0DDC.c
 * @brief GS render engine segment -- split from gs_render.c.
 *
 * XD source unit: unknown (best guess GSmodel head, e.g. effects.cpp analog)
 * Address range: 0x800E0DDC - 0x800E202C (3 functions)
 *
 * 3 fns (0x38, 0x730, 0xAE8). GSmemDumpMap(0x730) ~ XD modelDistortionRender(0x798) which LEADS GSmodel/effects.cpp immediately after GSmath in XD - same position as here. In XD the GSmodel/GSpart/GSscratch block spans up to GStexture.cpp, matching our following gs_range_800E202C.c which runs to gs_texture.c at 0x800EF098; this segment is the head of that model block, cut by the existing bucket boundary. Zero anchors -> trivial pass.
 *
 * Split from src/game/gs_render.c (physical XD source-unit split).
 * The dead #ifdef PCPORT reference block (never defined in configure.py,
 * same situation as gs_gfx.c) was stripped during the split.
 */

#include "dolphin/types.h"
#include "dolphin/os/OS.h"

/* ===== External SDK / engine functions ===== */
extern void  GSlogWrite(const char*, ...);             /* OSReport / GSlog */
extern void* memcpy(void* dst, const void* src, u32 n);
extern void* memset(void* dst, int val, u32 size);

/* External functions referenced from asm wrappers */
extern void DCFlushRange(void* addr, u32 size);
extern u64 OSGetTime(void);
extern void GSgfxSetViewportJitter(s32, f32, f32, f32, f32, f32, f32);
extern void gfxRecordCommand(u32, ...);
extern void gfxBegin(u16);
extern void gfxSetupChannelsTev(u32);

/* GSmem */
extern u16   _toolentryAlloc__FUl(u32 size);                    /* GSmemAllocRaw */
extern void* GSmemLock(u16 handle);                  /* GSmemGetPtr */

/* SDK GX functions */
extern void  VIGetNextField(void);                        /* GXSetViewport */
extern void  GXSetViewportJitter(void);                        /* GXSetProjection */
extern void  GXSetViewport(void);                        /* GXLoadPosMtxImm */
extern void  GXInvalidateTexAll(void);                        /* GXInvalidateTexAll */

/* ===== String constants (rodata) ===== */
extern const char lbl_80270440[]; /* "GSgfx: invalid matrix index" */
extern const char lbl_80270460[]; /* "GSgfx: matrix stack underflow!" */
extern const char lbl_80270480[]; /* "GSgfx: matrix stack overflow!" */
extern const char lbl_802704A0[]; /* "0123456789ABCDEF" */
extern const char lbl_80270528[]; /* "GSmaterialSetPEdescr: Warning..." */
extern const char lbl_8027056C[]; /* "GSmaterialCreate: Run out of materials..." */
extern const char lbl_802705C0[]; /* "GSmaterial MObj" */
extern const char lbl_802705D0[]; /* "GSmaterial: Unsupported texture format..." */
extern const char lbl_80270610[]; /* "GSmaterial: Error creating environment map..." */

/* ===== BSS / global state ===== */
extern u32 lbl_8047AA80;   /* GSgfx state pointer (sda21) */
extern u8 lbl_80400248[];  /* GSgfx state backup buffer (0x5A0 bytes) */
extern u8 lbl_80400B28[];  /* light/material command buffer */

/* ===== Combined forward-decls (duplicated across split segments) ===== */

/* No-op functions (1) */
/* Address: 0x800DC874 | Size: 0x4 */
/* Forward declarations for self-referencing asm blocks */
extern void gfxFlushPendingVertex(void);
extern void _gfxEmitTexCoord1x8(u32 idx);
extern void _gfxEmitTexCoord1x16(u32 idx);
extern void _gfxEmitTexCoord2u8(u32 idx);
extern void _gfxEmitTexCoord2s16(u32 idx);
extern void _gfxEmitTexCoord2u16(u32 idx);
extern void _gfxEmitTexCoord2f32(u32 idx);
extern void _gfxEmitColor1x8(u32 idx);
extern void _gfxEmitColor1x16(u32 idx);
extern void _gfxEmitColor1u16(u32 idx);
extern void _gfxEmitColor1u32(u32 idx);
extern void _gfxEmitColor3u8(u32 idx);
extern void _gfxEmitColor4u8(u32 idx);
extern void _gfxEmitNormal1x8(void);
extern void _gfxEmitNormal1x16(void);
extern void _gfxEmitNormal3s8(void);
extern void _gfxEmitNormal3s16(void);
extern void _gfxEmitNormal3f32(void);
extern void _gfxEmitPos1x8(void);
extern void _gfxEmitPos1x16(void);
extern void _gfxEmitPos2s8(void);
extern void _gfxEmitPos2u8(void);
extern void _gfxEmitPos2s16(void);
extern void _gfxEmitPos2u16(void);
extern void _gfxEmitPos2f32(void);
extern void _gfxEmitPos3s8(void);
extern void _gfxEmitPos3u8(void);
extern void _gfxEmitPos3s16(void);
extern void _gfxEmitPos3u16(void);
extern void gfxVtxDescClear(u8*);
extern void gfxVtxDescSetAttr(u8*, u32, u32, u32, u32, u8, u32, u8);
extern void fn_800D7940(u32, u16);
extern void gfxLoadVtxDesc(u32);
extern void fn_800DB098(void);
extern void fn_800DB758(u16);
extern void lightGetFrameCount__FP9_HSD_AObj(u8*);
extern void GSlogVPrintf(void);
extern void GSlogFormat(void);
extern void fn_800E09E8(void*, void*, u32);
extern u8 GSmemDumpMap(u8, u8);
extern u32 _matGSmatObjMakeTExp(void*, void*, void*, void*, void*);
extern void _matGSmatEnableEnvMapExt(u8*);
extern s32 _matGSmatObjLoad(u8*);
extern void GSmtxConcat(void*, void*, void*);
extern void fn_800E02C4(void*);
extern void GSmtxRotateZ(void*, f32);
extern void GSmtxRotateY(void*, f32);
extern void GSmtxRotateX(void*, f32);
extern void fn_800E03E8(void*, f32, f32, f32);
extern void fn_800E0628(void*, void*);
extern void fn_800E064C(void*);
extern void GSmtx44Perspective(u8*);
extern void GSmtx44Ortho(void*, f32, f32, f32, f32, f32, f32);
extern void fn_800E0C78(void);
extern void GSmathInitCosTable(void);


/* ===== Combined externs (duplicated across all gs_render.c split segments;
 * de-duplicated by identifier from the whole original TU so any
 * cross-segment call/reference resolves regardless of which segment
 * the callee's real definition ended up in). ===== */
extern u8 lbl_8047AA91;
extern u32 lbl_8047AA80;
extern void GXSetPointSize(u32, u32);
extern f32 lbl_8047CA30;
extern f32 lbl_8047CA34;
extern f32 lbl_8047CA38;
extern void GXSetLineWidth(u32, u32);
extern void _gfxEmitMtxIdx(void);
extern void _gfxEmitPos3f32(void);
extern void GXBegin(u32, u32, u16);
extern u8 lbl_80314350[];
extern u8 lbl_804001F0[];
extern void fn_800D6A80(u16, s32, u32*, u32*);
extern u8 lbl_804007E8[];
extern void GXSetVtxAttrFmt(u32, u32, u32, u32, u8);
extern void GXClearVtxDesc(void);
extern void GXSetVtxDesc(u32, u32);
extern void GXSetArray(u32, u32, u8);
extern u8 lbl_80314370[];
extern u8 lbl_803143B4[];
extern u8 lbl_803143D8[];
extern u8 lbl_803143A8[];
extern u32 lbl_8047AAB0;
extern u32 lbl_8047AAAC;
extern u8 lbl_803144D0[];
extern u32 lbl_8047AAB4;
extern u16 lbl_8047AAA8;
extern u32 GScameraGetActiveCamera(void);
extern u32 GScameraGetMatrixPtr(void);
extern u32 GScameraGetInvMatrixPtr(void);
extern u32 GScameraGetProjMatrixPtr(void);
extern void GXLoadPosMtxImm(u32, u32);
extern void GXLoadNrmMtxImm(u32, u32);
extern void GXSetCurrentMtx(u32);
extern u8 lbl_8047AAC8;
extern u8 lbl_80314610[];
extern u32 lbl_8047AAC0;
extern u8 lbl_80400948[];
extern u32 lbl_8047AAC4;
extern u32 lbl_8047AABC;
extern u16 lbl_8047AAB8;
extern void GXSetTexCoordGen2(u32, u32, u32, u32, u32, u32);
extern void GXLoadTexMtxImm(void*, u32, u32);
extern u8 lbl_80314404[];
extern u8 lbl_80314454[];
extern u8 lbl_803144A8[];
extern u8 lbl_80314424[];
extern void GXInitTexObjWrapMode();
extern void GXInitTexObjLOD();
extern void GXLoadTlut();
extern void GXLoadTexObj();
extern u8 lbl_80314530[];
extern u32 lbl_8047CA40;
extern u32 lbl_8047CA48;
extern u8 lbl_80314510[];
extern u8 lbl_803144F0[];
extern void GXSetTevDirect(u32);
extern void GXSetNumIndStages(u32);
extern void GXSetNumChans();
extern void GXSetChanCtrl();
extern void GXSetNumTexGens();
extern void GXSetNumTevStages();
extern void GXSetIndTexOrder();
extern void GXSetIndTexCoordScale();
extern void GXSetIndTexMtx();
extern void GXSetTevOrder();
extern void GXSetTevColorOp();
extern void GXSetTevAlphaOp();
extern void GXSetTevColorIn();
extern void GXSetTevAlphaIn();
extern void GXSetTevKColorSel();
extern void GXSetTevKAlphaSel();
extern void GXSetTevIndirect();
extern void fn_800BBC7C();
extern void fn_800BBCE0();
extern void fn_800BBE8C();
extern void fn_800BBF98();
extern void fn_800BBFDC();
extern void GXSetTevKColor();
extern void GXSetTevSwapModeTable();
extern void GXSetProjection(void*, u32);
extern f32 lbl_8047CA50;
extern f32 lbl_8047CA54;
extern void GScameraSetViewport(void*, u16, u16, u16, u16);
extern u32 lbl_8047CA60;
extern u32 lbl_8047CA68;
extern u32 lbl_8047CA58;
extern void GXSetScissor(u32, u32, u32, u32);
extern void GScameraSetScissor(u32, u16, u16, u16, u16);
extern void HSD_FogSet(u32);
extern u32 lbl_8047AA8C;
extern void GXSetClipMode(u32);
extern void GXSetCullMode(u32);
extern u8 lbl_8031453C[];
extern void GXSetDither(u32);
extern void GXSetAlphaCompare(u32, u8, u32, u32, u8);
extern u8 lbl_8031457C[];
extern u8 lbl_8031456C[];
extern void GXSetZMode(u32, u32, u32);
extern void GXSetZCompLoc(u32);
extern u8 lbl_8031454C[];
extern void GXSetDstAlpha(u32);
extern void GXSetBlendMode(u32, u32, u32, u32);
extern u8 lbl_803145D0[];
extern u32 lbl_8031459C[];
extern u32 lbl_803145A8[];
extern void jumptable_803152B8();
extern void jumptable_80315340();
extern void jumptable_80315320();
extern void fn_800E24B0(u16);
extern void GSmemFree(u16);
extern void GXCallDisplayList(u32, u32);
extern void fn_800E2AF8(u16);
extern u16 GSmemAlloc(u32, u32);
extern u32 lbl_8047AAD8;
extern u32 lbl_8047AAD4;
extern void jumptable_80315364();
extern u32 GStextureUnlockImage(void*);
extern u8 lbl_80400EE0[];
extern u8 lbl_8047AAE0;
extern void GStextureGetFormat(void);
extern void GStextureSetWrap(void);
extern void GStextureSetFilter(void);
extern void* GStextureLockImage(void*, u32);
extern void GStextureConvertFromHW(void);
extern void HSD_LObjReqAnimAll(void*, f32);
extern void HSD_LObjAnimAll(void*);
extern u32 lbl_8047AAEC;
extern u32 lbl_8047CA80;
extern f32 lbl_8047CA70;
extern u32 lbl_8047CA74;
extern f32 lbl_8047CA78;
extern u32 lbl_8047AAF0;
extern void HSD_LObjSetPosition();
extern void HSD_LObjSetInterest();
extern void HSD_LObjRemoveAnimAll(void*);
extern void HSD_LObjAddAnimAll(void*, void*);
extern void HSD_ForeachAnim(void*, u32, u32, void*, u32, ...);
extern s32 fn_800D37CC(void);
extern void HSD_AObjSetRate(void);
extern f32 lbl_8047AAF4;
extern f32 lbl_8047CA88;
extern void GSlightSetAnimIndex(u8*, u32);
extern void HSD_LObjGetPosition(void*, void*);
extern void HSD_LObjGetInterest(void*, void*);
extern void HSD_LObjSetColor(u32, u8*);
extern void HSD_LObjClearFlags(u32, u32);
extern void HSD_LObjSetFlags(u32, u32);
extern u32 HSD_LObjLoadDesc(void*);
extern u32 lbl_8047CA8C;
extern u16 lbl_8047AAE8;
extern void __assert(u8*, u32, u8*);
extern u8 lbl_8047CA90;
extern u8 lbl_8047CA98;
extern void HSD_LObjDeleteCurrentAll(void*);
extern void HSD_LObjAddCurrentAll(void);
extern void HSD_LObjSetup(void*);
extern u32 lbl_8047AAF8;
extern u32 lbl_8047AB08;
extern u32 lbl_8047AAFA;
extern u32 lbl_8047AAFC;
extern void OSTicksToCalendarTime(void);
extern void logVsnprintf_float(void);
extern u32 strlen(const char* s);
extern u32 lbl_8047AB11;
extern u8 lbl_80400F30[];
extern u8 lbl_802704B4[];
extern u8 lbl_80400F44[];
extern u32 lbl_8047AB0C;
extern u32 lbl_8047AB00;
extern u32 lbl_8047AB04;
extern u8 lbl_80401044[];
extern u8 lbl_80401058[];
extern u32 lbl_8047AB10;
extern void jumptable_80315388();
extern void __va_arg();
extern u8 lbl_80401168[];
extern u8 lbl_80401178[];
extern u32 lbl_80478AE8;
extern u8 lbl_8047CAA0[];
extern u8 lbl_8047CAA8[];
extern void GXDrawDone(u32);
extern void HSD_ImageDescFree(u32);
extern void* HSD_ImageDescAlloc(void);
extern u16 GStextureGetXsize(void*);
extern u16 GStextureGetYsize(void*);
extern void* GStextureGetGXformat(void*, u32);
extern u8 GStextureGetMiplevels(void*);
extern f32 lbl_8047CAC8;
extern void HSD_MObjSetAlpha(u32, ...);
extern f64 lbl_8047CAD0;
extern f32 lbl_8047CACC;
extern void HSD_TObjRemove(void*);
extern void HSD_MObjCompileTev(void*);
extern u32 HSD_MObjGetFlags(void*);
extern void HSD_MObjClearFlags(void*, u32);
extern void HSD_MObjSetFlags(void*, void*);
extern u32 lbl_8047AB20;
extern u32 lbl_8047AB1C;
extern void HSD_MObjSetDefaultClass(void*);
extern u16 lbl_8047AB18;
extern u8 lbl_80315490[];
extern void hsdInitClassInfo(void*, void*, void*, void*, u32, u32);
extern u8 lbl_8036CB30[];
extern void HSD_TExpGetType();
extern void fn_801B6DC0();
extern void HSD_TExpCnst();
extern void HSD_TExpTev();
extern void HSD_TExpColorOp();
extern void HSD_TExpColorIn();
extern void HSD_TExpAlphaOp();
extern void HSD_TExpAlphaIn();
extern void HSD_ImageDescRemove(void);
extern void HSD_TObjLoadDesc(void);
extern void HSD_MObjGetTObj(void);
extern void HSD_MObjAddTObjNext(void);
extern void fn_801A6DA0(void);
extern u8 lbl_803154E4[];
extern void PSMTXMultVec(void*, void*, void*);
extern void PSVECCrossProduct(void*, void*, void*);
extern void PSVECDotProduct(void);
extern void PSVECSquareDistance(void);
extern void PSVECDistance(void);
extern void PSVECNormalize(void*, void*);
extern void PSVECMag(void);
extern void PSVECScale(void*, void*, f32);
extern const f32 lbl_8047CAD8;
extern void PSVECSubtract(void*, void*, void*);
extern void PSVECAdd(void*, void*, void*);
extern f32 lbl_8047CADC;
extern void C_MTXLookAt(void);
extern void PSMTXTranspose(void*, void*);
extern void PSMTXInverse(void*, void*);
extern void PSMTXConcat(void*, void*, void*);
extern void PSMTXScaleApply(void*, void*);
extern void PSMTXRotRad(void*, u32);
extern void PSMTXTransApply(void*, void*, f32, f32, f32);
extern void PSMTXQuat(void);
extern u8 lbl_80315568[];
extern void C_MTXPerspective(void);
extern void C_MTXOrtho(void);
extern void C_QUATSlerp(void*, void*, void*);
extern void EulerToQuat(void*, void*);
extern void C_QUATRotAxisRad(void);
extern void PSQUATMultiply(void*, void*, void*);
extern u32 lbl_8047CAE4;
extern u32 lbl_8047CAE0;
extern u32 lbl_8047CAE8;
extern f32 lbl_8047CAF0;
extern f32 lbl_8047CAF4;
extern u32 lbl_8047CB00;
extern u32 lbl_8047CAF8;
extern u32 lbl_8047CB08;
extern u32 lbl_8047CAFC;
extern f32 HSD_Randf(void);
extern f32 lbl_8047CB10;
extern u32 HSD_Rand(void);
extern u32 lbl_80478C94;
extern f64 fmod(f64 x, f64 y);
extern s32 __cvt_fp2unsigned(f32 x);
extern f32 lbl_8047CB20;
extern f32 lbl_8047CB1C;
extern f32 lbl_8047CB24;
extern f64 lbl_8047CB28;
extern f32 lbl_8047CB18;
extern f32 lbl_8047CB34;
extern f32 lbl_8047CB30;
extern f32 lbl_804011B8[];
extern f64 cos(f32);
extern f32 lbl_8047CB38;
extern f64 lbl_8047CB40;
extern u8 lbl_80270658[];
extern u32 lbl_8047AB30;
extern u32 lbl_8047AB68;
extern u32 lbl_8047AB64;
extern u32 lbl_8047AB38;
extern u32 lbl_8047AB34;
extern u32 lbl_8047AB28;
extern u32 lbl_8047AB4C;
extern u32 lbl_8047AB48;
extern u32 lbl_8047AB60;
extern u32 lbl_8047AB5C;
extern u32 lbl_8047AB58;
extern u32 lbl_8047AB54;
extern u32 lbl_8047AB50;
extern f64 lbl_8047CB50;
extern f32 lbl_8047CB48;
extern u32 lbl_8047AB3C;
extern u32 lbl_8047AB40;
extern u32 lbl_8047AB44;


#if !defined(GS_RANGE_800E0E14_ONLY)
u32 GSmemGetFreeTotal(void)
{
    typedef struct GSFreeBlock {
        struct GSFreeBlock* prev;
        struct GSFreeBlock* next;
        u32 size;
    } GSFreeBlock;
    typedef struct GSScratchState {
        u32 reserved[4];
        GSFreeBlock* head;
        u32 value;
    } GSScratchState;
    GSFreeBlock* cursor;
    GSScratchState* state;
    u32 total;

    state = (GSScratchState*)&__OSStartTime;
    cursor = state->head;
    lbl_8047AB44 = (u32)cursor;
    lbl_8047AB40 = state->value;
    for (total = 0, cursor = cursor->next; cursor != 0;
         cursor = cursor->next) {
        total += cursor->size;
    }
    return total;
}
#endif


#if !defined(GS_RANGE_800E0DDC_ONLY)
/* GSmem free-list node (lives at the start of each free block). */
typedef struct GSFreeBlock {
    struct GSFreeBlock* prev;
    struct GSFreeBlock* next;
    u32 size;
} GSFreeBlock;

/* GSmem handle-table entry; the table grows down from lbl_8047AB34. */
typedef struct GSMemHandle {
    u16 used;
    u16 locked;
    u8* data;
    u32 size;
    u16 pinned;
    u16 checksum;
} GSMemHandle;

static inline u32 gsMemLargestFree(void) {
    GSFreeBlock* block;
    u32 largest = 0;

    for (block = (GSFreeBlock*)lbl_8047AB30; block != NULL; block = block->next) {
        if (block->size > largest) {
            largest = block->size;
        }
    }
    return largest;
}

static inline GSMemHandle* gsMemFindHandle(u8* data) {
    GSMemHandle* handle;

    for (handle = (GSMemHandle*)lbl_8047AB34; handle >= (GSMemHandle*)lbl_8047AB38; handle--) {
        if (handle->used != 0 && handle->data == data) {
            return handle;
        }
    }
    return NULL;
}

/* Forward copy used to slide allocations down; word-wise when the
 * regions are at least a word apart. */
static inline void gsMemCopy(u8* dst, u8* src, u32 size) {
    u32 distance = src - dst;

    if ((distance > 0 ? distance : -distance) >= 4) {
        u32* wordDst = (u32*)dst;
        u32* wordSrc = (u32*)src;
        u32 words = size >> 2;
        u32 bytes = size & 3;

        while (words != 0) {
            *wordDst++ = *wordSrc++;
            words--;
        }
        dst = (u8*)wordDst;
        src = (u8*)wordSrc;
        while (bytes != 0) {
            *dst++ = *src++;
            bytes--;
        }
    } else {
        while (size != 0) {
            *dst++ = *src++;
            size--;
        }
    }
}

static inline void gsMemMergeNext(GSFreeBlock* block) {
    if (block->next != NULL && block->next == (GSFreeBlock*)((u8*)block + block->size)) {
        block->size += block->next->size;
        if (block->next->next != NULL) {
            block->next->next->prev = block;
        }
        block->next = block->next->next;
    }
}

static inline void gsMemMergePrev(GSFreeBlock* block) {
    if (block->prev != NULL && (GSFreeBlock*)((u8*)block->prev + block->prev->size) == block) {
        block->prev->size += block->size;
        if (block->next != NULL) {
            block->next->prev = block->prev;
        }
        block->prev->next = block->next;
    }
}

static inline void gsMemInsertFree(GSFreeBlock* block, u32 size) {
    GSFreeBlock* before = NULL;
    GSFreeBlock* scan;

    for (scan = (GSFreeBlock*)lbl_8047AB30; scan != NULL && scan < block; scan = scan->next) {
        before = scan;
    }
    if (before != NULL) {
        block->prev = before;
        block->next = before->next;
    } else {
        block->prev = NULL;
        block->next = (GSFreeBlock*)lbl_8047AB30;
        lbl_8047AB30 = (u32)block;
    }
    block->size = size;
    if (block->prev != NULL) {
        block->prev->next = block;
    }
    if (block->next != NULL) {
        block->next->prev = block;
    }
    gsMemMergeNext(block);
    gsMemMergePrev(block);
}

static inline u8 gsMemPaddingIntact(GSMemHandle* handle) {
    u8* tail;

    if (handle->data[0] != 0) {
        return 0;
    }
    if (handle->data[1] != 0) {
        return 0;
    }
    if (handle->data[2] != 0) {
        return 0;
    }
    if (handle->data[3] != 0) {
        return 0;
    }
    tail = handle->data + handle->size - 4;
    if (tail[0] != 0) {
        return 0;
    }
    if (tail[1] != 0) {
        return 0;
    }
    if (tail[2] != 0) {
        return 0;
    }
    if (tail[3] != 0) {
        return 0;
    }
    return 1;
}

static inline void gsMemClearPadding(GSMemHandle* handle) {
    u8* tail;

    tail = handle->data;
    tail[0] = 0;
    tail[1] = 0;
    tail[2] = 0;
    tail[3] = 0;
    tail = handle->data + handle->size - 4;
    tail[0] = 0;
    tail[1] = 0;
    tail[2] = 0;
    tail[3] = 0;
}

static inline u16 gsMemChecksum(GSMemHandle* handle) {
    u32 sum = 0x3D94;
    u16* half = (u16*)handle->data;
    u32 halves = handle->size >> 1;
    u32 bytes = handle->size & 1;
    u8* data;

    while (halves != 0) {
        sum += *half++;
        halves--;
    }
    data = (u8*)half;
    while (bytes != 0) {
        sum += *data++;
        bytes--;
    }
    return sum;
}

#if !defined(GS_RANGE_800E0E14_ONLY)
/* Compact the movable allocations in the GS scratch heap. */
u32 GSmemOptimize(void)
{
    extern const char lbl_80270BB8[];
    GSFreeBlock* block;
    GSFreeBlock* fresh;
    GSFreeBlock* prev;
    GSFreeBlock* next;
    GSMemHandle* desc;
    GSMemHandle* saved;
    GSMemHandle** entry;
    GSMemHandle* top;
    GSMemHandle* bottom;
    GSMemHandle* candidates[4];
    u8* destination;
    u8* source;
    u32 oldLargest;
    u32 size;
    u32 remaining;
    u32 total;
    u32 sum;
    s32 count;
    s32 i;
    s32 j;
    u8 wasHead;

    oldLargest = gsMemLargestFree();
    block = (GSFreeBlock*)lbl_8047AB30;
    if (block != NULL && block->next != NULL) {
        while (block != NULL) {
            size = block->size;
            bottom = (GSMemHandle*)lbl_8047AB38;
            if ((u8*)block + size != (u8*)bottom) {
            top = (GSMemHandle*)lbl_8047AB34;
            desc = gsMemFindHandle((u8*)block + size);
            if (desc == NULL) {
                GSlogWrite(lbl_80270BB8);
                return 0;
            }
            if (desc->locked == 0 && desc->pinned == 0) {
                wasHead = 0;
                if ((GSFreeBlock*)lbl_8047AB30 == block) {
                    wasHead = 1;
                }
                prev = block->prev;
                next = block->next;
                fresh = (GSFreeBlock*)((u8*)block + desc->size);
                if (block->prev != NULL) {
                    block->prev->next = fresh;
                }
                if (block->next != NULL) {
                    block->next->prev = fresh;
                }
                gsMemCopy((u8*)block, desc->data, desc->size);
                desc->data = (u8*)block;
                fresh->prev = prev;
                fresh->next = next;
                fresh->size = size;
                gsMemMergeNext(fresh);
                if (wasHead) {
                    lbl_8047AB30 = (u32)fresh;
                }
                block = (GSFreeBlock*)lbl_8047AB30;
                continue;
            }

            candidates[0] = NULL;
            candidates[1] = NULL;
            candidates[2] = NULL;
            candidates[3] = NULL;
            entry = candidates;
            count = 0;
            total = 0;
            for (desc = top; desc >= bottom; desc--) {
                if (desc->used == 0 || desc->locked != 0 || desc->size > block->size ||
                    desc->data <= (u8*)block || desc->pinned != 0) {
                    continue;
                }
                if (count < 4 && total + desc->size <= block->size) {
                    candidates[count++] = desc;
                    total += desc->size;
                    continue;
                }
                for (i = 0; i < count; i++) {
                    saved = candidates[i];
                    candidates[i] = desc;
                    sum = 0;
                    for (j = 0; j < count; j++) {
                        sum += candidates[j]->size;
                    }
                    if (sum > total && sum <= block->size) {
                        total = sum;
                        i = count;
                    } else {
                        candidates[i] = saved;
                    }
                }
            }
            if (count > 0) {

            prev = block->prev;
            next = block->next;
            remaining = block->size;
            if (block == (GSFreeBlock*)lbl_8047AB30) {
                lbl_8047AB30 = (u32)next;
            }
            if (prev != NULL) {
                prev->next = next;
            }
            if (next != NULL) {
                next->prev = prev;
            }
            destination = (u8*)block;
            for (i = 0; i < count; i++) {
                source = (*entry)->data;
                gsMemCopy(destination, source, (*entry)->size);
                (*entry)->data = destination;
                destination += (*entry)->size;
                remaining -= (*entry)->size;
                gsMemInsertFree((GSFreeBlock*)source, (*entry)->size);
                entry++;
            }
            if (remaining >= sizeof(GSFreeBlock)) {
                fresh = (GSFreeBlock*)destination;
                fresh->prev = prev;
                fresh->next = next;
                fresh->size = remaining;
                if (fresh->prev != NULL) {
                    fresh->prev->next = fresh;
                } else {
                    lbl_8047AB30 = (u32)fresh;
                }
                if (fresh->next != NULL) {
                    fresh->next->prev = fresh;
                }
                gsMemMergeNext(fresh);
                gsMemMergePrev(fresh);
            } else {
                desc = candidates[count - 1];
                desc->size += remaining;
                if (*(u8*)&lbl_8047AB28 != 0) {
                    gsMemClearPadding(desc);
                    desc->checksum = gsMemChecksum(desc);
                }
            }
            block = (GSFreeBlock*)lbl_8047AB30;
            continue;
            }
            }
            block = block->next;
        }
    }
    return gsMemLargestFree() - oldLargest;
}
#endif

extern u8 lbl_80270658[];
extern u32 lbl_8047AB30;
extern u32 lbl_8047AB68;
extern u32 lbl_8047AB64;
extern u32 lbl_8047AB38;
extern u32 lbl_8047AB34;
extern u32 lbl_8047AB28;
extern u32 lbl_8047AB4C;
extern u32 lbl_8047AB48;
extern u32 lbl_8047AB60;
extern u32 lbl_8047AB5C;
extern u32 lbl_8047AB58;
extern u32 lbl_8047AB54;
extern u32 lbl_8047AB50;
extern f64 lbl_8047CB50;
extern f32 lbl_8047CB48;
extern u32 lbl_8047AB3C;
u8 GSmemDumpMap(u8 verbose, u8 dumpMap) {
    extern void GSlogWritef(const char*, ...);
    GSFreeBlock* block;
    GSFreeBlock* next;
    GSMemHandle* desc;
    u32 cursor;
    u32 size;
    u32 largest;
    u32 total;
    u8 ok;
    u32 allocatedCount;
    u32 freeCount;

    ok = 1;
    allocatedCount = 0;
    freeCount = 0;
    if (verbose) {
        GSlogWrite("--[ GSmem state check ]--------------------------\n");
    }
    if ((GSFreeBlock*)lbl_8047AB30 != NULL && ((GSFreeBlock*)lbl_8047AB30)->prev != NULL) {
        GSlogWrite("!!! first block's previous pointer is not NULL\n");
        ok = 0;
    }
    for (block = (GSFreeBlock*)lbl_8047AB30; block != NULL; block = block->next) {
        freeCount++;
        if ((u32)block < lbl_8047AB68 || (u32)block > lbl_8047AB64) {
            GSlogWrite("!!! Free block is outside of GSmem area\n");
            ok = 0;
        }
        if ((u32)block >= lbl_8047AB38) {
            GSlogWrite("!!! Free block exists in memory entry table\n");
            ok = 0;
        }
        if ((u32)block + block->size > lbl_8047AB38) {
            GSlogWrite("!!! Free block includes space in memory entry table\n");
            ok = 0;
        }
        next = block->next;
        if (next != NULL) {
            if (next->prev != block) {
                GSlogWrite("!!! Free block list pointers inconsistent\n");
                ok = 0;
            }
            if (next == (GSFreeBlock*)((u8*)block + block->size)) {
                GSlogWrite("!!! Free blocks have not been merged correctly\n");
                ok = 0;
            }
            if (block > next) {
                GSlogWrite("!!! Free block list pointers not linear\n");
                ok = 0;
            }
        }
    }

    for (desc = (GSMemHandle*)lbl_8047AB34; (u32)desc >= lbl_8047AB38; desc--) {
        if (desc->used == 0) {
            continue;
        }
        allocatedCount++;
        if ((u32)desc->data < lbl_8047AB68 || (u32)(desc->data + desc->size) > lbl_8047AB38) {
            GSlogWrite("!!! Block exists in an invalid memory area\n");
            ok = 0;
        }
        if (*(u8*)&lbl_8047AB28 == 0) {
            continue;
        }
        if (!gsMemPaddingIntact(desc)) {
            GSlogWrite("!!! Padding bytes overwritten in handle %d\n", desc->used);
            ok = 0;
            gsMemClearPadding(desc);
        }
        if (desc->locked == 0 && desc->checksum != gsMemChecksum(desc)) {
            GSlogWrite("!!! Block modified without lock in handle %d\n", desc->used);
            ok = 0;
        }
    }

    if (dumpMap) {
        GSlogWrite("Memory map dump:\n");
    }
    cursor = lbl_8047AB68;
    while (cursor < lbl_8047AB38) {
        desc = gsMemFindHandle((u8*)cursor);
        if (desc != NULL && desc->used != 0) {
            if (dumpMap) {
                GSlogWrite("  %08Xh -> %08Xh: allocated block (size=%d, handle=%d, locks=%d, align=%d)\n",
                           cursor, cursor + (desc->size - 1), desc->size, desc->used,
                           desc->locked, desc->pinned);
            }
            cursor += desc->size;
            continue;
        }
        block = (GSFreeBlock*)cursor;
        if (dumpMap) {
            GSlogWrite("  %08Xh -> %08Xh: free block (size=%d, prev=%08Xh, next=%08Xh)\n", cursor,
                       cursor + (block->size - 1), block->size, block->prev, block->next);
        }
        if ((block->prev != NULL && ((u32)block->prev < lbl_8047AB68 || (u32)block->prev > lbl_8047AB64)) ||
            (block->next != NULL && ((u32)block->next < lbl_8047AB68 || (u32)block->next > lbl_8047AB64))) {
            GSlogWrite("!!! pointer at %08Xh does not appear to be a valid free block\n", cursor);
            ok = 0;
            break;
        }
        size = block->size;
        if (cursor + size > lbl_8047AB38 || size == 0) {
            GSlogWrite("!!! pointer at %08Xh has invalid size %08Xh\n", cursor, size);
            ok = 0;
            break;
        }
        cursor += size;
    }
    if (cursor != lbl_8047AB38) {
        GSlogWrite("!!! End of memory map does not line up with last block! [last:%08X != end:%08X]\n",
                   cursor, lbl_8047AB38);
        ok = 0;
    }
    if (allocatedCount != lbl_8047AB4C) {
        GSlogWrite("!!! Used block count does not match internal counter\n");
        ok = 0;
    }

    if (verbose) {
        GSlogWrite("GSmem memory area:   %08Xh -> %08Xh\n", lbl_8047AB68, lbl_8047AB64);
        GSlogWrite("Active allocations:  %d\n", lbl_8047AB4C);
        GSlogWrite("Active locks:        %d\n", lbl_8047AB48);
        GSlogWrite("Entry table size:    %d bytes\n", lbl_8047AB34 - lbl_8047AB38 + 0x10);
        largest = 0;
        for (block = (GSFreeBlock*)lbl_8047AB30; block != NULL; block = block->next) {
            if (block->size > largest) {
                largest = block->size;
            }
        }
        GSlogWrite("Largest free space:  %d bytes\n", largest);
        total = 0;
        for (block = (GSFreeBlock*)lbl_8047AB30; block != NULL; block = block->next) {
            total += block->size;
        }
        GSlogWrite("Total free space:    %d bytes\n", total);
        GSlogWrite("Total free blocks:   %d\n", freeCount);
        GSlogWrite("Total allocations:   %d\n", lbl_8047AB60);
        GSlogWrite("Total locks:         %d\n", lbl_8047AB5C);
        GSlogWrite("Total unlocks:       %d\n", lbl_8047AB58);
        GSlogWrite("Total frees:         %d\n", lbl_8047AB54);
        GSlogWrite("Total entry resizes: %d\n", lbl_8047AB50);
        GSlogWritef("Fragmentation:       %.1f%%\n",
                    100.0f * ((f32)(freeCount - 1) / (f32)(freeCount + allocatedCount)));
        GSlogWrite("-------------------------------------------------\n");
    }
    if (!ok) {
        lbl_8047AB3C = 0;
    }
    return ok;
}
#endif

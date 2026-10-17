/**
 * @file gs_gfx_layer_state.c
 * @brief GS render engine segment -- split from gs_render.c.
 *
 * XD source unit: game/pxdvs/GSAPI/GSgfxM/layerState.cpp (structural)
 * Address range: 0x800D6B00 - 0x800D9AF0 (62 functions)
 *
 * No anchors but strong structural mirror of XD layerState.cpp (0x3D8C): our 0x730 lead fn ~ SetupGPUState (XD 0x1510 after inlining), a 31-fn tiny accessor farm (0x14-0x34) = dirty-state helpers, then ~20 setters sized 0x78-0x1D8 mirroring the GSgfx_GCSet{Fog,AlphaCompare,BlendMode,TevSwap,TevKColor,Ind*,Tev*,MatColor,AmbColor,ChanCtrl,Scissor,Projection} farm, closing with renderState trio 0x910/0x400/0x4B4 mirroring XD SetupRenderState/renderState_NL_VC_T/NL_NVC_T/NL_NVC_NT...
 *
 * Split from src/game/gs_render.c (physical XD source-unit split).
 * The dead #ifdef PCPORT reference block (never defined in configure.py,
 * same situation as gs_gfx.c) was stripped during the split.
 */

#include "dolphin/types.h"

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
extern void fn_800D7940(u8*, u16);
extern void gfxLoadVtxDesc(u32);
extern void _gfxFlushTevState(void);
extern void gfxSetTevOp(u32, s32);
extern void fn_800DB098(void);
extern void fn_800DB758(u16);
extern void lightGetFrameCount__FP9_HSD_AObj(u8*);
extern void GSlogVPrintf(void);
extern void GSlogFormat(void);
extern void fn_800E09E8(void*, void*, u32);
extern u8 GSmemDumpMap(u32, u32);
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
extern u32 lbl_80478AE0[];
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
extern u32 lbl_8047CB50;
extern u32 lbl_8047CB48;
extern u32 lbl_8047AB3C;

typedef struct GSgfxStatePartial {
    /* 0x000 */ s32 recordMode;
    /* 0x004 */ u8 pad_004[0x4A0];
    /* 0x4A4 */ u8 fifoAttrBase;
    /* 0x4A5 */ u8 pad_4A5[0x13];
    /* 0x4B8 */ f32 fifoVecX;
    /* 0x4BC */ f32 fifoVecY;
    /* 0x4C0 */ f32 fifoVecZ;
} GSgfxStatePartial;


/* ==================================================================
 * Matrix accessor functions (0x800D7230 - 0x800D75D0)
 *
 * A block of ~40 tiny functions (0x14-0x24 bytes each) that get/set
 * individual matrix elements, indices, and stack pointers.
 *
 * These follow two patterns:
 *   Pattern A (getter, 0x1C bytes):
 *     lwz r3, offset(r13/r0)   ; load from sda
 *     lwz r3, field(r3)        ; read struct field
 *     blr
 *
 *   Pattern B (setter, 0x24 bytes):
 *     lwz r4, offset(r13/r0)   ; load state ptr
 *     stw r3, field(r4)        ; store value
 *     blr
 * ================================================================== */

#if defined(GS_GFX_LAYER_800D7230_800D76A8)
void _gfxEmitMtxIdx(void) {
    GSgfxStatePartial* state = (GSgfxStatePartial*)lbl_8047AA80;
    u8 value = state->fifoAttrBase;

    *(volatile u8*)0xCC008000 = value + (value << 1);
}
#endif

/* ==================================================================
 * fn_800D892C -- GSgfx_ConfigurePipeline
 *
 * Configure the full GX rendering pipeline. At 2320 bytes, this is
 * a major setup function. Called once per render mode change.
 * ================================================================== */
/* fn_800D892C: owner is the standalone gs_gfx_layer_candidate_800D892C.c. */

/* gfxFlushPendingVertex: owner is the standalone gs_gfx_layer_candidate_800D6B00.c. */

#if defined(GS_GFX_LAYER_800D7230_800D76A8)
extern u32 lbl_8047AA80;
#if 0
asm void fn_800D724C(void) {
#include "src/game/gs_render_fn_800D724C.inc"
}
#else
void _gfxEmitTexCoord1x8(u32 idx) { *(volatile u8*)0xCC008000 = *(u8*)(lbl_8047AA80 + 0x520 + idx*16); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7268(void) {
#include "src/game/gs_render_fn_800D7268.inc"
}
#else
void _gfxEmitTexCoord1x16(u32 idx) { *(volatile u16*)0xCC008000 = *(u16*)(lbl_8047AA80 + 0x522 + idx*16); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7284(void) {
#include "src/game/gs_render_fn_800D7284.inc"
}
#else
void _gfxEmitTexCoord2s8(u32 idx) { u8 v = *(u8*)(lbl_8047AA80 + 0x520 + idx*16); *(volatile u8*)0xCC008000 = v; *(volatile u8*)0xCC008000 = v; }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D72A4(void) {
#include "src/game/gs_render_fn_800D72A4.inc"
}
#else
void _gfxEmitTexCoord2u8(u32 idx) { u8 v = *(u8*)(lbl_8047AA80 + 0x520 + idx*16); *(volatile u8*)0xCC008000 = v; *(volatile u8*)0xCC008000 = v; }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D72C4(void) {
#include "src/game/gs_render_fn_800D72C4.inc"
}
#else
void _gfxEmitTexCoord2s16(u32 idx) { u16 v = *(u16*)(lbl_8047AA80 + 0x522 + idx*16); *(volatile u16*)0xCC008000 = v; *(volatile u16*)0xCC008000 = v; }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D72E4(void) {
#include "src/game/gs_render_fn_800D72E4.inc"
}
#else
void _gfxEmitTexCoord2u16(u32 idx) { u16 v = *(u16*)(lbl_8047AA80 + 0x522 + idx*16); *(volatile u16*)0xCC008000 = v; *(volatile u16*)0xCC008000 = v; }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7304(void) {
#include "src/game/gs_render_fn_800D7304.inc"
}
#else
void _gfxEmitTexCoord2f32(u32 idx) {
    u32 base = lbl_8047AA80 + idx*16;
    f32 a = *(f32*)(base + 0x528);
    f32 b = *(f32*)(base + 0x52c);
    *(volatile f32*)0xCC008000 = a;
    *(volatile f32*)0xCC008000 = b;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7328(void) {
#include "src/game/gs_render_fn_800D7328.inc"
}
#else
void _gfxEmitColor1x8(u32 idx) { *(volatile u8*)0xCC008000 = *(u8*)(lbl_8047AA80 + 0x4e8 + idx*12); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7344(void) {
#include "src/game/gs_render_fn_800D7344.inc"
}
#else
void _gfxEmitColor1x16(u32 idx) { *(volatile u16*)0xCC008000 = *(u16*)(lbl_8047AA80 + 0x4ec + idx*12); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7360(void) {
#include "src/game/gs_render_fn_800D7360.inc"
}
#else
void _gfxEmitColor1u16(u32 idx) { *(volatile u16*)0xCC008000 = *(u16*)(lbl_8047AA80 + 0x4ec + idx*12); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D737C(void) {
#include "src/game/gs_render_fn_800D737C.inc"
}
#else
void _gfxEmitColor1u32(u32 idx) { *(volatile u32*)0xCC008000 = *(u32*)(lbl_8047AA80 + 0x4f0 + idx*12); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7398(void) {
#include "src/game/gs_render_fn_800D7398.inc"
}
#else
void _gfxEmitColor3u8(u32 idx) {
    u8 b;
    u8 c;
    u8 a;
    u32 base = lbl_8047AA80 + idx*12;
    c = *(u8*)(base + 0x4ea);
    b = *(u8*)(base + 0x4e9);
    a = *(u8*)(base + 0x4e8);
    *(volatile u8*)0xCC008000 = a;
    *(volatile u8*)0xCC008000 = b;
    *(volatile u8*)0xCC008000 = c;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D73C4(void) {
#include "src/game/gs_render_fn_800D73C4.inc"
}
#else
void _gfxEmitColor4u8(u32 idx) {
    u8 b;
    u8 c;
    u8 d;
    u8 a;
    u32 base = lbl_8047AA80 + idx*12;
    d = *(u8*)(base + 0x4eb);
    c = *(u8*)(base + 0x4ea);
    b = *(u8*)(base + 0x4e9);
    a = *(u8*)(base + 0x4e8);
    *(volatile u8*)0xCC008000 = a;
    *(volatile u8*)0xCC008000 = b;
    *(volatile u8*)0xCC008000 = c;
    *(volatile u8*)0xCC008000 = d;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D73F8(void) {
#include "src/game/gs_render_fn_800D73F8.inc"
}
#else
void _gfxEmitNormal1x8(void) { *(volatile u8*)0xCC008000 = *(u8*)(lbl_8047AA80 + 0x4c8); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D740C(void) {
#include "src/game/gs_render_fn_800D740C.inc"
}
#else
void _gfxEmitNormal1x16(void) { *(volatile u16*)0xCC008000 = *(u16*)(lbl_8047AA80 + 0x4cc); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7420(void) {
#include "src/game/gs_render_fn_800D7420.inc"
}
#else
void _gfxEmitNormal3s8(void) {
    u8 a, b, c;
    u8 *base = (u8*)lbl_8047AA80;
    c = base[0x4ca];
    b = base[0x4c9];
    a = base[0x4c8];
    *(volatile u8*)0xCC008000 = a;
    *(volatile u8*)0xCC008000 = b;
    *(volatile u8*)0xCC008000 = c;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7444(void) {
#include "src/game/gs_render_fn_800D7444.inc"
}
#else
void _gfxEmitNormal3s16(void) {
    u16 a, b, c;
    u8 *base = (u8*)lbl_8047AA80;
    c = *(u16*)(base + 0x4d0);
    b = *(u16*)(base + 0x4ce);
    a = *(u16*)(base + 0x4cc);
    *(volatile u16*)0xCC008000 = a;
    *(volatile u16*)0xCC008000 = b;
    *(volatile u16*)0xCC008000 = c;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7468(void) {
#include "src/game/gs_render_fn_800D7468.inc"
}
#else
void _gfxEmitNormal3f32(void) {
    u8 *base = (u8*)lbl_8047AA80;
    f32 b;
    f32 c;
    f32 a;
    c = *(f32*)(base + 0x4dc);
    b = *(f32*)(base + 0x4d8);
    a = *(f32*)(base + 0x4d4);
    *(volatile f32*)0xCC008000 = a;
    *(volatile f32*)0xCC008000 = b;
    *(volatile f32*)0xCC008000 = c;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D748C(void) {
#include "src/game/gs_render_fn_800D748C.inc"
}
#else
void _gfxEmitPos1x8(void) { *(volatile u8*)0xCC008000 = *(u8*)(lbl_8047AA80 + 0x4ac); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D74A0(void) {
#include "src/game/gs_render_fn_800D74A0.inc"
}
#else
void _gfxEmitPos1x16(void) { *(volatile u16*)0xCC008000 = *(u16*)(lbl_8047AA80 + 0x4b0); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D74B4(void) {
#include "src/game/gs_render_fn_800D74B4.inc"
}
#else
void _gfxEmitPos2s8(void) {
    u8 a, b;
    u8 *base = (u8*)lbl_8047AA80;
    b = *(u8*)(base + 0x4ad);
    a = *(u8*)(base + 0x4ac);
    *(volatile u8*)0xCC008000 = a;
    *(volatile u8*)0xCC008000 = b;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D74D0(void) {
#include "src/game/gs_render_fn_800D74D0.inc"
}
#else
void _gfxEmitPos2u8(void) {
    u8 a, b;
    u8 *base = (u8*)lbl_8047AA80;
    b = *(u8*)(base + 0x4ad);
    a = *(u8*)(base + 0x4ac);
    *(volatile u8*)0xCC008000 = a;
    *(volatile u8*)0xCC008000 = b;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D74EC(void) {
#include "src/game/gs_render_fn_800D74EC.inc"
}
#else
void _gfxEmitPos2s16(void) {
    u16 a, b;
    u8 *base = (u8*)lbl_8047AA80;
    b = *(u16*)(base + 0x4b2);
    a = *(u16*)(base + 0x4b0);
    *(volatile u16*)0xCC008000 = a;
    *(volatile u16*)0xCC008000 = b;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7508(void) {
#include "src/game/gs_render_fn_800D7508.inc"
}
#else
void _gfxEmitPos2u16(void) {
    u16 a, b;
    u8 *base = (u8*)lbl_8047AA80;
    b = *(u16*)(base + 0x4b2);
    a = *(u16*)(base + 0x4b0);
    *(volatile u16*)0xCC008000 = a;
    *(volatile u16*)0xCC008000 = b;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7524(void) {
#include "src/game/gs_render_fn_800D7524.inc"
}
#else
void _gfxEmitPos2f32(void) {
    u8 *base = (u8*)lbl_8047AA80;
    f32 b = *(f32*)(base + 0x4bc);
    f32 a = *(f32*)(base + 0x4b8);
    *(volatile f32*)0xCC008000 = a;
    *(volatile f32*)0xCC008000 = b;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7540(void) {
#include "src/game/gs_render_fn_800D7540.inc"
}
#else
void _gfxEmitPos3s8(void) {
    u8 a, b, c;
    u8 *base = (u8*)lbl_8047AA80;
    c = *(u8*)(base + 0x4ae);
    b = *(u8*)(base + 0x4ad);
    a = *(u8*)(base + 0x4ac);
    *(volatile u8*)0xCC008000 = a;
    *(volatile u8*)0xCC008000 = b;
    *(volatile u8*)0xCC008000 = c;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7564(void) {
#include "src/game/gs_render_fn_800D7564.inc"
}
#else
void _gfxEmitPos3u8(void) {
    u8 a, b, c;
    u8 *base = (u8*)lbl_8047AA80;
    c = *(u8*)(base + 0x4ae);
    b = *(u8*)(base + 0x4ad);
    a = *(u8*)(base + 0x4ac);
    *(volatile u8*)0xCC008000 = a;
    *(volatile u8*)0xCC008000 = b;
    *(volatile u8*)0xCC008000 = c;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7588(void) {
#include "src/game/gs_render_fn_800D7588.inc"
}
#else
void _gfxEmitPos3s16(void) {
    u16 a, b, c;
    u8 *base = (u8*)lbl_8047AA80;
    c = *(u16*)(base + 0x4b4);
    b = *(u16*)(base + 0x4b2);
    a = *(u16*)(base + 0x4b0);
    *(volatile u16*)0xCC008000 = a;
    *(volatile u16*)0xCC008000 = b;
    *(volatile u16*)0xCC008000 = c;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D75AC(void) {
#include "src/game/gs_render_fn_800D75AC.inc"
}
#else
void _gfxEmitPos3u16(void) {
    u16 a, b, c;
    u8 *base = (u8*)lbl_8047AA80;
    c = *(u16*)(base + 0x4b4);
    b = *(u16*)(base + 0x4b2);
    a = *(u16*)(base + 0x4b0);
    *(volatile u16*)0xCC008000 = a;
    *(volatile u16*)0xCC008000 = b;
    *(volatile u16*)0xCC008000 = c;
}
#endif

#if 0
asm void fn_800D75F4(void) {
#include "src/game/gs_render_fn_800D75F4.inc"
}
#else
void _gfxEmitPos3f32(void) {
    GSgfxStatePartial* state = (GSgfxStatePartial*)lbl_8047AA80;
    f32 x;
    f32 y;
    f32 z;

    z = state->fifoVecZ;
    y = state->fifoVecY;
    x = state->fifoVecX;

    *(volatile f32*)0xCC008000 = x;
    *(volatile f32*)0xCC008000 = y;
    *(volatile f32*)0xCC008000 = z;
}

void gfxVtxDescFree(u8* obj) {
    if (*(u32*)(lbl_8047AA80 + 0x24) == (u32)obj) *(u32*)(lbl_8047AA80 + 0x24) = 0;
    obj[0x008] = 0; obj[0x024] = 0; obj[0x040] = 0; obj[0x05c] = 0;
    obj[0x078] = 0; obj[0x094] = 0; obj[0x0b0] = 0; obj[0x0cc] = 0;
    obj[0x0e8] = 0; obj[0x104] = 0; obj[0x120] = 0; obj[0x13c] = 0;
    obj[0x158] = 0; obj[0x174] = 0; obj[0x000] = 0;
}
#endif

#if 0
asm void fn_800D7650(void) {
#include "src/game/gs_render_fn_800D7650.inc"
}
#else
void gfxVtxDescClear(u8* obj) {
    if (*(u32*)(lbl_8047AA80 + 0x24) == (u32)obj) *(u32*)(lbl_8047AA80 + 0x24) = 0;
    obj[0x008] = 0; obj[0x024] = 0; obj[0x040] = 0; obj[0x05c] = 0;
    obj[0x078] = 0; obj[0x094] = 0; obj[0x0b0] = 0; obj[0x0cc] = 0;
    obj[0x0e8] = 0; obj[0x104] = 0; obj[0x120] = 0; obj[0x13c] = 0;
    obj[0x158] = 0; obj[0x174] = 0;
}
#endif
#endif

extern u8 lbl_80314370[];
extern u8 lbl_803143B4[];
extern u8 lbl_803143D8[];
extern u8 lbl_803143A8[];
#if defined(GS_GFX_LAYER_800D76A8_800D7820)
#if 0
asm void fn_800D76A8(void) {
#include "src/game/gs_render_fn_800D76A8.inc"
}
#else
void gfxSetVtxDescRaw(u32 obj, u16 vertCount) {
    u32 state;
    u32 oldObj;

    state = lbl_8047AA80;
    if (*(u8*)(state + 0x47e) == 1) {
        oldObj = *(u32*)(state + 0x24);
        *(u32*)(state + 0x24) = obj;
        fn_800D7940((u8*)obj, vertCount);
        *(u32*)(state + 0x24) = oldObj;
        return;
    }

    if (*(s32*)state == 1) {
        gfxRecordCommand(0x47, 2, obj, (u32)vertCount);
        return;
    }

    gfxLoadVtxDesc(obj);
    gfxSetupChannelsTev(obj);
    fn_800D7940((u8*)obj, vertCount);
}
#endif
#endif

#if defined(GS_GFX_LAYER_800D7820_800D7894)
#if 0
asm void fn_800D7820(void) {
#include "src/game/gs_render_fn_800D7820.inc"
}
#else
void gfxSetVtxDesc(u32 val) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x46, 1, val); }
    else { *(s32*)(lbl_8047AA80 + 0x24) = val; }
}
#endif

#if 0
asm void fn_800D7868(void) {
#include "src/game/gs_render_fn_800D7868.inc"
}
#else
void gfxVtxDescSetAttr(u8* arr, u32 idx, u32 a, u32 b, u32 c, u8 d, u32 e, u8 f) {
    u8* p = arr + idx * 0x1c;
    p[0x8] = 1;
    *(u32*)(p + 0xc) = a;
    *(u32*)(p + 0x10) = b;
    *(u32*)(p + 0x14) = c;
    p[0x18] = d;
    *(u32*)(p + 0x1c) = e;
    p[0x20] = f;
}
#endif
#endif

#if defined(GS_GFX_LAYER_800D7894_800D7940)
extern u32 lbl_8047AAB0;
extern u32 lbl_8047AAAC;
extern u8 lbl_803144D0[];
extern u32 lbl_8047AAB4;
#if 0
asm void fn_800D7894(void) {
#include "src/game/gs_render_fn_800D7894.inc"
}
#else
u8* gfxVtxDescAlloc(void) {
    u32 i;
    u32 cnt = lbl_8047AAB0;
    u8* p = (u8*)lbl_8047AAAC;
    for (i = 0; i < cnt; i++) {
        if (*p == 0) {
            *p = 1;
            *(u32*)(p + 4) = *(u32*)(lbl_803144D0 + lbl_8047AAB4 * 4);
            lbl_8047AAB4++;
            if (lbl_8047AAB4 >= 8) {
                lbl_8047AAB4 = 0;
            }
            p[0x8] = 0;
            p[0x24] = 0;
            p[0x40] = 0;
            p[0x5c] = 0;
            p[0x78] = 0;
            p[0x94] = 0;
            p[0xb0] = 0;
            p[0xcc] = 0;
            p[0xe8] = 0;
            p[0x104] = 0;
            p[0x120] = 0;
            p[0x13c] = 0;
            p[0x158] = 0;
            p[0x174] = 0;
            return p;
        }
        p += 0x190;
    }
    return 0;
}
#endif

#endif

#if defined(GS_GFX_LAYER_800D7940_800D7A70)
#if 0
asm void fn_800D7940(void) {
#include "src/game/gs_render_fn_800D7940.inc"
}
#else
extern void gfxVtxPos1x16(u16);
extern void gfxVtxPos1x8(u8);
extern void gfxVtxNormal1x16(u16);
extern void gfxVtxNormal1x8(u8);
extern void gfxVtxColor1x16(u32, u16);
extern void gfxVtxColor1x8(u32, u8);
extern void gfxVtxTexCoord1x16(u32, u16);
extern void gfxVtxTexCoord1x8(u32, u8);
extern void gfxEnd(void);

void fn_800D7940(u8* obj, u16 vertCount) {
    u16 vertex;
    s32 i;
    u32 layer;
    u8* entry;

    gfxBegin(vertCount);
    for (vertex = 0; vertex < vertCount; vertex++) {
        if (*(s32*)(obj + 0x28) == 2) {
            gfxVtxPos1x16(vertex);
        } else {
            gfxVtxPos1x8(vertex);
        }

        if (*(u8*)(obj + 0x40) == 1) {
            if (*(s32*)(obj + 0x44) == 2) {
                gfxVtxNormal1x16(vertex);
            } else {
                gfxVtxNormal1x8(vertex);
            }
        }

        for (i = 4; i <= 5; i++) {
            entry = (u8*)(obj + i * 0x1c);
            if (entry[0x8] == 1) {
                layer = i - 4;
                if (*(s32*)(entry + 0xc) == 2) {
                    gfxVtxColor1x16(layer, vertex);
                } else {
                    gfxVtxColor1x8(layer, vertex);
                }
            }
        }

        for (i = 6; i <= 13; i++) {
            entry = (u8*)(obj + i * 0x1c);
            if (entry[0x8] == 1) {
                layer = i - 6;
                if (*(s32*)(entry + 0xc) == 2) {
                    gfxVtxTexCoord1x16(layer, vertex);
                } else {
                    gfxVtxTexCoord1x8(layer, vertex);
                }
            }
        }
    }
    gfxEnd();
}
#endif
#endif

#if defined(GS_GFX_LAYER_800D7A70_800D7D90)
#if 0
asm void fn_800D7A70(u32) {
#include "src/game/gs_render_fn_800D7A70.inc"
}
#else
void gfxLoadVtxDesc(u32 obj) {
    u32 i;
    u8* entry;
    u32 attr;

    for (i = 0; (s32)i < 14; i++) {
        entry = (u8*)(obj + i * 0x1c);
        if (entry[0x8] != 0 && (s32)i != 0) {
            attr = ((u32*)lbl_80314370)[i];
            GXSetVtxAttrFmt(*(u32*)(obj + 0x4), attr,
                        ((u32*)lbl_803143B4)[*(u32*)(entry + 0x10)],
                        ((u32*)lbl_803143D8)[*(u32*)(entry + 0x14)],
                        entry[0x18]);
        }
    }

    GXClearVtxDesc();

    for (i = 0; (s32)i < 14; i++) {
        entry = (u8*)(obj + i * 0x1c);
        if (entry[0x8] != 0) {
            GXSetVtxDesc(((u32*)lbl_80314370)[i], ((u32*)lbl_803143A8)[*(u32*)(entry + 0xc)]);
            if (*(u32*)(entry + 0x1c) != 0) {
                GXSetArray(((u32*)lbl_80314370)[i], *(u32*)(entry + 0x1c), entry[0x20]);
            }
        }
    }

    *(u32*)(lbl_804001F0 + 0x14) += 1;
}
#endif

extern u32 lbl_8047AAB0;
extern u16 lbl_8047AAA8;
extern u32 lbl_8047AAAC;
extern u32 lbl_8047AAB4;
#if 0
asm void fn_800D7B80(void) {
#include "src/game/gs_render_fn_800D7B80.inc"
}
#else
void gfxVtxDescPoolInit(u32 count) {
    u16 handle;
    u32 off;
    u32 i;
    lbl_8047AAB0 = count;
    off = _toolentryAlloc__FUl(count * 0x190);
    handle = off;
    lbl_8047AAA8 = handle;
    if (handle != 0) {
        lbl_8047AAAC = (u32)GSmemLock(handle);
        for (off = 0, i = 0; i < lbl_8047AAB0; i++, off += 0x190) {
            *(u8*)(lbl_8047AAAC + off) = 0;
        }
        lbl_8047AAB4 = 0;
    }
}
#endif

#if 0
asm void fn_800D7BF8(void) {
#include "src/game/gs_render_fn_800D7BF8.inc"
}
#else
u32 gfxGetCameraMatrix(s32 mode) {
    if (!GScameraGetActiveCamera()) return 0;
    switch (mode) {
        case 0: return GScameraGetMatrixPtr();
        case 1: return GScameraGetInvMatrixPtr();
        case 2: return GScameraGetProjMatrixPtr();
        default: return 0;
    }
}
#endif

extern u8 lbl_8047AAC8;
extern u8 lbl_80314610[];
extern u32 lbl_8047AAC0;
extern u8 lbl_80400948[];
#if 0
asm void fn_800D7C74(void) {
#include "src/game/gs_render_fn_800D7C74.inc"
}
#else
void gfxFlushMatrix(void) {
    u32 state = lbl_8047AA80;
    u32 mtx;
    if (*(s32*)state == 1) {
        gfxRecordCommand(0x45, 0);
    } else if (lbl_8047AAC8) {
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        GXSetCurrentMtx(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

#if 0
asm void fn_800D7D10(void) {
#include "src/game/gs_render_fn_800D7D10.inc"
}
#else
void gfxGetMatrix(u8 idx, void* src) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x44, 2, (u32)idx, src); }
    else if (idx > 9) { GSlogWrite(lbl_80270440); }
    else { fn_800E0628(src, &lbl_80400948[idx * 0x30]); }
}
#endif
#endif

#if defined(GS_GFX_LAYER_800D7D90_800D7E5C)
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D7D90(void) {
#include "src/game/gs_render_fn_800D7D90.inc"
}
#else
void gfxSetMatrix(u8 idx, void* src) {
    if (*(s32*)lbl_8047AA80 == 1) {
        gfxRecordCommand(0x43, 0x11, (u32)idx, src);
    } else if (idx > 9) {
        GSlogWrite(lbl_80270440);
    } else {
        GXLoadPosMtxImm((u32)src, *(u32*)(lbl_80314610 + idx * 4));
        fn_800E0628(lbl_80400948 + idx * 0x30, src);
        if (idx == 9) {
            lbl_8047AAC8 = 1;
        }
    }
}
#endif
#endif

#if defined(GS_GFX_LAYER_800D7E5C_800D85D4)
extern u32 lbl_8047AAC0;
extern u32 lbl_8047AAC4;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D7E5C(void) {
#include "src/game/gs_render_fn_800D7E5C.inc"
}
#else
void gfxPopMatrix(void) {
    u32 state = lbl_8047AA80;
    u32 mtx;
    if (*(s32*)state == 1) {
        gfxRecordCommand(0x42, 0);
    } else if (lbl_8047AAC0 >= lbl_8047AAC4) {
        GSlogWrite(lbl_80270460);
    } else {
        lbl_8047AAC0 = lbl_8047AAC0 + 0x30;
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        GXSetCurrentMtx(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u32 lbl_8047AAC0;
extern u32 lbl_8047AABC;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D7F14(void) {
#include "src/game/gs_render_fn_800D7F14.inc"
}
#else
void gfxPushMultMatrix(void* src) {
    u32 state = lbl_8047AA80;
    u32 mtx;
    if (*(s32*)state == 1) {
        gfxRecordCommand(0x41, 0x10, src);
    } else if (lbl_8047AAC0 <= lbl_8047AABC) {
        GSlogWrite(lbl_80270480);
    } else {
        lbl_8047AAC0 = lbl_8047AAC0 - 0x30;
        GSmtxConcat((void*)lbl_8047AAC0, (void*)(lbl_8047AAC0 + 0x30), src);
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        GXSetCurrentMtx(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u32 lbl_8047AAC0;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D7FE4(void) {
#include "src/game/gs_render_fn_800D7FE4.inc"
}
#else
void gfxMultMatrix(void* param) {
    u32 state = lbl_8047AA80;
    u32 mtx;
    if (*(s32*)state == 1) {
        gfxRecordCommand(0x40, 0x10, param);
    } else {
        GSmtxConcat((void*)lbl_8047AAC0, (void*)lbl_8047AAC0, param);
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        GXSetCurrentMtx(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u32 lbl_8047AAC0;
extern u32 lbl_8047AABC;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D8088(void) {
#include "src/game/gs_render_fn_800D8088.inc"
}
#else
void gfxPushLoadMatrix(void* src) {
    u32 state = lbl_8047AA80;
    u32 mtx;
    if (*(s32*)state == 1) {
        gfxRecordCommand(0x3f, 0x10, src);
    } else if (lbl_8047AAC0 <= lbl_8047AABC) {
        GSlogWrite(lbl_80270480);
    } else {
        lbl_8047AAC0 = lbl_8047AAC0 - 0x30;
        fn_800E0628((void*)lbl_8047AAC0, src);
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        GXSetCurrentMtx(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u32 lbl_8047AAC0;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D8154(void) {
#include "src/game/gs_render_fn_800D8154.inc"
}
#else
void gfxScaleMatrix(f32 a, f32 b, f32 c) {
    u32 state = lbl_8047AA80;
    u32 mtx;
    if (*(s32*)state == 1) {
        gfxRecordCommand(0x3e, 0xd, a, b, c);
    } else {
        fn_800E02C4((void*)lbl_8047AAC0);
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        GXSetCurrentMtx(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u32 lbl_8047AAC0;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D81EC(void) {
#include "src/game/gs_render_fn_800D81EC.inc"
}
#else
void gfxTranslateMatrix(f32 a, f32 b, f32 c) {
    u32 state = lbl_8047AA80;
    u32 mtx;
    if (*(s32*)state == 1) {
        gfxRecordCommand(0x3d, 0xd, a, b, c);
    } else {
        fn_800E03E8((void*)lbl_8047AAC0, a, b, c);
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        GXSetCurrentMtx(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u32 lbl_8047AAC0;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D8284(void) {
#include "src/game/gs_render_fn_800D8284.inc"
}
#else
void gfxRotateXYZ(f32 a, f32 b, f32 c) {
    f32 y;
    f32 z;
    u32 matrixIndex;
    y = b;
    z = c;
    if (*(s32*)lbl_8047AA80 == 1) {
        gfxRecordCommand(0x3c, 0xd, a, y, z);
    } else {
        GSmtxRotateX((void*)lbl_8047AAC0, a);
        GSmtxRotateY((void*)lbl_8047AAC0, y);
        GSmtxRotateZ((void*)lbl_8047AAC0, z);
        matrixIndex = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, matrixIndex);
        GXLoadNrmMtxImm(lbl_8047AAC0, matrixIndex);
        GXSetCurrentMtx(matrixIndex);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u32 lbl_8047AAC0;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D834C(void) {
#include "src/game/gs_render_fn_800D834C.inc"
}
#else
void gfxLoadIdentity(void) {
    u32 state = lbl_8047AA80;
    u32 mtx;
    if (*(s32*)state == 1) {
        gfxRecordCommand(0x3b, 0);
    } else {
        fn_800E064C((void*)lbl_8047AAC0);
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        GXSetCurrentMtx(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u16 lbl_8047AAB8;
extern u32 lbl_8047AABC;
extern u32 lbl_8047AAC4;
extern u32 lbl_8047AAC0;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D83E4(void) {
#include "src/game/gs_render_fn_800D83E4.inc"
}
#else
void gfxMatrixStackInit(u32 count) {
    u16 handle;
    u32 mtx;
    u32 ptr;
    u32 val;
    handle = _toolentryAlloc__FUl(count * 0x30);
    val = (u16)handle;
    lbl_8047AAB8 = handle;
    if (val != 0) {
        ptr = (u32)GSmemLock((u16)val);
        lbl_8047AABC = ptr;
        ptr = ptr + (count - 1) * 0x30;
        lbl_8047AAC4 = ptr;
        lbl_8047AAC0 = ptr;
        fn_800E064C((void*)ptr);
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        GXSetCurrentMtx(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u8 lbl_80314404[];
extern u8 lbl_80314454[];
extern u8 lbl_803144A8[];
extern u8 lbl_80314424[];
#if 0
asm void fn_800D848C(void) {
#include "src/game/gs_render_fn_800D848C.inc"
}
#else
void gfxLoadTexMatrix(u32 idx, s32 mode, u32 arg, void* mtx) {
    if (*(s32*)lbl_8047AA80 == 1) {
        gfxRecordCommand(0x27, 0x13, idx, mode, arg, mtx);
    } else {
        if (mode == 0) {
            GXSetTexCoordGen2(((u32*)lbl_80314404)[idx], 1, ((u32*)lbl_80314454)[arg], 0x3c, 0, 0x7d);
        } else {
            if (mode == 1) {
                GXLoadTexMtxImm(mtx, ((u32*)lbl_803144A8)[idx], 0);
            } else if (mode == 2) {
                GXLoadTexMtxImm(mtx, ((u32*)lbl_803144A8)[idx], 1);
            }
            GXSetTexCoordGen2(((u32*)lbl_80314404)[idx], ((u32*)lbl_80314424)[mode], ((u32*)lbl_80314454)[arg], ((u32*)lbl_803144A8)[idx], 0, 0x7d);
        }
    }
}
#endif
#endif

#if defined(GS_GFX_LAYER_800D85D4_800D87AC)
extern u8 lbl_80314530[];
extern u32 lbl_8047CA40;
extern u32 lbl_8047CA48;
extern u8 lbl_80314510[];
extern u8 lbl_803144F0[];
#if 0
asm void fn_800D85D4(void) {
#include "src/game/gs_render_fn_800D85D4.inc"
}
#else
void gfxLoadTexture(s32 slot, void* model) {
    u32 state;
    u8* obj;
    s32 mode;

    if (model == 0) {
        return;
    }

    state = lbl_8047AA80;
    obj = (u8*)model;
    if (*(s32*)state == 1) {
        gfxRecordCommand(0x26, 2, slot, model);
        return;
    }
    if (*(void**)(state + 0x28 + slot * 4) == model) {
        return;
    }

    if (obj[0x7] != 0) {
        GXInitTexObjWrapMode(obj + 0x54, ((u32*)lbl_80314530)[*(u32*)(obj + 0x10)],
                    ((u32*)lbl_80314530)[*(u32*)(obj + 0x14)]);
        switch (*(s32*)(obj + 0x20)) {
        case 0:
            mode = (*(s32*)(obj + 0x18) == 2);
            break;
        case 1:
            mode = (*(s32*)(obj + 0x18) == 2) + 2;
            break;
        case 2:
            mode = (*(s32*)(obj + 0x18) == 2) + 4;
            break;
        }
        GXInitTexObjLOD(obj + 0x54, mode, (*(u32*)(obj + 0x1c) == 2),
                    *(f32*)&lbl_8047CA40, (f32)(obj[0x5] - 1),
                    *(f32*)&lbl_8047CA48, 0, 0, 0);
        obj[0x7] = 0;
    }

    *(void**)(state + 0x28 + slot * 4) = model;
    if (*(u32*)(obj + 0x48) != 0) {
        GXLoadTlut(obj + 0x74, ((u32*)lbl_80314510)[slot]);
    }
    GXLoadTexObj(obj + 0x54, ((u32*)lbl_803144F0)[slot]);
    *(u32*)(lbl_804001F0 + 0x24) += 1;
}
#endif
#endif

#if defined(GS_GFX_LAYER_800D87AC_800D892C)
#if 0
asm void fn_800D87AC(void) {
#include "src/game/gs_render_fn_800D87AC.inc"
}
#else
void _gfxSetFlagsImm(u32 mask) {
    s32 i;
    u8* textureSlot;

    *(u32*)(lbl_8047AA80 + 0x414) |= mask;
    if (mask & 2) {
        for (i = 0; i < 8; i++) {
            ((u32*)lbl_8047AA80)[i + 10] = 0;
            GXSetTexCoordGen2(((u32*)lbl_80314404)[i], 1, i + 4, 0x3c, 0, 0x7d);
        }
    }
    if (mask & 4) {
        for (i = 0; i < 0x10; i++) {
            textureSlot = (u8*)lbl_8047AA80 + i + 0x25c;
            if (*textureSlot) {
                *textureSlot = 0;
                GXSetTevDirect(i);
            }
        }
        *(u8*)(lbl_8047AA80 + 0x3ac) = 0;
        GXSetNumIndStages(0);
    }
}
#endif

#if 0
asm void fn_800D888C(void) {
#include "src/game/gs_render_fn_800D888C.inc"
}
#else
void gfxClearFlags(u32 mask) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x29, 1, mask); }
    else { *(s32*)(lbl_8047AA80 + 0x10) &= ~mask; }
}
#endif

#if 0
asm void fn_800D88DC(void) {
#include "src/game/gs_render_fn_800D88DC.inc"
}
#else
void gfxSetFlags(u32 mask) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x28, 1, mask); }
    else { *(s32*)(lbl_8047AA80 + 0x10) |= mask; }
}
#endif
#endif

/* _gfxFlushTevState: linked owner is the standalone gs_gfx_layer_candidate_800D923C.c. */

/* fn_800D963C: linked owner is the standalone gs_gfx_layer_candidate_800D963C.c. */
